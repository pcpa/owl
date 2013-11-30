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
ovm_cc_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    mpfr_set_d(orr(r), real(r->v.dd), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_d(ori(r), imag(r->v.dd), thr_rnd);
		l->v.w = mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 0;
	    break;
	case t_cqq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_q(ori(r), oqi(r), thr_rnd);
		l->v.w = mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 0;
	    break;
	case t_mpc:
	    l->v.w = mpfr_equal_p(orr(l), orr(r)) &&
		     mpfr_equal_p(ori(l), ori(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_cc_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    mpfr_set_d(orr(r), real(r->v.dd), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_d(ori(r), imag(r->v.dd), thr_rnd);
		l->v.w = !mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 1;
	    break;
	case t_cqq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_q(ori(r), oqi(r), thr_rnd);
		l->v.w = !mpfr_equal_p(ori(l), ori(r));
	    }
	    else
		l->v.w = 1;
	    break;
	case t_mpc:
	    l->v.w = !mpfr_equal_p(orr(l), orr(r)) ||
		     !mpfr_equal_p(ori(l), ori(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_cc_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_si(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpc_set_d(occ(r), rat_get_d(r->v.r), thr_rndc);
	    break;
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_si(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpc_set_d(occ(r), rat_get_d(r->v.r), thr_rndc);
	    break;
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_si(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpc_set_d(occ(r), rat_get_d(r->v.r), thr_rndc);
	    break;
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_si(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpc_set_d(occ(r), rat_get_d(r->v.r), thr_rndc);
	    break;
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_ui(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
    mpc_atan(occ(l), occ(l), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_pow(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpc_set_ui(occ(r), 0, thr_rndc);
	    break;
	case t_word:
	    mpc_set_si(occ(r), r->v.w, thr_rndc);
	    break;
	case t_float:
	    mpc_set_d(occ(r), r->v.d, thr_rndc);
	    break;
	case t_mpz:
	    mpc_set_z(occ(r), ozr(r), thr_rndc);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpc_set_q(occ(r), oqr(r), thr_rndc);
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    break;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    break;
	case t_mpc:
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
    check_mpc(l);
}

void
ovm_cc_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    goto mpr;
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    goto mpr;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    goto mpr;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	case t_mpr:
	mpr:
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    goto mpc;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	case t_mpc:
	mpc:
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(ori(l), orr(r), ori(r), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
    l->t = t_mpr;
}
