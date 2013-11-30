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
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd += r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_add(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
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
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    l->v.dd -= r->v.dd;
	    check_cdd(l);
	    break;
	case t_mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_sub(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
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
	    check_mpc(l);
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
	    check_mpc(l);
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
	    check_mpc(l);
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
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_atan2(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    goto flt;
	case t_word:
	    if (r->v.w) {
		real(r->v.dd) = r->v.w;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	flt:
	    l->t = t_float;
	    l->v.d = real(l->v.dd) >= 0.0 ? M_PI_2 : -M_PI_2;
	    break;
	case t_float:
	    if (r->v.d) {
		real(r->v.dd) = r->v.d;
		imag(r->v.dd) = 0.0;
		goto cdd;
	    }
	    goto flt;
	case t_mpz:
	    real(r->v.dd) = mpz_get_d(ozr(r));
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_rat:
	    real(r->v.dd) = rat_get_d(r->v.r);
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    goto mpc;
	case t_cdd:
	cdd:
	    l->v.dd = catan(l->v.dd / r->v.dd);
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    goto cdd;
	case t_mpc:
	mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_div(occ(l), occ(l), occ(r), thr_rndc);
	    mpc_atan(occ(l), occ(l), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_pow(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_float;
	    l->v.d = 1.0;
	    break;
	case t_word:
	    real(r->v.dd) = r->v.w;
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_float:
	    real(r->v.dd) = r->v.d;
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpz:
	    real(r->v.dd) = mpz_get_d(ozr(r));
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_rat:
	    real(r->v.dd) = rat_get_d(r->v.r);
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = 0.0;
	    goto cdd;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    goto mpc;
	case t_cdd:
	cdd:
	    l->v.dd = cpow(l->v.dd, r->v.dd);
	    check_cdd(l);
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    goto cdd;
	case t_mpc:
	mpc:
	    l->t = t_mpc;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpc_pow(occ(l), occ(l), occ(r), thr_rndc);
	    check_mpc(l);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}

void
ovm_dd_hypot(oregister_t *l, oregister_t *r)
{
    switch (r->t) {
	case t_void:
	    l->t = t_float;
	    l->v.d = cabs(l->v.dd);
	    break;
	case t_word:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)), r->v.w);
	    break;
	case t_float:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)), r->v.d);
	    break;
	case t_mpz:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)),
			   mpz_get_d(ozr(r)));
	    break;
	case t_rat:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)),
			   rat_get_d(r->v.r));
	    break;
	case t_mpq:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)),
			   mpq_get_d(oqr(r)));
	    break;
	case t_mpr:
	    mpc_set_fr(occ(r), orr(r), thr_rndc);
	    goto mpc;
	case t_cdd:
	cdd:
	    l->t = t_float;
	    l->v.d = hypot(hypot(real(l->v.dd), imag(l->v.dd)),
			   hypot(real(r->v.dd), imag(r->v.dd)));
	    break;
	case t_cqq:
	    real(r->v.dd) = mpq_get_d(oqr(r));
	    imag(r->v.dd) = mpq_get_d(oqi(r));
	    goto cdd;
	case t_mpc:
	mpc:
	    l->t = t_mpr;
	    mpc_set_d_d(occ(l), real(l->v.dd), imag(l->v.dd), thr_rndc);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    mpfr_hypot(ori(l), orr(r), ori(r), thr_rnd);
	    mpfr_hypot(orr(l), orr(l), ori(l), thr_rnd);
	    break;
	default:
	    ovm_raise(except_not_a_number);
    }
}
