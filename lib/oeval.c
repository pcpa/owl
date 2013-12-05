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
 * Prototypes
 */
static void
eval_unary(oast_t *ast);

static void
eval_unary_wrapped(oast_t *ast);

static void
eval_binary(oast_t *ast);

static void
eval_binary_wrapped(oast_t *ast);

static void
eval_boolean(oast_t *ast);

static void
eval_setup(oregister_t *r, oobject_t v);
#define eval_unary_setup(v)		eval_setup(&thread_main->r0, v)
#define eval_binary_setup(l, r)						\
    do {								\
	eval_setup(&thread_main->r0, l);				\
	eval_setup(&thread_main->r1, r);				\
    } while (0)

static void
eval_fetch(oobject_t *p);

/*
 * Implementation
 */
void
init_eval(void)
{
}

void
finish_eval(void)
{
}

void
ofold(oast_t *ast)
{
    switch (ast->token) {
	case tok_com:		case tok_plus:
	case tok_neg:		case tok_not:
	case tok_integer_p:	case tok_rational_p:
	case tok_float_p:	case tok_real_p:
	case tok_complex_p:	case tok_number_p:
	case tok_finite_p:	case tok_inf_p:
	case tok_nan_p:		case tok_num:
	case tok_den:		case tok_real:
	case tok_imag:		case tok_signbit:
	case tok_abs:		case tok_signum:
	case tok_rational:	case tok_arg:
	case tok_conj:		case tok_floor:
	case tok_trunc:		case tok_round:
	case tok_ceil:		case tok_sqrt:
	case tok_cbrt:		case tok_sin:
	case tok_cos:		case tok_tan:
	case tok_asin:		case tok_acos:
	case tok_atan:		case tok_sinh:
	case tok_cosh:		case tok_tanh:
	case tok_asinh:		case tok_acosh:
	case tok_atanh:		case tok_proj:
	case tok_exp:		case tok_log:
	case tok_log2:		case tok_log10:
	    eval_unary(ast);
	    break;
	case tok_andand:	case tok_oror:
	    eval_boolean(ast);
	    break;
	case tok_ne:		case tok_lt:
	case tok_le:		case tok_eq:
	case tok_ge:		case tok_gt:
	case tok_and:		case tok_or:
	case tok_xor:		case tok_mul2:
	case tok_div2:		case tok_shl:
	case tok_shr:		case tok_add:
	case tok_sub:		case tok_mul:
	case tok_div:		case tok_trunc2:
	case tok_rem:		case tok_complex:
	case tok_atan2:		case tok_pow:
	case tok_hypot:
	    eval_binary(ast);
	    break;
	default:
#if DEBUG
	    oparse_warn(ast,
			"fold: not handling %s", otoken_to_charp(ast->token));
#endif
	    break;
    }
}

void
ocoerce(oobject_t *pointer, otype_t type, oobject_t value)
{
    union {
	oint64_t	 i;
	ofloat32_t	 f;
	ofloat_t	 d;
    } data;

    switch (type) {
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	case t_int64:		case t_uint64:
	    switch (otype(value)) {
		case t_word:
		    data.i = *(oword_t *)value;
		    break;
		case t_float:
		    data.i = *(ofloat_t *)value;
		    break;
		case t_mpz:
		    data.i = ompz_get_sl((ompz_t)value);
		    break;
		case t_mpq:
		    data.i = ompq_get_sl((ompq_t)value);
		    break;
		case t_mpr:
		    data.i = ompr_get_sl((ompr_t)value);
		    break;
		case t_cdd:
		    data.i = real(*(ocdd_t *)value);
		    break;
		case t_cqq:
		    data.i = ompq_get_sl(cqq_realref((ocqq_t)value));
		    break;
		default:
		    assert(otype(value) == t_mpc);
		    data.i = ompr_get_sl(mpc_realref((ompc_t)value));
		    break;
	    }
	    break;
	case t_float32:		case t_float64:
	    switch (otype(value)) {
		case t_word:
		    data.d = *(oword_t *)value;
		    break;
		case t_float:
		    data.d = *(ofloat_t *)value;
		    break;
		case t_mpz:
		    data.d = mpz_get_d((ompz_t)value);
		    break;
		case t_mpq:
		    data.d = mpq_get_d((ompq_t)value);
		    break;
		case t_mpr:
		    data.d = mpfr_get_d((ompr_t)value, thr_rnd);
		    break;
		case t_cdd:
		    data.d = real(*(ocdd_t *)value);
		    break;
		case t_cqq:
		    data.d = mpq_get_d(cqq_realref((ocqq_t)value));
		    break;
		default:
		    assert(otype(value) == t_mpc);
		    data.i = mpfr_get_d(mpc_realref((ompc_t)value), thr_rnd);
		    break;
	    }
	    break;
	default:
	    break;
    }

    switch (type) {
	case t_int8:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = (oint8_t)data.i;
	    break;
	case t_uint8:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = (ouint8_t)data.i;
	    break;
	case t_int16:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = (oint16_t)data.i;
	    break;
	case t_uint16:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = (ouint16_t)data.i;
	    break;
	case t_int32:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = (oint32_t)data.i;
	    break;
	case t_uint32:
#if __WORDSIZE == 32
	    if ((ouword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		mpz_set_ui((ompz_t)pointer, (ouword_t)data.i);
	    }
	    else
#endif
	    {
		oget_word(pointer);	pointer = *pointer;
		*(oword_t *)pointer = (ouint32_t)data.i;
	    }
	    break;
	case t_int64:
#if __WORDSIZE == 32
	    if ((oword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		ompz_set_sl((ompz_t)pointer, data.i);
	    }
	    else
#endif
	    {
		oget_word(pointer);	pointer = *pointer;
		*(oword_t *)pointer = (oword_t)data.i;
	    }
	    break;
	case t_uint64:
	    if ((ouword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		mpz_set_ui((ompz_t)pointer, (ouword_t)data.i);
	    }
	    else {
		oget_word(pointer);	pointer = *pointer;
		*(oword_t *)pointer = (ouword_t)data.i;
	    }
	    break;
	case t_float32:
	    data.f = data.d;
	    data.d = data.f;
	case t_float:
	    oget_float(pointer);	pointer = *pointer;
	    *(ofloat_t *)pointer = data.d;
	    break;
	default:
	    ocopy(pointer, value);
	    break;
    }
}

void
ocopy(oobject_t *pointer, oobject_t value)
{
    switch (otype(value)) {
	case t_word:
	    oget_word(pointer);		pointer = *pointer;
	    *(oword_t *)pointer = *(oword_t *)value;
	    break;
	case t_float:
	    oget_float(pointer);	pointer = *pointer;
	    *(ofloat_t *)pointer = *(ofloat_t *)value;
	    break;
	case t_rat:
	    oget_rat(pointer);		pointer = *pointer;
	    *(orat_t)pointer = *(orat_t)value;
	    break;
	case t_mpz:
	    oget_mpz(pointer);		pointer = *pointer;
	    mpz_set((ompz_t)pointer, (ompz_t)value);
	    break;
	case t_mpq:
	    oget_mpq(pointer);		pointer = *pointer;
	    mpq_set((ompq_t)pointer, (ompq_t)value);
	    break;
	case t_mpr:
	    oget_mpr(pointer);		pointer = *pointer;
	    mpfr_set((ompr_t)pointer, (ompr_t)value, thr_rnd);
	    break;
	case t_cdd:
	    oget_cdd(pointer);		pointer = *pointer;
	    *(ocdd_t *)pointer = *(ocdd_t *)value;
	    break;
	case t_cqq:
	    oget_cqq(pointer);		pointer = *pointer;
	    cqq_set((ocqq_t)pointer, (ocqq_t)value);
	    break;
	default:
	    assert(otype(value) == t_mpc);
	    oget_mpc(pointer);		pointer = *pointer;
	    mpc_set((ompc_t)pointer, (ompc_t)value, thr_rndc);
	    break;
    }
}

obool_t
ofalse_p(oobject_t object)
{
    if (object == null)
	return (true);
    switch (otype(object)) {
	case t_word:
	    return (*(oword_t *)object == 0);
	case t_float:
	    return (*(ofloat_t *)object == 0.0);
	case t_mpr:
	    return (mpfr_zero_p((ompr_t)object) != 0);
	default:
	    return (false);
    }
}

static void
eval_unary(oast_t *ast)
{
    if (!sigsetjmp(thread_main->env, 1))
	eval_unary_wrapped(ast);
    memcpy(&thread_main->env, &cfg_env, sizeof(sigjmp_buf));
}

static void
eval_unary_wrapped(oast_t *ast)
{
    oregister_t		*l;

    if (ast->l.ast->token != tok_number)
	return;

    eval_unary_setup(ast->l.ast->l.value);
    l = &thread_main->r0;
    switch (ast->token) {
	case tok_not:		ovm_not(l);		break;
	case tok_com:		ovm_com(l);		break;
	case tok_plus:
	    omove_left_ast_up(ast);
	    ast->token = tok_number;
	    return;
	case tok_neg:		ovm_neg(l);		break;
	case tok_signbit:	ovm_signbit(l);		break;
	case tok_signum:	ovm_signum(l);		break;
	case tok_rational:	ovm_rational(l);	break;
	case tok_integer_p:	ovm_integer_p(l);	break;
	case tok_rational_p:	ovm_rational_p(l);	break;
	case tok_float_p:	ovm_float_p(l);		break;
	case tok_real_p:	ovm_real_p(l);		break;
	case tok_complex_p:	ovm_complex_p(l);	break;
	case tok_number_p:	ovm_number_p(l);	break;
	case tok_finite_p:	ovm_finite_p(l);	break;
	case tok_inf_p:		ovm_inf_p(l);		break;
	case tok_nan_p:		ovm_nan_p(l);		break;
	case tok_num:		ovm_num(l);		break;
	case tok_den:		ovm_den(l);		break;
	case tok_real:		ovm_real(l);		break;
	case tok_imag:		ovm_imag(l);		break;
	case tok_arg:		ovm_arg(l);		break;
	case tok_conj:		ovm_conj(l);		break;
	case tok_floor:		ovm_floor(l);		break;
	case tok_trunc:		ovm_trunc(l);		break;
	case tok_round:		ovm_round(l);		break;
	case tok_ceil:		ovm_ceil(l);		break;
	case tok_abs:		ovm_abs(l);		break;
	case tok_sqrt:		ovm_sqrt(l);		break;
	case tok_cbrt:		ovm_cbrt(l);		break;
	case tok_sin:		ovm_sin(l);		break;
	case tok_cos:		ovm_cos(l);		break;
	case tok_tan:		ovm_tan(l);		break;
	case tok_asin:		ovm_asin(l);		break;
	case tok_acos:		ovm_acos(l);		break;
	case tok_atan:		ovm_atan(l);		break;
	case tok_sinh:		ovm_sinh(l);		break;
	case tok_cosh:		ovm_cosh(l);		break;
	case tok_tanh:		ovm_tanh(l);		break;
	case tok_asinh:		ovm_asinh(l);		break;
	case tok_acosh:		ovm_acosh(l);		break;
	case tok_atanh:		ovm_atanh(l);		break;
	case tok_proj:		ovm_proj(l);		break;
	case tok_exp:		ovm_exp(l);		break;
	case tok_log:		ovm_log(l);		break;
	case tok_log2:		ovm_log2(l);		break;
	case tok_log10:		ovm_log10(l);		break;
	default:		abort();
    }
    odel_object(&ast->l.value);
    eval_fetch(&ast->l.value);
    ast->token = tok_number;
}

static void
eval_binary(oast_t *ast)
{
    if (!sigsetjmp(thread_main->env, 1))
	eval_binary_wrapped(ast);
    memcpy(&thread_main->env, &cfg_env, sizeof(sigjmp_buf));
}

static void
eval_binary_wrapped(oast_t *ast)
{
    oregister_t		*l, *r;

    if (ast->l.ast->token != tok_number || ast->r.ast->token != tok_number)
	return;

    eval_binary_setup(ast->l.ast->l.value, ast->r.ast->l.value);
    l = &thread_main->r0;
    r = &thread_main->r1;
    switch (ast->token) {
	case tok_ne:		ovm_o_ne(l, r);		break;
	case tok_lt:		ovm_o_lt(l, r);		break;
	case tok_le:		ovm_o_le(l, r);		break;
	case tok_eq:		ovm_o_eq(l, r);		break;
	case tok_ge:		ovm_o_ge(l, r);		break;
	case tok_gt:		ovm_o_gt(l, r);		break;
	case tok_and:		ovm_o_and(l, r);	break;
	case tok_or:		ovm_o_or(l, r);		break;
	case tok_xor:		ovm_o_xor(l, r);	break;
	case tok_mul2:		ovm_o_mul2(l, r);	break;
	case tok_div2:		ovm_o_div2(l, r);	break;
	case tok_shl:		ovm_o_shl(l, r);	break;
	case tok_shr:		ovm_o_shr(l, r);	break;
	case tok_add:		ovm_o_add(l, r);	break;
	case tok_sub:		ovm_o_sub(l, r);	break;
	case tok_mul:		ovm_o_mul(l, r);	break;
	case tok_div:		ovm_o_div(l, r);	break;
	case tok_trunc2:	ovm_o_trunc2(l, r);	break;
	case tok_rem:		ovm_o_rem(l, r);	break;
	case tok_complex:	ovm_o_complex(l, r);	break;
	case tok_atan2:		ovm_o_atan2(l, r);	break;
	case tok_pow:		ovm_o_pow(l, r);	break;
	case tok_hypot:		ovm_o_hypot(l, r);	break;
	default:		abort();
    }
    odel_object(&ast->l.value);
    odel_object(&ast->r.value);
    eval_fetch(&ast->l.value);
    ast->token = tok_number;
}

#define boolean_nothing		0
#define boolean_false		1
#define boolean_true		2
#define boolean_left		3
#define boolean_right		4
static void
eval_boolean(oast_t *ast)
{
    oint32_t		state;

    state = boolean_nothing;
    if (ast->l.ast->token == tok_number) {
	if (ofalse_p(ast->l.ast->l.value)) {
	    if (ast->token == tok_andand)
		state = boolean_false;
	    else
		state = boolean_right;
	}
	else {
	    if (ast->token == tok_andand)
		state = boolean_right;
	    else
		state = boolean_true;
	}
	if (state == boolean_right && ast->r.ast->token == tok_number) {
	    if (ofalse_p(ast->r.ast->l.value))
		state = boolean_false;
	    else
		state = boolean_true;
	}
    }
    else if (ast->r.ast->token == tok_number) {
	if (ofalse_p(ast->r.ast->l.value)) {
	    if (ast->token == tok_oror)
		state = boolean_left;
	    else {
		/* expr && 0 -> (expr, 0) */
		ast->l.ast->next = ast->r.ast;
		ast->r.ast = null;
		ast->token = tok_list;
	    }
	}
	else
	    state = boolean_left;
    }
    switch (state) {
	case boolean_nothing:
	    break;
	case boolean_false:	case boolean_true:
	    odel_object(&ast->l.value);
	    odel_object(&ast->r.value);
	    oget_word(&ast->l.value);
	    *(oword_t *)ast->l.value = state == boolean_true;
	    ast->token = tok_number;
	    break;
	case boolean_left:
	    omove_ast_up_full(ast, ast->l.ast);
	    break;
	default:
	    assert(state == boolean_right);
	    omove_ast_up_full(ast, ast->r.ast);
	    break;
    }
}
#undef boolean_nothing
#undef boolean_false
#undef boolean_true
#undef boolean_left
#undef boolean_right

static void
eval_setup(oregister_t *r, oobject_t v)
{
    if (v == null) {
	r->t = t_void;
	r->v.o = null;
    }
    else {
	switch (r->t = otype(v)) {
	    case t_word:
		r->v.w = *(oword_t *)v;
		break;
	    case t_float:
		r->v.d = *(ofloat_t *)v;
		break;
	    case t_rat:
		r->v.r = *(orat_t)v;
		break;
	    case t_mpz:
		mpz_set(ozr(r), (ompz_t)v);
		break;
	    case t_mpq:
		mpq_set(oqr(r), (ompq_t)v);
		break;
	    case t_mpr:
		mpfr_set(orr(r), (ompr_t)v, thr_rnd);
		break;
	    case t_cdd:
		r->v.dd = *(ocdd_t *)v;
		break;
	    case t_cqq:
		cqq_set(oqq(r), (ocqq_t)v);
		break;
	    default:
		assert(r->t == t_mpc);
		mpc_set(occ(r), (ompc_t)v, thr_rndc);
		break;
	}
    }
}

static void
eval_fetch(oobject_t *p)
{
    oregister_t		*r;
    oobject_t		 v;

    r = &thread_main->r0;
    switch (r->t) {
	case t_void:
	    *p = null;
	    break;
	case t_word:
	    oget_word(p);		v = *p;
	    *(oword_t *)v = r->v.w;
	    break;
	case t_float:
	    oget_float(p);		v = *p;
	    *(ofloat_t *)v = r->v.d;
	    break;
	case t_rat:
	    oget_rat(p);		v = *p;
	    *(orat_t )v = r->v.r;
	    break;
	case t_mpz:
	    oget_mpz(p);		v = *p;
	    mpz_set((ompz_t)v, ozr(r));
	    break;
	case t_mpq:
	    oget_mpq(p);		v = *p;
	    mpq_set((ompq_t)v, oqr(r));
	    break;
	case t_mpr:
	    oget_mpr(p);		v = *p;
	    mpfr_set((ompr_t)v, orr(r), thr_rnd);
	    break;
	case t_cdd:
	    oget_cdd(p);		v = *p;
	    *(ocdd_t *)v = r->v.dd;
	    break;
	case t_cqq:
	    oget_cqq(p);		v = *p;
	    cqq_set((ocqq_t)v, oqq(r));
	    break;
	default:
	    assert(r->t == t_mpc);
	    oget_mpc(p);		v = *p;
	    mpc_set((ompc_t)v, occ(r), thr_rndc);
	    break;
    }
}
