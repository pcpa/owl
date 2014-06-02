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

#include "otypes.h"

#ifndef _omath_h
#define _omath_h

#define oadd_over_p(u, v, w)		((w) >= 0 ? (u) < (v) : (u) > (v))
#define osub_over_p(u, v, w)		((w) >= 0 ? (u) > (v) : (u) < (v))

#define thr_qq				&thread_self->qq
#define thr_qr				cqq_realref(thr_qq)
#define thr_qi				cqq_imagref(thr_qq)
#define thr_zr				mpq_numref(thr_qr)
#define thr_zs				mpq_denref(thr_qr)
#define thr_zi				mpq_numref(thr_qi)
#define thr_cc				&thread_self->cc
#define thr_rr				mpc_realref(thr_cc)
#define thr_ri				mpc_imagref(thr_cc)
#define thr_f				&thread_self->f
#define thr_vec				thread_self->vec

#define thr_prc				mpfr_get_default_prec()
#ifdef MPFR_RNDN
#  define thr_rnd			MPFR_RNDN
#else
#  define thr_rnd			GMP_RNDN
#endif
#define thr_rndc			MPC_RNDNN

#if !HAVE_MPC_SET_DC
#  define mpc_set_dc(a, b, c)		mpc_set_d_d(a, real(b), imag(b), c)
#endif

#if !HAVE_MPC_MUL_2EXP
#  define mpc_mul_2exp(a, b, c, d)	mpc_mul_2ui(a, b, c, d)
#  define mpc_div_2exp(a, b, c, d)	mpc_div_2ui(a, b, c, d)
#endif

#define SHIFT_MAX			 32767
#define SHIFT_MIN			-32767

/*
 * Prototypes
 */
#if !HAVE_CLOG2
extern complex double
clog2(complex double dd);
#endif

#if !HAVE_CLOG10
extern complex double
clog10(complex double dd);
#endif

extern complex double
ccbrt(complex double a);

#define ofloat_get_w(f)			(oword_t)(f)
extern oword_t
ompz_get_w(ompz_t z);

extern oword_t
ompq_get_w(ompq_t q);

extern oword_t
ompr_get_w(ompr_t r);

#define ocdd_get_w(dd)			(oword_t)real(dd)
#define ocdd_get_d(dd)			real(dd)
#define ocqq_get_w(qq)			ompq_get_w(cqq_realref(qq))
#define ocqq_get_d(dd)			mpq_get_d(cqq_realref(qq))
#define ompc_get_w(qq)			ompr_get_w(mpc_realref(qq))
#define ompc_get_d(dd)			mpr_get_d(mpc_realref(qq))

#if __WORDSIZE == 32
extern obool_t
ompz_fit_sl(ompz_t z);

extern oint64_t
ompz_get_sl(ompz_t z);
#define ompz_get_ul(z)			(ouint64_t)ompz_get_sl(z)

extern void
ompz_set_sisi(ompz_t z, oint32_t l, oint32_t h);

extern void
ompz_set_uiui(ompz_t z, ouint32_t l, ouint32_t h);

extern void
ompz_set_sl(ompz_t z, oint64_t u);

extern void
ompz_set_ul(ompz_t z, ouint64_t u);

extern oint64_t
ompq_get_sl(ompq_t q);

extern oint64_t
ompr_get_sl(ompr_t r);
#else
#  define ompz_get_sl(z)		ompz_get_w(z)
#  define ompz_get_ul(z)		(ouint64_t)ompz_get_w(z)
#  define ompz_set_sl(z, u)		mpz_set_si(z, u)
#  define ompz_set_ul(z, u)		mpz_set_ui(z, u)
#  define ompq_get_sl(q)		ompq_get_w(q)
#  define ompr_get_sl(r)		ompr_get_w(r)
#endif

#define ofloat_get_sl(f)		(oint64_t)(f)
#define ocdd_get_sl(dd)			(oint64_t)real(dd)
#define ocqq_get_sl(qq)			ompq_get_ll(cqq_realref(qq))
#define ompc_get_sl(qq)			ompr_get_ll(mpc_realref(qq))

extern void
ompz_set_r(ompz_t z, ompr_t r);

extern oword_t
ow_mul_w_w(oword_t a, oword_t b);

/*
Not available in mpc
 */
#undef mpc_cbrt
#define mpc_cbrt(a, b, c)		ompc_cbrt(a, b)
extern void
ompc_cbrt(ompc_t c, ompc_t a);

/*
Not available in mpc
 */
#undef mpc_log2
#define mpc_log2(a, b, c)		ompc_log2(a, b)
extern void
ompc_log2(ompc_t c, ompc_t a);

/*
Only available in newer mpc versions, but has problems
https://gforge.inria.fr/tracker/index.php?func=detail&aid=16651&group_id=131&atid=607
 */
#undef mpc_log10
#define mpc_log10(a, b, c)		ompc_log10(a, b)
extern void
ompc_log10(ompc_t c, ompc_t a);

#endif /* _omath_h */
