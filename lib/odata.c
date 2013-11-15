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

#define i8(o)				*(oint8_t *)o
#define i16(o)				*(oint16_t *)o
#define i32(o)				*(oint32_t *)o
#define i64(o)				*(oint64_t *)o
#define wrd(o)				*(oword_t *)o
#define f32(o)				*(ofloat32_t *)o
#define f64(o)				*(ofloat64_t *)o
#define flt(o)				*(ofloat_t *)o
#define mpz(o)				(ompz_t)o
#define rat(o)				(orat_t)o
#define mpq(o)				(ompq_t)o
#define mpr(o)				(ompr_t)o
#define cdd(o)				*(ocdd_t *)o
#define cqq(o)				(ocqq_t)o
#define mpc(o)				(ompc_t)o

/*
 * Prototypes
 */
static void
data_stat(oast_t *ast);

static void
data_decl(oast_t *ast);

static void
data_set(oast_t *ast);

static void
data_init(oobject_t *p, otag_t *tag, oast_t *ast);

static void
data_record(oobject_t *p, otag_t *tag, oast_t *ast);

static void
data_vector(oobject_t *p, otag_t *tag, oast_t *ast);

/*
 * Implementation
 */
void
init_data(void)
{
}

void
finish_data(void)
{
}

void
odata(oast_t *ast)
{
    switch (ast->token) {
	case tok_set:
	    data_set(ast);
	    break;
	case tok_andset:	case tok_orset:
	case tok_xorset:	case tok_mul2set:
	case tok_div2set:	case tok_shlset:
	case tok_shrset:	case tok_addset:
	case tok_subset:	case tok_mulset:
	case tok_divset:	case tok_trunc2set:
	case tok_remset:
	    odata(ast->l.ast);
	    odata(ast->r.ast);
	    break;
	case tok_vector:
	    odata(ast->l.ast);
	    data_stat(ast->r.ast);
	    break;
	case tok_dot:
	    odata(ast->l.ast);
	    break;
	case tok_andand:	case tok_oror:
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
	    odata(ast->l.ast);
	    odata(ast->r.ast);
	    break;
	case tok_inc:		case tok_dec:
	case tok_postinc:	case tok_postdec:
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
	    odata(ast->l.ast);
	    break;
	case tok_init:
	    odata(ast->r.ast);
	    break;
	case tok_atan2:		case tok_pow:
	case tok_hypot:		case tok_subtypeof:
	    odata(ast->l.ast);
	    odata(ast->r.ast);
	    break;
	case tok_question:
	    odata(ast->t.ast);
	    odata(ast->l.ast);
	    odata(ast->r.ast);
	    break;
	case tok_if:
	    data_stat(ast->t.ast);
	    data_stat(ast->l.ast);
	    data_stat(ast->r.ast);
	    break;
	case tok_return:	case tok_throw:
	    if (ast->l.ast)
		odata(ast->l.ast);
	    break;
	case tok_switch:
	    data_stat(ast->t.ast);
	    data_stat(ast->c.ast);
	    break;
	case tok_for:
	    data_stat(ast->l.ast);
	    data_stat(ast->t.ast);
	    data_stat(ast->r.ast);
	    data_stat(ast->c.ast);
	    break;
	case tok_do:		case tok_while:
	    data_stat(ast->c.ast);
	    data_stat(ast->t.ast);
	    break;
	case tok_call:
	    odata(ast->l.ast);
	    data_stat(ast->r.ast);
	    break;
	case tok_decl:
	    data_decl(ast->r.ast);
	    break;
	case tok_stat:		case tok_code:
	case tok_list:		case tok_finally:
	    data_stat(ast->l.ast);
	case tok_try:		case tok_catch:
	    data_stat(ast->r.ast);
	    break;
	case tok_sizeof:	case tok_symbol:	case tok_goto:
	case tok_type:		case tok_number:	case tok_string:
	case tok_class:		case tok_label:		case tok_break:
	case tok_continue:	case tok_case:		case tok_default:
	case tok_function:	case tok_ellipsis:	case tok_typeof:
	    break;
	default:
#if DEBUG
	    oparse_warn(ast,
			"data: not handling %s", otoken_to_charp(ast->token));
#endif
	    break;
    }
}

void
data_stat(oast_t *ast)
{
    for (; ast; ast = ast->next)
	odata(ast);
}

static void
data_decl(oast_t *ast)
{
    for (; ast; ast = ast->next) {
	switch (ast->token) {
	    case tok_set:
		data_set(ast);
		break;
	    case tok_symbol:		case tok_vecdcl:
	    case tok_proto:
		break;
	    default:
		abort();
	}
    }
}

static void
data_set(oast_t *ast)
{
    oast_t		*last;
    oast_t		*rast;
    osymbol_t		*symbol;

    rast = ast->r.ast;
    if (rast->token == tok_data) {
	last = ast->l.ast;
	for (;;) {
	    switch (last->token) {
		case tok_symbol:
		    symbol = last->l.value;
		    data_init(&rast->r.value, symbol->tag, rast);
		    return;
		case tok_vecdcl:	case tok_proto:
		    last = last->l.ast;
		    break;
		case tok_dot:
		    last = last->r.ast;
		    break;
		default:
		    abort();
	    }
	}
    }
}

static void
data_init(oobject_t *p, otag_t *tag, oast_t *ast)
{
    otype_t		t;
    oobject_t		o;

    t = otag_to_type(tag);
    if (ast->token == tok_number) {
	o = ast->l.value;
	switch (otype(o)) {
	    case t_word:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = wrd(o);
			break;
		    case t_int16:	case t_uint16:
			i16(p) = wrd(o);
			break;
		    case t_int32:	case t_uint32:
			i32(p) = wrd(o);
			break;
		    case t_int64:	case t_uint64:
			i64(p) = wrd(o);
			break;
		    case t_float32:
			f32(p) = wrd(o);
			break;
		    case t_float64:
			f64(p) = wrd(o);
			break;
		    case t_undef:
			oget_word(p);
			**(oword_t **)p = wrd(o);
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_float:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = flt(o);
			break;
		    case t_int16:	case t_uint16:
			i16(p) = flt(o);
			break;
		    case t_int32:	case t_uint32:
			i32(p) = flt(o);
			break;
		    case t_int64:	case t_uint64:
			i64(p) = flt(o);
			break;
		    case t_float32:
			f32(p) = flt(o);
			break;
		    case t_float64:
			f64(p) = flt(o);
			break;
		    case t_undef:
			oget_float(p);
			**(ofloat_t **)p = flt(o);
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_mpz:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = ompz_get_w(mpz(o));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = ompz_get_w(mpz(o));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = ompz_get_w(mpz(o));
			break;
		    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
			i64(p) = ompz_get_sl(mpz(o));
#else
			i64(p) = ompz_get_w(mpz(o));
#endif
			break;
		    case t_float32:
			f32(p) = mpz_get_d(mpz(o));
			break;
		    case t_float64:
			f64(p) = mpz_get_d(mpz(o));
			break;
		    case t_undef:
			oget_mpz(p);
			mpz_set(*(ompz_t *)p, mpz(o));
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_rat:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = orat_num(rat(o)) / orat_den(rat(o));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = orat_num(rat(o)) / orat_den(rat(o));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = orat_num(rat(o)) / orat_den(rat(o));
			break;
		    case t_int64:	case t_uint64:
			i64(p) = orat_num(rat(o)) / orat_den(rat(o));
			break;
		    case t_float32:
			f32(p) = orat_get_d(rat(o));
			break;
		    case t_float64:
			f64(p) = orat_get_d(rat(o));
			break;
		    case t_undef:
			oget_rat(p);
			**(rat_t **)p = *(rat_t *)o;
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_mpq:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = ompq_get_w(mpq(o));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = ompq_get_w(mpq(o));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = ompq_get_w(mpq(o));
			break;
		    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
			i64(p) = ompq_get_sl(mpq(o));
#else
			i64(p) = ompq_get_w(mpq(o));
#endif
			break;
		    case t_float32:
			f32(p) = mpq_get_d(mpq(o));
			break;
		    case t_float64:
			f64(p) = mpq_get_d(mpq(o));
			break;
		    case t_undef:
			oget_mpq(p);
			mpq_set(*(ompq_t *)p, mpq(o));
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_mpr:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = ompr_get_w(mpr(o));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = ompr_get_w(mpr(o));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = ompr_get_w(mpr(o));
			break;
		    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
			i64(p) = ompr_get_sl(mpr(o));
#else
			i64(p) = ompr_get_w(mpr(o));
#endif
			break;
		    case t_float32:
			f32(p) = mpfr_get_d(mpr(o), thr_rnd);
			break;
		    case t_float64:
			f64(p) = mpfr_get_d(mpr(o), thr_rnd);
			break;
		    case t_undef:
			oget_mpr(p);
			mpfr_set(*(ompr_t *)p, mpr(o), thr_rnd);
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_cdd:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = real(cdd(o));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = real(cdd(o));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = real(cdd(o));
			break;
		    case t_int64:	case t_uint64:
			i64(p) = real(cdd(o));
			break;
		    case t_float32:
			f32(p) = real(cdd(o));
			break;
		    case t_float64:
			f64(p) = real(cdd(o));
			break;
		    case t_undef:
			oget_cdd(p);
			**(ocdd_t **)p = cdd(o);
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    case t_cqq:
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = ompq_get_w(cqq_realref(cqq(o)));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = ompq_get_w(cqq_realref(cqq(o)));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = ompq_get_w(cqq_realref(cqq(o)));
			break;
		    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
			i64(p) = ompq_get_w(cqq_realref(cqq(o)));
#else
			i64(p) = ompq_get_w(cqq_realref(cqq(o)));
#endif
			break;
		    case t_float32:
			f32(p) = mpq_get_d(cqq_realref(cqq(o)));
			break;
		    case t_float64:
			f64(p) = mpq_get_d(cqq_realref(cqq(o)));
			break;
		    case t_undef:
			oget_cqq(p);
			cqq_set(*(ocqq_t *)p, cqq(o));
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	    default:
		assert(otype(o) == t_mpc);
		switch (t) {
		    case t_int8:	case t_uint8:
			i8(p) = ompr_get_w(mpc_realref(mpc(o)));
			break;
		    case t_int16:	case t_uint16:
			i16(p) = ompr_get_w(mpc_realref(mpc(o)));
			break;
		    case t_int32:	case t_uint32:
			i32(p) = ompr_get_w(mpc_realref(mpc(o)));
			break;
		    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
			i64(p) = ompr_get_w(mpc_realref(mpc(o)));
#else
			i64(p) = ompr_get_w(mpc_realref(mpc(o)));
#endif
			break;
		    case t_float32:
			f32(p) = mpfr_get_d(mpc_realref(mpc(o)), thr_rnd);
			break;
		    case t_float64:
			f64(p) = mpfr_get_d(mpc_realref(mpc(o)), thr_rnd);
			break;
		    case t_undef:
			oget_mpc(p);
			mpc_set(*(ompc_t *)p, mpc(o), thr_rndc);
			break;
		    default:
			oparse_error(ast, "invalid initializer");
		}
		break;
	}
    }
    else if (ast->token == tok_data) {
	if (t & t_vector) {
	    if (tag->size == 0)
		tag = otag_ast_data(tag, ast);
	    ast->t.value = tag;
	    data_vector(&ast->r.value, tag, ast->l.ast);
	}
	else if (t > t_mpc) {
	    ast->t.value = tag;
	    data_record(&ast->r.value, tag, ast->l.ast);
	}
	else
	    oparse_error(ast, "invalid initializer");
    }
    /* else dynamic initializer */
}

static void
data_record(oobject_t *p, otag_t *tag, oast_t *ast)
{
    oint8_t		*data;
    oword_t		 index;
    oword_t		 offset;
    orecord_t		*record;
    osymbol_t		*symbol;
    ovector_t		*vector;

    /* Input correctness must have been already checked */
    assert(tag->type == tag_class);
    record = tag->name;
    assert(record && otype(record) == t_record);
    onew_object(p, record->type, record->length);
    data = *p;
    vector = record->vector;
    for (offset = 0; ast; offset++) {
	if (ast->token == tok_init) {
	    symbol = ast->l.ast->l.value;
	    symbol = oget_symbol(record, symbol->name);
	    assert(symbol);
	    for (index = 0; index < vector->offset; index++) {
		if (vector->v.ptr[index] == symbol)
		    break;
	    }
	    assert(index < vector->offset);
	    offset = index;
	    data_init((oobject_t *)(data + symbol->offset),
		      symbol->tag, ast->r.ast);
	}
	else {
	    assert(offset < vector->offset);
	    symbol = vector->v.ptr[offset];
	    if (!symbol->field)
		continue;
	    data_init((oobject_t *)(data + symbol->offset),
		      symbol->tag, ast);
	}
	ast = ast->next;
    }
}

static void
data_vector(oobject_t *p, otag_t *tag, oast_t *ast)
{
    otag_t		*base;
    oint8_t		*data;
    oword_t		 size;
    otype_t		 type;
    oword_t		 offset;
    ovector_t		*vector;

    /* Input correctness must have been already checked */
    assert(tag->type == tag_vector);
    base = tag->base;
    if (base->type == tag_basic)
	size = base->size;
    else
	size = sizeof(oobject_t);
    type = otag_to_type(base);
    if (type < 0 || type > t_mpc)
	type = t_void;
    onew_vector(p, type, tag->size);
    vector = *p;
    data = vector->v.i8;
    for (offset = 0; ast; ast = ast->next, offset++) {
	if (ast->token == tok_init) {
	    offset = *(oword_t *)ast->l.ast->l.value;
	    assert(offset >= 0 && offset < tag->size);
	    data_init((oobject_t *)(data + offset * size), base, ast->r.ast);
	}
	else {
	    assert(offset >= 0 && offset < tag->size);
	    data_init((oobject_t *)(data + offset * size), base, ast);
	}
    }
}
