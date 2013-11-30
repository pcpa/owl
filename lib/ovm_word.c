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
ovm_coerce_w(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w = r->v.w;
	    break;
	case t_float:
	    l->v.w = r->v.d;
	    break;
	case t_mpz:
	    l->v.w = ompz_get_w(ozr(r));
	    break;
	case t_rat:
	    l->v.w = rat_num(r->v.r) / rat_den(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    l->v.w = ompq_get_w(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    l->v.w = ompr_get_w(orr(r));
	    break;
	case t_cdd:
	    l->v.w = real(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    l->t = t_word;
}

void
ovm_coerce_uw(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w = r->v.w;
	    break;
	case t_float:
	    l->v.w = r->v.d;
	    break;
	case t_mpz:
	    l->v.w = ompz_get_w(ozr(r));
	    break;
	case t_rat:
	    l->v.w = rat_num(r->v.r) / rat_den(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    l->v.w = ompq_get_w(oqr(r));
	    break;
	case t_mpr:		case t_mpc:
	    l->v.w = ompr_get_w(orr(r));
	    break;
	case t_cdd:
	    l->v.w = real(r->v.dd);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (l->v.w < 0) {
	l->t = t_mpz;
	mpz_set_ui(ozr(l), l->v.w);
    }
    else
	l->t = t_word;
}

#if __WORDSIZE == 32
void
ovm_coerce_ww(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    if (l != r) {
		l->t = t_word;
		l->v.w = r->v.w;
	    }
	    break;
	case t_float:
	    mpz_set_d(ozr(l), r->v.d);
	    l->v.l = ompz_get_sl(ozr(l));
	    goto mpz;
	case t_mpz:
	    l->v.l = ompz_get_sl(ozr(r));
	mpz:
	    if ((oword_t)l->v.l == l->v.l) {
		l->t = t_word;
		l->v.w = l->v.l;
	    }
	    else {
		l->t = t_mpz;
		ompz_set_sisi(ozr(l), l->v.s.l, l->v.s.h);
	    }
	    break;
	case t_rat:
	    l->t = t_word;
	    l->v.w = rat_num(r->v.r) / rat_den(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    l->v.l = ompq_get_sl(oqr(r));
	    goto mpz;
	    break;
	case t_mpr:		case t_mpc:
	    l->v.l = ompr_get_sl(orr(r));
	    goto mpz;
	case t_cdd:
	    l->v.l = real(r->v.dd);
	    goto mpz;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

void
ovm_coerce_uwuw(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    if (l != r) {
		l->t = t_word;
		l->v.w = r->v.w;
	    }
	    break;
	case t_float:
	    mpz_set_d(ozr(l), r->v.d);
	    l->v.l = ompz_get_sl(ozr(l));
	    goto mpz;
	case t_mpz:
	    l->v.l = ompz_get_sl(ozr(r));
	mpz:
	    if (l->v.l >= 0 && (oword_t)l->v.l == l->v.l) {
		l->t = t_word;
		l->v.w = l->v.l;
	    }
	    else {
		l->t = t_mpz;
		ompz_set_uiui(ozr(l), l->v.s.l, l->v.s.h);
	    }
	    break;
	case t_rat:
	    l->t = t_word;
	    l->v.w = rat_num(r->v.r) / rat_den(r->v.r);
	    break;
	case t_mpq:		case t_cqq:
	    l->v.l = ompq_get_sl(oqr(r));
	    goto mpz;
	    break;
	case t_mpr:		case t_mpc:
	    l->v.l = ompr_get_sl(orr(r));
	    goto mpz;
	case t_cdd:
	    l->v.l = real(r->v.dd);
	    goto mpz;
	default:
	    ovm_raise(except_invalid_argument);
    }
}
#endif

extern void
ovm_store_w(oword_t w, oobject_t *p, oint32_t t)
{
    if (t != t_void)
	ovm_raise(except_invalid_argument);
    if (*p && otype(*p) == t_word)
	**(oword_t **)p = w;
    else
	onew_word(p, w);
}

void
ovm_w_lt(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w < 0;
	    break;
	case t_word:
	    l->v.w = l->v.w < r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w < r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) >= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, l->v.w) >= 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), l->v.w, 1) >= 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = mpfr_less_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_le(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w <= 0;
	    break;
	case t_word:
	    l->v.w = l->v.w <= r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w <= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) > 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, l->v.w) > 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), l->v.w, 1) > 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = mpfr_lessequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_eq(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w == 0;
	    break;
	case t_word:
	    l->v.w = l->v.w == r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w == r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) == 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_w_ge(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w >= 0;
	    break;
	case t_word:
	    l->v.w = l->v.w >= r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w >= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) < 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, l->v.w) < 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), l->v.w, 1) < 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = mpfr_greaterequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_gt(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w > 0;
	    break;
	case t_word:
	    l->v.w = l->v.w > r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w > r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) <= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, l->v.w) <= 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), l->v.w, 1) <= 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = mpfr_greater_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_ne(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = l->v.w != 0;
	    break;
	case t_word:
	    l->v.w = l->v.w != r->v.w;
	    break;
	case t_float:
	    l->v.w = l->v.w != r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), l->v.w) != 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_w_and(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w &= r->v.w;
	    break;
	case t_mpz:
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_and(ozr(l), ozr(l), ozr(r));
	    if (likely(mpz_fits_slong_p(ozr(l))))
		l->v.w = mpz_get_si(ozr(l));
	    else
		l->t = t_mpz;
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_w_or(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.w |= r->v.w;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_ior(ozr(l), ozr(l), ozr(r));
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_w_xor(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.w ^= r->v.w;
	    break;
	case t_mpz:
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_xor(ozr(l), ozr(l), ozr(r));
	    if (likely(mpz_fits_slong_p(ozr(l))))
		l->v.w = mpz_get_si(ozr(l));
	    else
		l->t = t_mpz;
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_w_add(oregister_t *l, oregister_t *r)
{
    oword_t		w;

    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    w = l->v.w + r->v.w;
	    if (oadd_over_p(w, l->v.w, r->v.w)) {
		l->t = t_mpz;
		mpz_set_si(ozr(l), l->v.w);
		if (r->v.w >= 0)
		    mpz_add_ui(ozr(l), ozr(l), r->v.w);
		else {
		    mpz_set_si(ozr(r), r->v.w);
		    mpz_add(ozr(l), ozr(l), ozr(r));
		}
	    }
	    else
		l->v.w = w;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = l->v.w + r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_add(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    if (orat_si_add(&l->v.r, l->v.w, &r->v.r)) {
		l->t = t_rat;
		check_rat(l);
	    }
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), l->v.w, 1);
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpq_add(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.w + r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    cqq_set_si(oqq(l), l->v.w, 0);
	    cqq_add(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_si(occ(l), l->v.w, thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_sub(oregister_t *l, oregister_t *r)
{
    oword_t		w;

    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    w = l->v.w - r->v.w;
	    if (osub_over_p(w, l->v.w, r->v.w)) {
		l->t = t_mpz;
		mpz_set_si(ozr(l), l->v.w);
		if (r->v.w >= 0)
		    mpz_sub_ui(ozr(l), ozr(l), r->v.w);
		else {
		    mpz_set_si(ozr(r), r->v.w);
		    mpz_sub(ozr(l), ozr(l), ozr(r));
		}
	    }
	    else
		l->v.w = w;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = l->v.w - r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_sub(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    if (orat_si_sub(&l->v.r, l->v.w, &r->v.r)) {
		l->t = t_rat;
		check_rat(l);
	    }
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), l->v.w, 1);
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpq_sub(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.w - r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    cqq_set_si(oqq(l), l->v.w, 0);
	    cqq_sub(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_si(occ(l), l->v.w, thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_mul(oregister_t *l, oregister_t *r)
{
    oword_t		w;

    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    w = ow_mul_w_w(l->v.w, r->v.w);
	    if (w != MININT)
		l->v.w = w;
	    else {
		l->t = t_mpz;
		mpz_set_si(ozr(l), l->v.w);
		mpz_mul_si(ozr(l), ozr(l), r->v.w);
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = l->v.w * r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_mul_si(ozr(l), ozr(r), l->v.w);
	    check_mpz(l);
	    break;
	case t_rat:
	    if (orat_si_mul(&l->v.r, l->v.w, &r->v.r)) {
		l->t = t_rat;
		check_rat(l);
	    }
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), l->v.w, 1);
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpq_mul(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.w * r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_mul(oqi(l), oqi(r), oqr(l));
	    mpq_mul(oqr(l), oqr(r), oqr(l));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_mul(ori(l), ori(r), orr(l), thr_rnd);
	    mpfr_mul(orr(l), orr(r), orr(l), thr_rnd);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    ovm_raise(except_floating_point_error);
	case t_word:
	    if (unlikely(r->v.w == 0))
		ovm_raise(except_floating_point_error);
	    if (l->v.w % r->v.w) {
		if (likely(orat_set_si(&l->v.r, l->v.w, r->v.w))) {
		    l->t = t_rat;
		    orat_canonicalize(&l->v.r);
		}
		else {
		    l->t = t_mpq;
		    if (l->v.w >= 0 && r->v.w >= 0)
			mpq_set_ui(oqr(l), l->v.w, r->v.w);
		    else {
			mpz_set_si(ozr(l), l->v.w);
			mpz_set_si(ozs(l), r->v.w);
		    }
		    mpq_canonicalize(oqr(l));
		}
	    }
	    else
		l->v.w /= r->v.w;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = l->v.w / r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpq;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_set(ozs(l), ozr(r));
	    mpq_canonicalize(oqr(l));
	    check_mpq(l);
	    break;
	case t_rat:
	    if (orat_si_div(&l->v.r, l->v.w, &r->v.r)) {
		l->t = t_rat;
		check_rat(l);
	    }
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), l->v.w, 1);
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpq_div(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = l->v.w / r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    if (likely(l->v.w != 0)) {
		l->t = t_cqq;
		cqq_si_div(oqq(l), l->v.w, oqq(r));
		check_cqq(l);
	    }
	    else {
		l->t = t_word;
		l->v.w = 0;
	    }
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_si(occ(l), l->v.w, thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_trunc2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	    l->t = t_word;
	    l->v.w /= r->v.w;
	    break;
	case t_float:
	    ovm_trunc_d(l, l->v.w / r->v.d);
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_tdiv_q(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    l->t = t_mpz;
	    if (l->v.w >= 0)
		mpz_mul_ui(ozr(l), ozs(r), l->v.w);
	    else {
		mpz_set_si(ozr(l), l->v.w);
		mpz_mul(ozr(l), ozr(l), ozs(r));
	    }
	    mpz_tdiv_q(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    ovm_trunc_r(l, orr(r));
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_rem(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	    l->t = t_word;
	    l->v.w %= r->v.w;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = fmod(l->v.w, r->v.d);
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set_si(ozr(l), l->v.w);
	    mpz_tdiv_r(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    l->t = t_mpq;
	    if (l->v.w >= 0)
		mpz_mul_ui(ozs(l), ozs(r), l->v.w);
	    else {
		mpz_set_si(ozr(l), l->v.w);
		mpz_mul(ozs(l), ozs(r), ozr(l));
	    }
	    mpz_tdiv_r(ozr(l), ozs(l), ozr(r));
	    mpz_set(ozs(l), ozs(r));
	    mpq_canonicalize(oqr(l));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_fmod(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_complex(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->t = t_cqq;
	    cqq_set_si(oqq(l), l->v.w, r->v.w);
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.w;
	    imag(l->v.dd) = r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_set_z(oqi(l), ozr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_set_si(oqi(l), rat_num(r->v.r), rat_den(r->v.r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), l->v.w, 1);
	    mpq_set(oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_set(ori(l), orr(r), thr_rnd);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_w_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(l->v.w, 0.0);
	    }
	    else {
		mpfr_set_ui(orr(r), 0, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(l->v.w, r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = atan2(l->v.w, r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(l->v.w, mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(l->v.w, rat_get_d(r->v.r));
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(l->v.w, mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    l->t = t_mpr;
	    mpfr_atan2(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    if (l->v.w) {
		l->t = t_cdd;
		real(l->v.dd) = l->v.w;
		imag(l->v.dd) = 0.0;
		l->v.dd = catan(l->v.dd / r->v.dd);
		check_cdd(l);
	    }
	    else {
		l->t = t_float;
		l->v.d = real(r->v.dd) >= 0 ? 0.0 : M_PI;
	    }
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    if (l->v.w) {
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    l->t = t_mpr;
	    if (r->v.w >= 0)
		mpfr_set_ui(orr(l), 0, thr_rnd);
	    else
		mpfr_const_pi(orr(l), thr_rnd);
	    break;
	case t_mpc:
	    if (l->v.w) {
	    mpc:
		l->t = t_mpc;
		mpc_set_si(occ(l), l->v.w, thr_rndc);
		mpc_div(occ(l), occ(l), occ(r), thr_rndc);
		mpc_atan(occ(l), occ(l), thr_rndc);
		check_mpc(l);
	    }
	    else {
		l->t = t_mpr;
		if (mpfr_sgn(orr(r)) >= 0)
		    mpfr_set_ui(orr(l), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(l), thr_rnd);
	    }
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_pow(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = 1.0;
	    }
	    else {
		l->t = t_mpr;
		mpfr_set_ui(orr(l), 1, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = pow(l->v.w, r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    if (l->v.w < 0 && finite(r->v.d) && rint(r->v.d) != r->v.d) {
		real(r->v.dd) = r->v.d;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    l->t = t_float;
	    l->v.d = pow(l->v.w, r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = pow(l->v.w, mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (l->v.w < 0) {
		if (!cfg_float_format) {
		    real(r->v.dd) = rat_get_d(r->v.r);
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    else {
		if (!cfg_float_format) {
		    l->t = t_float;
		    l->v.d = pow(l->v.w, rat_get_d(r->v.r));
		}
		else {
		    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    goto mpr;
		}
	    }
	    break;
	case t_mpq:
	    if (l->v.w < 0) {
		if (!cfg_float_format) {
		    real(r->v.dd) = mpq_get_d(oqr(r));
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
		mpc_set_q(occ(r), oqr(r), thr_rndc);
		goto mpc;
	    }
	    else {
		if (!cfg_float_format) {
		    l->t = t_float;
		    l->v.d = pow(l->v.w, mpq_get_d(oqr(r)));
		}
		else {
		    mpfr_set_q(orr(r), oqr(r), thr_rnd);
		    goto mpr;
		}
	    }
	    break;
	case t_mpr:
	    if (l->v.w < 0 && mpfr_number_p(orr(r))) {
		mpc_set_fr(occ(r), orr(r), thr_rndc);
		goto mpc;
	    }
	mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_pow(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = l->v.w;
	    imag(l->v.dd) = 0.0;
	    l->v.dd = cpow(l->v.dd, r->v.dd);
	    check_cdd(l);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	case t_mpc:
	mpc:
	    l->t = t_mpc;
	    mpc_set_si(occ(l), l->v.w, thr_rndc);
	    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_w_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = fabs(l->v.w);
	    }
	    else {
		mpfr_set_ui(orr(r), 0, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(l->v.w, r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = hypot(l->v.w, r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(l->v.w, mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(l->v.w, rat_get_d(r->v.r));
	    }
	    else {
		mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpq:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(l->v.w, mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), l->v.w, thr_rnd);
	    mpfr_hypot(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    l->t = t_float;
	    l->v.d = hypot(l->v.w, hypot(real(r->v.dd), imag(r->v.dd)));
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	case t_mpc:
	    l->t = t_mpr;
	    mpc_set_si(occ(l), l->v.w, thr_rndc);
	    mpfr_hypot(ori(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(orr(l), orr(r), ori(r), thr_rnd);
	    mpfr_hypot(orr(l), ori(l), orr(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
