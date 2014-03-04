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

#if HAVE_EXECINFO_H
#  include <execinfo.h>
#endif
#include <unistd.h>

#define THREAD_STACK_SIZE	1024 * 256

/*
 * Prototypes
 */
static void
vm_signal_handler(int signo);

static void
search_instruction_pointer(void);

static void
vm_builtin(othread_t *thread);

/*
 * Initialization
 */
jit_function_t			 jit_main;

static pthread_cond_t		 count_cond;
static pthread_mutex_t		 count_mutex;
static ovector_t		*thread_vector;

/*
 * Implementation
 */
void
init_vm(void)
{
    pthread_cond_init(&count_cond, null);
    omutex_init(&count_mutex);
    /*
     * SIGFPE and SIGSEGV are (or should be) sent to the thread causing it
     * other signals are (or may be) sent to random threads
     */
    signal(SIGFPE, vm_signal_handler);
    signal(SIGSEGV, vm_signal_handler);

    oadd_root((oobject_t *)&thread_vector);
    onew_vector((oobject_t *)&thread_vector, t_word, 16);
}

void
finish_vm(void)
{
    orem_root((oobject_t *)&thread_vector);
}

static void
vm_signal_handler(int signo)
{
    GET_THREAD_SELF()
    switch (signo) {
	case SIGFPE:
	    thread_self->xcpt = except_floating_point_error;
	    break;
	case SIGSEGV:
	    thread_self->xcpt = except_segmentation_violation;
	    break;
	default:
	    write(STDERR_FILENO, "unexpected signal\n", 18);
	    _exit(EXIT_FAILURE);
    }
    thread_self->r0.t = t_word;
    thread_self->r0.v.w = thread_self->xcpt;
    /* While information in the stack */
    search_instruction_pointer();
    siglongjmp(thread_self->env, 1);
}

static void
search_instruction_pointer(void)
{
    GET_THREAD_SELF()
    oint8_t		*ip;
#if HAVE_EXECINFO_H
    oint32_t		 length;
    oint32_t		 offset;
    oobject_t		 frames[32];

    length = backtrace(frames, sizeof(frames) / sizeof(frames[0]));
    for (offset = 0; offset < length; offset++) {
	ip = frames[offset];
	if (jit_pointer_p(ip)) {
	    thread_self->ip = ip;
	    break;
	}
    }
#else	/* this may still have problems if not compiled with
	 * -fno-omit-frame-pointer on i686 */
    /* __builtin_return_address() and __builtin_frame_address()
     * require an integer constant argument */
#  define search(number)						\
    do {								\
	ip = __builtin_return_address(number);				\
	if (ip == null || __builtin_frame_address(number) == null)	\
	    goto fail;							\
	if (jit_pointer_p(ip))						\
	    goto done;							\
    } while (0)
    search(0);
#  if !defined(__x86_64__)
    search(1);
    search(2);
    search(3);
    search(4);
    search(5);
    search(6);
    search(7);
    search(8);
    search(9);
    search(10);
    search(11);
    search(12);
    search(13);
    search(14);
    search(15);
    search(16);
    search(17);
    search(18);
    search(19);
    search(20);
    search(21);
    search(22);
    search(23);
    search(24);
    search(25);
    search(26);
    search(27);
    search(28);
    search(29);
    search(30);
    search(31);
    /* Greed is Good! (the more stack frames the better) */
#  endif
#  undef search
fail:
    /* will probably print a bogus backtrace with an early address */
    return;
done:
    thread_self->ip = ip;
#endif
}

void
ovm(othread_t *thread)
{
    oframe_t		*frame;

#if HAVE_TLS
    thread_self = thread;
#else
    if (pthread_setspecific(thread_self_key, thread))
	oerror("pthread_setspecific: %s", strerror(errno));
#endif
    if (thread == thread_main) {
	assert(thread->bp == null);
	onew_object((oobject_t *)&thread->bp, t_void, cfg_stack_size);
	thread->sp = thread->fp = thread->bp + cfg_stack_size -
	    (thread->frame + sizeof(oframe_t));
	frame = (oframe_t *)thread->sp;
	frame->type = thread->type;
    }

    othreads_lock();
    thread->run = 1;
    thread->xcpt = except_nothing;

    (*jit_main)(thread);
}

static void
vm_builtin(othread_t *thread)
{
    oint32_t		 ac;
    oobject_t		 list;
    oframe_t		*frame;
    onative_t		 native;

#if HAVE_TLS
    thread_self = thread;
#else
    if (pthread_setspecific(thread_self_key, thread))
	oerror("pthread_setspecific: %s", strerror(errno));
#endif

    native = (onative_t)thread->ip;
    frame = (oframe_t *)thread->sp;
    ac = frame->size / sizeof(oobject_t);
    list = thread->sp + sizeof(oframe_t);

    thread->run = 1;
    thread->xcpt = except_nothing;

    (*native)(list, ac);

    ovm_exit();
}

void
ovm_thread(oword_t type, oint8_t *ip, obool_t builtin, obool_t ret)
{
    GET_THREAD_SELF()
    oint8_t		*fp;
    pthread_attr_t	 attr;
    ortti_t		*rtti;
    othread_t		*thread;
    oframe_t		*cur_frame;
    oframe_t		*new_frame;

    assert(type > t_mpc && type <= rtti_vector->offset);
    rtti = rtti_vector->v.ptr[type];

    onew_thread(&thread_self->obj);
    thread = thread_self->obj;

    thread->ip = ip;
    thread->type = type;
    thread->frame = rtti->frame;
    thread->stack = rtti->stack;
    thread->ret = ret;
    onew_object((oobject_t *)&thread->bp, t_void, cfg_stack_size);

    cur_frame = (oframe_t *)thread_self->sp;
    cur_frame->next = null;
    fp = thread->bp + cfg_stack_size - sizeof(oframe_t);
    /* fake root frame to work as a gc marker */
    new_frame = (oframe_t *)fp;
    new_frame->type = t_root;
    thread->sp = fp - (thread->frame + cur_frame->size);
    new_frame->next = thread->sp;

    new_frame = (oframe_t *)thread->sp;

    memcpy(thread->sp + sizeof(oframe_t), thread_self->sp + sizeof(oframe_t),
	   thread->frame + cur_frame->size - sizeof(oframe_t));
    new_frame->type = thread->type;
    new_frame->size = cur_frame->size;
    new_frame->ret = jit_code_exit;

    /* Assigning fp must be done before zero'ing new thread stack and
     * "atomically" so that gc will now inspect the new thread stack */
    othreads_lock();
    thread->fp = fp;
    thread_self->r0.t = t_thread;
    thread_self->r0.v.o = thread;
    othreads_unlock();

    memset(thread_self->sp + sizeof(oframe_t), 0,
	   thread->frame + cur_frame->size - sizeof(oframe_t));

    pthread_attr_init(&attr);
    if (pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE))
	oerror("pthread_attr_setstacksize: %s", strerror(errno));
    pthread_create(&thread->pthread, &attr,
		   builtin ?
		   (void*(*)(void*))&vm_builtin : (void*(*)(void*))&ovm,
		   thread);
    pthread_attr_destroy(&attr);
}

void
ovm_exit(void)
{
    GET_THREAD_SELF()
    oword_t		ncpus;
    oword_t		offset;
    oword_t		length;

    /* lock thread linked lists */
    othreads_lock();

    /* Make sure return value, if any, is gc reachable */
    if (thread_self->ret) {
	switch (thread_self->r0.t) {
	    case t_void:	case t_word:	case t_float:	case t_rat:
	    case t_mpz:		case t_mpq:	case t_mpr:	case t_cdd:
	    case t_cqq:		case t_mpc:
		break;
	    default:
		thread_self->obj = thread_self->r0.v.o;
		break;
	}
    }
    else {
	thread_self->r0.t = t_void;
	thread_self->r0.v.o = null;
    }

    thread_self->run = 0;
    thread_self->fp = null;

#if defined(_SC_NPROCESSORS_ONLN)
    ncpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (ncpus < 0)
	ncpus = 16;
    else
	ncpus = (ncpus + 16) & -16;
#else
    ncpus = 16;
#endif

    /* Avoid filling thread_vector for long running programs spawning
     * several threads; only want to prevent race conditions of children
     * threads exiting after the main thread. */
    if (thread_vector->offset > ncpus) {
	offset = thread_vector->offset & -ncpus;
	length = thread_vector->offset - offset;
	memcpy(thread_vector->v.w,
	       thread_vector->v.w + offset, length * sizeof(oword_t));
	thread_vector->offset = length;
    }

    if (thread_self == thread_main) {
	/* thread_main is not reassigned */
	omutex_lock(&count_mutex);
	othreads_unlock();
	while (thread_main->next != thread_main)
	    /* wait for children threads to exit */
	    pthread_cond_wait(&count_cond, &count_mutex);

	/* mutex is implicitly released by pthread_cond_wait */
	/* but helgrind does not agree */
	omutex_unlock(&count_mutex);

	/* free mpfr per thread cache data */
	othreads_lock();
	mpfr_free_cache();

	/* Wait any thread not yet fully finished */
	for (offset = 0; offset < thread_vector->offset; offset++)
	    pthread_join(thread_vector->v.w[offset], null);
	thread_vector->offset = 0;

	othreads_unlock();
    }
    else {
	othread_t	*ptr;
	/* remove from thread circular list */
	for (ptr = thread_self->next; ptr->next != thread_self; ptr = ptr->next)
	    ;
	ptr->next = thread_self->next;
	omutex_lock(&count_mutex);
	if (ptr->next == ptr)
	    pthread_cond_signal(&count_cond);

	/* free mpfr per thread cache data */
	mpfr_free_cache();
	omutex_unlock(&count_mutex);

	if (thread_vector->offset >= thread_vector->length)
	    orenew_vector(thread_vector, thread_vector->length + 16);
	thread_vector->v.w[thread_vector->offset++] = thread_self->pthread;

	othreads_unlock();

	pthread_exit(null);
    }
}

void
ovm_raise(oint32_t xcpt)
{
    GET_THREAD_SELF()
    thread_self->r0.t = t_word;
    thread_self->r0.v.w = xcpt;
    thread_self->xcpt = xcpt;
    /* While information in the stack */
    search_instruction_pointer();
    siglongjmp(thread_self->env, 1);
}

oint32_t
ovm_get_shift(oregister_t *r)
{
    oint32_t		s;

    switch (r->t) {
	case t_void:
	    s = 0;
	    break;
	case t_word:
	    if (r->v.w < SHIFT_MIN || r->v.w > SHIFT_MAX)
		ovm_raise(except_shift_too_large);
	    s = r->v.w;
	    break;
	case t_float:
	    if ((oint32_t)r->v.d != r->v.d)
		ovm_raise(except_invalid_argument);
	    if (r->v.d < SHIFT_MIN || r->v.d > SHIFT_MAX)
		ovm_raise(except_shift_too_large);
	    s = r->v.d;
	    break;
	case t_mpr:
	    if (!mpfr_integer_p(orr(r)) || !mpfr_fits_sint_p(orr(r), GMP_RNDZ))
		ovm_raise(except_invalid_argument);
	    s = mpfr_get_si(orr(r), thr_rnd);
	    if (s < SHIFT_MIN || s > SHIFT_MAX)
		ovm_raise(except_shift_too_large);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    return (s);
}

void
ovm_case(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
#if __WORDSIZE == 64
	    if ((oint32_t)r->v.w != r->v.w)
		ovm_raise(except_not_a_32_bits_integer);
#endif
	    break;
	case t_float:
	    if ((oint32_t)r->v.d != r->v.d)
		ovm_raise(except_not_a_32_bits_integer);
	    r->t = t_word;
	    r->v.w = r->v.d;
	    break;
	case t_mpr:
	    if (!mpfr_integer_p(orr(r)) || !mpfr_fits_sint_p(orr(r), GMP_RNDZ))
		ovm_raise(except_not_a_32_bits_integer);
	    r->t = t_word;
	    r->v.w = mpfr_get_si(orr(r), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_32_bits_integer);
    }
}

void
ovm_offset(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    break;
	case t_float:
	    if ((oword_t)r->v.d != r->v.d)
		ovm_raise(except_not_an_integer);
	    r->t = t_word;
	    r->v.w = r->v.d;
	    break;
	case t_mpr:
	    if (!mpfr_integer_p(orr(r)) || !mpfr_fits_slong_p(orr(r), GMP_RNDZ))
		ovm_raise(except_not_an_integer);
	    r->t = t_word;
	    r->v.w = mpfr_get_si(orr(r), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
    if (unlikely(r->v.w < 0))
	ovm_raise(except_out_of_bounds);
}

void
ovm_not(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 1;
	    break;
	case t_word:
	    r->v.w = !r->v.w;
	    break;
	case t_float:
	    r->t = t_word;
	    r->v.w = !r->v.d;
	    break;
	case t_mpr:
	    r->t = t_word;
	    r->v.w = mpfr_zero_p(orr(r));
	    break;
	default:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
    }
}

void
ovm_bool(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    r->v.w = !!r->v.w;
	    break;
	case t_float:
	    r->t = t_word;
	    r->v.w = !!r->v.d;
	    break;
	case t_mpr:
	    r->t = t_word;
	    r->v.w = !mpfr_zero_p(orr(r));
	    break;
	default:
	    r->t = t_word;
	    r->v.w = 1;
	    break;
    }
}

void
ovm_com(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = -1;
	    break;
	case t_word:
	    r->v.w = ~r->v.w;
	    break;
	case t_mpz:
	    mpz_com(ozr(r), ozr(r));
	    if (unlikely(mpz_fits_slong_p(ozr(r)))) {
		r->t = t_word;
		r->v.w = mpz_get_si(ozr(r));
	    }
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_plus(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:	case t_float:
	case t_mpz:	case t_rat:
	case t_mpq:	case t_mpr:
	case t_cdd:	case t_cqq:
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_neg(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (likely(r->v.w != MININT))
		r->v.w = -r->v.w;
	    else {
		r->t = t_mpz;
		mpz_set_ui(ozr(r), MININT);
	    }
	    break;
	case t_float:
	    r->v.d = -r->v.d;
	    break;
	case t_mpz:
	    mpz_neg(ozr(r), ozr(r));
	    if (unlikely(mpz_fits_slong_p(ozr(r)))) {
		r->t = t_word;
		r->v.w = mpz_get_si(ozr(r));
	    }
	    break;
	case t_rat:
	    if (rat_num(r->v.r) == MININT) {
		r->t = t_mpq;
		mpq_set_ui(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpq_neg(oqr(r), oqr(r));
		mpq_canonicalize(oqr(r));
	    }
	    else
		rat_num(r->v.r) = -rat_num(r->v.r);
	    break;
	case t_mpq:
	    mpq_neg(oqr(r), oqr(r));
	    break;
	case t_mpr:
	    mpfr_neg(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    r->v.dd = -r->v.dd;
	    break;
	case t_cqq:
	    cqq_neg(oqq(r), oqq(r));
	    break;
	case t_mpc:
	    mpc_neg(occ(r), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_copy(oregister_t *reg, oobject_t o)
{
    if (o == null) {
	reg->t = t_void;
	reg->v.o = null;
    }
    else {
	switch (reg->t = otype(o)) {
	    case t_word:
		reg->v.w = *(oword_t *)o;
		break;
	    case t_float:
		reg->v.d = *(ofloat_t *)o;
		break;
	    case t_mpz:
		mpz_set(ozr(reg), (ompz_t)o);
		break;
	    case t_rat:
		reg->v.r = *(rat_t *)o;
		break;
	    case t_mpq:
		mpq_set(oqr(reg), (ompq_t)o);
		break;
	    case t_mpr:
		mpfr_set(orr(reg), (ompr_t)o, thr_rnd);
		break;
	    case t_cdd:
		reg->v.dd = *(ocdd_t *)o;
		break;
	    case t_cqq:
		cqq_set(oqq(reg), (ocqq_t)o);
		break;
	    case t_mpc:
		mpc_set(occ(reg), (ompc_t)o, thr_rndc);
		break;
	    default:
		reg->v.o = o;
		break;
	}
    }
}

void
ovm_load(oregister_t *reg, oobject_t *p)
{
    oobject_t		o = *p;
    if (o == null) {
	reg->t = t_void;
	reg->v.o = null;
    }
    else {
	switch (reg->t = otype(o)) {
	    case t_word:
		reg->v.w = *(oword_t *)o;
		break;
	    case t_float:
		reg->v.d = *(ofloat_t *)o;
		break;
	    case t_mpz:
		mpz_set(ozr(reg), (ompz_t)o);
		break;
	    case t_rat:
		reg->v.r = *(rat_t *)o;
		break;
	    case t_mpq:
		mpq_set(oqr(reg), (ompq_t)o);
		break;
	    case t_mpr:
		mpfr_set(orr(reg), (ompr_t)o, thr_rnd);
		break;
	    case t_cdd:
		reg->v.dd = *(ocdd_t *)o;
		break;
	    case t_cqq:
		cqq_set(oqq(reg), (ocqq_t)o);
		break;
	    case t_mpc:
		mpc_set(occ(reg), (ompc_t)o, thr_rndc);
		break;
	    default:
		reg->v.o = o;
		break;
	}
    }
}

void
ovm_store_i8(oregister_t *r, oint8_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = ofloat_get_w(r->v.d);
	    break;
	case t_mpz:
	    *o = ompz_get_w(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_si(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = ompq_get_w(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = ompr_get_w(orr(r));
	    break;
	case t_cdd:
	    *o = ocdd_get_w(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store_i16(oregister_t *r, oint16_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = ofloat_get_w(r->v.d);
	    break;
	case t_mpz:
	    *o = ompz_get_w(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_si(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = ompq_get_w(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = ompr_get_w(orr(r));
	    break;
	case t_cdd:
	    *o = ocdd_get_w(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store_i32(oregister_t *r, oint32_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = ofloat_get_w(r->v.d);
	    break;
	case t_mpz:
	    *o = ompz_get_w(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_si(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = ompq_get_w(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = ompr_get_w(orr(r));
	    break;
	case t_cdd:
	    *o = ocdd_get_w(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store_i64(oregister_t *r, oint64_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = ofloat_get_sl(r->v.d);
	    break;
	case t_mpz:
	    *o = ompz_get_sl(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_si(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = ompq_get_sl(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = ompr_get_sl(orr(r));
	    break;
	case t_cdd:
	    *o = ocdd_get_sl(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store_f32(oregister_t *r, ofloat32_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = r->v.d;
	    break;
	case t_mpz:
	    *o = mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_d(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = mpq_get_d(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = mpfr_get_d(orr(r), thr_rnd);
	    break;
	case t_cdd:
	    *o = real(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store_f64(oregister_t *r, ofloat64_t *o)
{
    switch (r->t) {
	case t_void:
	    *o = 0;
	    break;
	case t_word:
	    *o = r->v.w;
	    break;
	case t_float:
	    *o = r->v.d;
	    break;
	case t_mpz:
	    *o = mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    *o = rat_get_d(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    *o = mpq_get_d(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    *o = mpfr_get_d(orr(r), thr_rnd);
	    break;
	case t_cdd:
	    *o = real(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_store(oregister_t *reg, oobject_t *p, oint32_t t)
{
    oobject_t		o = *p;
    if (o && otype(o) == reg->t) {
	switch (reg->t) {
	    case t_void:
		*p = null;
		break;
	    case t_word:
		*(oword_t *)o = reg->v.w;
		break;
	    case t_float:
		*(ofloat_t *)o = reg->v.d;
		break;
	    case t_mpz:
		mpz_set((ompz_t)o, ozr(reg));
		break;
	    case t_rat:
		*(rat_t *)o = reg->v.r;
		break;
	    case t_mpq:
		mpq_set((ompq_t)o, oqr(reg));
		break;
	    case t_mpr:
		mpfr_set((ompr_t)o, orr(reg), thr_rnd);
		break;
	    case t_cdd:
		*(ocdd_t *)o = reg->v.dd;
		break;
	    case t_cqq:
		cqq_set((ocqq_t)o, oqq(reg));
		break;
	    case t_mpc:
		mpc_set((ompc_t)o, occ(reg), thr_rndc);
		break;
	    default:
		*p = reg->v.o;
		break;
	}
    }
    else {
	if (t != t_void && reg->t != t_void && t != reg->t) {
	    /* Do not allow mixing signed and unsigned integers?
	     * Should not cause a fault if allowing using a float32_t
	     * vector where an int32_t were expected (or vice versa)
	     * but for the sake of correctness do not allow it. */
	    ortti_t		*rtti;

	    if (t & t_vector) {
		if (!(reg->t & t_vector))
		    ovm_raise(except_invalid_argument);
		t &= ~t_vector;
		if (t < t_mpc)
		    ovm_raise(except_invalid_argument);
	    }
	    else {
		/* Derived classes can be used where a base class
		 * would be expected, due to language nature of
		 * only allowing single inheritance */
		while (t > t_mpc) {
		    /* t_hashtable or other special internal type */
		    if (t > rtti_vector->offset)
			ovm_raise(except_invalid_argument);
		    rtti = rtti_vector->v.ptr[t];
		    if (rtti->super == t)
			break;
		    t = rtti->super;
		}
		if (t == 0)
		    ovm_raise(except_invalid_argument);
	    }
	}
	switch (reg->t) {
	    case t_void:
		*p = null;
		break;
	    case t_word:
		onew_word(p, reg->v.w);
		break;
	    case t_float:
		onew_float(p, reg->v.d);
		break;
	    case t_mpz:
		onew_mpz(p, ozr(reg));
		break;
	    case t_rat:
		onew_rat(p, &reg->v.r);
		break;
	    case t_mpq:
		onew_mpq(p, oqr(reg));
		break;
	    case t_mpr:
		onew_mpr(p, orr(reg));
		break;
	    case t_cdd:
		onew_cdd(p, reg->v.dd);
		break;
	    case t_cqq:
		onew_cqq(p, oqq(reg));
		break;
	    case t_mpc:
		onew_mpc(p, occ(reg));
		break;
	    default:
		*p = reg->v.o;
		break;
	}
    }
}

void
ovm_move(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.o = null;
	    break;
	case t_word:
	    l->v.w = r->v.w;
	    break;
	case t_float:
	    l->v.d = r->v.d;
	    break;
	case t_mpz:
	    mpz_set(ozr(l), ozr(r));
	    break;
	case t_rat:
	    l->v.r = r->v.r;
	    break;
	case t_mpq:
	    mpq_set(oqr(l), oqr(r));
	    break;
	case t_mpr:
	    mpfr_set(orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->v.dd = r->v.dd;
	    break;
	case t_cqq:
	    cqq_set(oqq(l), oqq(r));
	    break;
	case t_mpc:
	    mpc_set(occ(l), occ(r), thr_rndc);
	    break;
	default:
	    l->v.o = r->v.o;
	    break;
    }
    l->t = r->t;
}

static ohashentry_t *
hash_test(ohashtable_t *hash, ohashentry_t *test, oregister_t *r)
{
    switch ((test->nt = r->t)) {
	case t_void:
	    test->nv.o = null;
	    return (null);
	case t_word:
	    test->nv.w = r->v.w;
	    break;
	case t_float:
	    test->nv.d = r->v.d;
	    break;
	case t_mpz:
	    test->nv.z = ozr(r);
	    break;
	case t_rat:
	    test->nv.x = r->v.r;
	    break;
	case t_mpq:
	    test->nv.q = oqr(r);
	    break;
	case t_mpr:
	    test->nv.r = orr(r);
	    break;
	case t_cdd:
	    test->nv.dd = r->v.dd;
	    break;
	case t_cqq:
	    test->nv.qq = oqq(r);
	    break;
	case t_mpc:
	    test->nv.cc = occ(r);
	    break;
	default:
	    test->nv.o = r->v.o;
	    break;
    }
    okey_hashentry(test);
    if ((r->v.o = oget_hashentry(hash, test)))
	r->t = t_hashentry;
    else
	r->t = t_void;
    return (r->v.o);
}

static ohashentry_t *
hash_test_create(ohashtable_t *hash, ohashentry_t *test, oregister_t *r)
{
    GET_THREAD_SELF()
    ohashentry_t	*entry;

    if ((entry = hash_test(hash, test, r)) == null) {
	if (test->nt == t_void)
	    return (null);
	onew_object(&thread_self->obj, t_hashentry, sizeof(ohashentry_t));
	entry = thread_self->obj;
	switch (test->nt) {
	    case t_word:
		entry->nt = t_word;
		entry->nv.w = test->nv.w;
		break;
	    case t_float:
		entry->nt = t_float;
		entry->nv.d = test->nv.d;
		break;
	    case t_mpz:
		onew_mpz(&entry->nv.o, test->nv.z);
		entry->nt = t_mpz;
		break;
	    case t_rat:
		entry->nt = t_rat;
		entry->nv.x = test->nv.x;
		break;
	    case t_mpq:
		onew_mpq(&entry->nv.o, test->nv.q);
		entry->nt = t_mpq;
		break;
	    case t_mpr:
		onew_mpr(&entry->nv.o, test->nv.r);
		entry->nt = t_mpr;
		break;
	    case t_cdd:
		entry->nt = t_cdd;
		entry->nv.dd = test->nv.dd;
		break;
	    case t_cqq:
		onew_cqq(&entry->nv.o, test->nv.qq);
		entry->nt = t_cqq;
		break;
	    case t_mpc:
		onew_mpc(&entry->nv.o, test->nv.cc);
		entry->nt = t_mpc;
		break;
	    case t_string:
		onew_vector(&entry->nv.o, t_uint8, test->nv.v->length);
		memcpy(entry->nv.v->v.u8, test->nv.v->v.u8, test->nv.v->length);
		entry->nt = t_string;
		break;
	    default:
		test->nv.o = r->v.o;
		entry->nt = test->nt;
		break;
	}
	entry->key = test->key;
	oput_hashentry(hash, entry);
    }
    return (entry);
}

static void
entry_load(ohashentry_t *entry, oregister_t *r)
{
    switch ((r->t = entry->vt)) {
	case t_word:
	    r->v.w = entry->vv.w;
	    break;
	case t_float:
	    r->v.d = entry->vv.d;
	    break;
	case t_mpz:
	    mpz_set(ozr(r), entry->vv.z);
	    break;
	case t_rat:
	    r->v.r = entry->vv.x;
	    break;
	case t_mpq:
	    mpq_set(oqr(r), entry->vv.q);
	    break;
	case t_mpr:
	    mpfr_set(orr(r), entry->vv.r, thr_rnd);
	    break;
	case t_cdd:
	    r->v.dd = entry->vv.dd;
	    break;
	case t_cqq:
	    cqq_set(oqq(r), entry->vv.qq);
	    break;
	case t_mpc:
	    mpc_set(occ(r), entry->vv.cc, thr_rndc);
	    break;
	default:
	    r->v.o = entry->vv.o;
	    break;
    }
}

static void
entry_store(ohashentry_t *entry, oregister_t *r)
{
    GET_THREAD_SELF()
    switch (r->t) {
	case t_word:
	    entry->vt = t_word;
	    entry->vv.w = r->v.w;
	    break;
	case t_float:
	    entry->vt = t_float;
	    entry->vv.d = r->v.d;
	    break;
	case t_mpz:
	    if (entry->vt == t_mpz)
		mpz_set(entry->vv.z, ozr(r));
	    else {
		onew_mpz(&thread_self->obj, ozr(r));
		entry->vv.z = thread_self->obj;
		entry->vt = t_mpz;
	    }
	    break;
	case t_rat:
	    entry->vt = t_rat;
	    entry->vv.x = r->v.r;
	    break;
	case t_mpq:
	    if (entry->vt == t_mpz)
		mpq_set(entry->vv.q, oqr(r));
	    else {
		onew_mpq(&thread_self->obj, oqr(r));
		entry->vv.q = thread_self->obj;
		entry->vt = t_mpq;
	    }
	    break;
	case t_mpr:
	    if (entry->vt == t_mpr)
		mpfr_set(entry->vv.r, orr(r), thr_rnd);
	    else {
		onew_mpr(&thread_self->obj, orr(r));
		entry->vv.r = thread_self->obj;
		entry->vt = t_mpr;
	    }
	    break;
	case t_cdd:
	    entry->vv.dd = r->v.dd;
	    break;
	case t_cqq:
	    if (entry->vt == t_mpr)
		cqq_set(entry->vv.qq, oqq(r));
	    else {
		onew_cqq(&thread_self->obj, oqq(r));
		entry->vv.qq = thread_self->obj;
		entry->vt = t_cqq;
	    }
	    break;
	case t_mpc:
	    if (entry->vt == t_mpr)
		mpc_set(entry->vv.cc, occ(r), thr_rndc);
	    else {
		onew_mpc(&thread_self->obj, occ(r));
		entry->vv.cc = thread_self->obj;
		entry->vt = t_mpc;
	    }
	    break;
	default:
	    entry->vv.o = r->v.o;
	    entry->vt = t_rat;
	    break;
    }
}

void
ovm_gethash(oregister_t *b, oregister_t *l, oregister_t *r)
{
    ohashentry_t	 test;
    ohashentry_t	*entry;

    if (unlikely(b->t == t_void || l->t == t_void))
	ovm_raise(except_null_dereference);
    if (unlikely(b->t != t_hashtable))
	ovm_raise(except_invalid_argument);
    if ((entry = hash_test(b->v.o, &test, l)))
	entry_load(entry, r);
    else {
	r->t = t_void;
	r->v.o = null;
    }
}

void
ovm_puthash(oregister_t *b, oregister_t *l, oregister_t *r)
{
    ohashentry_t	 test;
    ohashentry_t	*entry;

    if (unlikely(b->t == t_void || l->t == t_void))
	ovm_raise(except_null_dereference);
    if (unlikely(b->t != t_hashtable))
	ovm_raise(except_invalid_argument);
    if (r->t == t_void) {
	if ((entry = hash_test(b->v.o, &test, l)))
	    orem_hashentry(b->v.o, entry);
    }
    else
	entry_store(hash_test_create(b->v.o, &test, l), r);
}

void
ovm_putentry(oregister_t *l, oregister_t *r)
{
    if (unlikely(l->t == t_void))
	ovm_raise(except_null_dereference);
    if (unlikely(l->t != t_hashentry))
	ovm_raise(except_invalid_argument);
    entry_store(l->v.o, r);
}

void
ovm_inc(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 1;
	    break;
	case t_word:
	    if (likely(r->v.w < SGN_MASK))
		++r->v.w;
	    else {
		r->t = t_mpz;
		mpz_set_si(ozr(r), r->v.w);
		mpz_add_ui(ozr(r), ozr(r), 1);
	    }
	    break;
	case t_float:
	    r->v.d += 1;
	    break;
	case t_mpz:
	    mpz_add_ui(ozr(r), ozr(r), 1);
	    if (mpz_fits_slong_p(ozr(r))) {
		r->t = t_word;
		r->v.w = mpz_get_si(ozr(r));
	    }
	    break;
	case t_rat:
	    if (likely(rat_num(r->v.r) <= SGN_MASK - rat_den(r->v.r))) {
		rat_num(r->v.r) += rat_den(r->v.r);
		break;
	    }
	    r->t = t_mpq;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:		case t_cqq:
	    mpz_add(ozr(r), ozr(r), ozs(r));
	    break;
	case t_mpr:		case t_mpc:
	    mpfr_add_ui(orr(r), orr(r), 1, thr_rnd);
	    break;
	case t_cdd:
	    r->v.dd += 1;
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dec(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = -1;
	    break;
	case t_word:
	    if (likely(r->v.w > -SGN_MASK))
		--r->v.w;
	    else {
		r->t = t_mpz;
		mpz_set_si(ozr(r), r->v.w);
		mpz_sub_ui(ozr(r), ozr(r), 1);
	    }
	    break;
	case t_float:
	    r->v.d -= 1;
	    break;
	case t_mpz:
	    mpz_sub_ui(ozr(r), ozr(r), 1);
	    if (mpz_fits_slong_p(ozr(r))) {
		r->t = t_word;
		r->v.w = mpz_get_si(ozr(r));
	    }
	    break;
	case t_rat:
	    if (likely(rat_num(r->v.r) >= -(SGN_MASK - rat_den(r->v.r)))) {
		rat_num(r->v.r) -= rat_den(r->v.r);
		break;
	    }
	    r->t = t_mpq;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:		case t_cqq:
	    mpz_sub(ozr(r), ozr(r), ozs(r));
	    break;
	case t_mpr:		case t_mpc:
	    mpfr_sub_ui(orr(r), orr(r), 1, thr_rnd);
	    break;
	case t_cdd:
	    r->v.dd -= 1;
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_lt(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_lt(l, r);
	    break;
	case t_word:
	    ovm_w_lt(l, r);
	    break;
	case t_float:
	    ovm_d_lt(l, r);
	    break;
	case t_mpz:
	    ovm_z_lt(l, r);
	    break;
	case t_rat:
	    ovm_x_lt(l, r);
	    break;
	case t_mpq:
	    ovm_q_lt(l, r);
	    break;
	case t_mpr:
	    ovm_r_lt(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_le(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_le(l, r);
	    break;
	case t_word:
	    ovm_w_le(l, r);
	    break;
	case t_float:
	    ovm_d_le(l, r);
	    break;
	case t_mpz:
	    ovm_z_le(l, r);
	    break;
	case t_rat:
	    ovm_x_le(l, r);
	    break;
	case t_mpq:
	    ovm_q_le(l, r);
	    break;
	case t_mpr:
	    ovm_r_le(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_eq(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_eq(l, r);
	    break;
	case t_word:
	    ovm_w_eq(l, r);
	    break;
	case t_float:
	    ovm_d_eq(l, r);
	    break;
	case t_mpz:
	    ovm_z_eq(l, r);
	    break;
	case t_rat:
	    ovm_x_eq(l, r);
	    break;
	case t_mpq:
	    ovm_q_eq(l, r);
	    break;
	case t_mpr:
	    ovm_r_eq(l, r);
	    break;
	case t_cdd:
	    ovm_dd_eq(l, r);
	    break;
	case t_cqq:
	    ovm_qq_eq(l, r);
	    break;
	case t_mpc:
	    ovm_cc_eq(l, r);
	    break;
	default:
	    if (l->v.o == r->v.o)
		l->v.w = 1;
	    else if (l->t == r->t && l->t == t_string &&
		     l->v.v->length == r->v.v->length)
		l->v.w = memcmp(l->v.v->v.u8,
				r->v.v->v.u8, l->v.v->length) == 0;
	    else
		l->v.w = 0;
	    l->t = t_word;
	    break;
    }
}

void
ovm_o_ge(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_ge(l, r);
	    break;
	case t_word:
	    ovm_w_ge(l, r);
	    break;
	case t_float:
	    ovm_d_ge(l, r);
	    break;
	case t_mpz:
	    ovm_z_ge(l, r);
	    break;
	case t_rat:
	    ovm_x_ge(l, r);
	    break;
	case t_mpq:
	    ovm_q_ge(l, r);
	    break;
	case t_mpr:
	    ovm_r_ge(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_gt(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_gt(l, r);
	    break;
	case t_word:
	    ovm_w_gt(l, r);
	    break;
	case t_float:
	    ovm_d_gt(l, r);
	    break;
	case t_mpz:
	    ovm_z_gt(l, r);
	    break;
	case t_rat:
	    ovm_x_gt(l, r);
	    break;
	case t_mpq:
	    ovm_q_gt(l, r);
	    break;
	case t_mpr:
	    ovm_r_gt(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_ne(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_ne(l, r);
	    break;
	case t_word:
	    ovm_w_ne(l, r);
	    break;
	case t_float:
	    ovm_d_ne(l, r);
	    break;
	case t_mpz:
	    ovm_z_ne(l, r);
	    break;
	case t_rat:
	    ovm_x_ne(l, r);
	    break;
	case t_mpq:
	    ovm_q_ne(l, r);
	    break;
	case t_mpr:
	    ovm_r_ne(l, r);
	    break;
	case t_cdd:
	    ovm_dd_ne(l, r);
	    break;
	case t_cqq:
	    ovm_qq_ne(l, r);
	    break;
	case t_mpc:
	    ovm_cc_ne(l, r);
	    break;
	default:
	    if (l->v.o == r->v.o)
		l->v.w = 0;
	    else if (l->t == r->t && l->t == t_string &&
		     l->v.v->length == r->v.v->length)
		l->v.w = memcmp(l->v.v->v.u8,
				r->v.v->v.u8, l->v.v->length) != 0;
	    else
		l->v.w = 1;
	    l->t = t_word;
	    break;
    }
}

void
ovm_o_and(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_and(l, r);
	    break;
	case t_word:
	    ovm_w_and(l, r);
	    break;
	case t_mpz:
	    ovm_z_and(l, r);
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_o_or(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_or(l, r);
	    break;
	case t_word:
	    ovm_w_or(l, r);
	    break;
	case t_mpz:
	    ovm_z_or(l, r);
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_o_xor(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_xor(l, r);
	    break;
	case t_word:
	    ovm_w_xor(l, r);
	    break;
	case t_mpz:
	    ovm_z_xor(l, r);
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_o_mul2(oregister_t *l, oregister_t *r)
{
    oint32_t		s;

    s = ovm_get_shift(r);
    if (s >= 0)
	ovm_mul2exp(l, s);
    else
	ovm_div2exp(l, -s);
}

void
ovm_mul2exp(oregister_t *r, oint32_t s)
{
    oword_t		w;

    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (s < __WORDSIZE) {
		w = r->v.w << s;
		if (w != MININT && w >> s == r->v.w) {
		    r->v.w = w;
		    break;
		}
	    }
	    r->t = t_mpz;
	    mpz_set_si(ozr(r), r->v.w);
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    break;
	case t_float:
	    r->v.d = ldexp(r->v.d, s);
	    break;
	case t_mpz:
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    break;
	case t_rat:
	    if (s < __WORDSIZE) {
		w = rat_num(r->v.r) << s;
		if (likely(w != MININT && w >> s == rat_num(r->v.r))) {
		    rat_num(r->v.r) = w;
		    orat_canonicalize(&r->v.r);
		    check_rat(r);
		    break;
		}
	    }
	    r->t = t_mpq;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpq_mul_2exp(oqr(r), oqr(r), s);
	    check_mpq(r);
	    break;
	case t_mpr:
	    mpfr_mul_2exp(orr(r), orr(r), s, thr_rnd);
	    break;
	case t_cdd:
	    real(r->v.dd) = ldexp(real(r->v.dd), s);
	    imag(r->v.dd) = ldexp(imag(r->v.dd), s);
	    break;
	case t_cqq:
	    cqq_mul_2exp(oqq(r), oqq(r), s);
	    check_cqq(r);
	    break;
	case t_mpc:
	    mpc_mul_2exp(occ(r), occ(r), s, thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_div2(oregister_t *l, oregister_t *r)
{
    oint32_t		s;

    s = ovm_get_shift(r);
    if (s >= 0)
	ovm_div2exp(l, s);
    else
	ovm_mul2exp(l, -s);
}

void
ovm_div2exp(oregister_t *r, oint32_t s)
{
    oword_t		w;

    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (s < __WORDSIZE) {
		w = r->v.w >> s;
		if (w << s == r->v.w) {
		    r->v.w = w;
		    break;
		}
	    }
	    r->t = t_mpq;
	    mpq_set_si(oqr(r), r->v.w, 1);
	    mpq_div_2exp(oqr(r), oqr(r), s);
	    break;
	case t_float:
	    r->v.d = ldexp(r->v.d, -s);
	    break;
	case t_mpz:
	    r->t = t_mpq;
	    mpz_set_ui(ozs(r), 1);
	    goto mpq;
	case t_rat:
	    if (s < __WORDSIZE) {
		w = rat_den(r->v.r) >> s;
		if (likely(w << s == rat_den(r->v.r))) {
		    rat_den(r->v.r) = w;
		    orat_canonicalize(&r->v.r);
		    check_rat(r);
		    break;
		}
	    }
	    r->t = t_mpq;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	mpq:
	    mpq_div_2exp(oqr(r), oqr(r), s);
	    check_mpq(r);
	    break;
	case t_mpr:
	    mpfr_div_2exp(orr(r), orr(r), s, thr_rnd);
	    break;
	case t_cdd:
	    real(r->v.dd) = ldexp(real(r->v.dd), -s);
	    imag(r->v.dd) = ldexp(imag(r->v.dd), -s);
	    break;
	case t_cqq:
	    cqq_div_2exp(oqq(r), oqq(r), s);
	    check_cqq(r);
	    break;
	case t_mpc:
	    mpc_div_2exp(occ(r), occ(r), s, thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_shl(oregister_t *l, oregister_t *r)
{
    oint32_t		s;

    s = ovm_get_shift(r);
    if (s >= 0)
	ovm_shl(l, s);
    else
	ovm_shr(l, -s);
}

void
ovm_shl(oregister_t *r, oint32_t s)
{
    GET_THREAD_SELF()
    oword_t		w;

    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (s < __WORDSIZE) {
		w = r->v.w << s;
		if (w != MININT && w >> s == r->v.w) {
		    r->v.w = w;
		    break;
		}
	    }
	    r->t = t_mpz;
	    mpz_set_si(ozr(r), r->v.w);
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    break;
	case t_float:
	    r->t = t_mpz;
	    mpz_set_d(ozr(r), floor(r->v.d));
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    check_mpz(r);
	    break;
	case t_mpz:
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    r->t = t_mpz;
	    mpz_fdiv_q(ozr(r), ozr(r), ozs(r));
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_floor(orr(r), orr(r));
	    if (unlikely(!mpfr_number_p(orr(r))))
		othread_kill(SIGFPE);
	    mpfr_get_z(ozr(r), orr(r), GMP_RNDZ);
	    mpz_mul_2exp(ozr(r), ozr(r), s);
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_shr(oregister_t *l, oregister_t *r)
{
    oint32_t		s;

    s = ovm_get_shift(r);
    if (s >= 0)
	ovm_shr(l, s);
    else
	ovm_shl(l, -s);
}

void
ovm_shr(oregister_t *r, oint32_t s)
{
    GET_THREAD_SELF()
    ofloat_t		d;

    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (s < __WORDSIZE)
		r->v.w >>= s;
	    else
		r->v.w >>= __WORDSIZE - 1;
	    break;
	case t_float:
	    d = floor(ldexp(r->v.d, -s));
	    if ((oword_t)d == d) {
		r->t = t_word;
		r->v.w = d;
	    }
	    else {
		r->t = t_mpz;
		mpz_set_d(ozr(r), r->v.d);
		mpz_fdiv_q_2exp(ozr(r), ozr(r), s);
		check_mpz(r);
	    }
	    break;
	case t_mpz:
	    mpz_fdiv_q_2exp(ozr(r), ozr(r), s);
	    check_mpz(r);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    r->t = t_mpz;
	    mpz_fdiv_q(ozr(r), ozr(r), ozs(r));
	    mpz_fdiv_q_2exp(ozr(r), ozr(r), s);
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_div_2exp(orr(r), orr(r), s, thr_rnd);
	    mpfr_floor(orr(r), orr(r));
	    if (unlikely(!mpfr_number_p(orr(r))))
		othread_kill(SIGFPE);
	    mpfr_get_z(ozr(r), orr(r), GMP_RNDZ);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_add(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_add(l, r);
	    break;
	case t_word:
	    ovm_w_add(l, r);
	    break;
	case t_float:
	    ovm_d_add(l, r);
	    break;
	case t_mpz:
	    ovm_z_add(l, r);
	    break;
	case t_rat:
	    ovm_x_add(l, r);
	    break;
	case t_mpq:
	    ovm_q_add(l, r);
	    break;
	case t_mpr:
	    ovm_r_add(l, r);
	    break;
	case t_cdd:
	    ovm_dd_add(l, r);
	    break;
	case t_cqq:
	    ovm_qq_add(l, r);
	    break;
	case t_mpc:
	    ovm_cc_add(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_sub(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_sub(l, r);
	    break;
	case t_word:
	    ovm_w_sub(l, r);
	    break;
	case t_float:
	    ovm_d_sub(l, r);
	    break;
	case t_mpz:
	    ovm_z_sub(l, r);
	    break;
	case t_rat:
	    ovm_x_sub(l, r);
	    break;
	case t_mpq:
	    ovm_q_sub(l, r);
	    break;
	case t_mpr:
	    ovm_r_sub(l, r);
	    break;
	case t_cdd:
	    ovm_dd_sub(l, r);
	    break;
	case t_cqq:
	    ovm_qq_sub(l, r);
	    break;
	case t_mpc:
	    ovm_cc_sub(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_mul(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_mul(l, r);
	    break;
	case t_word:
	    ovm_w_mul(l, r);
	    break;
	case t_float:
	    ovm_d_mul(l, r);
	    break;
	case t_mpz:
	    ovm_z_mul(l, r);
	    break;
	case t_rat:
	    ovm_x_mul(l, r);
	    break;
	case t_mpq:
	    ovm_q_mul(l, r);
	    break;
	case t_mpr:
	    ovm_r_mul(l, r);
	    break;
	case t_cdd:
	    ovm_dd_mul(l, r);
	    break;
	case t_cqq:
	    ovm_qq_mul(l, r);
	    break;
	case t_mpc:
	    ovm_cc_mul(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_div(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_div(l, r);
	    break;
	case t_word:
	    ovm_w_div(l, r);
	    break;
	case t_float:
	    ovm_d_div(l, r);
	    break;
	case t_mpz:
	    ovm_z_div(l, r);
	    break;
	case t_rat:
	    ovm_x_div(l, r);
	    break;
	case t_mpq:
	    ovm_q_div(l, r);
	    break;
	case t_mpr:
	    ovm_r_div(l, r);
	    break;
	case t_cdd:
	    ovm_dd_div(l, r);
	    break;
	case t_cqq:
	    ovm_qq_div(l, r);
	    break;
	case t_mpc:
	    ovm_cc_div(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_trunc2(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_trunc2(l, r);
	    break;
	case t_word:
	    ovm_w_trunc2(l, r);
	    break;
	case t_float:
	    ovm_d_trunc2(l, r);
	    break;
	case t_mpz:
	    ovm_z_trunc2(l, r);
	    break;
	case t_rat:
	    ovm_x_trunc2(l, r);
	    break;
	case t_mpq:
	    ovm_q_trunc2(l, r);
	    break;
	case t_mpr:
	    ovm_r_trunc2(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_rem(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_rem(l, r);
	    break;
	case t_word:
	    ovm_w_rem(l, r);
	    break;
	case t_float:
	    ovm_d_rem(l, r);
	    break;
	case t_mpz:
	    ovm_z_rem(l, r);
	    break;
	case t_rat:
	    ovm_x_rem(l, r);
	    break;
	case t_mpq:
	    ovm_q_rem(l, r);
	    break;
	case t_mpr:
	    ovm_r_rem(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_o_complex(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_complex(l, r);
	    break;
	case t_word:
	    ovm_w_complex(l, r);
	    break;
	case t_float:
	    ovm_d_complex(l, r);
	    break;
	case t_rat:
	    ovm_x_complex(l, r);
	    break;
	case t_mpz:
	    ovm_z_complex(l, r);
	    break;
	case t_mpq:
	    ovm_q_complex(l, r);
	    break;
	case t_mpr:
	    ovm_r_complex(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_o_atan2(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_atan2(l, r);
	    break;
	case t_word:
	    ovm_w_atan2(l, r);
	    break;
	case t_float:
	    ovm_d_atan2(l, r);
	    break;
	case t_mpz:
	    ovm_z_atan2(l, r);
	    break;
	case t_rat:
	    ovm_x_atan2(l, r);
	    break;
	case t_mpq:
	    ovm_q_atan2(l, r);
	    break;
	case t_mpr:
	    ovm_r_atan2(l, r);
	    break;
	case t_cdd:
	    ovm_dd_atan2(l, r);
	    break;
	case t_cqq:
	    ovm_qq_atan2(l, r);
	    break;
	case t_mpc:
	    ovm_cc_atan2(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_pow(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_pow(l, r);
	    break;
	case t_word:
	    ovm_w_pow(l, r);
	    break;
	case t_float:
	    ovm_d_pow(l, r);
	    break;
	case t_mpz:
	    ovm_z_pow(l, r);
	    break;
	case t_rat:
	    ovm_x_pow(l, r);
	    break;
	case t_mpq:
	    ovm_q_pow(l, r);
	    break;
	case t_mpr:
	    ovm_r_pow(l, r);
	    break;
	case t_cdd:
	    ovm_dd_pow(l, r);
	    break;
	case t_cqq:
	    ovm_qq_pow(l, r);
	    break;
	case t_mpc:
	    ovm_cc_pow(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_o_hypot(oregister_t *l, oregister_t *r)
{
    switch (l->t) {
	case t_void:
	    ovm_v_hypot(l, r);
	    break;
	case t_word:
	    ovm_w_hypot(l, r);
	    break;
	case t_float:
	    ovm_d_hypot(l, r);
	    break;
	case t_mpz:
	    ovm_z_hypot(l, r);
	    break;
	case t_rat:
	    ovm_x_hypot(l, r);
	    break;
	case t_mpq:
	    ovm_q_hypot(l, r);
	    break;
	case t_mpr:
	    ovm_r_hypot(l, r);
	    break;
	case t_cdd:
	    ovm_dd_hypot(l, r);
	    break;
	case t_cqq:
	    ovm_qq_hypot(l, r);
	    break;
	case t_mpc:
	    ovm_cc_hypot(l, r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_integer_p(oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:
	case t_mpz:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_rational_p(oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:
	case t_mpz:	case t_rat:
	case t_mpq:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_float_p(oregister_t *r)
{
    switch (r->t) {
	case t_float:	case t_mpr:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_real_p(oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:
	case t_float:	case t_mpz:
	case t_rat:	case t_mpq:
	case t_mpr:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_complex_p(oregister_t *r)
{
    switch (r->t) {
	case t_cdd:	case t_cqq:
	case t_mpc:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_number_p(oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:
	case t_float:	case t_mpz:
	case t_rat:	case t_mpq:
	case t_mpr:	case t_cdd:
	case t_cqq:	case t_mpc:
	    r->v.w = 1;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_finite_p(oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:
	case t_mpz:	case t_rat:
	case t_mpq:
	    r->v.w = 1;
	    break;
	case t_float:
	    r->v.w = finite(r->v.d) ? 1 : 0;
	    break;
	case t_mpr:
	    r->v.w = mpfr_number_p(orr(r)) ? 1 : 0;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_inf_p(oregister_t *r)
{
    switch (r->t) {
	case t_float:
	    r->v.w = isinf(r->v.d) ? signbit(r->v.d) ? -1 : 1 : 0;
	    break;
	case t_mpr:
	    r->v.w = mpfr_inf_p(orr(r)) ? mpfr_signbit(orr(r)) ? -1 : 1 : 0;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_nan_p(oregister_t *r)
{
    switch (r->t) {
	case t_float:
	    r->v.w = isnan(r->v.d) ? 1 : 0;
	    break;
	case t_mpr:
	    r->v.w = mpfr_nan_p(orr(r)) ? 1 : 0;
	    break;
	default:
	    r->v.w = 0;
	    break;
    }
    r->t = t_word;
}

void
ovm_num(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	    break;
	case t_rat:
	    r->t = t_word;
	    r->v.w = rat_num(r->v.r);
	    break;
	case t_mpq:
	    r->t = t_mpz;
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_rational_number);
	    break;
    }
}

void
ovm_den(oregister_t *r)
{
    switch (r->t) {
	case t_void:		case t_word:
	case t_mpz:
	    r->t = t_word;
	    r->v.w = 1;
	    break;
	case t_rat:
	    r->t = t_word;
	    r->v.w = rat_den(r->v.r);
	    break;
	case t_mpq:
	    r->t = t_mpz;
	    mpz_swap(ozr(r), ozs(r));
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_rational_number);
	    break;
    }
}

void
ovm_real(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_float:
	case t_mpz:		case t_rat:
	case t_mpq:		case t_mpr:
	    break;
	case t_cdd:
	    r->t = t_float;
	    break;
	case t_cqq:
	    r->t = t_mpq;
	    check_mpq(r);
	    break;
	case t_mpc:
	    r->t = t_mpr;
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_imag(oregister_t *r)
{
    switch (r->t) {
	case t_void:		case t_word:
	case t_float:		case t_mpz:
	case t_rat:		case t_mpq:
	case t_mpr:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_cdd:
	    r->t = t_float;
	    r->v.d = imag(r->v.dd);
	    break;
	case t_cqq:
	    r->t = t_mpq;
	    mpq_swap(oqr(r), oqi(r));
	    check_mpq(r);
	    break;
	case t_mpc:
	    r->t = t_mpr;
	    mpfr_swap(orr(r), ori(r));
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_signbit(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->v.w = 0;
	    break;
	case t_word:
	    r->v.w = r->v.w < 0;
	    break;
	case t_float:
	    r->v.w = signbit(r->v.d) != 0;
	    break;
	case t_rat:
	    r->v.w = rat_sgn(r->v.r) < 0;
	    break;
	case t_mpz:
	    r->v.w = mpz_sgn(ozr(r)) < 0;
	    break;
	case t_mpq:
	    r->v.w = mpq_sgn(oqr(r)) < 0;
	    break;
	case t_mpr:
	    r->v.w = mpfr_signbit(orr(r)) != 0;
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
    r->t = t_word;
}

void
ovm_abs(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    if (likely(r->v.w != MININT)) {
		if (r->v.w < 0)
		    r->v.w = -r->v.w;
	    }
	    else {
		mpz_set_ui(ozr(r), MININT);
		r->t = t_mpz;
	    }
	    break;
	case t_float:
	    r->v.d = fabs(r->v.d);
	    break;
	case t_rat:
	    if (likely(rat_num(r->v.r) != MININT)) {
		if (r->v.w < 0)
		    rat_num(r->v.r) = -rat_num(r->v.r);
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		r->t = t_mpq;
	    }
	    break;
	case t_mpz:
	    mpz_abs(ozr(r), ozr(r));
	    check_mpz(r);
	    break;
	case t_mpq:
	    mpq_abs(oqr(r), oqr(r));
	    check_mpq(r);
	    break;
	case t_mpr:
	    mpfr_abs(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    r->t = t_float;
	    r->v.d = cabs(r->v.dd);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		r->t = t_float;
		r->v.d = cabs(r->v.dd);
	    }
	    else {
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		mpc_abs(orr(r), occ(r), thr_rndc);
		r->t = t_mpr;
	    }
	    break;
	case t_mpc:
	    mpc_abs(orr(r), occ(r), thr_rndc);
	    r->t = t_mpr;
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_signum(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:
	    r->v.w = r->v.w ? r->v.w > 0 ? 1 : -1 : 0;
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d)))
		r->v.d = r->v.d ? r->v.d > 0.0 ? 1.0 : -1.0 : 0.0;
	    break;
	case t_mpz:
	    r->t = t_word;
	    r->v.w = mpz_sgn(ozr(r));
	    break;
	case t_rat:
	    r->t = t_word;
	    if (rat_num(r->v.r))
		r->v.w = rat_num(r->v.r) > 0 ? 1 : -1;
	    else
		r->v.w = 0;
	    break;
	case t_mpq:
	    r->t = t_word;
	    r->v.w = mpq_sgn(oqr(r));
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r))))
		mpfr_set_si(orr(r), mpfr_sgn(orr(r)), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd /= cabs(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		r->t = t_cdd;
		goto cdd;
	    }
	    else {
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		r->t = t_mpc;
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpfr_hypot(thr_rr, orr(r), ori(r), thr_rnd);
	    mpfr_div(orr(r), orr(r), thr_rr, thr_rnd);
	    mpfr_div(ori(r), ori(r), thr_rr, thr_rnd);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_rational(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	case t_rat:		case t_mpq:
	    break;
	case t_float:
	    if (unlikely(!finite(r->v.d)))
		ovm_raise(except_not_a_finite_real_number);
	    r->t = t_mpq;
	    mpq_set_d(oqr(r), r->v.d);
	    check_mpq(r);
	    break;
	case t_mpr:
	    if (unlikely(!mpfr_number_p(orr(r))))
		ovm_raise(except_not_a_finite_real_number);
	    r->t = t_mpq;
	    mpfr_get_f(thr_f, orr(r), thr_rnd);
	    mpq_set_f(oqr(r), thr_f);
	    check_mpq(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_arg(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = r->v.w >= 0 ? 0.0 : M_PI;
	    }
	    else {
		r->t = t_mpr;
		if (r->v.w >= 0)
		    mpfr_set_ui(orr(r), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d)))
		r->v.d = r->v.d >= 0.0 ? 0.0 : M_PI;
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = mpz_sgn(ozr(r)) >= 0 ? 0.0 : M_PI;
	    }
	    else {
		r->t = t_mpr;
		if (mpz_sgn(ozr(r)) >= 0)
		    mpfr_set_ui(orr(r), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = rat_num(r->v.r) >= 0 ? 0.0 : M_PI;
	    }
	    else {
		r->t = t_mpr;
		if (rat_num(r->v.r) >= 0)
		    mpfr_set_ui(orr(r), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = mpq_sgn(oqr(r)) >= 0 ? 0.0 : M_PI;
	    }
	    else {
		r->t = t_mpr;
		if (mpq_sgn(oqr(r)) >= 0)
		    mpfr_set_ui(orr(r), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (mpfr_sgn(orr(r)) >= 0)
		    mpfr_set_ui(orr(r), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(r), thr_rnd);
	    }
	    break;
	case t_cdd:
	cdd:
	    r->t = t_float;
	    r->v.d = carg(r->v.dd);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    r->t = t_mpr;
	    mpc_arg(orr(r), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_conj(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_float:
	case t_rat:		case t_mpz:
	case t_mpq:		case t_mpr:
	    break;
	case t_cdd:
	    r->v.dd = conj(r->v.dd);
	    break;
	case t_cqq:
	    mpq_neg(oqi(r), oqi(r));
	    break;
	case t_mpc:
	    mpfr_neg(ori(r), ori(r), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_floor(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	    break;
	case t_float:
	    r->v.d = floor(r->v.d);
	    if ((oword_t)r->v.d == r->v.d) {
		r->t = t_word;
		r->v.w = r->v.d;
	    }
	    else {
		mpz_set_d(ozr(r), r->v.d);
		check_mpz(r);
	    }
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:
	    r->t = t_mpz;
	    mpz_fdiv_q(ozr(r), ozr(r), ozs(r));
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_floor(orr(r), orr(r));
	    ompz_set_r(ozr(r), orr(r));
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_trunc(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	    break;
	case t_float:
	    r->v.d = trunc(r->v.d);
	    if ((oword_t)r->v.d == r->v.d) {
		r->t = t_word;
		r->v.w = r->v.d;
	    }
	    else {
		r->t = t_mpz;
		mpz_set_d(ozr(r), r->v.d);
		check_mpz(r);
	    }
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:
	    r->t = t_mpz;
	    mpz_tdiv_q(ozr(r), ozr(r), ozs(r));
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_trunc(orr(r), orr(r));
	    ompz_set_r(ozr(r), orr(r));
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_round(oregister_t *r)
{
    GET_THREAD_SELF()
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	    break;
	case t_float:
	    r->v.d = round(r->v.d);
	    if ((oword_t)r->v.d == r->v.d) {
		r->t = t_word;
		r->v.w = r->v.d;
	    }
	    else {
		r->t = t_mpz;
		mpz_set_d(ozr(r), r->v.d);
		check_mpz(r);
	    }
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:
	    r->t = t_mpz;
	    if (mpq_sgn(oqr(r)) > 0) {
		mpz_cdiv_qr(ozr(r), thr_zr, ozr(r), ozs(r));
		mpz_mul_2exp(thr_zr, thr_zr, 1);
		if (mpz_cmpabs(thr_zr, ozs(r)) > 0)
		    mpz_sub_ui(ozr(r), ozr(r), 1);
	    }
	    else {
		mpz_fdiv_qr(ozr(r), thr_zr, ozr(r), ozs(r));
		mpz_mul_2exp(thr_zr, thr_zr, 1);
		if (mpz_cmpabs(thr_zr, ozs(r)) > 0)
		    mpz_add_ui(ozr(r), ozr(r), 1);
	    }
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_round(orr(r), orr(r));
	    ompz_set_r(ozr(r), orr(r));
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_ceil(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	    break;
	case t_float:
	    r->v.d = ceil(r->v.d);
	    if ((oword_t)r->v.d == r->v.d) {
		r->t = t_word;
		r->v.w = r->v.d;
	    }
	    else {
		r->t = t_mpz;
		mpz_set_d(ozr(r), r->v.d);
		check_mpz(r);
	    }
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:
	    r->t = t_mpz;
	    mpz_cdiv_q(ozr(r), ozr(r), ozs(r));
	    check_mpz(r);
	    break;
	case t_mpr:
	    r->t = t_mpz;
	    mpfr_ceil(orr(r), orr(r));
	    ompz_set_r(ozr(r), orr(r));
	    check_mpz(r);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
	    break;
    }
}

void
ovm_sqrt(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		if (r->v.w >= 0) {
		    r->t = t_float;
		    r->v.d = sqrt(r->v.w);
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0;
		    r->v.dd = csqrt(r->v.dd);
		}
	    }
	    else if (r->v.w >= 0) {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_sqrt(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		mpc_sqrt(occ(r), occ(r), thr_rndc);
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (likely(r->v.d >= 0.0))
		    r->v.d = sqrt(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0;
		    r->v.dd = csqrt(r->v.dd);
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		if (mpz_sgn(ozr(r)) >= 0) {
		    r->t = t_float;
		    r->v.d = sqrt(mpz_get_d(ozr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpz_get_d(ozr(r));
		    imag(r->v.dd) = 0;
		    r->v.dd = csqrt(r->v.dd);
		}
	    }
	    else if (mpz_sgn(ozr(r)) >= 0) {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_sqrt(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		mpc_sqrt(occ(r), occ(r), thr_rndc);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		if (rat_sgn(r->v.r) >= 0) {
		    r->t = t_float;
		    r->v.d = sqrt(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0;
		    r->v.dd = csqrt(r->v.dd);
		}
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		if (mpfr_sgn(orr(r)) >= 0)
		    mpfr_sqrt(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    mpc_sqrt(occ(r), occ(r), thr_rndc);
		}
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		if (mpq_sgn(oqr(r)) >= 0) {
		    r->t = t_float;
		    r->v.d = sqrt(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0;
		    r->v.dd = csqrt(r->v.dd);
		}
	    }
	    else if (mpq_sgn(oqr(r)) >= 0) {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_sqrt(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		mpc_sqrt(occ(r), occ(r), thr_rndc);
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (mpfr_sgn(orr(r)) >= 0)
		    mpfr_sqrt(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    mpc_sqrt(occ(r), occ(r), thr_rndc);
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = csqrt(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_sqrt(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_cbrt(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cbrt(r->v.w);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_cbrt(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = cbrt(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cbrt(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_cbrt(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cbrt(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cbrt(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cbrt(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cbrt(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_cbrt(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = ccbrt(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_cbrt(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_sin(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sin(r->v.w);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_sin(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = sin(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sin(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_sin(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sin(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_sin(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sin(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_sin(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_sin(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = csin(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_sin(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_cos(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 1.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 1, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cos(r->v.w);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_cos(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = cos(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cos(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_cos(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cos(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cos(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cos(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cos(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_cos(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = ccos(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_cos(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_tan(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tan(r->v.w);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_tan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = tan(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tan(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_tan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tan(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_tan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tan(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_tan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_tan(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = ctan(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_tan(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_asin(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(thr_rr, 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (r->v.w <= 1 && r->v.w >= -1) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = asin(r->v.w);
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_si(orr(r), r->v.w, thr_rnd);
		    mpfr_asin(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = r->v.w;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (fabs(r->v.d) <= 1.0)
		    r->v.d = asin(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpz_get_d(ozr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if ((rat_sgn(r->v.r) >= 0 && orat_cmp_si(&r->v.r,  1) <= 0) ||
		(rat_sgn(r->v.r) <  0 && orat_cmp_si(&r->v.r, -1) >= 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = asin(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_mpr;
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_asin(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = rat_get_d(r->v.r);
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpq:
	    if ((mpq_sgn(oqr(r)) >= 0 && mpq_cmp_ui(oqr(r),  1, 1) <= 0) ||
		(mpq_sgn(oqr(r)) <  0 && mpq_cmp_si(oqr(r), -1, 1) >= 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = asin(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_asin(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if ((mpfr_sgn(orr(r)) >= 0 && mpfr_cmp_ui(orr(r),  1) <= 0) ||
		    (mpfr_sgn(orr(r)) <  0 && mpfr_cmp_si(orr(r), -1) >= 0))
		    mpfr_asin(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = casin(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_asin(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_acos(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = acos(0.0);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(thr_rr, 0, thr_rnd);
		mpfr_acos(orr(r), orr(r), thr_rnd);
		break;
	    }
	    break;
	case t_word:
	    if (r->v.w <= 1 && r->v.w >= -1) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acos(r->v.w);
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_si(orr(r), r->v.w, thr_rnd);
		    mpfr_acos(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = r->v.w;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (fabs(r->v.d) <= 1.0)
		    r->v.d = acos(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpz_get_d(ozr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if ((rat_sgn(r->v.r) >= 0 && orat_cmp_si(&r->v.r,  1) <= 0) ||
		(rat_sgn(r->v.r) <  0 && orat_cmp_si(&r->v.r, -1) >= 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acos(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_mpr;
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_acos(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = rat_get_d(r->v.r);
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpq:
	    if ((mpq_sgn(oqr(r)) >= 0 && mpq_cmp_ui(oqr(r),  1, 1) <= 0) ||
		(mpq_sgn(oqr(r)) <  0 && mpq_cmp_si(oqr(r), -1, 1) >= 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acos(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_acos(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if ((mpfr_sgn(orr(r)) >= 0 && mpfr_cmp_ui(orr(r), 1) <= 0) ||
		    (mpfr_sgn(orr(r)) <  0 && mpfr_cmp_si(orr(r), -1) >= 0))
		    mpfr_acos(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = cacos(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_acos(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_atan(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = atan(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_atan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = atan(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = atan(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_atan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = atan(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_atan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = atan(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_atan(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_atan(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = catan(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_atan(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_sinh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sinh(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_sinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = sinh(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sinh(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_sinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sinh(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_sinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = sinh(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_sinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_sinh(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = csinh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_sinh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_cosh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 1.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 1, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cosh(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_cosh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = cosh(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cosh(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_cosh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cosh(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cosh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = cosh(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_cosh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_cosh(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = ccosh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_cosh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_tanh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tanh(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_tanh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = tanh(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tanh(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_tanh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tanh(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_tanh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = tanh(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_tanh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_tanh(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = ctanh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_tanh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_asinh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 0.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(orr(r), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = asinh(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_asinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = asinh(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = asinh(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_asinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = asinh(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_asinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = asinh(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_asinh(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_asinh(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = casinh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_asinh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_acosh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = 0;
		imag(r->v.dd) = 0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_ui(occ(r), 0, thr_rnd);
		goto mpc;
	    }
	    break;
	case t_word:
	    if (r->v.w >= 1) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acosh(r->v.w);
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_si(orr(r), r->v.w, thr_rnd);
		    mpfr_acosh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else {
		if (!cfg_float_format) {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0;
		    goto cdd;
		}
		else {
		    r->t = t_mpc;
		    mpc_set_si(occ(r), r->v.w, thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (r->v.d >= 1.0)
		    r->v.d = acosh(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (mpz_sgn(ozr(r)) > 0) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acosh(mpz_get_d(ozr(r)));
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_z(orr(r), ozr(r), thr_rnd);
		    mpfr_acosh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else {
		if (!cfg_float_format) {
		    r->t = t_cdd;
		    real(r->v.dd) = mpz_get_d(ozr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
		else {
		    r->t = t_mpr;
		    mpc_set_z(occ(r), ozr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_rat:
	    if (orat_cmp_si(&r->v.r, 1) >= 0) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acosh(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_mpr;
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_acosh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else {
		if (!cfg_float_format) {
		    r->t = t_cdd;
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
		else {
		    r->t = t_mpr;
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpc_set_q(occ(r), oqr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_mpq:
	    if (mpq_cmp_ui(oqr(r), 1, 1) >= 0) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = acosh(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_acosh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else {
		if (!cfg_float_format) {
		    r->t = t_cdd;
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
		else {
		    r->t = t_mpr;
		    mpc_set_q(occ(r), oqr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (mpfr_cmp_ui(orr(r), 1) >= 0) {
		    mpfr_acosh(orr(r), orr(r), thr_rnd);
		}
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = cacosh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_acosh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_atanh(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = atanh(0.0);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(thr_rr, 0, thr_rnd);
		mpfr_atanh(orr(r), orr(r), thr_rnd);
		break;
	    }
	    break;
	case t_word:
	    if (!r->v.w) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = atanh(r->v.w);
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_si(orr(r), r->v.w, thr_rnd);
		    mpfr_atanh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = r->v.w;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (fabs(r->v.d) < 1.0)
		    r->v.d = atanh(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpz_get_d(ozr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if ((rat_sgn(r->v.r) >= 0 && orat_cmp_si(&r->v.r,  1) < 0) ||
		(rat_sgn(r->v.r) <  0 && orat_cmp_si(&r->v.r, -1) > 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = atanh(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_mpr;
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_atanh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = rat_get_d(r->v.r);
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpq:
	    if ((mpq_sgn(oqr(r)) >= 0 && mpq_cmp_ui(oqr(r),  1, 1) < 0) ||
		(mpq_sgn(oqr(r)) <  0 && mpq_cmp_si(oqr(r), -1, 1) > 0)) {
		if (!cfg_float_format) {
		    r->t = t_float;
		    r->v.d = atanh(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_atanh(orr(r), orr(r), thr_rnd);
		}
	    }
	    else if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if ((mpfr_sgn(orr(r)) >= 0 && mpfr_cmp_ui(orr(r),  1) < 0) ||
		    (mpfr_sgn(orr(r)) <  0 && mpfr_cmp_si(orr(r), -1) > 0))
		    mpfr_atanh(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = catanh(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_atanh(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_proj(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    r->t = t_word;
	    r->v.w = 0;
	    break;
	case t_word:		case t_mpz:
	case t_rat:		case t_mpq:
	case t_cqq:
	    break;
	case t_float:
	    if (unlikely(isinf(r->v.d)))
		r->v.d = INFINITY;
	    break;
	case t_mpr:
	    if (unlikely(mpfr_inf_p(orr(r))))
		mpfr_set_inf(orr(r), 1);
	    break;
	case t_cdd:
	    r->v.dd = cproj(r->v.dd);
	    check_cdd(r);
	    break;
	case t_mpc:
	    mpc_proj(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_exp(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = 1.0;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_ui(thr_rr, 1, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = exp(r->v.w);
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_exp(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_float:
	    r->v.d = exp(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = exp(mpz_get_d(ozr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_exp(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = exp(rat_get_d(r->v.r));
	    }
	    else {
		r->t = t_mpr;
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_exp(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = exp(mpq_get_d(oqr(r)));
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_exp(orr(r), orr(r), thr_rnd);
	    }
	    break;
	case t_mpr:
	    mpfr_exp(orr(r), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = cexp(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_exp(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_log(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = -INFINITY;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_inf(thr_rr, -1);
		break;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		if (likely(r->v.w >= 0)) {
		    r->t = t_float;
		    r->v.d = log(r->v.w);
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0;
		    goto cdd;
		}
	    }
	    else if (likely(r->v.w >= 0)) {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_log(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (likely(r->v.d >= 0.0))
		    r->v.d = log(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		if (likely(mpz_sgn(ozr(r)) >= 0)) {
		    r->t = t_float;
		    r->v.d = log(mpz_get_d(ozr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpz_get_d(ozr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpz_sgn(ozr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_log(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		if (likely(rat_sgn(r->v.r) >= 0)) {
		    r->t = t_float;
		    r->v.d = log(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_log(orr(r), orr(r), thr_rnd);
		}
		else {
		    r->t = t_mpc;
		    mpc_set_q(occ(r), oqr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_float;
		    r->v.d = log(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpq_sgn(oqr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_log(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (likely(mpfr_sgn(orr(r)) >= 0))
		    mpfr_log(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = clog(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_log(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_log2(oregister_t *r)
{
    GET_THREAD_SELF()
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = -INFINITY;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_inf(thr_rr, -1);
		break;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		if (likely(r->v.w >= 0)) {
		    r->t = t_float;
		    r->v.d = log2(r->v.w);
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0;
		    goto cdd;
		}
	    }
	    else if (likely(r->v.w >= 0)) {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_log2(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (likely(r->v.d >= 0.0))
		    r->v.d = log2(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		if (likely(mpz_sgn(ozr(r))) >= 0) {
		    r->t = t_float;
		    r->v.d = log2(mpz_get_d(ozr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpz_get_d(ozr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpz_sgn(ozr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_log2(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		if (likely(rat_sgn(r->v.r) >= 0)) {
		    r->t = t_float;
		    r->v.d = log2(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_log2(orr(r), orr(r), thr_rnd);
		}
		else {
		    r->t = t_mpc;
		    mpc_set_q(occ(r), oqr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_float;
		    r->v.d = log2(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpq_sgn(oqr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_log2(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (likely(mpfr_sgn(orr(r)) >= 0))
		    mpfr_log2(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = clog2(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_log2(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

void
ovm_log10(oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		r->t = t_float;
		r->v.d = -INFINITY;
	    }
	    else {
		r->t = t_mpr;
		mpfr_set_inf(thr_rr, -1);
		break;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		if (likely(r->v.w >= 0)) {
		    r->t = t_float;
		    r->v.d = log10(r->v.w);
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0;
		    goto cdd;
		}
	    }
	    else if (likely(r->v.w >= 0)) {
		r->t = t_mpr;
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		mpfr_log10(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (likely(!isnan(r->v.d))) {
		if (likely(r->v.d >= 0.0))
		    r->v.d = log10(r->v.d);
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = r->v.d;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		if (likely(mpz_sgn(ozr(r)) >= 0)) {
		    r->t = t_float;
		    r->v.d = log10(mpz_get_d(ozr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpz_get_d(ozr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpz_sgn(ozr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		mpfr_log10(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_z(occ(r), ozr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		if (likely(rat_sgn(r->v.r) >= 0)) {
		    r->t = t_float;
		    r->v.d = log10(rat_get_d(r->v.r));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_mpr;
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    mpfr_log10(orr(r), orr(r), thr_rnd);
		}
		else {
		    r->t = t_mpc;
		    mpc_set_q(occ(r), oqr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		if (likely(mpq_sgn(oqr(r)) >= 0)) {
		    r->t = t_float;
		    r->v.d = log10(mpq_get_d(oqr(r)));
		}
		else {
		    r->t = t_cdd;
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    }
	    else if (likely(mpq_sgn(oqr(r)) >= 0)) {
		r->t = t_mpr;
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		mpfr_log10(orr(r), orr(r), thr_rnd);
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpr:
	    if (likely(!mpfr_nan_p(orr(r)))) {
		if (likely(mpfr_sgn(orr(r)) >= 0))
		    mpfr_log10(orr(r), orr(r), thr_rnd);
		else {
		    r->t = t_mpc;
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	    break;
	case t_cdd:
	cdd:
	    r->v.dd = clog10(r->v.dd);
	    check_cdd(r);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		r->t = t_cdd;
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    else {
		r->t = t_mpc;
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	mpc:
	    mpc_log10(occ(r), occ(r), thr_rndc);
	    check_mpc(r);
	    break;
	default:
	    ovm_raise(except_not_a_number);
	    break;
    }
}

#if __WORDSIZE == 32
void
ovm_load_ww(oregister_t *r, oint64_t *p)
{
    r->v.l = *p;
    if ((oword_t)r->v.l == r->v.l) {
	r->t = t_word;
	r->v.w = r->v.l;
    }
    else {
	r->t = t_mpz;
	ompz_set_sisi(ozr(r), r->v.s.l, r->v.s.h);
    }
}

void
ovm_load_uwuw(oregister_t *r, oint64_t *p)
{
    r->v.l = *p;
    if (r->v.l >= 0 && (oword_t)r->v.l == r->v.l) {
	r->t = t_word;
	r->v.w = r->v.l;
    }
    else {
	r->t = t_mpz;
	ompz_set_uiui(ozr(r), r->v.s.l, r->v.s.h);
    }
}

void
ovm_truncr_f(oregister_t *r, ofloat32_t f)
{
    r->v.l = f;
    if ((oword_t)r->v.l == r->v.l) {
	r->t = t_word;
	r->v.w = r->v.l;
    }
    else {
	ompz_set_sl(ozr(r), r->v.l);
	r->t = t_mpz;
    }
}

void
ovm_truncr_d(oregister_t *r, ofloat64_t d)
{
    r->v.l = d;
    if ((oword_t)r->v.l == r->v.l) {
	r->t = t_word;
	r->v.w = r->v.l;
    }
    else {
	ompz_set_sl(ozr(r), r->v.l);
	r->t = t_mpz;
    }
}
#endif
