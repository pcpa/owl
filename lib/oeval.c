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

#define P(n)		eval_vector->v.ptr + (n)
#define N(n)		eval_vector->v.ptr[n]

#define collect(n)	odel_object(P(n))
#define coerced(n)	cache_mask |= 1 << n

#define _pwp(v)		*(oword_t *)v
#define pwp(v)		_pwp(N(v))
#define get_word(n)	oget_word(P(n))

#define _pdp(v)		*(ofloat_t *)v
#define pdp(v)		_pdp(N(v))
#define get_float(n)	oget_float(P(n))

#define _rat(v)		(orat_t)v
#define _prat(v)	*(rat_t *)v
#define rat(v)		_rat(N(v))
#define prat(v)		_prat(N(v))
#define get_rat(n)	oget_rat(P(n))

#define _mpz(v)		(ompz_t)(v)
#define mpz(v)		(ompz_t)(N(v))
#define get_mpz(n)	oget_mpz(P(n))

#define _pddp(v)	*(ocdd_t *)v
#define pddp(v)		_pddp(N(v))
#define get_cdd(n)	oget_cdd(P(n))

#define _mpq(v)		(ompq_t)(v)
#define mpq(v)		(ompq_t)(N(v))
#define get_mpq(n)	oget_mpq(P(n))

#define _mpr(v)		(ompr_t)(v)
#define mpr(v)		(ompr_t)(N(v))
#define get_mpr(n)	oget_mpr(P(n))

#define _cqq(v)		(ocqq_t)(v)
#define cqq(v)		(ocqq_t)(N(v))
#define get_cqq(n)	oget_cqq(P(n))

#define _mpc(v)		(ompc_t)(v)
#define mpc(v)		(ompc_t)(N(v))
#define get_mpc(n)	oget_mpc(P(n))

/*
 * Prototypes
 */
static void
eval_unary_setup(oobject_t value);

static void
eval_binary_setup(oobject_t lvalue, oobject_t rvalue);

static void
check_real(oast_t *ast);

static void
check_rational(oast_t *ast);

static void
check_integer(oast_t *ast);

static oint32_t
shift_power(oast_t *ast);

static oint32_t
shift_canonicalize(oast_t *ast, oint32_t shift);

static oobject_t
cleanup(void);

static oobject_t
canonicalize(void);

/*
 * Initialization
 */
static ovector_t	*eval_vector;
static oint32_t		 cache_mask;

/*
 * Implementation
 */
void
init_eval(void)
{
    oadd_root((oobject_t *)&eval_vector);
    onew_vector((oobject_t *)&eval_vector, t_void, 8);
}

void
finish_eval(void)
{
    orem_root((oobject_t *)&eval_vector);
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
	case tok_abs:
	    oeval_unary(ast);
	    break;
	case tok_andand:	case tok_oror:
	    oeval_boolean(ast);
	    break;
	case tok_ne:		case tok_lt:
	case tok_le:		case tok_eq:
	case tok_ge:		case tok_gt:
	case tok_and:		case tok_or:
	case tok_xor:		case tok_add:
	case tok_sub:		case tok_mul:
	case tok_div:		case tok_trunc2:
	case tok_rem:		case tok_complex:
	    oeval_binary(ast);
	    break;
	case tok_mul2:		case tok_div2:
	case tok_shl:		case tok_shr:
	    oeval_shift(ast);
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
		    data.i = _pwp(value);
		    break;
		case t_float:
		    data.i = _pdp(value);
		    break;
		case t_mpz:
		    data.i = ompz_get_sl(_mpz(value));
		    break;
		case t_mpq:
		    data.i = ompq_get_sl(_mpq(value));
		    break;
		case t_mpr:
		    data.i = ompr_get_sl(_mpr(value));
		    break;
		case t_cdd:
		    data.i = real(_pddp(value));
		    break;
		case t_cqq:
		    data.i = ompq_get_sl(cqq_realref(_cqq(value)));
		    break;
		default:
		    assert(otype(value) == t_mpc);
		    data.i = ompr_get_sl(mpc_realref(_mpc(value)));
		    break;
	    }
	    break;
	case t_float32:		case t_float64:
	    switch (otype(value)) {
		case t_word:
		    data.d = _pwp(value);
		    break;
		case t_float:
		    data.d = _pdp(value);
		    break;
		case t_mpz:
		    data.d = mpz_get_d(_mpz(value));
		    break;
		case t_mpq:
		    data.d = mpq_get_d(_mpq(value));
		    break;
		case t_mpr:
		    data.d = mpfr_get_d(_mpr(value), thr_rnd);
		    break;
		case t_cdd:
		    data.d = real(_pddp(value));
		    break;
		case t_cqq:
		    data.d = mpq_get_d(cqq_realref(_cqq(value)));
		    break;
		default:
		    assert(otype(value) == t_mpc);
		    data.i = mpfr_get_d(mpc_realref(_mpc(value)), thr_rnd);
		    break;
	    }
	    break;
	default:
	    break;
    }

    switch (type) {
	case t_int8:
	    oget_word(pointer);		pointer = *pointer;
	    _pwp(pointer) = (oint8_t)data.i;
	    break;
	case t_uint8:
	    oget_word(pointer);		pointer = *pointer;
	    _pwp(pointer) = (ouint8_t)data.i;
	    break;
	case t_int16:
	    oget_word(pointer);		pointer = *pointer;
	    _pwp(pointer) = (oint16_t)data.i;
	    break;
	case t_uint16:
	    oget_word(pointer);		pointer = *pointer;
	    _pwp(pointer) = (ouint16_t)data.i;
	    break;
	case t_int32:
	    oget_word(pointer);		pointer = *pointer;
	    _pwp(pointer) = (oint32_t)data.i;
	    break;
	case t_uint32:
#if __WORDSIZE == 32
	    if ((ouword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		mpz_set_ui(_mpz(pointer), (ouword_t)data.i);
	    }
	    else
#endif
	    {
		oget_word(pointer);	pointer = *pointer;
		_pwp(pointer) = (ouint32_t)data.i;
	    }
	    break;
	case t_int64:
#if __WORDSIZE == 32
	    if ((oword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		ompz_set_sl(_mpz(pointer), data.i);
	    }
	    else
#endif
	    {
		oget_word(pointer);	pointer = *pointer;
		_pwp(pointer) = (oword_t)data.i;
	    }
	    break;
	case t_uint64:
	    if ((ouword_t)data.i != data.i) {
		oget_mpz(pointer);	pointer = *pointer;
		mpz_set_ui(_mpz(pointer), (ouword_t)data.i);
	    }
	    else {
		oget_word(pointer);	pointer = *pointer;
		_pwp(pointer) = (ouword_t)data.i;
	    }
	    break;
	case t_float32:
	    data.f = data.d;
	    data.d = data.f;
	case t_float:
	    oget_float(pointer);	pointer = *pointer;
	    _pdp(pointer) = data.d;
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
	    _pwp(pointer) = _pwp(value);
	    break;
	case t_float:
	    oget_float(pointer);	pointer = *pointer;
	    _pdp(pointer) = _pdp(value);
	    break;
	case t_rat:
	    oget_rat(pointer);		pointer = *pointer;
	    _prat(pointer) = _prat(value);
	    break;
	case t_mpz:
	    oget_mpz(pointer);		pointer = *pointer;
	    mpz_set(_mpz(pointer), _mpz(value));
	    break;
	case t_mpq:
	    oget_mpq(pointer);		pointer = *pointer;
	    mpq_set(_mpq(pointer), _mpq(value));
	    break;
	case t_mpr:
	    oget_mpr(pointer);		pointer = *pointer;
	    mpfr_set(_mpr(pointer), _mpr(value), thr_rnd);
	    break;
	case t_cdd:
	    oget_cdd(pointer);		pointer = *pointer;
	    _pddp(pointer) = _pddp(value);
	    break;
	case t_cqq:
	    oget_cqq(pointer);		pointer = *pointer;
	    cqq_set(_cqq(pointer), _cqq(value));
	    break;
	default:
	    assert(otype(value) == t_mpc);
	    oget_mpc(pointer);		pointer = *pointer;
	    mpc_set(_mpc(pointer), _mpc(value), thr_rndc);
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
	    return (_pwp(object) == 0);
	case t_float:
	    return (_pdp(object) == 0.0);
	case t_mpr:
	    return (mpfr_zero_p(_mpr(object)) != 0);
	default:
	    return (false);
    }
}

void
oeval_unary(oast_t *ast)
{
    obool_t		lnum;
    /* object is gc proteced in result of oeval_xyz() */
    oobject_t		object;

    switch (ast->token) {
	case tok_plus:
	    if (ast->l.ast->token == tok_number) {
		omove_left_ast_up(ast);
		ast->token = tok_number;
	    }
	    return;
	case tok_neg:		case tok_not:
	case tok_integer_p:	case tok_rational_p:
	case tok_float_p:	case tok_real_p:
	case tok_complex_p:	case tok_number_p:
	case tok_finite_p:	case tok_inf_p:
	case tok_nan_p:		case tok_real:
	case tok_imag:		case tok_abs:
	    lnum = ast->l.ast->token == tok_number;
	    break;
	case tok_com:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_integer(ast->l.ast);
	    break;
	case tok_num:		case tok_den:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_rational(ast->l.ast);
	    break;
	case tok_signbit:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_real(ast->l.ast);
	    break;
	default:
	    abort();
    }

    if (!lnum)
	return;

    eval_unary_setup(ast->l.ast->l.value);
    switch (ast->token) {
	case tok_neg:		object = oeval_neg();		break;
	case tok_not:		object = oeval_not();		break;
	case tok_com:		object = oeval_com();		break;
	case tok_integer_p:	object = oeval_integer_p();	break;
	case tok_rational_p:	object = oeval_rational_p();	break;
	case tok_float_p:	object = oeval_float_p();	break;
	case tok_real_p:	object = oeval_real_p();	break;
	case tok_complex_p:	object = oeval_complex_p();	break;
	case tok_number_p:	object = oeval_number_p();	break;
	case tok_finite_p:	object = oeval_finite_p();	break;
	case tok_inf_p:		object = oeval_inf_p();		break;
	case tok_nan_p:		object = oeval_nan_p();		break;
	case tok_num:		object = oeval_num();		break;
	case tok_den:		object = oeval_den();		break;
	case tok_real:		object = oeval_real();		break;
	case tok_imag:		object = oeval_imag();		break;
	case tok_signbit:	object = oeval_signbit();	break;
	case tok_abs:		object = oeval_abs();		break;
	default:		abort();
    }
    odel_object(&ast->l.value);
    ast->l.value = object;
    ast->token = tok_number;
}

void
oeval_binary(oast_t *ast)
{
    obool_t		lnum;
    obool_t		rnum;
    oobject_t		object;

    switch (ast->token) {
	case tok_ne:		case tok_eq:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_div:
	    lnum = ast->l.ast->token == tok_number;
	    rnum = ast->r.ast->token == tok_number;
	    break;
	case tok_lt:		case tok_le:
	case tok_ge:		case tok_gt:
	case tok_trunc2:	case tok_rem:
	case tok_complex:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_real(ast->l.ast);
	    if ((rnum = ast->r.ast->token == tok_number))
		check_real(ast->r.ast);
	    break;
	case tok_and:		case tok_or:
	case tok_xor:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_integer(ast->l.ast);
	    if ((rnum = ast->r.ast->token == tok_number))
		check_integer(ast->r.ast);
	    break;
	default:
	    abort();
    }

    if (!lnum || !rnum)
	return;

    eval_binary_setup(ast->l.ast->l.value, ast->r.ast->l.value);
    switch (ast->token) {
	case tok_ne:		object = oeval_ne();		break;
	case tok_lt:		object = oeval_lt();		break;
	case tok_le:		object = oeval_le();		break;
	case tok_eq:		object = oeval_eq();		break;
	case tok_ge:		object = oeval_ge();		break;
	case tok_gt:		object = oeval_gt();		break;
	case tok_and:		object = oeval_and();		break;
	case tok_or:		object = oeval_or();		break;
	case tok_xor:		object = oeval_xor();		break;
	case tok_add:		object = oeval_add();		break;
	case tok_sub:		object = oeval_sub();		break;
	case tok_mul:		object = oeval_mul();		break;
	case tok_div:		object = oeval_div();		break;
	case tok_trunc2:	object = oeval_trunc2();	break;
	case tok_rem:		object = oeval_rem();		break;
	case tok_complex:	object = oeval_complex();	break;
	default:		abort();
    }
    odel_object(&ast->l.value);
    odel_object(&ast->r.value);
    ast->l.value = object;
    ast->token = tok_number;
}

void
oeval_shift(oast_t *ast)
{
    obool_t		lnum;
    obool_t		rnum;
    oint32_t		shift;
    oobject_t		object;

    switch (ast->token) {
	case tok_mul2:		case tok_div2:
	    lnum = ast->l.ast->token == tok_number;
	    break;
	case tok_shl:		case tok_shr:
	    if ((lnum = ast->l.ast->token == tok_number))
		check_real(ast->l.ast);
	    break;
	default:
	    abort();
    }

    if ((rnum = ast->r.ast->token == tok_number)) {
	shift = shift_power(ast->r.ast);
	if (shift < 0) {
	    _pwp(ast->r.ast->l.value) = -shift;
	    switch (ast->token) {
		case tok_mul2:	ast->token = tok_div2;		break;
		case tok_div2:	ast->token = tok_mul2;		break;
		case tok_shl:	ast->token = tok_shr;		break;
		case tok_shr:	ast->token = tok_shl;		break;
		default:	abort();
	    }
	}
    }

    if (!lnum || !rnum)
	return;

    eval_unary_setup(ast->l.ast->l.value);
    if (shift >= 0) {
	switch (ast->token) {
	    case tok_mul2:	object = oeval_mul2(shift);	break;
	    case tok_div2:	object = oeval_div2(shift);	break;
	    case tok_shl:	object = oeval_shl(shift);	break;
	    case tok_shr:	object = oeval_shr(shift);	break;
	    default:		abort();
	}
    }
    else {
	shift = -shift;
	switch (ast->token) {
	    case tok_mul2:	object = oeval_div2(shift);	break;
	    case tok_div2:	object = oeval_mul2(shift);	break;
	    case tok_shl:	object = oeval_shr(shift);	break;
	    case tok_shr:	object = oeval_shl(shift);	break;
	    default:		abort();
	}
    }
    odel_object(&ast->l.value);
    odel_object(&ast->r.value);
    ast->l.value = object;
    ast->token = tok_number;
}

#define boolean_nothing		0
#define boolean_false		1
#define boolean_true		2
#define boolean_left		3
#define boolean_right		4
void
oeval_boolean(oast_t *ast)
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

oobject_t
oeval_integer_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_void:	case t_word:
	case t_mpz:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_rational_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:	case t_mpz:
	case t_rat:	case t_mpq:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_float_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_float:	case t_mpr:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_real_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:	case t_float:
	case t_mpz:	case t_rat:
	case t_mpq:	case t_mpr:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_complex_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_cdd:	case t_cqq:
	case t_mpc:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_number_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:	case t_float:
	case t_mpz:	case t_rat:
	case t_mpq:	case t_mpr:
	case t_cdd:	case t_cqq:
	case t_mpc:
	    w = 1;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_finite_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:	case t_mpz:
	case t_rat:	case t_mpq:
	    w = 1;
	    break;
	case t_float:
	    w = finite(pdp(1)) ? 1 : 0;
	    break;
	case t_mpr:
	    w = mpfr_number_p(mpr(1)) ? 1 : 0;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_inf_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_float:
	    w = isinf(pdp(1)) ? signbit(pdp(1)) ? -1 : 1 : 0;
	    break;
	case t_mpr:
	    w = mpfr_inf_p(mpr(1)) ? mpfr_signbit(mpr(1)) ? -1 : 1 : 0;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_nan_p(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_float:
	    w = isnan(pdp(1)) ? 1 : 0;
	    break;
	case t_mpr:
	    w = mpfr_nan_p(mpr(1)) ? 1 : 0;
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_num(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_set(mpz(0), mpz(1));
	    break;
	case t_rat:
	    get_word(0);
	    pwp(0) = orat_num(rat(1));
	    break;
	default:
	    get_mpz(0);
	    mpz_set(mpz(0), mpq_numref(mpq(1)));
	    break;
    }
    return (canonicalize());
}

oobject_t
oeval_den(void)
{
    switch (otype(N(1))) {
	case t_word:		case t_mpz:
	    get_word(0);
	    pwp(0) = 1;
	    break;
	case t_rat:
	    get_word(0);
	    pwp(0) = orat_den(rat(1));
	    break;
	default:
	    get_mpz(0);
	    mpz_set(mpz(0), mpq_denref(mpq(1)));
	    break;
    }
    return (canonicalize());
}

oobject_t
oeval_real(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1);
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = pdp(1);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_set(mpz(0), mpz(1));
	    break;
	case t_rat:
	    get_rat(0);
	    prat(0) = prat(1);
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_set(mpq(0), mpq(1));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_set(mpr(0), mpr(1), thr_rnd);
	    break;
	case t_cdd:
	    get_float(0);
	    pdp(0) = real(pddp(1));
	    break;
	case t_cqq:
	    get_mpq(0);
	    mpq_set(mpq(0), cqq_realref(cqq(1)));
	    break;
	default:
	    get_mpr(0);
	    mpc_real(mpr(0), mpc(1), thr_rndc);
	    break;
    }
    return (canonicalize());
}

oobject_t
oeval_imag(void)
{
    switch (otype(N(1))) {
	case t_word:		case t_mpz:
	case t_rat:		case t_mpq:
	    get_word(0);
	    pwp(0) = 0;
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = 0.0;
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_set_ui(mpr(0), 0, thr_rnd);
	    break;
	case t_cdd:
	    get_float(0);
	    pdp(0) = imag(pddp(1));
	    break;
	case t_cqq:
	    get_mpq(0);
	    mpq_set(mpq(0), cqq_imagref(cqq(1)));
	    break;
	default:
	    get_mpr(0);
	    mpc_imag(mpr(0), mpc(1), thr_rndc);
	    break;
    }
    return (canonicalize());
}

oobject_t
oeval_signbit(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) < 0;
	    break;
	case t_float:
	    w = signbit(pdp(1)) != 0;
	    break;
	case t_rat:
	    w = orat_sgn(rat(1)) < 0;
	    break;
	case t_mpz:
	    w = mpz_sgn(mpz(1)) < 0;
	    break;
	case t_mpq:
	    w = mpq_sgn(mpq(1)) < 0;
	    break;
	default:
	    w = mpfr_signbit(mpr(1)) != 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;
    return (cleanup());
}

oobject_t
oeval_abs(void)
{
    switch (otype(N(1))) {
	case t_word:
	    if (pwp(1) != MININT) {
		get_word(0);
		pwp(0) = pwp(1) < 0 ? -pwp(1) : pwp(1);
	    }
	    else {
		get_mpz(0);
		mpz_set_ui(mpz(0), MININT);
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = fabs(pdp(1));
	    break;
	case t_rat:
	    if (likely(orat_num(rat(1)) != MININT)) {
		get_rat(0);
		orat_num(rat(0)) = orat_num(rat(1)) < 0 ?
		    -orat_num(rat(1)) : orat_num(rat(1));
		orat_den(rat(0)) = orat_den(rat(1));
	    }
	    else {
		get_mpq(0);
		mpq_set_si(mpq(0), orat_num(rat(1)), orat_den(rat(1)));
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_abs(mpz(0), mpz(1));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_abs(mpq(0), mpq(1));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_abs(mpr(0), mpr(1), thr_rnd);
	    break;
	case t_cdd:
	    get_float(0);
	    pdp(0) = cabs(pddp(1));
	    break;
	case t_cqq:
	    if (!cfg_float_format) {
		get_cdd(2);
		real(pddp(2)) = mpq_get_d(cqq_realref(cqq(1)));
		imag(pddp(2)) = mpq_get_d(cqq_imagref(cqq(1)));
		get_float(0);
		pdp(0) = cabs(pddp(2));
	    }
	    else {
		get_mpc(2);
		mpc_set_q_q(mpc(2), cqq_realref(cqq(1)),
			    cqq_imagref(cqq(1)), thr_rndc);
		get_mpr(0);
		mpc_abs(mpr(0), mpc(2), thr_rndc);
	    }
	    collect(2);
	    break;
	default:
	    get_mpr(0);
	    mpc_abs(mpr(0), mpc(1), thr_rndc);
	    break;
    }
    return (canonicalize());
}

oobject_t
oeval_neg(void)
{
    switch (otype(N(1))) {
	case t_word:
	    if (pwp(1) == MININT) {
		get_mpz(0);
		mpz_set_ui(mpz(0), MININT);
	    }
	    else {
		get_word(0);
		pwp(0) = -pwp(1);
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = -pdp(1);
	    break;
	case t_rat:
	    if (orat_num(rat(1)) == MININT) {
		get_mpq(0);
		mpq_set_ui(mpq(0), orat_num(rat(1)), orat_den(rat(1)));
		mpq_neg(mpq(0), mpq(0));
		mpq_canonicalize(mpq(0));
	    }
	    else {
		get_rat(0);
		orat_num(rat(0)) = -orat_num(rat(1));
		orat_den(rat(0)) =  orat_den(rat(1));
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_neg(mpz(0), mpz(1));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_neg(mpq(0), mpq(1));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_neg(mpr(0), mpr(1), thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    pddp(0) = -pddp(1);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_neg(cqq(0), cqq(1));
	    break;
	default:
	    get_mpc(0);
	    mpc_neg(mpc(0), mpc(1), thr_rndc);
	    break;
    }

    return (canonicalize());
}

oobject_t
oeval_not(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = !pwp(1);
	    break;
	case t_float:
	    w = !pdp(1);
	    break;
	case t_rat:
	    w = orat_num(rat(1)) != 0;
	    break;
	case t_mpq:
	    w = mpq_sgn(mpq(1)) != 0;
	    break;
	case t_mpr:
	    w = !!mpfr_zero_p(mpr(1));
	    break;
	default:
	    w = 0;
	    break;
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_com(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = ~pwp(1);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_com(mpz(0), mpz(1));
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

oobject_t
oeval_ne(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) != pwp(2);
	    break;
	case t_float:
	    w = pdp(1) != pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) != 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) != 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) != 0;
	    break;
	case t_mpr:
	    w = !mpfr_equal_p(mpr(1), mpr(2));
	    break;
	case t_cdd:
	    w = pddp(1) != pddp(2);
	    break;
	case t_cqq:
	    w = ((mpq_cmp(cqq_realref(cqq(1)),
			  cqq_realref(cqq(2))) != 0) ||
		 (mpq_cmp(cqq_imagref(cqq(1)),
			  cqq_imagref(cqq(2))) != 0));
	    break;
	default:
	    w = (!mpfr_equal_p(mpc_realref(mpc(1)),
			       mpc_realref(mpc(2))) ||
		 !mpfr_equal_p(mpc_imagref(mpc(1)),
			       mpc_imagref(mpc(2))));
	    break;
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_lt(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) < pwp(2);
	    break;
	case t_float:
	    w = pdp(1) < pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) < 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) < 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) < 0;
	    break;
	case t_mpr:
	    w = mpfr_less_p(mpr(1), mpr(2));
	    break;
	default:
	    abort();
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_le(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) <= pwp(2);
	    break;
	case t_float:
	    w = pdp(1) <= pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) <= 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) <= 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) <= 0;
	    break;
	case t_mpr:
	    w = mpfr_lessequal_p(mpr(1), mpr(2));
	    break;
	default:
	    abort();
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_eq(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) == pwp(2);
	    break;
	case t_float:
	    w = pdp(1) == pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) == 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) == 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) == 0;
	    break;
	case t_mpr:
	    w = mpfr_equal_p(mpr(1), mpr(2));
	    break;
	case t_cdd:
	    w = pddp(1) == pddp(2);
	    break;
	case t_cqq:
	    w = ((mpq_cmp(cqq_realref(cqq(1)),
			  cqq_realref(cqq(2))) == 0) &&
		 (mpq_cmp(cqq_imagref(cqq(1)),
			  cqq_imagref(cqq(2))) == 0));
	    break;
	default:
	    w = (mpfr_equal_p(mpc_realref(mpc(1)),
			      mpc_realref(mpc(2))) &&
		 mpfr_equal_p(mpc_imagref(mpc(1)),
			      mpc_imagref(mpc(2))));
	    break;
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_ge(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) >= pwp(2);
	    break;
	case t_float:
	    w = pdp(1) >= pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) >= 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) >= 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) >= 0;
	    break;
	case t_mpr:
	    w = mpfr_greaterequal_p(mpr(1), mpr(2));
	    break;
	default:
	    abort();
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_gt(void)
{
    oword_t		w;
    switch (otype(N(1))) {
	case t_word:
	    w = pwp(1) > pwp(2);
	    break;
	case t_float:
	    w = pdp(1) > pdp(2);
	    break;
	case t_rat:
	    w = orat_cmp(rat(1), rat(2)) > 0;
	    break;
	case t_mpz:
	    w = mpz_cmp(mpz(1), mpz(2)) > 0;
	    break;
	case t_mpq:
	    w = mpq_cmp(mpq(1), mpq(2)) > 0;
	    break;
	case t_mpr:
	    w = mpfr_greater_p(mpr(1), mpr(2));
	    break;
	default:
	    abort();
    }
    get_word(0);
    pwp(0) = w;

    return (cleanup());
}

oobject_t
oeval_and(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1) & pwp(2);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_and(mpz(0), mpz(1), mpz(2));
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

oobject_t
oeval_or(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1) | pwp(2);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_ior(mpz(0), mpz(1), mpz(2));
	    break;
	default:
	    abort();
    }

    return (cleanup());
}

oobject_t
oeval_xor(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1) ^ pwp(2);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_xor(mpz(0), mpz(1), mpz(2));
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

oobject_t
oeval_mul2(oint32_t shift)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    if (shift < __WORDSIZE) {
		w = pwp(1) << shift;
		if (likely(w >> shift == pwp(1) && w != MININT)) {
		    get_word(0);
		    pwp(0) = w;
		    goto done;
		}
	    }
	    get_mpz(0);
	    mpz_set_si(mpz(0), pwp(1));
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = ldexp(pdp(1), shift);
	    break;
	case t_rat:
	    if (shift < __WORDSIZE) {
		w = orat_num(rat(1)) << shift;
		if (likely(w >> shift == orat_num(rat(1)) && w != MININT)) {
		    get_rat(0);
		    orat_num(rat(0)) = w;
		    orat_den(rat(0)) = orat_den(rat(1));
		    goto done;
		}
	    }
	    get_mpq(0);
	    mpq_set_si(mpq(0), orat_num(rat(1)), orat_den(rat(1)));
	    mpq_mul_2exp(mpq(0), mpq(0), shift);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_mul_2exp(mpz(0), mpz(1), shift);
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_mul_2exp(mpq(0), mpq(1), shift);
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_mul_2exp(mpr(0), mpr(1), shift, thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    real(pddp(0)) = ldexp(real(pddp(1)), shift);
	    imag(pddp(0)) = ldexp(imag(pddp(1)), shift);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_mul_2exp(cqq(0), cqq(1), shift);
	    break;
	default:
	    assert(otype(N(1)) == t_mpc);
	    get_mpc(0);
	    mpc_mul_2exp(mpc(0), mpc(1), shift, thr_rndc);
	    break;
    }

done:
    return (canonicalize());
}

oobject_t
oeval_div2(oint32_t shift)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    if (shift < __WORDSIZE) {
		w = pwp(1) >> shift;
		if (w << shift == pwp(1)) {
		    get_word(0);
		    pwp(0) = w;
		}
		else {
		    w = pwp(1);
		    get_rat(0);
		    orat_num(rat(0)) = w;
		    orat_den(rat(0)) = 1 << shift;
		}
		goto done;
	    }
	    get_mpq(0);
	    mpq_set_si(mpq(0), pwp(1), 1);
	    mpq_div_2exp(mpq(0), mpq(0), shift);
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = ldexp(pdp(1), -shift);
	    break;
	case t_rat:
	    if (shift < __WORDSIZE) {
		w = orat_num(rat(1)) >> shift;
		if (likely(w << shift == orat_num(rat(1)) && w != MININT)) {
		    get_rat(0);
		    orat_num(rat(0)) = w;
		    orat_den(rat(0)) = orat_den(rat(1));
		    goto done;
		}
	    }
	    get_mpq(0);
	    mpq_set_si(mpq(0), orat_num(rat(1)), orat_den(rat(1)));
	    mpq_div_2exp(mpq(0), mpq(0), shift);
	    break;
	case t_mpz:
	    get_mpq(0);
	    mpq_set_z(mpq(0), mpz(1));
	    mpq_div_2exp(mpq(0), mpq(0), shift);
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_div_2exp(mpq(0), mpq(1), shift);
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_div_2exp(mpr(0), mpr(1), shift, thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    real(pddp(0)) = -ldexp(real(pddp(1)), shift);
	    imag(pddp(0)) = -ldexp(imag(pddp(1)), shift);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_div_2exp(cqq(0), cqq(1), shift);
	    break;
	default:
	    assert(otype(N(1)) == t_mpc);
	    get_mpc(0);
	    mpc_div_2exp(mpc(0), mpc(1), shift, thr_rndc);
	    break;
    }

done:
    return (canonicalize());
}

oobject_t
oeval_shl(oint32_t shift)
{
    GET_THREAD_SELF()
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    if (shift < __WORDSIZE) {
		w = pwp(1) << shift;
		if (w >> shift == pwp(1) && w != MININT) {
		    get_word(0);
		    pwp(0) = w;
		    goto done;
		}
	    }
	    get_mpz(0);
	    mpz_set_si(mpz(0), pwp(1));
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_rat:
	    if (shift < __WORDSIZE) {
		w = orat_num(rat(1)) / orat_den(rat(1));
		if (w < 0)
		    --w;
		if ((w << shift) >> shift == w) {
		    get_word(0);
		    pwp(0) = w << shift;
		    goto done;
		}
	    }
	    get_mpz(0);
	    get_mpq(2);
	    mpq_set_si(mpq(2), orat_num(rat(1)), orat_den(rat(1)));
	    mpz_fdiv_q(mpz(0), mpq_numref(mpq(2)), mpq_denref(mpq(2)));
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    collect(2);
	    break;
	case t_float:
	    get_mpz(0);
	    mpz_set_d(mpz(0), floor(pdp(1)));
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_mul_2exp(mpz(0), mpz(1), shift);
	    break;
	case t_mpq:
	    get_mpz(0);
	    mpz_fdiv_q(mpz(0), mpq_numref(mpq(1)), mpq_denref(mpq(1)));
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_mpr:
	    get_mpr(2);
	    mpfr_floor(mpr(2), mpr(1));
	    get_mpz(0);
	    if (unlikely(!mpfr_number_p(mpr(2))))
		othread_kill(SIGFPE);
	    mpfr_get_z(mpz(0), mpr((2)), GMP_RNDZ);
	    mpz_mul_2exp(mpz(0), mpz(0), shift);
	    collect(2);
	    break;
	default:
	    abort();
    }

done:
    return (canonicalize());
}

oobject_t
oeval_shr(oint32_t shift)
{
    GET_THREAD_SELF()
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    if (shift < __WORDSIZE)
		pwp(0) = pwp(1) >> shift;
	    else
		pwp(0) = pwp(1) >> (__WORDSIZE - 1);
	    break;
	case t_float:
	    get_mpz(0);
	    mpz_set_d(mpz(0), floor(pdp(1)));
	    mpz_fdiv_q_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_rat:
	    if (shift < __WORDSIZE) {
		w = orat_num(rat(1)) / orat_den(rat(1));
		if (w < 0)
		    --w;
		if ((w >> shift) << shift == w) {
		    get_word(0);
		    pwp(0) = w >> shift;
		    goto done;
		}
	    }
	    get_mpz(0);
	    get_mpq(2);
	    mpq_set_si(mpq(2), orat_num(rat(1)), orat_den(rat(1)));
	    mpz_fdiv_q(mpz(0), mpq_numref(mpq(2)), mpq_denref(mpq(2)));
	    mpz_fdiv_q_2exp(mpz(0), mpz(0), shift);
	    collect(2);
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_fdiv_q_2exp(mpz(0), mpz(1), shift);
	    break;
	case t_mpq:
	    get_mpz(0);
	    mpz_fdiv_q(mpz(0), mpq_numref(mpq(1)), mpq_denref(mpq(1)));
	    mpz_fdiv_q_2exp(mpz(0), mpz(0), shift);
	    break;
	case t_mpr:
	    get_mpr(2);
	    mpfr_floor(mpr(2), mpr(1));
	    get_mpz(0);
	    if (unlikely(!mpfr_number_p(mpr(2))))
		othread_kill(SIGFPE);
	    mpfr_get_z(mpz(0), mpr((2)), GMP_RNDZ);
	    mpz_fdiv_q_2exp(mpz(0), mpz(0), shift);
	    collect(2);
	    break;
	default:
	    abort();
    }

done:
    return (canonicalize());
}

oobject_t
oeval_add(void)
{
    oword_t		u, v, w;

    switch (otype(N(1))) {
	case t_word:
	    v = pwp(1);
	    w = pwp(2);
	    u = v + w;
	    if (oadd_over_p(u, v, w)) {
		get_mpz(0);
		mpz_set_si(mpz(0), v);
		if (w >= 0)
		    mpz_add_ui(mpz(0), mpz(0), w);
		else {
		    collect(1);
		    get_mpz(1);
		    mpz_set_si(mpz(1), w);
		    mpz_add(mpz(0), mpz(0), mpz(1));
		}
	    }
	    else {
		get_word(0);
		pwp(0) = u;
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = pdp(1) + pdp(2);
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_add(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpq(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_add(mpq(0), mpq(3), mpq(4));
		collect(3);
		collect(4);
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_add(mpz(0), mpz(1), mpz(2));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_add(mpq(0), mpq(1), mpq(2));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_add(mpr(0), mpr(1), mpr(2), thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    pddp(0) = pddp(1) + pddp(2);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_add(cqq(0), cqq(1), cqq(2));
	    break;
	default:
	    get_mpc(0);
	    mpc_add(mpc(0), mpc(1), mpc(2), thr_rndc);
	    break;
    }

    return (canonicalize());
}

oobject_t
oeval_sub(void)
{
    oword_t		u, v, w;

    switch (otype(N(1))) {
	case t_word:
	    v = pwp(1);
	    w = pwp(2);
	    u = v - w;
	    if (osub_over_p(u, v, w)) {
		get_mpz(0);
		mpz_set_si(mpz(0), v);
		if (w >= 0)
		    mpz_sub_ui(mpz(0), mpz(0), w);
		else {
		    collect(1);
		    get_mpz(1);
		    mpz_set_si(mpz(1), w);
		    mpz_sub(mpz(0), mpz(0), mpz(1));
		}
	    }
	    else {
		get_word(0);
		pwp(0) = u;
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = pdp(1) - pdp(2);
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_sub(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpq(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_sub(mpq(0), mpq(3), mpq(4));
		collect(3);
		collect(4);
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_sub(mpz(0), mpz(1), mpz(2));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_sub(mpq(0), mpq(1), mpq(2));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_sub(mpr(0), mpr(1), mpr(2), thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    pddp(0) = pddp(1) - pddp(2);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_sub(cqq(0), cqq(1), cqq(2));
	    break;
	default:
	    get_mpc(0);
	    mpc_sub(mpc(0), mpc(1), mpc(2), thr_rndc);
	    break;
    }

    return (canonicalize());
}

oobject_t
oeval_mul(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    if ((w = ow_mul_w_w(pwp(1), pwp(2))) == MININT) {
		get_mpz(0);
		mpz_set_si(mpz(0), pwp(1));
		if (pwp(2) > 0)
		    mpz_mul_ui(mpz(0), mpz(0), pwp(2));
		else
		    mpz_mul_si(mpz(0), mpz(0), pwp(2));
	    }
	    else {
		get_word(0);
		pwp(0) = w;
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = pdp(1) * pdp(2);
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_mul(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpq(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_mul(mpq(0), mpq(3), mpq(4));
		collect(3);
		collect(4);
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_mul(mpz(0), mpz(1), mpz(2));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_mul(mpq(0), mpq(1), mpq(2));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_mul(mpr(0), mpr(1), mpr(2), thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    pddp(0) = pddp(1) * pddp(2);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_mul(cqq(0), cqq(1), cqq(2));
	    break;
	default:
	    get_mpc(0);
	    mpc_mul(mpc(0), mpc(1), mpc(2), thr_rndc);
	    break;
    }

    return (canonicalize());
}

oobject_t
oeval_div(void)
{
    oword_t		u, v, w;

    switch (otype(N(1))) {
	case t_word:
	    v = pwp(1);
	    w = pwp(2);
	    u = v / w;
	    if (v == w * u) {
		get_word(0);
		pwp(0) = u;
	    }
	    else if (v != MININT && w != MININT) {
		get_rat(0);
		orat_set_si(rat(0), v, w);
	    }
	    else {
		get_mpq(0);
		mpq_set_si(mpq(0), v, w);
		mpq_canonicalize(mpq(0));
	    }
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = pdp(1) / pdp(2);
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_div(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpq(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_div(mpq(0), mpq(3), mpq(4));
		collect(3);
		collect(4);
	    }
	    break;
	case t_mpz:
	    get_mpq(0);
	    mpz_set(mpq_numref(mpq(0)), mpz(1));
	    mpz_set(mpq_denref(mpq(0)), mpz(2));
	    mpq_canonicalize(mpq(0));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_div(mpq(0), mpq(1), mpq(2));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_div(mpr(0), mpr(1), mpr(2), thr_rnd);
	    break;
	case t_cdd:
	    get_cdd(0);
	    pddp(0) = pddp(1) / pddp(2);
	    break;
	case t_cqq:
	    get_cqq(0);
	    cqq_div(cqq(0), cqq(1), cqq(2));
	    break;
	default:
	    get_mpc(0);
	    mpc_div(mpc(0), mpc(1), mpc(2), thr_rndc);
	    break;
    }

    return (canonicalize());
}

oobject_t
oeval_trunc2(void)
{
    oword_t		w;
    ofloat_t		d;

    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1) / pwp(2);
	    break;
	case t_float:
	    modf(pdp(1) / pdp(2), &d);
	    if (d == (oword_t)d) {
		get_word(0);
		pwp(0) = d;
	    }
	    else {
		get_mpz(0);
		mpz_set_d(mpz(0), d);
	    }
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_div(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpz(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_div(mpq(3), mpq(3), mpq(4));
		mpz_tdiv_q(mpz(0), mpq_numref(mpq(3)), mpq_denref(mpq(3)));
		collect(3);
		collect(4);
	    }
	    else {
		w = orat_num(rat(0)) / orat_den(rat(0));
		collect(0);
		get_word(0);
		pwp(0) = w;
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_tdiv_q(mpz(0), mpz(1), mpz(2));
	    break;
	case t_mpq:
	    get_mpq(3);
	    mpq_div(mpq(3), mpq(1), mpq(2));
	    get_mpz(0);
	    mpz_tdiv_q(mpz(0), mpq_numref(mpq(3)), mpq_denref(mpq(3)));
	    collect(3);
	    break;
	case t_mpr:
	    get_mpr(3);
	    mpfr_div(mpr(3), mpr(1), mpr(2), thr_rnd);
	    mpfr_trunc(mpr(3), mpr(3));
	    get_mpz(0);
	    mpfr_get_z(mpz(0), mpr((3)), GMP_RNDZ);
	    collect(3);
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

oobject_t
oeval_rem(void)
{
    oword_t		w;

    switch (otype(N(1))) {
	case t_word:
	    get_word(0);
	    pwp(0) = pwp(1) % pwp(2);
	    break;
	case t_float:
	    get_float(0);
	    pdp(0) = fmod(pdp(1), pdp(2));
	    break;
	case t_rat:
	    get_rat(0);
	    if (!orat_div(rat(0), rat(1), rat(2))) {
		collect(0);
		get_mpq(0);
		get_mpq(3);
		get_mpq(4);
		mpq_set_si(mpq(3), orat_num(rat(1)), orat_den(rat(1)));
		mpq_set_si(mpq(4), orat_num(rat(2)), orat_den(rat(2)));
		mpq_div(mpq(0), mpq(3), mpq(4));
		mpz_tdiv_r(mpq_numref(mpq(0)),
			   mpq_numref(mpq(0)), mpq_denref(mpq(0)));
		mpz_mul(mpq_denref(mpq(0)),
			mpq_denref(mpq(3)), mpq_denref(mpq(4)));
		mpq_canonicalize(mpq(0));
		if (orat_num(rat(2)) < 0)
		    mpq_neg(mpq(0), mpq(0));
		collect(3);
		collect(4);
	    }
	    else {
		w = orat_num(rat(0)) % orat_den(rat(0));
		orat_num(rat(0)) = w;
		w = ow_mul_w_w(orat_den(rat(1)), orat_den(rat(2)));
		orat_den(rat(0)) = orat_num(rat(2)) < 0 ? -w : w;
	    }
	    break;
	case t_mpz:
	    get_mpz(0);
	    mpz_tdiv_r(mpz(0), mpz(1), mpz(2));
	    break;
	case t_mpq:
	    get_mpq(0);
	    mpq_div(mpq(0), mpq(1), mpq(2));
	    mpz_tdiv_r(mpq_numref(mpq(0)),
		       mpq_numref(mpq(0)), mpq_denref(mpq(0)));
	    mpz_mul(mpq_denref(mpq(0)),
		    mpq_denref(mpq(1)), mpq_denref(mpq(2)));
	    mpq_canonicalize(mpq(0));
	    if (mpq_sgn(mpq(2)) < 0)
		mpq_neg(mpq(0), mpq(0));
	    break;
	case t_mpr:
	    get_mpr(0);
	    mpfr_fmod(mpr(0), mpr(1), mpr(2), thr_rnd);
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

oobject_t
oeval_complex(void)
{
    switch (otype(N(1))) {
	case t_word:
	    get_cqq(0);
	    cqq_set_si(cqq(0), pwp(1), pwp(2));
	    break;
	case t_float:
	    get_cdd(0);
	    real(pddp(0)) = pdp(1);
	    imag(pddp(0)) = pdp(2);
	    break;
	case t_rat:
	    get_cqq(0);
	    mpq_set_si(cqq_realref(cqq(0)), orat_num(rat(1)), orat_den(rat(1)));
	    mpq_set_si(cqq_imagref(cqq(0)), orat_num(rat(2)), orat_den(rat(2)));
	    break;
	case t_mpz:
	    get_cqq(0);
	    mpq_set_z(cqq_realref(cqq(0)), mpz(1));
	    mpq_set_z(cqq_imagref(cqq(0)), mpz(2));
	    break;
	case t_mpq:
	    get_cqq(0);
	    mpq_set(cqq_realref(cqq(0)), mpq(1));
	    mpq_set(cqq_imagref(cqq(0)), mpq(2));
	    break;
	case t_mpr:
	    get_mpc(0);
	    mpfr_set(mpc_realref(mpc(0)), mpr(1), thr_rnd);
	    mpfr_set(mpc_imagref(mpc(0)), mpr(2), thr_rnd);
	    break;
	default:
	    abort();
    }

    return (canonicalize());
}

static void
eval_unary_setup(oobject_t value)
{
    if (value)
	N(1) = value;
    else {
	coerced(1);	get_word(1);	pwp(1) = 0;
    }
}

void
eval_binary_setup(oobject_t lvalue, oobject_t rvalue)
{
    otype_t		ltype;
    otype_t		rtype;

    if (lvalue)		ltype = otype(lvalue);
    else		ltype = t_void;
    if (rvalue)		rtype = otype(rvalue);
    else		rtype = t_void;
    switch (ltype) {
	case t_void:
	    switch (rtype) {
		case t_void:
		    coerced(1);		get_word(1);
		    pwp(1) = 0;
		    coerced(2);		get_word(2);
		    pwp(2) = 0;
		    break;
		case t_word:
		    coerced(1);		get_word(1);
		    pwp(1) = 0;
		    N(2) = rvalue;
		    break;
		case t_float:
		    coerced(1);		get_float(1);
		    pdp(1) = 0.0;
		    N(2) = rvalue;
		    break;
		case t_rat:
		    coerced(1);		get_rat(1);
		    orat_set_si(rat(1), 0, 1);
		    N(2) = rvalue;
		    break;
		case t_mpz:
		    coerced(1);		get_mpz(1);
		    mpz_set_ui(mpz(1), 0);
		    N(2) = rvalue;
		    break;
		case t_mpq:
		    coerced(1);		get_mpq(1);
		    mpq_set_ui(mpq(1), 0, 1);
		    N(2) = rvalue;
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_ui(mpr(1), 0, thr_rnd);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = 0.0;
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cqq(1);
		    cqq_set_ui(cqq(1), 0, 1);
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_ui(mpc(1), 0, thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_word:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_word(2);
		    pwp(2) = 0;
		    break;
		case t_word:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_float:
		    coerced(1);		get_float(1);
		    pdp(1) = _pwp(lvalue);
		    N(2) = rvalue;
		    break;
		case t_rat:
		    if (unlikely(_pwp(lvalue) == MININT)) {
			coerced(1);	get_mpq(1);
			mpq_set_si(mpq(1), _pwp(lvalue), 1);
			coerced(2);	get_mpq(2);
			mpq_set_si(mpq(2),
				   orat_num(_rat(rvalue)),
				   orat_den(_rat(rvalue)));
		    }
		    else {
			coerced(1);	get_rat(1);
			orat_set_si(rat(1), _pwp(lvalue), 1);
			N(2) = rvalue;
		    }
		    break;
		case t_mpz:
		    coerced(1);		get_mpz(1);
		    mpz_set_si(mpz(1), _pwp(lvalue));
		    N(2) = rvalue;
		    break;
		case t_mpq:
		    coerced(1);		get_mpq(1);
		    mpq_set_si(mpq(1), _pwp(lvalue), 1);
		    N(2) = rvalue;
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_si(mpr(1), _pwp(lvalue), thr_rnd);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = _pwp(lvalue);
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cqq(1);
		    cqq_set_si(cqq(1), _pwp(lvalue), 1);
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_si(mpc(1), 1, thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_float:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_float(2);
		    pdp(2) = 0.0;
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_float(2);
		    pdp(2) = _pwp(rvalue);
		    break;
		case t_float:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_rat:
		    N(1) = lvalue;
		    coerced(2);		get_float(2);
		    pdp(2) = orat_get_d(_rat(rvalue));
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    coerced(2);		get_float(2);
		    pdp(2) = mpz_get_d(_mpz(rvalue));
		    break;
		case t_mpq:
		    N(1) = lvalue;
		    coerced(2);		get_float(2);
		    pdp(2) = mpq_get_d(_mpq(rvalue));
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_d(mpr(1), _pdp(lvalue), thr_rnd);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = _pdp(lvalue);
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cdd(1);
		    pddp(1) = _pdp(lvalue);
		    coerced(2);		get_cdd(2);
		    real(pddp(2)) = mpq_get_d(cqq_realref(_cqq(rvalue)));
		    imag(pddp(2)) = mpq_get_d(cqq_imagref(_cqq(rvalue)));
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_d(mpc(1), _pdp(lvalue), thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_rat:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_rat(2);
		    orat_set_si(rat(2), 0, 1);
		    break;
		case t_word:
		    if (unlikely(_pwp(rvalue) == MININT)) {
			coerced(1);	get_mpq(1);
			mpq_set_si(mpq(1),
				   orat_num(_rat(lvalue)),
				   orat_den(_rat(lvalue)));
			coerced(2);	get_mpq(2);
			mpq_set_si(mpq(2), _pwp(rvalue), 1);
		    }
		    else {
			N(1) = lvalue;
			coerced(2);	get_rat(2);
			orat_set_si(rat(2), _pwp(rvalue), 1);
		    }
		    break;
		case t_float:
		    coerced(1);		get_float(1);
		    pdp(1) = orat_get_d(_rat(lvalue));
		    N(2) = rvalue;
		    break;
		case t_rat:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_mpz:
		    coerced(1);		get_mpq(1);
		    mpq_set_si(mpq(1),
			       orat_num(_rat(lvalue)),
			       orat_den(_rat(lvalue)));
		    coerced(2);		get_mpq(2);
		    mpq_set_z(mpq(2), _mpz(rvalue));
		    break;
		case t_mpq:
		    coerced(1);		get_mpq(1);
		    mpq_set_si(mpq(1),
			       orat_num(_rat(lvalue)),
			       orat_den(_rat(lvalue)));
		    N(2) = rvalue;
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_si(mpr(1), orat_num(_rat(lvalue)), thr_rnd);
		    coerced(2);		get_mpr(2);
		    mpfr_set_si(mpr(2), orat_den(_rat(lvalue)), thr_rnd);
		    mpfr_div(mpr(1), mpr(1), mpr(2), thr_rnd);
		    collect(2);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = orat_get_d(_rat(lvalue));
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cqq(1);
		    mpq_set_si(cqq_realref(cqq(1)),
			       orat_num(_rat(lvalue)),
			       orat_den(_rat(lvalue)));
		    mpq_set_si(cqq_imagref(cqq(1)), 0, 1);
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    coerced(2);		get_mpr(2);
		    mpfr_set_si(mpc_realref(mpc(1)), orat_num(_rat(lvalue)),
				thr_rnd);
		    mpfr_set_si(mpr(2), orat_den(_rat(lvalue)), thr_rnd);
		    mpfr_div(mpc_realref(mpc(1)), mpc_realref(mpc(1)),
			     mpr(2), thr_rnd);
		    mpfr_set_ui(mpc_imagref(mpc(1)), 0, thr_rnd);
		    collect(2);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_mpz:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_mpz(2);
		    mpz_set_ui(mpz(2), 0);
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_mpz(2);
		    mpz_set_si(mpz(2), _pwp(rvalue));
		    break;
		case t_float:
		    coerced(1);		get_float(1);
		    pdp(1) = mpz_get_d(_mpz(lvalue));
		    N(2) = rvalue;
		    break;
		case t_rat:
		    coerced(1);		get_mpq(1);
		    mpq_set_z(mpq(1), _mpz(lvalue));
		    coerced(2);		get_mpq(2);
		    mpq_set_si(mpq(2),
			       orat_num(_rat(rvalue)),
			       orat_den(_rat(rvalue)));
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_mpq:
		    coerced(1);		get_mpq(1);
		    mpq_set_z(mpq(1), _mpz(lvalue));
		    N(2) = rvalue;
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_z(mpr(1), _mpz(lvalue), thr_rnd);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = mpz_get_d(_mpz(lvalue));
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cqq(1);
		    cqq_set_z(cqq(1), _mpz(lvalue));
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_z(mpc(1), _mpz(lvalue), thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_mpq:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_mpq(2);
		    mpq_set_ui(mpq(2), 0, 1);
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_mpq(2);
		    mpq_set_ui(mpq(2), _pwp(rvalue), 1);
		    break;
		case t_float:
		    coerced(1);		get_float(1);
		    pdp(1) = mpq_get_d(_mpq(lvalue));
		    N(2) = rvalue;
		    break;
		case t_rat:
		    N(1) = lvalue;
		    coerced(2);		get_mpq(2);
		    mpq_set_si(mpq(2),
			       orat_num(_rat(rvalue)),
			       orat_den(_rat(rvalue)));
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    coerced(2);		get_mpq(2);
		    mpq_set_z(mpq(2), _mpz(rvalue));
		    break;
		case t_mpq:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_mpr:
		    coerced(1);		get_mpr(1);
		    mpfr_set_q(mpr(1), _mpq(lvalue), thr_rnd);
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    pddp(1) = mpq_get_d(_mpq(lvalue));
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    coerced(1);		get_cqq(1);
		    cqq_set_q(cqq(1), _mpq(lvalue));
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_q(mpc(1), _mpq(lvalue), thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_mpr:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_mpr(2);
		    mpfr_set_ui(mpr(2), 0, thr_rnd);
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_mpr(2);
		    mpfr_set_si(mpr(2), _pwp(rvalue), thr_rnd);
		    break;
		case t_float:
		    N(1) = lvalue;
		    coerced(2);		get_mpr(2);
		    mpfr_set_d(mpr(2), _pdp(rvalue), thr_rnd);
		    break;
		case t_rat:
		    N(1) = lvalue;
		    coerced(2);		get_mpr(2);
		    mpfr_set_si(mpr(2), orat_num(_rat(rvalue)), thr_rnd);
		    get_mpr(3);
		    mpfr_set_si(mpr(3), orat_den(_rat(rvalue)), thr_rnd);
		    mpfr_div(mpr(2), mpr(2), mpr(3), thr_rnd);
		    collect(3);
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    coerced(2);		get_mpr(2);
		    mpfr_set_z(mpr(2), _mpz(rvalue), thr_rnd);
		    break;
		case t_mpq:
		    N(1) = lvalue;
		    coerced(1);		get_mpr(2);
		    mpfr_set_q(mpr(2), _mpq(rvalue), thr_rnd);
		    break;
		case t_mpr:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_cdd:
		    coerced(1);		get_mpc(1);
		    mpc_set_fr(mpc(1), _mpr(lvalue), thr_rndc);
		    coerced(2);		get_mpc(2);
		    mpc_set_dc(mpc(2), _pddp(rvalue), thr_rndc);
		    break;
		case t_cqq:
		    coerced(1);		get_mpc(1);
		    mpc_set_fr(mpc(1), _mpr(lvalue), thr_rndc);
		    coerced(2);		get_mpc(2);
		    mpfr_set_q(mpc_realref(mpc(2)), cqq_realref(_cqq(rvalue)),
			       thr_rnd);
		    mpfr_set_q(mpc_imagref(mpc(2)), cqq_imagref(_cqq(rvalue)),
			       thr_rnd);
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_fr(mpc(1), _mpr(lvalue), thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_cdd:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = 0.0;
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = _pwp(rvalue);
		    break;
		case t_float:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = _pdp(rvalue);
		    break;
		case t_rat:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = orat_get_d(_rat(lvalue));
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = mpz_get_d(_mpz(rvalue));
		    break;
		case t_mpq:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    pddp(2) = mpq_get_d(_mpq(rvalue));
		    break;
		case t_mpr:
		    coerced(1);		get_mpc(1);
		    mpc_set_dc(mpc(1), _pddp(lvalue), thr_rndc);
		    coerced(2);		get_mpc(2);
		    mpc_set_fr(mpc(2), _mpr(rvalue), thr_rndc);
		    break;
		case t_cdd:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    N(1) = lvalue;
		    coerced(2);		get_cdd(2);
		    real(pddp(2)) = mpq_get_d(cqq_realref(_cqq(rvalue)));
		    imag(pddp(2)) = mpq_get_d(cqq_imagref(_cqq(rvalue)));
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpc_set_dc(mpc(1), _pddp(lvalue), thr_rndc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	case t_cqq:
	    switch (rtype) {
		case t_void:
		    N(1) = lvalue;
		    coerced(2);		get_cqq(2);
		    cqq_set_ui(cqq(2), 1, 0);
		    break;
		case t_word:
		    N(1) = lvalue;
		    coerced(2);		get_cqq(2);
		    cqq_set_si(cqq(2), _pwp(rvalue), 0);
		    break;
		case t_float:
		    coerced(1);		get_cdd(1);
		    real(pddp(1)) = mpq_get_d(cqq_realref(_cqq(lvalue)));
		    imag(pddp(1)) = mpq_get_d(cqq_imagref(_cqq(lvalue)));
		    coerced(2);		get_cdd(2);
		    pddp(2) = _pdp(rvalue);
		    break;
		case t_rat:
		    N(1) = rvalue;
		    coerced(2);		get_cqq(2);
		    cqq_set_si(cqq(2),
			       orat_num(_rat(rvalue)),
			       orat_den(_rat(rvalue)));
		    break;
		case t_mpz:
		    N(1) = lvalue;
		    coerced(2);		get_cqq(2);
		    cqq_set_z(cqq(2), _mpz(rvalue));
		    break;
		case t_mpq:
		    N(1) = lvalue;
		    coerced(2);		get_cqq(2);
		    cqq_set_q(cqq(2), _mpq(rvalue));
		    break;
		case t_mpr:
		    get_mpc(1);
		    mpfr_set_q(mpc_realref(mpc(1)), cqq_realref(_cqq(lvalue)),
			       thr_rnd);
		    mpfr_set_q(mpc_imagref(mpc(1)), cqq_imagref(_cqq(lvalue)),
			       thr_rnd);
		    coerced(2);		get_mpc(2);
		    mpc_set_fr(mpc(2), _mpr(rvalue), thr_rndc);
		    break;
		case t_cdd:
		    coerced(1);		get_cdd(1);
		    real(pddp(1)) = mpq_get_d(cqq_realref(_cqq(lvalue)));
		    imag(pddp(1)) = mpq_get_d(cqq_imagref(_cqq(lvalue)));
		    N(2) = rvalue;
		    break;
		case t_cqq:
		    N(1) = lvalue;
		    N(2) = rvalue;
		    break;
		default:
		    assert(rtype == t_mpc);
		    coerced(1);		get_mpc(1);
		    mpfr_set_q(mpc_realref(mpc(1)), cqq_realref(_cqq(lvalue)),
			       thr_rnd);
		    mpfr_set_q(mpc_imagref(mpc(1)), cqq_imagref(_cqq(lvalue)),
			       thr_rnd);
		    N(2) = rvalue;
		    break;
	    }
	    break;
	default:
	    assert(ltype == t_mpc);
	    N(1) = lvalue;
	    switch (rtype) {
		case t_void:
		    coerced(2);		get_mpc(2);
		    mpc_set_ui(mpc(2), 0, thr_rndc);
		    break;
		case t_word:
		    coerced(2);		get_mpc(2);
		    mpc_set_si(mpc(2), _pwp(lvalue), thr_rndc);
		    break;
		case t_float:
		    coerced(2);		get_mpc(2);
		    mpc_set_d(mpc(2), _pdp(rvalue), thr_rndc);
		    break;
		case t_rat:
		    coerced(2);		get_mpc(2);
		    get_mpr(3);
		    mpfr_set_si(mpc_realref(mpc(2)), orat_num(_rat(rvalue)),
				thr_rnd);
		    mpfr_set_si(mpr(3), orat_den(_rat(rvalue)), thr_rnd);
		    mpfr_div(mpc_realref(mpc(2)), mpc_realref(mpc(2)),
			     mpr(3), thr_rnd);
		    mpfr_set_ui(mpc_imagref(mpc(2)), 0, thr_rnd);
		    collect(3);
		    break;
		case t_mpz:
		    coerced(2);		get_mpc(2);
		    mpc_set_z(mpc(2), _mpz(rvalue), thr_rndc);
		    break;
		case t_mpq:
		    coerced(2);		get_mpc(2);
		    mpc_set_q(mpc(2), _mpq(rvalue), thr_rndc);
		    break;
		case t_mpr:
		    coerced(2);		get_mpc(2);
		    mpc_set_fr(mpc(2), _mpr(rvalue), thr_rndc);
		    break;
		case t_cdd:
		    coerced(2);		get_mpc(2);
		    mpc_set_dc(mpc(2), _pddp(rvalue), thr_rndc);
		    break;
		case t_cqq:
		    coerced(2);		get_mpc(2);
		    mpfr_set_q(mpc_realref(mpc(2)), cqq_realref(_cqq(rvalue)),
			       thr_rnd);
		    mpfr_set_q(mpc_imagref(mpc(2)), cqq_imagref(_cqq(rvalue)),
			       thr_rnd);
		    break;
		default:
		    assert(rtype == t_mpc);
		    N(2) = rvalue;
		    break;
	    }
	    break;
    }
}

static void
check_real(oast_t *ast)
{
    if (ast->l.value) {
	switch (otype(ast->l.value)) {
	    case t_word:	case t_float:		case t_rat:
	    case t_mpz:		case t_mpq:		case t_mpr:
		break;
	    default:
		oparse_error(ast, "not a real number %A", ast);
	}
    }
}

static void
check_rational(oast_t *ast)
{
    if (ast->l.value) {
	switch (otype(ast->l.value)) {
	    case t_word:	case t_rat:
	    case t_mpz:		case t_mpq:
		break;
	    default:
		oparse_error(ast, "not a rational number %A", ast);
	}
    }
}

static void
check_integer(oast_t *ast)
{
    otype_t		type;

    if (ast->l.value) {
	type = otype(ast->l.value);
	if (type != t_word && type != t_mpz)
	    oparse_error(ast, "not an integer %A", ast);
    }
}

static oint32_t
shift_power(oast_t *ast)
{
    oint32_t		 shift;
    union {
	oword_t		*wrd;
	ofloat_t	*flt;
	ompr_t		 mpr;
	oobject_t	 obj;
    } value;

    if ((value.obj = ast->l.value) == null)
	return (shift_canonicalize(ast, 0));
    switch (otype(value.obj)) {
	case t_word:
	    shift = *value.wrd;
#if __WORDSIZE == 64
	    if ((oint32_t)shift == *value.wrd)
#endif
		return (shift_canonicalize(ast, shift));
	    break;
	case t_float:
	    shift = *value.flt;
	    if (shift == *value.flt)
		return (shift_canonicalize(ast, shift));
	    break;
	case t_mpr:
	    if (mpfr_integer_p(value.mpr) &&
		mpfr_fits_sint_p(value.mpr, GMP_RNDZ))
		return (shift_canonicalize(ast,
					   mpfr_get_si(value.mpr, thr_rnd)));
	    break;
    }

    oparse_error(ast, "not a 32 bit integer %A", ast);
}

static oint32_t
shift_canonicalize(oast_t *ast, oint32_t shift)
{
    if (shift < SHIFT_MIN || shift > SHIFT_MAX)
	oparse_error(ast, "shift too large");
    odel_object(&ast->l.value);
    onew_word(&ast->l.value, shift);
    return (shift);
}

static oobject_t
canonicalize(void)
{
    oword_t		w;
    ofloat_t		d;
    ompz_t		z;
    ompq_t		q;
    rat_t		r;
    ocqq_t		qq;
    ompc_t		cc;

    switch (otype(N(0))) {
	case t_rat:
	    w = orat_canonicalize(rat(0));
	    assert(w != 0);
	    if (orat_den(rat(0)) == 1) {
		w = orat_num(rat(0));
		collect(0);
		get_word(0);
		pwp(0) = w;
	    }
	    else if (orat_num(rat(0)) == 0) {
		collect(0);
		get_word(0);
		pwp(0) = 0;
	    }
	    break;
	case t_mpz:
	    z = mpz(0);
	    if (mpz_fits_slong_p(z)) {
		w = mpz_get_si(z);
		collect(0);
		get_word(0);
		pwp(0) = w;
	    }
	    break;
	case t_mpq:
	    q = mpq(0);
	    if (mpz_cmp_ui(mpq_denref(q), 1) == 0) {
		z = mpq_numref(q);
		if (mpz_fits_slong_p(z)) {
		    w = mpz_get_si(z);
		    collect(0);
		    get_word(0);
		    pwp(0) = w;
		}
		else {
		    collect(0);
		    get_mpz(0);
		    mpz_set(mpz(0), z);
		}
	    }
	    else if (mpz_cmp_ui(mpq_numref(q), 0) == 0) {
		collect(0);
		get_word(0);
		pwp(0) = 0;
	    }
	    else if (mpz_fits_slong_p(mpq_numref(q)) &&
		     mpz_fits_slong_p(mpq_denref(q))) {
		rat_num(r) = mpz_get_si(mpq_numref(q));
		rat_den(r) = mpz_get_si(mpq_denref(q));
		if (rat_num(r) != MININT && rat_den(r) != MININT) {
		    collect(0);
		    get_rat(0);
		    orat_num(rat(0)) = rat_num(r);
		    orat_den(rat(0)) = rat_den(r);
		}
	    }
	    break;
	case t_cdd:
	    if (imag(pddp(0)) == 0) {
		d = real(pddp(0));
		collect(0);
		get_float(0);
		pdp(0) = d;
	    }
	    break;
	case t_cqq:
	    qq = cqq(0);
	    if (mpq_sgn(cqq_imagref(qq)) == 0) {
		q = cqq_realref(qq);
		if (mpz_cmp_ui(mpq_denref(q), 1) == 0) {
		    z = mpq_numref(q);
		    if (mpz_fits_slong_p(z)) {
			w = mpz_get_si(z);
			collect(0);
			get_word(0);
			pwp(0) = w;
		    }
		    else {
			collect(0);
			get_mpz(0);
			mpz_set(mpz(0), z);
		    }
		}
		else if (mpz_cmp_ui(mpq_numref(q), 0) == 0) {
		    collect(0);
		    get_word(0);
		    pwp(0) = 0;
		}
		else if (mpz_fits_slong_p(mpq_numref(q)) &&
			 mpz_fits_slong_p(mpq_denref(q))) {
		    rat_num(r) = mpz_get_si(mpq_numref(q));
		    rat_den(r) = mpz_get_si(mpq_denref(q));
		    if (rat_num(r) != MININT && rat_den(r) != MININT) {
			collect(0);
			get_rat(0);
			orat_num(rat(0)) = rat_num(r);
			orat_den(rat(0)) = rat_den(r);
		    }
		}
		else {
		    collect(0);
		    get_mpq(0);
		    mpq_set(mpq(0), q);
		}
	    }
	    break;
	case t_mpc:
	    cc = mpc(0);
	    if (mpfr_zero_p(mpc_imagref(cc))) {
		collect(0);
		get_mpr(0);
		mpfr_set(mpr(0), mpc_realref(cc), thr_rnd);
	    }
	    break;
	default:
	    break;
    }

    return (cleanup());
}

static oobject_t
cleanup(void)
{
    if (cache_mask & (1 << 1))
	odel_object(P(1));
    if (cache_mask & (1 << 2))
	odel_object(P(2));
    cache_mask = 0;

    return (N(0));
}
