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
ovm_qq_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    l->v.w = mpq_get_d(oqr(l)) == real(r->v.dd) &&
		     mpq_get_d(oqi(l)) == imag(r->v.dd);
	    break;
	case t_cqq:
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) == 0 &&
		     mpq_cmp(oqi(l), oqi(r)) == 0;
	    break;
	case t_mpc:
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_q(ori(l), oqi(l), thr_rnd);
		l->v.w = mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 0;
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_qq_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    l->v.w = mpq_get_d(oqr(l)) != real(r->v.dd) ||
		     mpq_get_d(oqi(l)) != imag(r->v.dd);
	    break;
	case t_cqq:
	    l->v.w = mpq_cmp(oqr(l), oqr(r)) != 0 ||
		     mpq_cmp(oqi(l), oqi(r)) != 0;
	    break;
	case t_mpc:
	    mpfr_set_q(orr(l), oqr(l), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_q(ori(l), oqi(l), thr_rnd);
		l->v.w = !mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 1;
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_qq_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    mpq_set_si(oqr(r), r->v.w, 1);
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd += r->v.d;
	    break;
	case t_mpz:
	    mpq_set_z(oqr(r), ozr(r));
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    mpq_add(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd += r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    cqq_add(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    mpq_set_si(oqr(r), r->v.w, 1);
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd -= r->v.d;
	    break;
	case t_mpz:
	    mpq_set_z(oqr(r), ozr(r));
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    mpq_sub(oqr(l), oqr(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd -= r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    cqq_sub(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_word;
	    l->v.w = 0;
	    break;
	case t_word:
	    mpq_set_si(oqr(r), r->v.w, 1);
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    mpq_mul(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd *= r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    mpq_set_z(oqr(r), ozr(r));
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    mpq_mul(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    mpq_mul(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    mpq_mul(oqr(l), oqr(l), oqr(r));
	    mpq_mul(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd *= r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    cqq_mul(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    ovm_raise(except_floating_point_error);
	case t_word:
	    mpq_set_si(oqr(r), r->v.w, 1);
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpq_div(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_float:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd /= r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    mpq_set_z(oqr(r), ozr(r));
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpq_div(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpq_div(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpq:
	    mpq_div(oqr(l), oqr(l), oqr(r));
	    mpq_div(oqi(l), oqi(l), oqr(r));
	    check_cqq(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd /= r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    cqq_div(oqq(l), oqq(l), oqq(r));
	    check_cqq(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = mpq_sgn(oqr(l)) >= 0 ? M_PI_2 : -M_PI_2;
	    }
	    else {
		mpc_set_ui(occ(r), 0, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		if (r->v.w) {
		    real(r->v.dd) = r->v.w;
		    imag(r->v.dd) = 0.0;
		    goto cdd;
		}
	    flt:
		l->t = t_float;
		l->v.d = mpq_sgn(oqr(l)) >= 0.0 ? M_PI_2 : -M_PI_2;
	    }
	    else {
		mpc_set_si(occ(r), r->v.w, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_float:
	    if (r->v.d) {
		real(r->v.dd) = r->v.d;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    goto flt;
	case t_mpz:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpz_get_d(ozr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    goto mpc;
	case t_rat:
	    if (!cfg_float_format) {
		real(r->v.dd) = rat_get_d(r->v.r);
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    goto mpc;
	case t_mpq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    goto mpc;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    goto mpc;
	case t_cdd:
	cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd = catan(l->v.dd / r->v.dd);
	    check_cdd(l);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	case t_mpc:
	mpc:
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    l->t = t_mpc;
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    mpc_atan(occ(l), occ(l), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_pow(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = 1;
	    }
	    else {
		mpc_set_ui(occ(r), 1, thr_rndc);
		goto mpc;
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		real(r->v.dd) = r->v.w;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    goto mpc;
	    break;
	case t_float:
	    real(r->v.dd) = r->v.d;
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpz:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpz_get_d(ozr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    goto mpc;
	case t_rat:
	    if (!cfg_float_format) {
		real(r->v.dd) = rat_get_d(r->v.r);
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    goto mpc;
	case t_mpq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    goto mpc;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    goto mpc;
	case t_cdd:
	cdd:
	    l->t = t_cdd;
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->v.dd = cpow(l->v.dd, r->v.dd);
	    check_cdd(l);
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		real(r->v.dd) = mpq_get_d(oqr(r));
		imag(r->v.dd) = mpq_get_d(oqi(r));
		goto cdd;
	    }
	case t_mpc:
	mpc:
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    l->t = t_mpc;
	    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_qq_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l)));
	    }
	    else {
		l->t = t_mpr;
		mpc_set_q_q(occ(l), oqr(r), oqi(l), thr_rndc);
		mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    }
	    break;
	case t_word:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l))),
			       r->v.w);
	    }
	    else {
		mpfr_set_si(orr(r), r->v.w, thr_rnd);
		goto mpr;
	    }
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = hypot(hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l))), r->v.d);
	    break;
	case t_mpz:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l))),
			       mpz_get_d(ozr(r)));
	    }
	    else {
		mpfr_set_z(orr(r), ozr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_rat:
	    if (!cfg_float_format) {
		l->t = t_float;
		l->v.d = hypot(hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l))),
			       rat_get_d(r->v.r));
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
		l->v.d = hypot(hypot(mpq_get_d(oqr(l)), mpq_get_d(oqi(l))),
			       mpq_get_d(oqr(r)));
	    }
	    else {
		mpfr_set_q(orr(r), oqr(r), thr_rnd);
		goto mpr;
	    }
	    break;
	case t_mpr:
	mpr:
	    l->t = t_mpr;
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	cdd:
	    real(l->v.dd) = mpq_get_d(oqr(l));
	    imag(l->v.dd) = mpq_get_d(oqi(l));
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)),
			   hypot(real(r->v.dd), imag(r->v.dd)));
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
	    mpc_set_q_q(occ(l), oqr(l), oqi(l), thr_rndc);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(ori(l), orr(r), ori(r), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
