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
ovm_r_lt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
    l->v.w = mpfr_less_p(orr(l), orr(r));
}

void
ovm_r_le(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
    l->v.w = mpfr_lessequal_p(orr(l), orr(r));
}

void
ovm_r_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = mpfr_zero_p(orr(l));
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_rat:
	    mpfr_set_d(orr(r), rat_get_d(r->v.r), thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_mpr:
	    l->v.w = mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 0;
	    break;
    }
}

void
ovm_r_ge(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
    l->v.w = mpfr_greaterequal_p(orr(l), orr(r));
}

void
ovm_r_gt(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
    l->v.w = mpfr_greater_p(orr(l), orr(r));
}

void
ovm_r_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_void:
	    l->v.w = !mpfr_zero_p(orr(l));
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	case t_mpr:
	    l->v.w = !mpfr_equal_p(orr(l), orr(r));
	    break;
	default:
	    l->v.w = 1;
	    break;
    }
}

void
ovm_r_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpr:
	    mpfr_add(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cqq:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpr:
	    mpfr_sub(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cqq:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_si(orr(r), 0, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpr:
	    mpfr_mul(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_cqq:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_si(orr(r), 0, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_mpr:
	    mpfr_div(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_cqq:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_trunc2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	     ovm_raise(except_floating_point_error);
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
    ovm_trunc_r(l, orr(r));
}

void
ovm_trunc_r(oregister_t *r, ompr_t v)
{
    GET_THREAD_SELF()
    r->t = t_mpz;
    mpfr_div(orr(r), orr(r), v, thr_rnd);
    mpfr_trunc(orr(r), orr(r));
    if (unlikely(!mpfr_number_p(orr(r))))
	othread_kill(SIGFPE);
    mpfr_get_z(ozr(r), orr(r), GMP_RNDZ);
    check_mpz(r);
}

void
ovm_r_rem(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_si(orr(r), 0, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    break;
	 default:
	     ovm_raise(except_not_a_real_number);
    }
    mpfr_fmod(orr(l), orr(l), orr(r), thr_rnd);
}

void
ovm_r_complex(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    return;
	case t_word:
	    mpfr_set_si(ori(l), r->v.w, thr_rnd);
	    break;
	case t_float:
	    mpfr_set_d(ori(l), r->v.d, thr_rnd);
	    break;
	case t_mpz:
	    mpfr_set_z(ori(l), ozr(r), thr_rnd);
	    break;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(ori(l), oqr(r), thr_rnd);
	    break;
	case t_mpr:
	    mpfr_set(ori(l), orr(r), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_real_number);
    }
    l->t = t_mpc;
    check_mpc(l);
}

void
ovm_r_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    goto mpr;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    goto mpr;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    goto mpr;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    goto mpr;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    goto mpr;
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    goto mpr;
	case t_mpr:
	mpr:
	    mpfr_atan2(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    if (mpfr_zero_p(orr(l))) {
		if (real(r->v.dd) >= 0)
		    mpfr_set_ui(orr(l), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(l), thr_rnd);
	    }
	    else {
		mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_cqq:
	    if (mpfr_zero_p(orr(l))) {
		if (mpq_sgn(oqr(r)) >= 0)
		    mpfr_set_ui(orr(l), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(l), thr_rnd);
	    }
	    else {
		mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
		goto mpc;
	    }
	    break;
	case t_mpc:
	    if (mpfr_zero_p(orr(l))) {
		if (mpfr_sgn(orr(r)) >= 0)
		    mpfr_set_ui(orr(l), 0, thr_rnd);
		else
		    mpfr_const_pi(orr(l), thr_rnd);
	    }
	    else {
	    mpc:
		l->t = t_mpc;
		mpc_set_fr(occ(l), orr(l), thr_rndc);
		mpc_div(occ(l), occ(l), occ(r), thr_rndc);
		mpc_atan(occ(l), occ(l), thr_rndc);
		check_mpc(l);
	    }
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_pow(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(l), 1, thr_rnd);
	    break;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    goto mpr;
	case t_float:
	    mpfr_set_d(orr(r), r->v.d, thr_rnd);
	    goto mpr_flt;
	case t_mpz:
	    mpfr_set_z(orr(r), ozr(r), thr_rnd);
	    goto mpr;
	case t_rat:
	    mpq_set_si(oqr(r), rat_num(r->v.r), rat_den(r->v.r));
	case t_mpq:
	    mpfr_set_q(orr(r), oqr(r), thr_rnd);
	    goto mpr_rat;
	case t_mpr:
	mpr_flt:
	    if (mpfr_number_p(orr(r)) && !mpfr_integer_p(orr(r))) {
	    mpr_rat:
		if (mpfr_number_p(orr(l)) && mpfr_sgn(orr(l)) < 0) {
		    mpc_set_fr(occ(r), orr(r), thr_rndc);
		    goto mpc;
		}
	    }
	mpr:
	    l->t = t_mpr;
	    mpfr_pow(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    goto mpc;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	case t_mpc:
	mpc:
	    l->t = t_mpc;
	    mpc_set_fr(occ(l), orr(l), thr_rndc);
	    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_r_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    mpfr_set_ui(orr(r), 0, thr_rnd);
	    goto mpr;
	case t_word:
	    mpfr_set_si(orr(r), r->v.w, thr_rnd);
	    goto mpr;
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
	    goto mpr;
	case t_mpr:
	mpr:
	    mpfr_hypot(orr(l), orr(l), orr(r), thr_rnd);
	    break;
	case t_cdd:
	    mpc_set_d_d(occ(r), real(r->v.dd), imag(r->v.dd), thr_rndc);
	    goto mpc;
	case t_cqq:
	    mpc_set_q_q(occ(r), oqr(r), oqi(r), thr_rndc);
	case t_mpc:
	mpc:
	    mpfr_abs(orr(l), orr(l), thr_rnd);
	    mpfr_hypot(ori(l), orr(r), ori(r), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
