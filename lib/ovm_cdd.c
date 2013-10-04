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
ovm_dd_eq(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    l->v.w = l->v.dd == r->v.dd;
	    break;
	case t_cqq:
	    l->v.w = mpq_get_d(oqr(r)) == real(l->v.dd) &&
		     mpq_get_d(oqi(r)) == imag(l->v.dd);
	    break;
	case t_mpc:
	    mpfr_set_d(orr(l), real(l->v.dd), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_d(ori(l), imag(l->v.dd), thr_rnd);
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
ovm_dd_ne(oregister_t *l, oregister_t *r)
{
    l->t = t_word;
    switch (r->t) {
	case t_cdd:
	    l->v.w = l->v.dd != r->v.dd;
	    break;
	case t_cqq:
	    l->v.w = mpq_get_d(oqr(r)) != real(l->v.dd) ||
		     mpq_get_d(oqi(r)) != imag(l->v.dd);
	    break;
	case t_mpc:
	    mpfr_set_d(orr(l), real(l->v.dd), thr_rnd);
	    if (mpfr_equal_p(orr(l), orr(r))) {
		mpfr_set_d(ori(l), imag(l->v.dd), thr_rnd);
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
ovm_dd_add(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.dd += r->v.w;
	    break;
	case t_float:
	    l->v.dd += r->v.d;
	    break;
	case t_mpz:
	    l->v.dd += mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.dd += rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.dd += mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->v.dd += r->v.dd;
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd += r->v.dd;
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_sub(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    break;
	case t_word:
	    l->v.dd -= r->v.w;
	    break;
	case t_float:
	    l->v.dd -= r->v.d;
	    break;
	case t_mpz:
	    l->v.dd -= mpz_get_d(ozr(r));
	    break;
	case t_rat:
	    l->v.dd -= rat_get_d(r->v.r);
	    break;
	case t_mpq:
	    l->v.dd -= mpq_get_d(oqr(r));
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->v.dd -= r->v.dd;
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd -= r->v.dd;
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_mul(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.dd *= 0.0;
	    check_cdd(l);
	    break;
	case t_word:
	    l->v.dd *= r->v.w;
	    check_cdd(l);
	    break;
	case t_float:
	    l->v.dd *= r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    l->v.dd *= mpz_get_d(ozr(r));
	    check_cdd(l);
	    break;
	case t_rat:
	    l->v.dd *= rat_get_d(r->v.r);
	    check_cdd(l);
	    break;
	case t_mpq:
	    l->v.dd *= mpq_get_d(oqr(r));
	    check_cdd(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->v.dd *= r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd *= r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_mul(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_div(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->v.dd /= 0.0;
	    check_cdd(l);
	    break;
	case t_word:
	    l->v.dd /= r->v.w;
	    check_cdd(l);
	    break;
	case t_float:
	    l->v.dd /= r->v.d;
	    check_cdd(l);
	    break;
	case t_mpz:
	    l->v.dd /= mpz_get_d(ozr(r));
	    check_cdd(l);
	    break;
	case t_rat:
	    l->v.dd /= rat_get_d(r->v.r);
	    check_cdd(l);
	    break;
	case t_mpq:
	    l->v.dd /= mpq_get_d(oqr(r));
	    check_cdd(l);
	    break;
	case t_mpr:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	case t_cdd:
	    l->v.dd /= r->v.dd;
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd /= r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
