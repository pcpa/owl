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
ovm_x_lt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = rat_sgn(l->v.r) < 0;
	    break;
	case t_word:
	    l->v.w = orat_cmp_si(&l->v.r, r->v.w) < 0;
	    break;
	case t_float:
	    l->v.w = rat_get_d(l->v.r) < r->v.d;
	    break;
	case t_mpz:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_z(oqr(r), ozr(r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) < 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) < 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) < 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = mpfr_less_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_x_le(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = rat_sgn(l->v.r) <= 0;
	    break;
	case t_word:
	    l->v.w = orat_cmp_si(&l->v.r, r->v.w) <= 0;
	    break;
	case t_float:
	    l->v.w = rat_get_d(l->v.r) <= r->v.d;
	    break;
	case t_mpz:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_z(oqr(r), ozr(r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) <= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) <= 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) <= 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = mpfr_lessequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_x_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_float:
	    l->v.w = rat_get_d(l->v.r) == r->v.d;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) == 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) == 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_x_ge(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = rat_sgn(l->v.r) >= 0;
	    break;
	case t_word:
	    l->v.w = orat_cmp_si(&l->v.r, r->v.w) >= 0;
	    break;
	case t_float:
	    l->v.w = rat_get_d(l->v.r) >= r->v.d;
	    break;
	case t_mpz:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_z(oqr(r), ozr(r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) >= 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) >= 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) >= 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = mpfr_greaterequal_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_x_gt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = rat_sgn(l->v.r) > 0;
	    break;
	case t_word:
	    l->v.w = orat_cmp_si(&l->v.r, r->v.w) > 0;
	    break;
	case t_float:
	    l->v.w = rat_get_d(l->v.r) > r->v.d;
	    break;
	case t_mpz:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_z(oqr(r), ozr(r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) > 0;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) > 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) > 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = mpfr_greater_p(orr(l), orr(r));
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
}

void
ovm_x_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_float:
	    l->v.w = rat_get_d(l->v.r) != r->v.d;
	    break;
	case t_rat:
	    l->v.w = orat_cmp(&l->v.r, &r->v.r) != 0;
	    break;
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) != 0;
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_x_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    if (orat_add_si(&l->v.r, &l->v.r, r->v.w))
		check_rat(l);
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
		mpq_set_si(oqr(r), r->v.w, 1);
		mpq_add(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = rat_get_d(l->v.r) + r->v.d;
	    break;
	case t_mpz:
	    mpz_set_ui(mpq_denref(oqr(r)), 1);
	    goto mpq;
	case t_rat:
	    if (orat_add(&l->v.r, &l->v.r, &r->v.r))
		break;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = rat_get_d(l->v.r) + r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_si(oqi(l), 0, 1);
	    cqq_add(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_cdd;
	    mpc_set_d(occ(l), rat_get_d(l->v.r), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_x_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    if (orat_sub_si(&l->v.r, &l->v.r, r->v.w))
		check_rat(l);
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
		mpq_set_si(oqr(r), r->v.w, 1);
		mpq_sub(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = rat_get_d(l->v.r) - r->v.d;
	    break;
	case t_mpz:
	    mpz_set_ui(mpq_denref(oqr(r)), 1);
	    goto mpq;
	case t_rat:
	    if (orat_sub(&l->v.r, &l->v.r, &r->v.r))
		break;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = rat_get_d(l->v.r) - r->v.dd;
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_si(oqi(l), 0, 1);
	    cqq_sub(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_cdd;
	    mpc_set_d(occ(l), rat_get_d(l->v.r), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_x_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    if (orat_mul_si(&l->v.r, &l->v.r, r->v.w))
		check_rat(l);
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
		mpq_set_si(oqr(r), r->v.w, 1);
		mpq_mul(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = rat_get_d(l->v.r) * r->v.d;
	    break;
	case t_mpz:
	    mpz_set_ui(mpq_denref(oqr(r)), 1);
	    goto mpq;
	case t_rat:
	    if (orat_mul(&l->v.r, &l->v.r, &r->v.r))
		break;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = rat_get_d(l->v.r) * r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_si(oqi(l), 0, 1);
	    cqq_mul(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_cdd;
	    mpc_set_d(occ(l), rat_get_d(l->v.r), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_x_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    ovm_raise(except_floating_point_error);
	case t_word:
	    if (unlikely(r->v.w) == 0)
		ovm_raise(except_floating_point_error);
	    if (orat_div_si(&l->v.r, &l->v.r, r->v.w))
		check_rat(l);
	    else {
		l->t = t_mpq;
		mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
		mpq_set_si(oqr(r), r->v.w, 1);
		mpq_div(oqr(l), oqr(l), oqr(r));
		check_mpq(l);
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = rat_get_d(l->v.r) / r->v.d;
	    break;
	case t_mpz:
	    mpz_set_ui(mpq_denref(oqr(r)), 1);
	    goto mpq;
	case t_rat:
	    if (orat_div(&l->v.r, &l->v.r, &r->v.r))
		break;
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    l->v.dd = rat_get_d(l->v.r) / r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    l->t = t_cqq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_set_si(oqi(l), 0, 1);
	    cqq_div(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_cdd;
	    mpc_set_d(occ(l), rat_get_d(l->v.r), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_x_trunc2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	    l->t = t_mpz;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    if (r->v.w > 0)
		mpz_mul_ui(ozr(r), mpq_denref(oqr(l)), r->v.w);
	    else {
		mpz_set_si(ozr(r), r->v.w);
		mpz_mul(ozr(r), mpq_denref(oqr(l)), ozr(r));
	    }
	    mpz_tdiv_q(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_float:
	    ovm_trunc_d(l, rat_get_d(l->v.r) / r->v.d);
	    break;
	case t_mpz:
	    l->t = t_mpz;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpz_tdiv_q(ozr(l), ozr(l), ozr(r));
	    check_mpz(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    l->t = t_mpz;
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpz_tdiv_q(ozr(l), ozr(l), mpq_denref(oqr(l)));
	    check_mpz(l);
	    break;
	case t_mpr:
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    ovm_trunc_r(l, orr(r));
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}

void
ovm_x_rem(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    if (r->v.w == 0)
		ovm_raise(except_floating_point_error);
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    if (r->v.w > 0)
		mpz_mul_ui(ozr(r), mpq_denref(oqr(l)), r->v.w);
	    else {
		mpz_set_si(ozr(r), r->v.w);
		mpz_mul(ozr(r), mpq_denref(oqr(l)), ozr(r));
	    }
	    mpz_tdiv_r(ozr(l), ozr(l), ozr(r));
	    mpq_canonicalize(oqr(l));
	    check_mpq(l);
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = fmod(rat_get_d(l->v.r), r->v.d);
	    break;
	case t_mpz:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpz_mul(ozr(r), mpq_denref(oqr(l)), ozr(r));
	    mpz_tdiv_r(ozr(l), ozr(l), ozr(r));
	    check_mpq(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    l->t = t_mpq;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpz_tdiv_r(ozr(l), ozr(l), mpq_denref(oqr(l)));
	    mpz_mul(mpq_denref(oqr(l)), mpq_denref(oqr(l)), mpq_denref(oqr(r)));
	    mpq_canonicalize(oqr(r));
	    check_mpq(l);
	    break;
	case t_mpr:
	    l->t = t_mpr;
	    mpq_set_si(oqr(l), rat_num(l->v.r), rat_den(l->v.r));
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    mpfr_fmod(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
}
