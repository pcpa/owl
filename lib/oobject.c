/*
 * Copyright (C) 2013  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "owl.h"

#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

/*
 * Defines
 */
#define mark_p(memory)		((oword_t)(memory)->next & 1)
#define mark(memory)							\
    (memory)->next = (memory_t *)((oword_t)(memory)->next | 1)
#define clear(memory)							\
    (memory)->next = (memory_t *)((oword_t)(memory)->next & ~1)
#define memory_to_object(type, memory)					\
    (type)((oint8_t *)(memory) + sizeof(memory_t))
#define object_to_memory(object)					\
    (memory_t *)((oint8_t *)(object) - sizeof(memory_t))

#define gc_lock()		omutex_lock(&gc_mutex)
#define gc_unlock()		omutex_unlock(&gc_mutex)

/*
 * Types
 */
typedef struct memory	memory_t;
struct memory {
    memory_t		*next;
#if __WORDSIZE == 64
    oint32_t		 align;
#endif
    oint32_t		 header;
};

/*
 * Prototypes
 */
static void
suspend_handler(int unused);

static void
sigfpe_handler(int unused);

static void
resume_handler(int unused);

static void
new_object(oobject_t *pointer, otype_t type, oword_t length);

static void
gc(void);

static void
gc_mark_record(oint8_t *base, oword_t *gcinfo, oword_t gcsize);

static void
gc_mark_arguments(oint8_t *base, oword_t *gcinfo, oword_t gcsize);

static void
gc_mark(memory_t *memory);

static void
gc_mark_stack(oint8_t *base);

static void
gc_mark_stack_next(oint8_t *base);

static void
gc_mark_varargs(oobject_t *pointer, oint32_t length);

static void
gc_mark_thread(othread_t *thread);

static void
gc_mark_roots(void);

/*
 * Initialization
 */
othread_t		 *thread_main;
ovector_t		 *rtti_vector;
oint32_t		  gc_offset;
oobject_t		  gc_vector[16];
#if GC_DEBUG
oint32_t		  gc_debug = 1;
#endif

oint8_t			 *gd;
ovector_t		 *cv;

pthread_mutex_t		  othread_mutex;

static memory_t		 *gc_root;
static memory_t		 *gc_tail;
static oobject_t	**gc_root_vector;
static oword_t		  gc_root_offset;
static oword_t		  gc_root_length;
#if !GC_DEBUG
static oword_t		  gc_bytes;
#endif

static pthread_mutex_t	  gc_mutex;
/* need to use multiple semaphores or it may dead lock */
static sem_t		  enter_sem;
static sem_t		  sleep_sem;
static sem_t		  leave_sem;
static volatile oint32_t  sr_int;
static sigset_t		  sr_set;

#if CACHE_DEBUG
eint32_t		  cache_debug = 1;
static eobject_t	  cache_object;
#endif

/*
 * Implementation
 */
void
init_object(void)
{
    sigset_t			set;
    struct sigaction		handler;

    omutex_init(&othread_mutex);
    omutex_init(&gc_mutex);

    if (cfg_use_semaphore) {
	if (sem_init(&enter_sem, 0, 0) || sem_init(&sleep_sem, 0, 0) ||
	    sem_init(&leave_sem, 0, 0))
	    oerror("sem_init: %s", strerror(errno));
    }

    /* FIXME this should be a noop */
    pthread_sigmask(SIG_SETMASK, null, &set);
    sigdelset(&set, SUSPEND_SIGNAL);

    if (!cfg_use_semaphore)
	sigdelset(&set, RESUME_SIGNAL);
    pthread_sigmask(SIG_SETMASK, &set, null);

    if (!cfg_use_semaphore) {
	sigfillset(&sr_set);
	sigdelset(&sr_set, RESUME_SIGNAL);
    }

    handler.sa_flags = 0;
    sigemptyset(&handler.sa_mask);
    handler.sa_handler = suspend_handler;
    if (sigaction(SUSPEND_SIGNAL, &handler, null))
	oerror("sigaction: %s", strerror(errno));

    if (!cfg_use_semaphore) {
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	handler.sa_handler = resume_handler;
	if (sigaction(RESUME_SIGNAL, &handler, null))
	    oerror("sigaction: %s", strerror(errno));
    }

    new_object((oobject_t *)&thread_main, t_thread, sizeof(othread_t));

    cqq_init(&thread_main->qq);
    mpc_init2(&thread_main->cc, thr_prc);
    mpf_init(&thread_main->f);
    onew_vector((oobject_t *)&thread_main->vec, t_uint8, BUFSIZ);

#define init_register(T, N)						\
    do {								\
	cqq_init(&T->r##N.qq);						\
	mpc_init2(&T->r##N.cc, thr_prc);				\
	onew_vector((oobject_t *)&T->r##N.vec, t_uint8, BUFSIZ);	\
    } while (0)
    init_register(thread_main, 0);
    init_register(thread_main, 1);
    init_register(thread_main, 2);
    init_register(thread_main, 3);

#define finish_register(T, N)						\
    do {								\
	cqq_clear(&T->r##N.qq);						\
	mpc_clear(&T->r##N.cc);						\
    } while (0)

    onew_vector((oobject_t *)&rtti_vector, t_void, 128);

    thread_main->next = thread_main;
    thread_main->pthread = pthread_self();

    /* Generic parser signal handler */
    signal(SIGFPE, sigfpe_handler);
    if (sigsetjmp(thread_main->env, 1))
	oerror("parser unhandled exception");

    /* Allow overriding exception handling during constant folding */
    memcpy(&cfg_env, &thread_main->env, sizeof(sigjmp_buf));
}

void
finish_object(void)
{
    gc_root_offset = 0;
    gc_offset = 0;
    rtti_vector = null;
    gd = null;
    cv = null;
    thread_main = null;

    /*   All memory should be released. */
#if CACHE_DEBUG
    cache_object = null;
    /* ewarn should not work anyway because streams are closed */
    cache_debug = 0;
#endif

    /*   finish_stream() must be called just before finish_object() */
    std_input = std_output = std_error = null;
    gc();

    assert(gc_root == null);
    free(gc_root_vector);
}

void
oadd_root(oobject_t *object)
{
    if (gc_root_offset >= gc_root_length) {
	if ((gc_root_length += 16) == 16) {
	    if ((gc_root_vector = malloc(sizeof(oobject_t) * 16)) == null)
		oerror("out of memory");
	}
	else if ((gc_root_vector = realloc(gc_root_vector, sizeof(oobject_t) *
					   gc_root_length)) == null)
	    oerror("out of memory");
    }
    assert(object);
    gc_root_vector[gc_root_offset++] = object;
}

void
orem_root(oobject_t *object)
{
    oword_t		offset;

    for (offset = gc_root_offset - 1; offset >= 0; offset--) {
	if (gc_root_vector[offset] == object) {
	    if (--gc_root_offset)
		gc_root_vector[offset] = gc_root_vector[gc_root_offset];
	    return;
	}
    }
    /*  Streams may no longer be available. */
    abort();
}

void
onew_object(oobject_t *pointer, otype_t type, oword_t length)
{
    gc_lock();

    /*  Run gc roughly every 16 megabytes. */
#if GC_DEBUG
    if (gc_debug)
	gc();
#else
    if (gc_bytes > (1 << 24)) {
	gc();
	gc_bytes = 0;
    }
    gc_bytes += length;
#endif

    new_object(pointer, type, length);

    gc_unlock();
}

void
onew_thread(oobject_t *pointer)
{
    GET_THREAD_SELF()
    othread_t		*next;
    othread_t		*thread;

    onew_object(pointer, t_thread, sizeof(othread_t));
    thread = *(othread_t **)pointer;

    cqq_init(&thread->qq);
    mpc_init2(&thread->cc, thr_prc);
    mpf_init(&thread->f);
    onew_vector((oobject_t *)&thread->vec, t_uint8, BUFSIZ);

    init_register(thread, 0);
    init_register(thread, 1);
    init_register(thread, 2);
    init_register(thread, 3);

    /* Add thread to "end" of circular list */
    othreads_lock();
    thread->next = thread_self;
    for (next = thread_self->next; next->next != thread_self; next = next->next)
	;
    next->next = thread;
    othreads_unlock();
}

void
onew_word(oobject_t *pointer, oword_t w)
{
    onew_object(pointer, t_word, sizeof(oword_t));
    **(oword_t **)pointer = w;
}

void
onew_float(oobject_t *pointer, ofloat_t d)
{
    onew_object(pointer, t_float, sizeof(ofloat_t));
    **(ofloat_t **)pointer = d;
}

void
onew_float_w(oobject_t *pointer, oword_t w)
{
    onew_object(pointer, t_float, sizeof(ofloat_t));
    **(ofloat_t **)pointer = w;
}

void
onew_rat(oobject_t *pointer, orat_t R)
{
    orat_t		r;

    onew_object(pointer, t_rat, sizeof(rat_t));
    r = *(orat_t *)pointer;
    orat_num(r) = orat_num(R);
    orat_den(r) = orat_den(R);
}

void
onew_rat_w(oobject_t *pointer, oword_t w)
{
    orat_t		r;

    onew_object(pointer, t_rat, sizeof(rat_t));
    r = *(orat_t *)pointer;
    orat_num(r) = w;
    orat_den(r) = 1;
}

void
onew_mpz(oobject_t *pointer, ompz_t Z)
{
    ompz_t		z;

    onew_object(pointer, t_mpz, sizeof(mpz_t));
    z = *(ompz_t *)pointer;
    mpz_init_set(z, Z);
}

void
onew_mpz_w(oobject_t *pointer, oword_t w)
{
    ompz_t		z;

    onew_object(pointer, t_mpz, sizeof(mpz_t));
    z = *(ompz_t *)pointer;
    mpz_init_set_si(z, w);
}

void
onew_mpq(oobject_t *pointer, ompq_t Q)
{
    ompq_t		q;

    onew_object(pointer, t_mpq, sizeof(mpq_t));
    q = *(ompq_t *)pointer;
    mpq_init(q);
    mpq_set(q, Q);
}

void
onew_mpq_w(oobject_t *pointer, oword_t w)
{
    ompq_t		q;

    onew_object(pointer, t_mpq, sizeof(mpq_t));
    q = *(ompq_t *)pointer;
    mpq_init(q);
    mpq_set_si(q, w, 1);
}

void
onew_mpr(oobject_t *pointer, ompr_t R)
{
    ompr_t		r;

    onew_object(pointer, t_mpr, sizeof(mpfr_t));
    r = *(ompr_t *)pointer;
    mpfr_init2(r, thr_prc);
    mpfr_set(r, R, thr_rnd);
}

void
onew_mpr_w(oobject_t *pointer, oword_t w)
{
    ompr_t		r;

    onew_object(pointer, t_mpr, sizeof(mpfr_t));
    r = *(ompr_t *)pointer;
    mpfr_init2(r, thr_prc);
    mpfr_set_si(r, w, thr_rnd);
}

void
onew_cdd(oobject_t *pointer, ocdd_t DD)
{
    ocdd_t		*dd;

    onew_object(pointer, t_cdd, sizeof(ocdd_t));
    dd = *(ocdd_t **)pointer;
    *dd = DD;
}

void
onew_cdd_w(oobject_t *pointer, oword_t w)
{
    ocdd_t		*c;

    onew_object(pointer, t_cdd, sizeof(ocdd_t));
    c = *(ocdd_t **)pointer;
    real(*c) = w;
}

void
onew_cqq(oobject_t *pointer, ocqq_t QQ)
{
    ocqq_t	qq;

    onew_object(pointer, t_cqq, sizeof(cqq_t));
    qq = *(ocqq_t *)pointer;
    cqq_init(qq);
    cqq_set(qq, QQ);
}

void
onew_cqq_w(oobject_t *pointer, oword_t w)
{
    ocqq_t	qq;

    onew_object(pointer, t_cqq, sizeof(cqq_t));
    qq = *(ocqq_t *)pointer;
    cqq_init(qq);
    mpz_set_si(mpq_numref(cqq_realref(qq)), w);
}

void
onew_mpc(oobject_t *pointer, ompc_t CC)
{
    ompc_t		cc;

    onew_object(pointer, t_mpc, sizeof(mpc_t));
    cc = *(ompc_t *)pointer;
    mpc_init2(cc, thr_prc);
    mpc_set(cc, CC, thr_rnd);
}

void
onew_mpc_w(oobject_t *pointer, oword_t w)
{
    ompc_t		cc;

    onew_object(pointer, t_mpc, sizeof(mpc_t));
    cc = *(ompc_t *)pointer;
    mpc_init2(cc, thr_prc);
    mpfr_set_si(mpc_realref(cc), w, thr_rnd);
    mpfr_set_ui(mpc_imagref(cc), 0, thr_rnd);
}

void
onew_vector_base(oobject_t *pointer,
		 otype_t type, otype_t base_type,
		 oword_t length, oword_t base_length)
{
    oword_t		 bytes;
    ovector_t		*vector;

    gc_lock();

    new_object((oobject_t *)pointer, base_type, base_length);
    vector = *pointer;
    vector->length = length;
    switch (type) {
	case t_int8:	case t_uint8:
	    bytes = length;
	    break;
	case t_int16:	case t_uint16:
	    bytes = length << 1;
	    break;
	case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
	default:
#endif
	    bytes = length << 2;
	    break;
	case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
	default:
#endif
	    bytes = length << 3;
	    break;
    }

    bytes = (bytes + 15) & ~15;
    if (bytes < length)
	oerror("out of bounds");
    new_object(&vector->v.obj, type, bytes);

    gc_unlock();
}

void
orenew_vector(ovector_t *vector, oword_t length)
{
    memory_t		*memory;
    oword_t		 new_bytes;
    oword_t		 old_bytes;
    oword_t		 new_length;
    oword_t		 old_length;
    memory_t		*new_memory;
    memory_t		*old_memory;

    gc_lock();

    old_memory = object_to_memory(vector->v.obj);
    switch (old_memory->header) {
	case t_int8:	case t_uint8:
	    new_length = length;
	    old_length = vector->length;
	    break;
	case t_int16:	case t_uint16:
	    new_length = length << 1;
	    old_length = vector->length << 1;
	    break;
	case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
	default:
#endif
	    new_length = length << 2;
	    old_length = vector->length << 2;
	    break;
	case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
	default:
#endif
	    new_length = length << 3;
	    old_length = vector->length << 3;
	    break;
    }

    new_bytes = ((new_length + 15) & ~15) + sizeof(memory_t);
    old_bytes = ((old_length + 15) & ~15) + sizeof(memory_t);

    if (unlikely(new_bytes < new_length))
	oerror("out of bounds");
    vector->length = length;

    if (new_bytes != old_bytes) {
	memory = object_to_memory(vector);
	assert(memory->next == old_memory);
	if (unlikely((new_memory = realloc(old_memory, new_bytes)) == null))
	    oerror("out of memory");
	if (old_memory != new_memory) {
	    if (gc_tail == old_memory)
		gc_tail = new_memory;
	    memory->next = new_memory;
	    vector->v.obj = memory_to_object(oobject_t, new_memory);
	}

	/*   Must to zero out increased vector before possible gc call. */
	if (new_length > old_length)
	    memset(vector->v.u8 + old_length, 0, new_length - old_length);

#if GC_DEBUG
	if (gc_debug)
	    gc();
#else
	gc_bytes += new_length - old_length;
	/*  Run gc roughly every 16 megabytes. */
	if (gc_bytes > (1 << 24)) {
	    gc();
	    gc_bytes = 0;
	}
#endif
    }
    else if (new_length > old_length)
	memset(vector->v.u8 + old_length, 0, new_length - old_length);

    gc_unlock();
}

static void
suspend_handler(int unused)
{
    if (cfg_use_semaphore) {
	if (sem_post(&enter_sem)) {
	    write(STDERR_FILENO, "sem_post: failed\n", 17);
	    _exit(EXIT_FAILURE);
	}
	while (sem_wait(&sleep_sem)) {
	    /* FIXME is this check safe? can this happen? */
	    if (errno != EINTR) {
		write(STDERR_FILENO, "sem_wait: failed\n", 17);
		_exit(EXIT_FAILURE);
	    }
	}
	if (sem_post(&leave_sem)) {
	    write(STDERR_FILENO, "sem_post: failed\n", 17);
	    _exit(EXIT_FAILURE);
	}
    }
    else {
	sr_int = 1;
	/* wait resume signal */
	sigsuspend(&sr_set);
    }
}

static void
sigfpe_handler(int unused)
{
    siglongjmp(thread_main->env, 1);
}

static void
resume_handler(int unused)
{
    sr_int = 1;
    /* returning from resume_handler() also means returing from sigsuspend() */
}

static void
new_object(oobject_t *pointer, otype_t type, oword_t length)
{
    oword_t		 bytes;
    oobject_t		 object;
    memory_t		*memory;

    if ((bytes = length + sizeof(memory_t)) < length)
	oerror("invalid object length");

    if ((memory = (memory_t *)calloc(1, bytes)) == null)
	oerror("out of memory");
    object = memory_to_object(oobject_t, memory);
    if (unlikely(gc_root == null))
	gc_root = gc_tail = memory;
    else
	gc_tail->next = memory;
    gc_tail = memory;
    memory->header = type;
    *pointer = object;
}

#if CACHE_DEBUG
void
gc_cache_debug(eobject_t object)
{
    cache_object = object;
    gc();
}
#endif

static void
gc(void)
{
    memory_t		*prev;
    memory_t		*next;
    othread_t		*thread;
    memory_t		*memory;
#if CACHE_DEBUG
    oint32_t		 cache_count = 0;
#endif
#if SDL
    union {
	ofont_t		*font;
	oobject_t	 object;
	orenderer_t	*renderer;
	osurface_t	*surface;
	otexture_t	*texture;
	owindow_t	*window;
	otimer_t	*timer;
    } o;
#endif

    /* Prevent threads from exiting */
    othreads_lock();

    gc_mark_roots();

    prev = next = gc_root;
    while ((memory = next)) {
	if (mark_p(memory)) {
	    clear(memory);
	    prev = memory;
	    next = memory->next;
#if CACHE_DEBUG
	    switch (memory->header) {
		case t_word:	case t_float:	case t_rat:
		case t_mpz:	case t_mpq:	case t_mpr:
		case t_cdd:	case t_cqq:	case t_mpc:
		case t_ast:
		    if (memory_to_object(eobject_t, memory) == cache_object) {
			++cache_count;
			assert(cache_count == 1);
		    }
		    break;
	    }
#endif
	}
	else {
	    next = memory->next;
	    /* release extra memory or free system resources */
	    switch (memory->header) {
		case t_stream:
		    oclose(memory_to_object(ostream_t *, memory));
		    break;
		case t_mpz:
		    mpz_clear(memory_to_object(ompz_t, memory));
#if CACHE_DEBUG
		    if (cache_debug)
			ewarn("cache leak");
#endif
		    break;
		case t_mpq:
		    mpq_clear(memory_to_object(ompq_t, memory));
#if CACHE_DEBUG
		    if (cache_debug)
			ewarn("cache leak");
#endif
		    break;
		case t_mpr:
		    mpfr_clear(memory_to_object(ompr_t, memory));
#if CACHE_DEBUG
		    if (cache_debug)
			ewarn("cache leak");
#endif
 		    break;
		case t_cqq:
		    cqq_clear(memory_to_object(ocqq_t, memory));
#if CACHE_DEBUG
		    if (cache_debug)
			ewarn("cache leak");
#endif
		    break;
		case t_mpc:
		    mpc_clear(memory_to_object(ompc_t, memory));
#if CACHE_DEBUG
		    if (cache_debug)
			ewarn("cache leak");
#endif
		    break;
		case t_thread:
		    thread = memory_to_object(othread_t *, memory);
		    cqq_clear(&thread->qq);
		    mpc_clear(&thread->cc);
		    mpf_clear(&thread->f);
		    finish_register(thread, 0);
		    finish_register(thread, 1);
		    finish_register(thread, 2);
		    finish_register(thread, 3);
		    break;
		case t_mutex:
		    omutex_destroy(memory_to_object(pthread_mutex_t *, memory));
		    break;
#if CACHE_DEBUG
		case t_word:	case t_float:	case t_rat:
		case t_ast:	case t_operand:
		    if (cache_debug)
			ewarn("cache leak");
		    break;
#endif
#if SDL
		case t_font:
		    o.object = memory_to_object(oobject_t, memory);
		    if (o.font)
			TTF_CloseFont(o.font->__font);
		    break;
		case t_renderer:
		    o.object = memory_to_object(oobject_t, memory);
		    if (o.renderer->__renderer)
			SDL_DestroyRenderer(o.renderer->__renderer);
		    break;
		case t_surface:
		    o.object = memory_to_object(oobject_t, memory);
		    if (o.surface->__surface)
			SDL_FreeSurface(o.surface->__surface);
		    break;
		case t_texture:
		    o.object = memory_to_object(oobject_t, memory);
		    odestroy_texture(o.texture);
		    break;
		case t_window:
		    o.object = memory_to_object(oobject_t, memory);
		    odestroy_window(o.window);
		    break;
		case t_timer:
		    o.object = memory_to_object(oobject_t, memory);
		    if (o.timer->__timer)
			SDL_RemoveTimer(o.timer->__timer);
#endif
		default:
		    break;
	    }
	    if (unlikely(gc_root == memory))
		gc_root = next;
	    else
		prev->next = next;
	    free(memory);
	}
    }
    gc_tail = prev;

#if CACHE_DEBUG
    if (cache_object)
	assert(cache_count == 1);
#endif

    /* Allow threads to exit */
    othreads_unlock();
}

static void
gc_mark_record(oint8_t *base, oword_t *gcinfo, oword_t gcsize)
{
    oword_t		 offset;
    oobject_t		 object;
    oobject_t		*pointer;

    for (offset = 0; offset < gcsize; offset++) {
	pointer = (oobject_t *)(base + gcinfo[offset]);
	if ((object = *pointer))
	    gc_mark(object_to_memory(object));
    }
}

static void
gc_mark_arguments(oint8_t *base, oword_t *gcinfo, oword_t gcsize)
{
    oword_t		 offset;
    oobject_t		 object;
    oobject_t		*pointer;

    for (offset = 0; offset < gcsize; offset++) {
	if (gcinfo[offset] < 0)
	    break;
	pointer = (oobject_t *)(base + gcinfo[offset]);
	if ((object = *pointer))
	    gc_mark(object_to_memory(object));
    }
}

static void
gc_mark(memory_t *memory)
{
    oint8_t		*base;
    ortti_t		*rtti;
    otype_t		 type;
    oentry_t		*entry;
    oword_t		 offset;
    oobject_t		*pointer;
    ohashentry_t	*hashentry;
    union {
	oast_t		*ast;
	obuiltin_t	*builtin;
	oentry_t	*entry;
	ofunction_t	*function;
	ohash_t		*hash;
	ohashentry_t	*hashentry;
	ohashtable_t	*hashtable;
	oinput_t	*input;
	ojump_t		*jump;
	omacro_t	*macro;
	oobject_t	 object;
	oobject_t	*pointer;
	orecord_t	*record;
	ortti_t		*rtti;
	ostream_t	*stream;
	osymbol_t	*symbol;
	othread_t	*thread;
	ovector_t	*vector;
    } o;

again:
    if (mark_p(memory))
	return;
    mark(memory);
    o.object = memory_to_object(oobject_t, memory);
    switch (type = memory->header) {
	case t_ast:
	    if (o.ast->l.value)
		gc_mark(object_to_memory(o.ast->l.value));
	    if (o.ast->r.value)
		gc_mark(object_to_memory(o.ast->r.value));
	    if (o.ast->t.value)
		gc_mark(object_to_memory(o.ast->t.value));
	    if (o.ast->c.value)
		gc_mark(object_to_memory(o.ast->c.value));
	    if (o.ast->next) {
		memory = object_to_memory(o.ast->next);
		goto again;
	    }
	    break;
	case t_builtin:				case t_entry:
	case t_tag:
	    if (o.entry->value) {
		if (o.entry->name)
		    gc_mark(object_to_memory(o.entry->name));
		memory = object_to_memory(o.entry->value);
		goto again;
	    }
	    else if (o.entry->name) {
		memory = object_to_memory(o.entry->name);
		goto again;
	    }
	    break;
	case t_function:
	    if (o.function->ast)
		gc_mark(object_to_memory(o.function->ast));
	    if (o.function->record)
		gc_mark(object_to_memory(o.function->record));
	    if (o.function->labels) {
		memory = object_to_memory(o.function->labels);
		goto again;
	    }
	    break;
	case t_hashtable:
	    if (o.hashtable->entries) {
		mark(object_to_memory(o.hashtable->entries));
		if (o.hashtable->count) {
		    for (offset = 0; offset < o.hashtable->size; offset++) {
			hashentry = o.hashtable->entries[offset];
			for (; hashentry; hashentry = hashentry->next)
			    gc_mark(object_to_memory(hashentry));
		    }
		}
	    }
	    break;
	case t_hashentry:
	    switch (o.hashentry->nt) {
		case t_void:		case t_word:
		case t_float:		case t_rat:
		case t_cdd:
		    break;
		default:
		    gc_mark(object_to_memory(o.hashentry->nv.o));
		    break;
	    }
	    switch (o.hashentry->vt) {
		case t_void:		case t_word:
		case t_float:		case t_rat:
		case t_cdd:
		    return;
		default:
		    break;
	    }
	    memory = object_to_memory(o.hashentry->vv.o);
	    goto again;
	case t_hash:
	    if (o.hash->entries) {
		mark(object_to_memory(o.hash->entries));
		if (o.hash->count) {
		    for (offset = 0; offset < o.hash->size; offset++) {
			entry = o.hash->entries[offset];
			for (; entry; entry = entry->next)
			    gc_mark(object_to_memory(entry));
		    }
		}
	    }
	    break;
	case t_input:
	    if (o.input->stream) {
		memory = object_to_memory(o.input->stream);
		goto again;
	    }
	    break;
	case t_record:				case t_namespace:
	case t_prototype:
	    if (o.record->parent)
		gc_mark(object_to_memory(o.record->parent));
	    if (o.record->gcinfo)
		gc_mark(object_to_memory(o.record->gcinfo));
	    if (o.record->vector)
		gc_mark(object_to_memory(o.record->vector));
	    if (o.record->methods)
		gc_mark(object_to_memory(o.record->methods));
	    if (o.record->function)
		gc_mark(object_to_memory(o.record->function));
	    if (o.record->fields) {
		memory = object_to_memory(o.record->fields);
		goto again;
	    }
	    break;
	case t_rtti:
	    if (o.rtti->gcinfo)
		mark(object_to_memory(o.rtti->gcinfo));
	    if (o.rtti->mdinfo)
		mark(object_to_memory(o.rtti->mdinfo));
	    break;
	case t_symbol:
	    if (o.symbol->value) {
		memory = object_to_memory(o.symbol->value);
		goto again;
	    }
	    break;
	case t_stream:
	    if (o.stream->ptr) {
		if (o.stream->name)
		    gc_mark(object_to_memory(o.stream->name));
		memory = object_to_memory(o.stream->ptr);
		goto again;
	    }
	    else if (o.stream->name) {
		memory = object_to_memory(o.stream->name);
		goto again;
	    }
	    break;
	case t_thread:
	    if (!o.thread->run)
		gc_mark_thread(o.thread);
	    break;
	case t_jump:
	    if (o.jump->t)
		gc_mark(object_to_memory(o.jump->t));
	    if (o.jump->f) {
		memory = object_to_memory(o.jump->f);
		goto again;
	    }
	    break;
	case t_macro:
	    if (o.macro->vector)
		gc_mark(object_to_memory(o.macro->vector));
	    if (o.macro->table) {
		memory = object_to_memory(o.macro->table);
		goto again;
	    }
	    break;
	case t_vector_int8:			case t_vector_uint8:
	case t_vector_int16:			case t_vector_uint16:
	case t_vector_int32:			case t_vector_uint32:
	case t_vector_int64:			case t_vector_uint64:
	case t_vector_float32:			case t_vector_float64:
	    if ((o.object = o.vector->v.obj))
		mark(object_to_memory(o.object));
	    break;
	default:
	    if (type & t_vector) {
		if ((pointer = o.vector->v.ptr + o.vector->length)) {
		    offset = o.vector->length;
		    o.object = o.vector->v.obj;
		    memory = object_to_memory(o.object);
		    mark(memory);
		    if (offset) {
			for (--pointer; o.pointer < pointer; o.pointer++) {
			    if (*o.pointer)
				gc_mark(object_to_memory(*o.pointer));
			}
			if ((o.object = *o.pointer)) {
			    memory = object_to_memory(o.object);
			    goto again;
			}
		    }
		}
	    }
	    else if (type > t_mpc && type <= rtti_vector->offset) {
		rtti = rtti_vector->v.ptr[type];
		base = memory_to_object(oint8_t *, memory);
		gc_mark_record(base, rtti->gcinfo, rtti->gcsize);
	    }
	    break;
    }
}

static void
gc_mark_stack(oint8_t *base)
{
    oint32_t		 size;
    oint32_t		 type;
    oint8_t		*next;
    ortti_t		*rtti;

    do {
	if ((next = *(oint8_t **)(base + NEXT_OFFSET)))
	    gc_mark_stack_next(next);
	type = *(oint32_t *)(base + TYPE_OFFSET);
	if (likely(type != t_root)) {
	    assert(type > t_mpc && type <= rtti_vector->offset);
	    rtti = rtti_vector->v.ptr[type];
	    gc_mark_record(base, rtti->gcinfo, rtti->gcsize);
	    if ((size = *(oint32_t *)(base + SIZE_OFFSET))) {
		size /= sizeof(oobject_t);
		gc_mark_varargs((oobject_t *)(base + rtti->varargs), size);
	    }
	}
	base = *(oint8_t **)(base + PREV_OFFSET);
    } while (base);
}

static void
gc_mark_stack_next(oint8_t *base)
{
    oint32_t		 size;
    oint32_t		 type;
    oint8_t		*next;
    ortti_t		*rtti;

    do {
	/* Stop if stack frame constructed */
	if (*(oobject_t *)(base + PREV_OFFSET))
	    break;
	/* Stop if still being constructed */
	if ((type = *(oint32_t *)(base + TYPE_OFFSET)) == 0)
	    break;
	assert(type > t_mpc && type <= rtti_vector->offset);
	rtti = rtti_vector->v.ptr[type];
	gc_mark_arguments(base, rtti->gcinfo, rtti->gcsize);
	if ((size = *(oint32_t *)(base + SIZE_OFFSET))) {
	    size /= sizeof(oobject_t);
	    gc_mark_varargs((oobject_t *)(base + rtti->varargs), size);
	}
	next = *(oint8_t **)(base + NEXT_OFFSET);
    } while ((base = next));
}

static void
gc_mark_varargs(oobject_t *pointer, oint32_t length)
{
    oobject_t		object;

    for (; length > 0; pointer++, length--) {
	if ((object = *pointer))
	    gc_mark(object_to_memory(object));
    }
}

static void
gc_mark_thread(othread_t *thread)
{
    obool_t		stop;
    oint32_t		error;

    stop = thread->run && !pthread_equal(pthread_self(), thread->pthread);

    /* Suspend control thread */
    if (stop) {
	if (cfg_use_semaphore) {
	    if ((error = pthread_kill(thread->pthread, SUSPEND_SIGNAL)))
		oerror("pthread_kill: %s", strerror(error));
	    /* wait for signal handler to enter */
	    while (sem_wait(&enter_sem)) {
		if (errno != EINTR)
		    oerror("sem_wait: %s", strerror(errno));
		sched_yield();
	    }
	}
	else {
	    sr_int = 0;
	    if ((error = pthread_kill(thread->pthread, SUSPEND_SIGNAL)))
		oerror("pthread_kill: %s", strerror(error));
	    while (!sr_int)
		sched_yield();
	}
    }

    mark(object_to_memory(thread));
    if (thread->obj)
	gc_mark(object_to_memory(thread->obj));

    if (thread->bp)
	mark(object_to_memory(thread->bp));
    if (thread->fp)
	gc_mark_stack(thread->fp);
    if (thread->ev)
	mark(object_to_memory(thread->ev));

    if (thread->vec)
	gc_mark(object_to_memory(thread->vec));
#define mark_register(N)						\
    do {								\
	if (thread->r##N.vec)						\
	    gc_mark(object_to_memory(thread->r##N.vec));		\
    } while (0)
    mark_register(0);
    mark_register(1);
    mark_register(2);
    mark_register(3);

    /* Resume control thread */
    if (stop) {
	if (cfg_use_semaphore) {
	    if (sem_post(&sleep_sem))
		oerror("sem_post: %s", strerror(errno));
	    /* Wait for signal handler to leave */
	    while (sem_wait(&leave_sem)) {
		if (errno != EINTR)
		    oerror("sem_wait: %s", strerror(errno));
		sched_yield();
	    }
	}
	else {
	    sr_int = 0;
	    if ((error = pthread_kill(thread->pthread, RESUME_SIGNAL)))
		oerror("pthread_kill: %s", strerror(error));
	    while (!sr_int)
		sched_yield();
	}
    }
}

static void
gc_mark_roots(void)
{
    othread_t		*thread;
    oword_t		 offset;
    oobject_t		 object;

    for (offset = 0; offset < gc_root_offset; offset++) {
	if ((object = *gc_root_vector[offset]))
	    gc_mark(object_to_memory(object));
    }
    for (offset = 0; offset < gc_offset; offset++) {
	if ((object = gc_vector[offset]))
	    gc_mark(object_to_memory(object));
    }

    if ((thread = thread_main)) {
	do {
	    gc_mark_thread(thread);
	    thread = thread->next;
	} while (thread != thread_main);
    }

    if (rtti_vector)
	gc_mark(object_to_memory(rtti_vector));

    if (gd)
	gc_mark(object_to_memory(gd));
    if (cv)
	mark(object_to_memory(cv));
}
