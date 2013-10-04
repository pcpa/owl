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
