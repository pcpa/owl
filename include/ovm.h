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

#ifndef _ovm_h
#define _ovm_h

#include "otypes.h"

#define oqq(r)			&((r)->qq)
#define oqr(r)			cqq_realref(oqq(r))
#define oqi(r)			cqq_imagref(oqq(r))
#define ozr(r)			mpq_numref(oqr(r))
#define ozs(r)			mpq_denref(oqr(r))
#define occ(r)			&((r)->cc)
#define orr(r)			mpc_realref(occ(r))
#define ori(r)			mpc_imagref(occ(r))
#define othrow(xcpt)		ovm_raise(xcpt)

#define check_mpz(R)							\
    do {								\
	if (mpz_fits_slong_p(ozr(R))) {					\
	    (R)->t = t_word;						\
	    (R)->v.w = mpz_get_si(ozr(R));				\
	}								\
    } while (0)
#define check_rat(R)							\
    do {								\
	if (rat_den((R)->v.r) == 1) {					\
	    (R)->t = t_word;						\
	    (R)->v.w = rat_num((R)->v.r);				\
	}								\
    } while (0)
#define check_mpq(R)							\
    do {								\
	if (mpz_fits_slong_p(ozr(R)) &&					\
	    mpz_fits_slong_p(mpq_denref(oqr(R)))) {			\
	    if (mpz_cmp_ui(mpq_denref(oqr(R)), 1) == 0) {		\
		(R)->t = t_word;					\
		(R)->v.w = mpz_get_si(ozr(R));				\
	    }								\
	    else {							\
		(R)->t = t_rat;						\
		rat_num((R)->v.r) = mpz_get_si(ozr(R));			\
		rat_den((R)->v.r) = mpz_get_si(mpq_denref(oqr(R)));	\
	    }								\
	}								\
    } while (0)
#define check_cdd(R)							\
    do {								\
	if (imag((R)->v.dd) == 0.0)					\
	    (R)->t = t_float;						\
    } while (0)
#define check_cqq(R)							\
    do {								\
	if (!mpq_sgn(oqi(R))) {						\
	    if (mpz_cmp_ui(mpq_denref(oqr(R)), 1))			\
		(R)->t = t_mpq;						\
	    else if (mpz_fits_slong_p(ozr(R))) {			\
		(R)->t = t_word;					\
		(R)->v.w = mpz_get_si(ozr(R));				\
	    }								\
	    else							\
		(R)->t = t_mpz;						\
	}								\
    } while (0)
#define check_mpc(R)							\
    do {								\
	if (mpfr_zero_p(ori(R)))					\
	    (R)->t = t_mpr;						\
    } while (0)

/*
 * Types
 */
enum oexcept {
    except_segmentation_violation = -1,
    except_nothing = 0,
    except_out_of_memory,
    except_floating_point_error,
    except_out_of_bounds,
    except_null_dereference,
    except_type_mismatch,
    except_input_output_error,
    except_invalid_argument,
    except_not_a_number,
    except_not_an_integer,
    except_not_a_real_number,
    except_not_a_rational_number,
    except_not_a_32_bits_integer,
    except_not_a_finite_real_number,
    except_shift_too_large,
    except_unhandled_exception,
};

/*
 * Prototypes
 */
extern void
init_vm(void);

extern void
finish_vm(void);

extern void
ovm(othread_t *thread);

extern void
ovm_raise(oint32_t xcpt) noreturn;

extern oint32_t
ovm_get_shift(oregister_t *r);

extern void
ovm_case(oregister_t *r);

extern void
ovm_offset(oregister_t *r);

extern void
ovm_coerce_w(oregister_t *r);

extern void
ovm_coerce_uw(oregister_t *r);

extern void
ovm_coerce_d(oregister_t *r);

#if __WORDSIZE == 32
extern void
ovm_coerce_ww(oregister_t *r);

extern void
ovm_coerce_uwuw(oregister_t *r);
#endif

extern void
ovm_not(oregister_t *r);

extern void
ovm_bool(oregister_t *r);

extern void
ovm_com(oregister_t *r);

extern void
ovm_neg(oregister_t *r);

extern void
ovm_copy(oregister_t *r, oobject_t o);

extern void
ovm_load(oregister_t *r, oobject_t *p);

extern void
ovm_store_i8(oregister_t *r, oint8_t *o);

extern void
ovm_store_i16(oregister_t *r, oint16_t *o);

extern void
ovm_store_i32(oregister_t *r, oint32_t *o);

extern void
ovm_store_i64(oregister_t *r, oint64_t *o);

extern void
ovm_store_f32(oregister_t *r, ofloat32_t *o);

extern void
ovm_store_f64(oregister_t *r, ofloat64_t *o);

extern void
ovm_store(oregister_t *r, oobject_t *p, oint32_t t);

extern void
ovm_store_w(oword_t w, oobject_t *p, oint32_t t);

extern void
ovm_store_d(ofloat_t d, oobject_t *p, oint32_t t);

extern void
ovm_o_copy(oregister_t *l, oregister_t *r);

extern void
ovm_inc(oregister_t *r);
extern void
ovm_dec(oregister_t *r);

extern void
ovm_v_lt(oregister_t *l, oregister_t *r);
extern void
ovm_v_le(oregister_t *l, oregister_t *r);
extern void
ovm_v_eq(oregister_t *l, oregister_t *r);
extern void
ovm_v_ge(oregister_t *l, oregister_t *r);
extern void
ovm_v_gt(oregister_t *l, oregister_t *r);
extern void
ovm_v_ne(oregister_t *l, oregister_t *r);
extern void
ovm_v_and(oregister_t *l, oregister_t *r);
extern void
ovm_v_or(oregister_t *l, oregister_t *r);
extern void
ovm_v_xor(oregister_t *l, oregister_t *r);
extern void
ovm_v_add(oregister_t *l, oregister_t *r);
extern void
ovm_v_sub(oregister_t *l, oregister_t *r);
extern void
ovm_v_mul(oregister_t *l, oregister_t *r);
extern void
ovm_v_div(oregister_t *l, oregister_t *r);
extern void
ovm_v_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_v_rem(oregister_t *l, oregister_t *r);

extern void
ovm_w_lt(oregister_t *l, oregister_t *r);
extern void
ovm_w_le(oregister_t *l, oregister_t *r);
extern void
ovm_w_eq(oregister_t *l, oregister_t *r);
extern void
ovm_w_ge(oregister_t *l, oregister_t *r);
extern void
ovm_w_gt(oregister_t *l, oregister_t *r);
extern void
ovm_w_ne(oregister_t *l, oregister_t *r);
extern void
ovm_w_and(oregister_t *l, oregister_t *r);
extern void
ovm_w_or(oregister_t *l, oregister_t *r);
extern void
ovm_w_xor(oregister_t *l, oregister_t *r);
extern void
ovm_w_add(oregister_t *l, oregister_t *r);
extern void
ovm_w_sub(oregister_t *l, oregister_t *r);
extern void
ovm_w_mul(oregister_t *l, oregister_t *r);
extern void
ovm_w_div(oregister_t *l, oregister_t *r);
extern void
ovm_w_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_w_rem(oregister_t *l, oregister_t *r);

extern void
ovm_d_lt(oregister_t *l, oregister_t *r);
extern void
ovm_d_le(oregister_t *l, oregister_t *r);
extern void
ovm_d_eq(oregister_t *l, oregister_t *r);
extern void
ovm_d_ge(oregister_t *l, oregister_t *r);
extern void
ovm_d_gt(oregister_t *l, oregister_t *r);
extern void
ovm_d_ne(oregister_t *l, oregister_t *r);
extern void
ovm_d_add(oregister_t *l, oregister_t *r);
extern void
ovm_d_sub(oregister_t *l, oregister_t *r);
extern void
ovm_d_mul(oregister_t *l, oregister_t *r);
extern void
ovm_d_div(oregister_t *l, oregister_t *r);
extern void
ovm_d_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_trunc_d(oregister_t *r, ofloat_t d);
extern void
ovm_d_rem(oregister_t *l, oregister_t *r);

extern void
ovm_z_lt(oregister_t *l, oregister_t *r);
extern void
ovm_z_le(oregister_t *l, oregister_t *r);
extern void
ovm_z_eq(oregister_t *l, oregister_t *r);
extern void
ovm_z_ge(oregister_t *l, oregister_t *r);
extern void
ovm_z_gt(oregister_t *l, oregister_t *r);
extern void
ovm_z_ne(oregister_t *l, oregister_t *r);
extern void
ovm_z_and(oregister_t *l, oregister_t *r);
extern void
ovm_z_or(oregister_t *l, oregister_t *r);
extern void
ovm_z_xor(oregister_t *l, oregister_t *r);
extern void
ovm_z_add(oregister_t *l, oregister_t *r);
extern void
ovm_z_sub(oregister_t *l, oregister_t *r);
extern void
ovm_z_mul(oregister_t *l, oregister_t *r);
extern void
ovm_z_div(oregister_t *l, oregister_t *r);
extern void
ovm_z_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_z_rem(oregister_t *l, oregister_t *r);

extern void
ovm_x_lt(oregister_t *l, oregister_t *r);
extern void
ovm_x_le(oregister_t *l, oregister_t *r);
extern void
ovm_x_eq(oregister_t *l, oregister_t *r);
extern void
ovm_x_ge(oregister_t *l, oregister_t *r);
extern void
ovm_x_gt(oregister_t *l, oregister_t *r);
extern void
ovm_x_ne(oregister_t *l, oregister_t *r);
extern void
ovm_x_add(oregister_t *l, oregister_t *r);
extern void
ovm_x_sub(oregister_t *l, oregister_t *r);
extern void
ovm_x_mul(oregister_t *l, oregister_t *r);
extern void
ovm_x_div(oregister_t *l, oregister_t *r);
extern void
ovm_x_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_x_rem(oregister_t *l, oregister_t *r);

extern void
ovm_q_lt(oregister_t *l, oregister_t *r);
extern void
ovm_q_le(oregister_t *l, oregister_t *r);
extern void
ovm_q_eq(oregister_t *l, oregister_t *r);
extern void
ovm_q_ge(oregister_t *l, oregister_t *r);
extern void
ovm_q_gt(oregister_t *l, oregister_t *r);
extern void
ovm_q_ne(oregister_t *l, oregister_t *r);
extern void
ovm_q_add(oregister_t *l, oregister_t *r);
extern void
ovm_q_sub(oregister_t *l, oregister_t *r);
extern void
ovm_q_mul(oregister_t *l, oregister_t *r);
extern void
ovm_q_div(oregister_t *l, oregister_t *r);
extern void
ovm_q_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_q_rem(oregister_t *l, oregister_t *r);

extern void
ovm_r_lt(oregister_t *l, oregister_t *r);
extern void
ovm_r_le(oregister_t *l, oregister_t *r);
extern void
ovm_r_eq(oregister_t *l, oregister_t *r);
extern void
ovm_r_ge(oregister_t *l, oregister_t *r);
extern void
ovm_r_gt(oregister_t *l, oregister_t *r);
extern void
ovm_r_ne(oregister_t *l, oregister_t *r);
extern void
ovm_r_add(oregister_t *l, oregister_t *r);
extern void
ovm_r_sub(oregister_t *l, oregister_t *r);
extern void
ovm_r_mul(oregister_t *l, oregister_t *r);
extern void
ovm_r_div(oregister_t *l, oregister_t *r);
extern void
ovm_r_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_trunc_r(oregister_t *r, ompr_t v);
extern void
ovm_r_rem(oregister_t *l, oregister_t *r);

extern void
ovm_dd_eq(oregister_t *l, oregister_t *r);
extern void
ovm_dd_ne(oregister_t *l, oregister_t *r);
extern void
ovm_dd_add(oregister_t *l, oregister_t *r);
extern void
ovm_dd_sub(oregister_t *l, oregister_t *r);
extern void
ovm_dd_mul(oregister_t *l, oregister_t *r);
extern void
ovm_dd_div(oregister_t *l, oregister_t *r);

extern void
ovm_qq_eq(oregister_t *l, oregister_t *r);
extern void
ovm_qq_ne(oregister_t *l, oregister_t *r);
extern void
ovm_qq_add(oregister_t *l, oregister_t *r);
extern void
ovm_qq_sub(oregister_t *l, oregister_t *r);
extern void
ovm_qq_mul(oregister_t *l, oregister_t *r);
extern void
ovm_qq_div(oregister_t *l, oregister_t *r);

extern void
ovm_cc_eq(oregister_t *l, oregister_t *r);
extern void
ovm_cc_ne(oregister_t *l, oregister_t *r);
extern void
ovm_cc_add(oregister_t *l, oregister_t *r);
extern void
ovm_cc_sub(oregister_t *l, oregister_t *r);
extern void
ovm_cc_mul(oregister_t *l, oregister_t *r);
extern void
ovm_cc_div(oregister_t *l, oregister_t *r);

extern void
ovm_o_lt(oregister_t *l, oregister_t *r);
extern void
ovm_o_le(oregister_t *l, oregister_t *r);
extern void
ovm_o_eq(oregister_t *l, oregister_t *r);
extern void
ovm_o_ge(oregister_t *l, oregister_t *r);
extern void
ovm_o_gt(oregister_t *l, oregister_t *r);
extern void
ovm_o_ne(oregister_t *l, oregister_t *r);
extern void
ovm_o_and(oregister_t *l, oregister_t *r);
extern void
ovm_o_or(oregister_t *l, oregister_t *r);
extern void
ovm_o_xor(oregister_t *l, oregister_t *r);
extern void
ovm_o_mul2(oregister_t *l, oregister_t *r);
extern void
ovm_mul2exp(oregister_t *r, oint32_t s);
extern void
ovm_o_div2(oregister_t *l, oregister_t *r);
extern void
ovm_div2exp(oregister_t *r, oint32_t s);
extern void
ovm_o_shl(oregister_t *l, oregister_t *r);
extern void
ovm_shl(oregister_t *r, oint32_t s);
extern void
ovm_o_shr(oregister_t *l, oregister_t *r);
extern void
ovm_shr(oregister_t *r, oint32_t s);
extern void
ovm_o_add(oregister_t *l, oregister_t *r);
extern void
ovm_o_sub(oregister_t *l, oregister_t *r);
extern void
ovm_o_mul(oregister_t *l, oregister_t *r);
extern void
ovm_o_div(oregister_t *l, oregister_t *r);
extern void
ovm_o_trunc2(oregister_t *l, oregister_t *r);
extern void
ovm_o_rem(oregister_t *l, oregister_t *r);

#  if __WORDSIZE == 32
extern void
ovm_load_ww(oregister_t *r, oint64_t *p);

extern void
ovm_load_uwuw(oregister_t *r, oint64_t *p);

extern void
ovm_truncr_f(oregister_t *r, ofloat32_t f);

extern void
ovm_truncr_d(oregister_t *r, ofloat64_t d);
#  endif

/*
 * Externs
 */
typedef void (*jit_function_t)(othread_t *);
extern jit_function_t		jit_main;

#endif /* _ovm_h */
