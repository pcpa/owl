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

/*
 * Types
 */
typedef struct oframe {
    oobject_t		next;
    oint32_t		type;
    oint32_t		size;
    oobject_t		prev;
    oobject_t		ret;
} oframe_t;

/*
 * Prototypes
 */
static void
vm_signal_handler(int signo);

static void
search_instruction_pointer(void);

/*
 * Initialization
 */
jit_function_t			jit_main;

static pthread_cond_t		count_cond;
static pthread_mutex_t		count_mutex;


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
}

void
finish_vm(void)
{
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
	if (jit_pointer_p(ip)) {					\
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
    assert(thread->bp == null);
    onew_object((oobject_t *)&thread->bp, t_void, 64 * 1024 * 1024);
    thread->fp = thread->bp + 64 * 1024 * 1024 - thread->frame;
    frame = (oframe_t *)thread->fp;
    frame->type = thread->type;
    thread->sp = thread->fp - thread->stack;

    othreads_lock();
    thread->run = 1;
    thread->xcpt = except_nothing;

    (*jit_main)(thread);
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
		ovm_raise(except_not_a_32_bits_integer);
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
	oint32_t	savet = t;
	if (t != t_void && reg->t != t_void &&
	    (t & t_vector) != (reg->t & t_vector)) {
	    /* Do not allow mixing signed and unsigned integers?
	     * Should not cause a fault if allowing using a float32_t
	     * vector where an int32_t were expected (or vice versa)
	     * but for the sake of correctness do not allow it. */
	    ortti_t		*rtti;

	    /* Derived classes can be used where a base class
	     * would be expected, due to language nature of
	     * only allowing single inheritance */
	    t &= ~t_vector;
	    while (t > t_mpc) {
		/* Only possible on memory corruption */
		assert(t <= rtti_vector->offset);
		rtti = rtti_vector->v.ptr[t];
		if (rtti->superc == t)
		    break;
		t = rtti->superc;
	    }
	    if (t == 0)
		ovm_raise(except_invalid_argument);
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
ovm_o_copy(oregister_t *l, oregister_t *r)
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
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:		case t_cqq:
	    mpz_add(ozr(r), ozr(r), mpq_denref(oqr(r)));
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
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    /* FALLTHROUGH */
	case t_mpq:		case t_cqq:
	    mpz_sub(ozr(r), ozr(r), mpq_denref(oqr(r)));
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
	    mpz_set_ui(mpq_denref(oqr(r)), 1);
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
	ovm_shl(l, -s);
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
	    mpz_fdiv_q(ozr(r), ozr(r), mpq_denref(oqr(r)));
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
	    mpz_fdiv_q(ozr(r), ozr(r), mpq_denref(oqr(r)));
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
