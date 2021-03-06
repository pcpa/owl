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

/*
 * Types
 */
typedef union {
    struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	ohalf_t		l;
	ohalf_t		h;
#else
	ohalf_t		h;
	ohalf_t		l;
#endif
    } h;
    oword_t		w;
} data_t;

/*
 * Implementation
 */
#if !HAVE_CLOG2
complex double
clog2(complex double dd)
{
    complex double	z;

    real(z) = log2(hypot(real(dd), imag(dd)));
    imag(z) = M_LOG2E * atan2(imag(dd), real(dd));

    return (z);
}
#endif

#if !HAVE_CLOG10
complex double
clog10(complex double dd)
{
    complex double	z;

    real(z) = log10(hypot(real(dd), imag(dd)));
    imag(z) = M_LOG10E * atan2(imag(dd), real(dd));

    return (z);
}
#endif

complex double
ccbrt(complex double a)
{
#if !HAVE_SINCOS
    double		f;
#endif
    double		d;
    complex double	dd;

    d = exp(log(cabs(a)) / 3.0);
#if HAVE_SINCOS
    sincos(atan2(imag(a), real(a)) / 3.0, &imag(dd), &real(dd));
#else
    f = atan2(imag(a), real(a)) / 3.0;
    imag(dd) = sin(f);
    real(dd) = cos(f);
#endif
    real(dd) *= d;
    imag(dd) *= d;

    return (dd);
}

oword_t
ompz_get_w(ompz_t z)
{
    mp_size_t		n;
    mp_ptr		p;
    oword_t		w;

    p = z->_mp_d;
    n = z->_mp_size;

    if (n > 0)
	w = p[0];
    else if (n < 0) {
	w = p[0];
	w = -w;
    }
    else
	w = 0;

    return (w);
}

oword_t
ompq_get_w(ompq_t q)
{
    GET_THREAD_SELF()
    if (mpz_fits_slong_p(mpq_numref(q)) && mpz_fits_slong_p(mpq_denref(q)))
	return (mpz_get_si(mpq_numref(q)) / mpz_get_si(mpq_denref(q)));
    mpz_set_q(thr_zr, q);
    return (ompz_get_w(thr_zr));
}

oword_t
ompr_get_w(ompr_t r)
{
    GET_THREAD_SELF()
    if (unlikely(!mpfr_number_p(r)))
	othread_kill(SIGFPE);
    if (mpfr_fits_slong_p(r, GMP_RNDZ))
	return (mpfr_get_si(r, GMP_RNDZ));
    mpfr_get_z(thr_zr, r, GMP_RNDZ);
    return (ompz_get_w(thr_zr));
}

#if __WORDSIZE == 32
obool_t
ompz_fit_sl(ompz_t z)
{
    switch (z->_mp_size) {
	case 0: case 1: case -1: return (true);
	case 2: case -2:	 return (z->_mp_d[1] <= 0x7fffffff);
	default:		 return (false);
    }
}

oint64_t
ompz_get_sl(ompz_t z)
{
    mp_size_t		n;
    mp_ptr		p;
    oint64_t		i;

    p = z->_mp_d;
    n = z->_mp_size;
    if (n > 0) {
	i = p[0];
	if (n > 1)
	    i |= (oint64_t)p[1] << 32;
    }
    else if (n < 0) {
	i = p[0];
	if (n < -1)
	    i |= (oint64_t)p[1] << 32;
	i = -i;
    }
    else
	i = 0;

    return (i);
}

void
ompz_set_sisi(ompz_t z, oint32_t l, oint32_t h)
{
    if (h) {
	mpz_set_si(z, h);
	mpz_mul_2exp(z, z, 32);
	mpz_add_ui(z, z, l);
    }
    else
	mpz_set_ui(z, l);
}

void
ompz_set_uiui(ompz_t z, ouint32_t l, ouint32_t h)
{
    if (h) {
	mpz_set_ui(z, h);
	mpz_mul_2exp(z, z, 32);
	mpz_add_ui(z, z, l);
    }
    else
	mpz_set_ui(z, l);
}

void
ompz_set_sl(ompz_t z, oint64_t i)
{
    if (i != (oint32_t)i) {
	mpz_set_si(z, i >> 32);
	mpz_mul_2exp(z, z, 32);
	mpz_add_ui(z, z, i & 0xffffffff);
    }
    else
	mpz_set_si(z, i);
}

void
ompz_set_ul(ompz_t z, ouint64_t u)
{
    if (u & 0xffffffff00000000LL) {
	mpz_set_ui(z, (ouint64_t)u >> 32);
	mpz_mul_2exp(z, z, 32);
	mpz_add_ui(z, z, u & 0xffffffff);
    }
    else
	mpz_set_ui(z, u);
}

oint64_t
ompq_get_sl(ompq_t q)
{
    GET_THREAD_SELF()
    if (ompz_fit_sl(mpq_numref(q)) && ompz_fit_sl(mpq_denref(q)))
	return (ompz_get_sl(mpq_numref(q)) / ompz_get_sl(mpq_denref(q)));
    else {
	mpz_set_q(thr_zr, q);
	return (ompz_get_sl(thr_zr));
    }
}

oint64_t
ompr_get_sl(ompr_t r)
{
    GET_THREAD_SELF()
    if (unlikely(!mpfr_number_p(r)))
	othread_kill(SIGFPE);
    if (mpfr_fits_slong_p(r, GMP_RNDZ))
	return (mpfr_get_si(r, GMP_RNDZ));
    mpfr_get_z(thr_zr, r, GMP_RNDZ);
    return (ompz_get_sl(thr_zr));
}
#endif

void
ompz_set_r(ompz_t z, ompr_t r)
{
    GET_THREAD_SELF()
    if (unlikely(!mpfr_number_p(r)))
	othread_kill(SIGFPE);
    mpfr_get_z(z, r, GMP_RNDZ);
}

oword_t
ow_mul_w_w(oword_t a, oword_t b)
{
    /* shameless cut&paste + adaptation/indent-style-change
     * from libgcc multiplication overflow check */
    const data_t uu = { .w = a };
    const data_t vv = { .w = b };

    if (likely(uu.h.h == uu.h.l >> HALFSHIFT)) {
	/* a fits in a single ohalf_t */
	if (likely(vv.h.h == vv.h.l >> HALFSHIFT)) {
	    /* b fits in a single ohalf_t as well */
	    /* a single multiplication, no overflow risk */
	    return ((oword_t)uu.h.l * (oword_t)vv.h.l);
	}
	else {
	    /* two multiplications */
	    data_t w0 = {
		.w = (ouword_t)(ouhalf_t)uu.h.l * (ouword_t)(ouhalf_t)vv.h.l
	    };
	    data_t w1 = {
		.w = (ouword_t)(ouhalf_t)uu.h.l * (ouword_t)(ouhalf_t)vv.h.h
	    };

	    if (vv.h.h < 0)
		w1.h.h -= uu.h.l;
	    if (uu.h.l < 0)
		w1.w -= vv.w;
	    w1.w += (ouhalf_t)w0.h.h;
	    if (likely(w1.h.h == w1.h.l >> HALFSHIFT)) {
		w0.h.h = w1.h.l;
		return (w0.w);
	    }
	}
    }
    else if (likely(vv.h.h == vv.h.l >> HALFSHIFT)) {
	/* b fits into a single ohalf_t */
	/* two multiplications */
	data_t w0 = {
	    .w = (ouword_t)(ouhalf_t)uu.h.l * (ouword_t)(ouhalf_t)vv.h.l
	};
	data_t w1 = {
	    .w = (ouword_t)(ouhalf_t)uu.h.h * (ouword_t)(ouhalf_t)vv.h.l
	};

	if (uu.h.h < 0)
	    w1.h.h -= vv.h.l;
	if (vv.h.l < 0)
	    w1.w -= uu.w;
	w1.w += (ouhalf_t)w0.h.h;
	if (likely(w1.h.h == w1.h.l >> 31)) {
	    w0.h.h = w1.h.l;
	    return (w0.w);
	}
    }
    /* A few sign checks and a single multiplication */
    else if (uu.h.h >= 0) {
	if (vv.h.h >= 0) {
	    if (uu.h.h == 0 && vv.h.h == 0) {
		const oword_t w = (ouword_t)(ouhalf_t)uu.h.l *
				  (ouword_t)(ouhalf_t)vv.h.l;

		if (likely(w >= 0))
		    return (w);
	    }
	}
	else if (uu.h.h == 0 && vv.h.h == (ohalf_t)-1) {
	    data_t w0 = {
		.w = (ouword_t)(ouhalf_t)uu.h.l * (ouword_t)(ouhalf_t)vv.h.l
	    };

	    w0.h.h -= uu.h.l;
	    if (likely(w0.h.h < 0))
		return (w0.w);
	}
    }
    else if (vv.h.h >= 0) {
	if (uu.h.h == (ohalf_t) -1 && vv.h.h == 0) {
	    data_t w0 = {
		.w = (ouword_t)(ouhalf_t) uu.h.l * (ouword_t)(ouhalf_t) vv.h.l
	    };

	    w0.h.h -= vv.h.l;
	    if (likely(w0.h.h < 0))
		return (w0.w);
	}
    }
    else if (uu.h.h == (ohalf_t)-1 && vv.h.h == (ohalf_t)-1) {
	data_t w0 = {
	    .w = (ouword_t)(ouhalf_t)uu.h.l * (ouword_t)(ouhalf_t) vv.h.l
	};

	w0.h.h -= uu.h.l;
	w0.h.h -= vv.h.l;
	if (likely(w0.h.h >= 0))
	    return (w0.w);
    }

    return (MININT);
}

void
ompc_cbrt(ompc_t c, ompc_t a)
{
    GET_THREAD_SELF()
    mpc_set_ui_ui(thr_cc, 1, 3, thr_rndc);
    mpfr_div(thr_rr, thr_rr, thr_ri, thr_rnd);
    mpc_pow_fr(c, a, thr_rr, thr_rndc);
}

void
ompc_log2(ompc_t c, ompc_t a)
{
    GET_THREAD_SELF()
    /*
     *	v = hypot(r	i)
     *	log2(v)		1/log(2)*atan2(i, r)
     */

    /* t = hypot(r+i)						*/
    mpfr_hypot(thr_rr, mpc_realref(a), mpc_imagref(a), thr_rnd);

    /* i = atan2(i, r)						*/
    mpfr_atan2(mpc_imagref(c), mpc_imagref(a), mpc_realref(a), thr_rnd);

    /* r = log2(t)						*/
    mpfr_log2(mpc_realref(a), thr_rr, thr_rnd);

    /* t = 1/log(2)						*/
    /* t = log(2)						*/
    mpfr_const_log2(thr_rr, thr_rnd);
    /* t1 = 1							*/
    mpfr_set_ui(thr_ri, 1, thr_rnd);
    /* t = t1/t							*/
    mpfr_div(thr_rr, thr_ri, thr_rr, thr_rnd);

    /* i = t*i							*/
    mpfr_mul(mpc_imagref(c), thr_rr, mpc_imagref(c), thr_rnd);
}

void
ompc_log10(ompc_t c, ompc_t a)
{
    GET_THREAD_SELF()
    /*
     *	v = hypot(r	i)
     *	log2(v)		1/log(10)*atan2(i, r)
     */

    /* t = hypot(r+i)						*/
    mpfr_hypot(thr_rr, mpc_realref(a), mpc_imagref(a), thr_rnd);

    /* i = atan2(i, r)						*/
    mpfr_atan2(mpc_imagref(c), mpc_imagref(a), mpc_realref(a), thr_rnd);

    /* r = log10(t)						*/
    mpfr_log10(mpc_realref(a), thr_rr, thr_rnd);

    /* t = 1/log(10)						*/
    /* t = 10							*/
    mpfr_set_ui(thr_rr, 10, thr_rnd);
    /* t = log(t)						*/
    mpfr_log(thr_rr, thr_rr, thr_rnd);
    /* t1 = 1							*/
    mpfr_set_ui(thr_ri, 1, thr_rnd);
    /* t = t1/t							*/
    mpfr_div(thr_rr, thr_ri, thr_rr, thr_rnd);

    /* i = t*i							*/
    mpfr_mul(mpc_imagref(c), thr_rr, mpc_imagref(c), thr_rnd);
}
