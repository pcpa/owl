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
ovm_v_lt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w = 0 < r->v.w;
	    break;
	case t_float:
	    l->v.w = 0 < r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), 0) >= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, 0) >= 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), 0, 1) >= 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    l->v.w = mpfr_less_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_le(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 1;
	    break;
	case t_word:
	    l->v.w = 0 <= r->v.w;
	    break;
	case t_float:
	    l->v.w = 0 <= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), 0) > 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, 0) > 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), 0, 1) > 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    l->v.w = mpfr_lessequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 1;
	    break;
	case t_word:
	    l->v.w = 0 == r->v.w;
	    break;
	case t_float:
	    l->v.w = 0.0 == r->v.d;
	    break;
	case t_mpr:
	    l->v.w = mpfr_zero_p(orr(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_v_ge(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 1;
	    break;
	case t_word:
	    l->v.w = 0 >= r->v.w;
	    break;
	case t_float:
	    l->v.w = 0 >= r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), 0) < 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, 0) < 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), 0, 1) < 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    l->v.w = mpfr_greaterequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_gt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w = 0 > r->v.w;
	    break;
	case t_float:
	    l->v.w = 0 > r->v.d;
	    break;
	case t_mpz:
	    l->v.w = mpz_cmp_si(ozr(r), 0) <= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp_si(&r->v.r, 0) <= 0;
	    break;
	case t_mpq:
	    l->v.w = mpq_cmp_si(oqr(r), 0, 1) <= 0;
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    l->v.w = mpfr_greater_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = 0;
	    break;
	case t_word:
	    l->v.w = 0 != r->v.w;
	    break;
	case t_float:
	    l->v.w = 0.0 != r->v.d;
	    break;
	case t_mpr:
	    l->v.w = !mpfr_zero_p(orr(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_v_and(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:		case t_word:		case t_mpz:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_v_or(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    l->t = t_word;
	    l->v.w = r->v.w;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set(ozr(l), ozr(r));
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_v_xor(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    l->t = t_word;
	    l->v.w = r->v.w;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set(ozr(l), ozr(r));
	    break;
	default:
	    ovm_raise(except_not_an_integer);
    }
}

void
ovm_v_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    l->t = t_word;
	    l->v.w = r->v.w;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_set(ozr(l), ozr(r));
	    break;
	case t_rat:
	    l->t = t_rat;
	    rat_num(l->v.r) = rat_num(r->v.r);
	    rat_den(l->v.r) = rat_den(r->v.r);
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set(oqr(l), oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set(orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    cqq_set(oqq(l), oqq(r));
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set(occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    if (likely(l->v.w != MININT)) {
		l->t = t_word;
		l->v.w = -r->v.w;
	    }
	    else {
		l->t = t_mpz;
		mpz_set_ui(ozr(l), MININT);
		mpz_neg(ozr(l), ozr(l));
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = -r->v.d;
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpz_neg(ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    if (rat_num(r->v.r) == MININT) {
		l->t = t_mpq;
		mpq_set_ui(oqr(l), rat_num(r->v.r), rat_den(r->v.r));
		mpq_neg(oqr(l), oqr(l));
		mpq_canonicalize(oqr(l));
		check_mpq(l);
	    }
	    else {
		l->t = t_rat;
		rat_num(l->v.r) = -rat_num(r->v.r);
		rat_den(l->v.r) =  rat_den(r->v.r);
		check_rat(l);
	    }
	    break;
	case t_mpq:
	    l->t = t_mpq;
	    mpq_neg(oqr(l), oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_neg(orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = -r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    cqq_neg(oqq(l), oqq(r));
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_neg(occ(r), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:	case t_word:	case t_mpz:
	case t_rat:	case t_mpq:	case t_cqq:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = 0.0 * r->v.d;
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = 0.0 * r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    mpfr_mul(orr(l), orr(r), orr(l), thr_rnd);
	    mpfr_mul(ori(l), ori(r), orr(l), thr_rnd);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    ovm_raise(except_floating_point_error);
	case t_word:
	    if (unlikely(r->v.w == 0))
		ovm_raise(except_floating_point_error);
	case t_mpz:
	case t_rat:	case t_mpq:	case t_cqq:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = 0.0 / r->v.d;
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = 0.0 / r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_si(occ(l), 0, thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_trunc2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	case t_mpz:	case t_rat:	case t_mpq:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_float:
	    ovm_trunc_d(l, 0.0 / r->v.d);
	    break;
	case t_mpr:
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    ovm_trunc_r(l, orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_rem(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	case t_mpz:	case t_rat:	case t_mpq:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = fmod(0.0, r->v.d);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpfr_set_si(orr(l), 0, thr_rnd);
	    mpfr_fmod(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_complex(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    l->t = t_cqq;
	    cqq_set_si(oqq(l), 0, r->v.w);
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = 0.0;
	    imag(l->v.dd) = r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    l->t = t_cqq;
	    mpq_set_ui(oqr(l), 0, 1);
	    mpq_set_z(oqi(l), ozr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    l->t = t_cqq;
	    mpq_set_ui(oqr(l), 0, 1);
	    mpq_set_si(oqi(l), rat_num(r->v.r), rat_den(r->v.r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    l->t = t_cqq;
	    mpq_set_ui(oqr(l), 0, 1);
	    mpq_set(oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpfr_set_ui(orr(l), 0, thr_rnd);
	    mpfr_set(ori(l), orr(r), thr_rnd);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_v_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = 0.0;
	    }
	    else {
		l->t = t_mpr;
		mpfr_set_ui(orr(l), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(0.0, r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = atan2(0.0, r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(0.0, mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = atan2(0.0, rat_get_d(r->v.r));
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
		l->v.d = atan2(0.0, mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    mpfr_set_ui(orr(l), 0, thr_rnd);
	    l->t = t_mpr;
	    mpfr_atan2(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_float;
	    l->v.d = real(r->v.dd) >= 0 ? 0.0 : M_PI;
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = mpq_sgn(oqr(r)) >= 0 ? 0.0 : M_PI;
	    }
	    else {
		l->t = t_mpr;
		if (mpq_sgn(oqr(r)) >= 0)
		    mpfr_set_ui(orr(l), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(l), thr_rnd);
	    }
	    break;
	case t_mpc:
	    l->t = t_mpr;
	    if (mpfr_sgn(orr(r)) >= 0)
		mpfr_set_ui(orr(l), 0, thr_rnd);
	    else
		mpfr_const_pi(orr(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_pow(oregister_t *l, oregister_t *r)
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
		l->v.d = pow(0.0, r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = pow(0.0, r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = pow(0.0, mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = pow(0.0, rat_get_d(r->v.r));
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
		l->v.d = pow(0.0, mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    l->t = t_mpr;
	    mpfr_set_ui(orr(l), 0, thr_rnd);
	    mpfr_pow(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = 0.0;
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
	    l->t = t_mpc;
	    mpc_set_ui(occ(l), 0, thr_rndc);
	    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_v_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = 0.0;
	    }
	    else {
		l->t = t_mpr;
		mpfr_set_ui(orr(l), 0, thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = fabs(r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = fabs(r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = fabs(mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = fabs(rat_get_d(r->v.r));
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
		l->v.d = fabs(mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    l->t = t_mpr;
	    mpfr_abs(orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    l->t = t_float;
	    l->v.dd = cabs(r->v.dd);
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
	    l->t = t_mpr;
	    mpc_abs(orr(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
