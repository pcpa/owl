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
 * Implementation
 */
void
ovm_coerce_d(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.d = 0.0;
	    break;
	case t_word:
	    l->v.d = r->v.w;
	    break;
	case t_float:
	    l->v.d = r->v.d;
	    break;
	case t_mpz:
	    l->v.d = mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.d = rat_get_d(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    l->v.d = mpq_get_d(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    l->v.d = mpfr_get_d(orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->v.d = real(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    l->t = t_float;
}

extern void
ovm_store_d(ofloat_t d, oobject_t *p, oint32_t t)
{
    if (t != t_void)
	ovm_raise(except_invalid_argument);
    if (*p && otype(*p) == t_float)
	**(ofloat_t **)p = d;
    else
	onew_float(p, d);
}

void
ovm_d_lt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d < 0.0;
	    break;
	case t_word:
	    l->v.w = l->v.d < r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d < r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d < mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d < rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d < mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = mpfr_less_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_d_le(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d <= 0.0;
	    break;
	case t_word:
	    l->v.w = l->v.d <= r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d <= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d <= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d <= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d <= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = mpfr_lessequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_d_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d == 0;
	    break;
	case t_word:
	    l->v.w = l->v.d == r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d == r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d == mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d == rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d == mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_d_ge(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d >= 0.0;
	    break;
	case t_word:
	    l->v.w = l->v.d >= r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d >= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d >= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d >= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d >= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = mpfr_greaterequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_d_gt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d > 0.0;
	    break;
	case t_word:
	    l->v.w = l->v.d > r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d > r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d > mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d > rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d > mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = mpfr_greater_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_d_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.d != 0;
	    break;
	case t_word:
	    l->v.w = l->v.d != r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.d != r->v.d;
	    break;
	case t_mpz:
	    l->v.w = l->v.d != mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.w = l->v.d != rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.w = l->v.d != mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_d_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.d += r->v.w;
	    break;
	case t_float:
	    l->v.d += r->v.d;
	    break;
	case t_mpz:
	    l->v.d += mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.d += rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.d += mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.d + r->v.dd;
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->t = t_cdd;
	    l->v.dd = l->v.d + r->v.dd;
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d(occ(l), l->v.d, thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_d_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.d -= r->v.w;
	    break;
	case t_float:
	    l->v.d -= r->v.d;
	    break;
	case t_mpz:
	    l->v.d -= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.d -= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.d -= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.d - r->v.dd;
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->t = t_cdd;
	    l->v.dd = l->v.d - r->v.dd;
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d(occ(l), l->v.d, thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_d_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.d *= 0.0;
	    break;
	case t_word:
	    l->v.d *= r->v.w;
	    break;
	case t_float:
	    l->v.d *= r->v.d;
	    break;
	case t_mpz:
	    l->v.d *= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.d *= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.d *= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.d * r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    l->t = t_cdd;
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd = l->v.d * r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d(occ(l), l->v.d, thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_d_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.d /= 0.0;
	    break;
	case t_word:
	    l->v.d /= r->v.w;
	    break;
	case t_float:
	    l->v.d /= r->v.d;
	    break;
	case t_mpz:
	    l->v.d /= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.d /= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.d /= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.d / r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    l->t = t_cdd;
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd = l->v.d / r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d(occ(l), l->v.d, thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_d_trunc2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    ovm_raise(except_floating_point_error);
	case t_word:
	    ovm_trunc_d(l, l->v.d / r->v.w);
	    break;
	case t_float:
	    ovm_trunc_d(l, l->v.d / r->v.d);
	    break;
	case t_mpz:
	    ovm_trunc_d(l, l->v.d / mpz_get_d(ozr(r)));
	    break;
	case t_rat:
	    ovm_trunc_d(l, l->v.d / rat_get_d(r->v.r));
	    break;
	case t_mpq:
	    ovm_trunc_d(l, l->v.d / mpq_get_d(oqr(r)));
	    break;
	case t_mpr:
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    ovm_trunc_r(l, orr(r));
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}

void
ovm_trunc_d(oregister_t *r, ofloat_t d)
{
    ofloat_t		f;

    modf(d, &f);
    if (f == (oword_t)f) {
	r->t = t_word;
	r->v.w = f;
    }
    else {
	mpz_set_d(ozr(r), f);
	if (mpz_fits_slong_p(ozr(r))) {
	    r->t = t_word;
	    r->v.w = mpz_get_si(ozr(r));
	}
	else
	    r->t = t_mpz;
    }
}

void
ovm_d_rem(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.d = fmod(l->v.d, 0.0);
	    break;
	case t_word:
	    l->v.d = fmod(l->v.d, r->v.w);
	    break;
	case t_float:
	    l->v.d = fmod(l->v.d, r->v.d);
	    break;
	case t_mpz:
	    l->v.d = fmod(l->v.d, mpz_get_d(ozr(r)));
	    break;
	case t_rat:
	    l->v.d = fmod(l->v.d, rat_get_d(r->v.r));
	    break;
	case t_mpq:
	    l->v.d = fmod(l->v.d, mpq_get_d(oqr(r)));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_fmod(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}

void
ovm_d_complex(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.d;
	    imag(l->v.dd) = r->v.w;
	    check_cdd(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.d;
	    imag(l->v.dd) = r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.d;
	    imag(l->v.dd) = mpz_get_d(ozr(r));
	    check_cdd(l);
	    break;
	case t_mpq:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.d;
	    imag(l->v.dd) = mpq_get_d(oqr(r));
	    check_cdd(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpfr_set_d(orr(l), l->v.d, thr_rnd);
	    mpfr_set(ori(l), orr(r), thr_rnd);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}
