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

#include <stdio.h>

#define dputc(ch)		oputc(std_output, ch)
#define dputs(object, length)	owrite(std_output, object, length)

#define print_ast(value)	write_ast(value, indent, format)
#define print_obj(value)	write_object(value, format)
#define print_int(value)	oprint_wrd(std_output, format, value)
#define print_flt(value)	oprint_flt(std_output, format, value)
#define print_str(value)	oprint_str(std_output, format, value)
#define print_rat(value)	oprint_rat(std_output, format, value)
#define print_mpz(value)	oprint_mpz(std_output, format, value)
#define print_mpq(value)	oprint_mpq(std_output, format, value)
#define print_mpr(value)	oprint_mpr(std_output, format, value)
#define print_cdd(value)	oprint_cdd(std_output, format, value)
#define print_cqq(value)	oprint_cqq(std_output, format, value)
#define print_mpc(value)	oprint_mpc(std_output, format, value)
#define print_ptr(value)	oprint_ptr(std_output, format, value)
#define print_sym(value)	dputs(value->name->v.obj, value->name->length)
#define print_vi8(vec)		write_i8_vector(vec->v.obj, vec->length, format)
#define print_vi16(vec)		write_i16_vector(vec->v.obj, vec->length, format)
#define print_vu16(vec)		write_u16_vector(vec->v.obj, vec->length, format)
#define print_vi32(vec)		write_i32_vector(vec->v.obj, vec->length, format)
#define print_vu32(vec)		write_u32_vector(vec->v.obj, vec->length, format)
#define print_vi64(vec)		write_i64_vector(vec->v.obj, vec->length, format)
#define print_vu64(vec)		write_u64_vector(vec->v.obj, vec->length, format)
#define print_vf32(vec)		write_f32_vector(vec->v.obj, vec->length, format)
#define print_vf64(vec)		write_f64_vector(vec->v.obj, vec->length, format)
#define print_vobj(vec)		write_obj_vector(vec->v.obj, vec->length, format)
#define print_rec(record,value)	write_record(record, value, format)
#define print_tag(tag, name, fields)					\
    write_tag(tag, name, fields, indent, format)
#define print_fields(value)	write_fields(value, indent, format)
#define print_ast_list(value)	write_ast_list(value, indent, format)
#define print_ast_call(value)	write_ast_call(value, indent, format)
#define print_ast_unary(string, length, value)				\
    write_ast_unary(string, length, value, indent, format)
#define print_ast_binary(string, length, value)				\
    write_ast_binary(string, length, value, indent, format)
#define print_ast_unary_function(string, length, value)			\
    write_ast_unary_function(string, length, value, indent, format)
#define print_ast_binary_function(string, length, value)		\
    write_ast_binary_function(string, length, value, indent, format)
#define print_ast_comma_list(value)					\
    write_ast_comma_list(value, indent, format)
#define print_ast_paren_comma_list(value)				\
    write_ast_paren_comma_list(value, indent, format)
#define print_ast_newline()	write_ast_newline(indent)

/*
 * Prototypes
 */
static oword_t
write_object(oobject_t object, oformat_t *format);

#define write_vector(id, type)						\
static oword_t								\
write_##id##_vector(type *pointer, oword_t length, oformat_t *format)
write_vector(i8, oint8_t);
write_vector(i16, oint16_t);
write_vector(u16, ouint16_t);
write_vector(i32, oint32_t);
write_vector(u32, ouint32_t);
write_vector(i64, oint64_t);
write_vector(u64, ouint64_t);
write_vector(f32, ofloat32_t);
write_vector(f64, ofloat64_t);
write_vector(obj, oobject_t);
#undef write_vector

static oword_t
write_record(orecord_t *record, oobject_t object, oformat_t *format);

static oword_t
write_ast(oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_list(oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_call(oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_unary(char *string, oword_t length,
		oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_binary(char *string, oword_t length,
		 oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_unary_function(char *string, oword_t length,
			 oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_binary_function(char *string, oword_t length,
			  oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_comma_list(oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_ast_paren_comma_list(oast_t *ast, oint32_t indent, oformat_t *format);

static oword_t
write_fields(orecord_t *record, oint32_t indent, oformat_t *format);

static oword_t
write_tag(otag_t *tag, osymbol_t *name, obool_t fields,
	  oint32_t indent, oformat_t *format);

static oword_t
write_ast_newline(oint32_t indent);

/*
 * Implementation
 */
void
init_debug(void)
{
}

void
finish_debug(void)
{
}

void
owrite_object(oobject_t object)
{
    oformat_t		 local;
    oformat_t		*format;

    format = &local;
    memset(format, 0, sizeof(oformat_t));
    local.read = 1;
    local.radix = 10;
    print_obj(object);
    dputc('\n');
    oflush(std_output);
}

static oword_t
write_object(oobject_t object, oformat_t *format)
{
    otype_t		 type;
    oword_t		 bytes;
    oentry_t		*entry;
    obool_t		 first;
    oint32_t		 indent;
    oword_t		 offset;
    union {
	oast_t		*ast;
	ocdd_t		*cdd;
	ocqq_t		 cqq;
	oentry_t	*entry;
	ohash_t		*hash;
	oword_t		*wptr;
	ofloat_t	*fptr;
	orat_t		 rat;
	ompz_t		 mpz;
	ompq_t		 mpq;
	ompr_t		 mpr;
	ompc_t		 mpc;
	omacro_t	*macro;
	oobject_t	 object;
	orecord_t	*record;
	osymbol_t	*symbol;
	otag_t		*tag;
	ovector_t	*vector;
    } data;

    if ((data.object = object) == null)
	bytes = dputs("null", 4);
    else {
	switch (type = otype(object)) {
	    case t_ast:
		indent = 0;
		bytes = print_ast_list(data.ast);
		break;
	    case t_basic:	case t_record:
		print_sym(data.record->name);
		break;
	    case t_word:
		bytes = print_int(*data.wptr);
		break;
	    case t_float:
		bytes = print_flt(*data.fptr);
		break;
	    case t_rat:
		bytes = print_rat(data.rat);
		break;
	    case t_mpz:
		bytes = print_mpz(data.mpz);
		break;
	    case t_mpq:
		bytes = print_mpq(data.mpq);
		break;
	    case t_mpr:
		bytes = print_mpr(data.mpr);
		break;
	    case t_cdd:
		bytes = print_cdd(data.cdd);
		break;
	    case t_cqq:
		bytes = print_cqq(data.cqq);
		break;
	    case t_mpc:
		bytes = print_mpc(data.mpc);
		break;
	    case t_string:
		bytes = print_str(data.vector);
		break;
	    case t_hash:
		first = true;
		bytes = dputs("hash_t {", 8);
		for (offset = 0; offset < data.hash->size; offset++) {
		    entry = data.hash->entries[offset];
		    for (; entry; entry = entry->next) {
			if (first) {
			    first = false;
			    dputc(' ');	++bytes;
			}
			else
			    bytes += dputs(", ", 2);
			bytes += print_obj(entry);
		    }
		}
		bytes += dputs(" }", 2);
		break;
	    case t_entry:
		bytes = dputs("entry_t { name: ", 16);
		bytes += print_obj(data.entry->name);
		bytes += dputs(", value: ", 9);
		bytes += print_obj(data.entry->value);
		dputc('}');		++bytes;
		break;
	    case t_symbol:
		bytes = print_sym(data.symbol);
		break;
	    case t_macro:
		bytes = print_sym(data.macro->name);
		break;
	    default:
		if (type & t_vector) {
		    if (data.vector->length) {
			type &= ~t_vector;
			bytes = dputs("{ ", 2);
			switch (type) {
			    case t_int8:
				bytes += print_vi8(data.vector);
				break;
			    case t_int16:
				bytes += print_vi16(data.vector);
				break;
			    case t_uint16:
				bytes += print_vu16(data.vector);
				break;
			    case t_int32:
				bytes += print_vi32(data.vector);
				break;
			    case t_uint32:
				bytes += print_vu32(data.vector);
				break;
			    case t_int64:
				bytes += print_vi64(data.vector);
				break;
			    case t_uint64:
				bytes += print_vu64(data.vector);
				break;
			    case t_float32:
				bytes += print_vf32(data.vector);
				break;
			    case t_float64:
				bytes += print_vf64(data.vector);
				break;
			    default:
				bytes += print_vobj(data.vector);
				break;
			}
			bytes += dputs(" }", 2);
		    }
		    else
			bytes = dputs("{ }", 3);
		}
		else if (type > t_mpc && type < type_vector->offset)
		    bytes = print_rec(type_vector->v.ptr[type], object);
		else
		    bytes = 0;
		break;
	}
    }

    return (bytes);
}

#define write_int_vector(id, type)					\
static oword_t								\
write_##id##_vector(type *pointer, oword_t length, oformat_t *format)	\
{									\
    oword_t		bytes;						\
									\
    for (bytes = 0; length > 1; length--, pointer++, bytes++) {		\
	bytes += print_int(*pointer);					\
	bytes += dputs(", ", 2);					\
    }									\
    bytes += print_int(*pointer);					\
									\
    return (bytes);							\
}
write_int_vector(i8, oint8_t)
write_int_vector(i16, oint16_t)
write_int_vector(u16, ouint16_t)
write_int_vector(i32, oint32_t)
#  if __WORDSIZE == 64
write_int_vector(u32, ouint32_t)
write_int_vector(i64, oint64_t)
#  endif
#undef write_int_vector

#if __WORDSIZE == 32
static oword_t
write_u32_vector(ouint32_t *pointer, oword_t length, oformat_t *format)
{
    GET_THREAD_SELF()
    oword_t		bytes;
    oword_t		word;

    for (bytes = 0; length > 1; length--, pointer++, bytes++) {
	word = *pointer;
	if (likely(word >= 0))
	    bytes += print_int(word);
	else {
	    mpz_set_ui(thr_zr, word);
	    bytes += print_mpz(thr_zr);
	}
	bytes += dputs(", ", 2);
    }
    word = *pointer;
    if (likely(word >= 0))
	bytes += print_int(word);
    else {
	mpz_set_ui(thr_zr, word);
	bytes += print_mpz(thr_zr);
    }

    return (bytes);
}

static oword_t
write_i64_vector(oint64_t *pointer, oword_t length, oformat_t *format)
{
    GET_THREAD_SELF()
    oword_t		bytes;
    oint64_t		integer;

    for (bytes = 0; length > 1; length--, pointer++, bytes++) {
	integer = *pointer;
	if ((oword_t)integer == integer)
	    bytes += print_int(integer);
	else {
	    ompz_set_sl(thr_zr, integer);
	    bytes += print_mpz(thr_zr);
	}
	bytes += dputs(", ", 2);
    }
    integer = *pointer;
    if ((oword_t)integer == integer)
	bytes += print_int(integer);
    else {
	ompz_set_sl(thr_zr, integer);
	bytes += print_mpz(thr_zr);
    }

    return (bytes);
}
#endif

static oword_t
write_u64_vector(ouint64_t *pointer, oword_t length, oformat_t *format)
{
    GET_THREAD_SELF()
    oword_t		bytes;
    oint64_t		integer;

    for (bytes = 0; length > 1; length--, pointer++, bytes++) {
	integer = *pointer;
	if (
#if __WORDSIZE == 32
	    (oword_t)integer == integer &&
#endif
	    integer >= 0
	    )
	    bytes += print_int(integer);
	else {
	    ompz_set_ul(thr_zr, integer);
	    bytes += print_mpz(thr_zr);
	}
	bytes += dputs(", ", 2);
    }
    integer = *pointer;
    if (
#if __WORDSIZE == 32
	(oword_t)integer == integer &&
#endif
	integer >= 0
	)
	bytes += print_int(integer);
    else {
	ompz_set_ul(thr_zr, integer);
	bytes += print_mpz(thr_zr);
    }

    return (bytes);
}

#define write_flt_vector(id, type)					\
static oword_t								\
write_##id##_vector(type *pointer, oword_t length, oformat_t *format)	\
{									\
    oword_t		bytes;						\
									\
    for (bytes = 0; length > 1; length--, pointer++, bytes++) {		\
	bytes += print_flt(*pointer);					\
	bytes += dputs(", ", 2);					\
    }									\
    bytes += print_flt(*pointer);					\
									\
    return (bytes);							\
}
write_flt_vector(f32, ofloat32_t)
write_flt_vector(f64, ofloat64_t)
#undef write_flt_vector

static oword_t
write_obj_vector(oobject_t *pointer, oword_t length, oformat_t *format)
{
    oword_t		bytes;

    for (bytes = 0; length > 1; length--, pointer++, bytes++) {
	bytes += print_obj(*pointer);
	bytes += dputs(", ", 2);
    }
    bytes += print_obj(*pointer);

    return (bytes);
}

static oword_t
write_record(orecord_t *record, oobject_t object, oformat_t *format)
{
    GET_THREAD_SELF()
#if __WORDSIZE == 32
    oint32_t		 i32;
#endif
    oint64_t		 i64;
    oint8_t		*base;
    oword_t		 bytes;
    obool_t		 first;
    oword_t		 offset;
    osymbol_t		*symbol;
    ovector_t		*vector;

    first = true;
    base = object;
    bytes = print_sym(record->name);
    bytes += dputs(" {", 2);
    vector = record->vector;
    for (offset = 0; offset < vector->offset; offset++) {
	symbol = vector->v.ptr[offset];
	if (!symbol->field)
	    continue;
	if (first) {
	    dputc(' ');
	    ++bytes;
	    first = false;
	}
	else
	    bytes += dputs(", ", 2);
	switch (otag_to_type(symbol->tag)) {
	    case t_int8:
		bytes += print_int(*(oint8_t *)(base + symbol->offset));
		break;
	    case t_uint8:
		bytes += print_int(*(ouint8_t *)(base + symbol->offset));
		break;
	    case t_int16:
		bytes += print_int(*(oint16_t *)(base + symbol->offset));
		break;
	    case t_uint16:
		bytes += print_int(*(ouint16_t *)(base + symbol->offset));
		break;
	    case t_int32:
		bytes += print_int(*(oint32_t *)(base + symbol->offset));
		break;
	    case t_uint32:
#if __WORDSIZE == 32
		i32 = *(oint32_t *)(base + symbol->offset);
		if (i32 >= 0)
		    bytes += print_int(i32);
		else {
		    mpz_set_ui(thr_zr, i32);
		    bytes += print_mpz(thr_zr);
		}
#else
		bytes += print_int(*(ouint32_t *)(base + symbol->offset));
#endif
		break;
	    case t_int64:
#if __WORDSIZE == 32
		i64 = *(oint64_t *)(base + symbol->offset);
		if ((oint32_t)i64 == i64)
		    bytes += print_int(i64);
		else {
		    ompz_set_sl(thr_zr, i64);
		    bytes += print_mpz(thr_zr);
		}
#else
		bytes += print_int(*(oint64_t *)(base + symbol->offset));
#endif
		break;
	    case t_uint64:
		i64 = *(oint64_t *)(base + symbol->offset);
		if (
#if __WORDSIZE == 32
		    (oword_t)i64 == i64 &&
#endif
		    i64 >= 0
		    )
		    bytes += print_int(i64);
		else {
		    ompz_set_ul(thr_zr, i64);
		    bytes += print_mpz(thr_zr);
		}
		break;
	    case t_float32:
		bytes += print_flt(*(ofloat32_t *)(base + symbol->offset));
		break;
	    case t_float64:
		bytes += print_flt(*(ofloat64_t *)(base + symbol->offset));
		break;
	    default:
		bytes += print_obj(*(oobject_t *)(base + symbol->offset));
		break;
	}
    }
    bytes += dputs(" }", 2);

    return (bytes);
}

static oword_t
write_ast(oast_t *ast, oint32_t indent, oformat_t *format)
{
    otag_t		*tag;
    oword_t		 bytes;
    ocase_t		*acase;
    osymbol_t		*symbol;
    ovector_t		*vector;

    /* do not crash if called from gdb with incomplete data */
    if (ast == null)
	return (0);
    bytes = 0;
    if (cfg_verbose > 1) {
	dputc('<');	++bytes;
	print_int(ast->offset);
	dputc('#');	++bytes;
    }
    switch (ast->token) {
	case tok_number:
	    bytes += print_obj(ast->l.value);
	    break;
	case tok_symbol:
	    symbol = ast->l.value;
	    if (symbol->field)
		bytes += dputs("this.", 5);
	    bytes += print_sym(symbol);
	    break;
	case tok_string:
	    if ((vector = ast->l.value))
		bytes += print_str(vector);
	    else
		bytes = dputs("null", 4);
	    break;
	case tok_class:
	    if (ast->l.ast->token == tok_type)
		bytes += print_ast(ast->l.ast);
	    else {
		bytes += dputs("class ", 6);
		bytes += print_ast_comma_list(ast->l.ast);
	    }
	    dputc(';');		++bytes;
	    break;
	case tok_type:
	    bytes += print_tag(ast->l.value, null, true);
	    break;
	case tok_defn:
	    bytes += print_tag(ast->l.value, null, false);
	    break;
	case tok_declexpr:
	    assert(ast->l.ast && ast->l.ast->token == tok_type);
	    bytes += print_tag(ast->l.ast->l.value, null, false);
	    dputc(' ');		++bytes;
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_decl:
	    assert(ast->l.ast && ast->l.ast->token == tok_type);
	    bytes += print_tag(ast->l.ast->l.value, null, false);
	    dputc(' ');		++bytes;
	    bytes += print_ast_comma_list(ast->r.ast);
	    dputc(';');		++bytes;
	    break;
	case tok_oparen:	dputc('(');		++bytes;	break;
	case tok_cparen:	dputc(')');		++bytes;	break;
	case tok_obrack:	dputc('[');		++bytes;	break;
	case tok_cbrack:	dputc(']');		++bytes;	break;
	case tok_obrace:	dputc('{');		++bytes;	break;
	case tok_cbrace:	dputc('}');		++bytes;	break;
	case tok_collon:	dputc(':');		++bytes;	break;
	case tok_semicollon:	dputc(';');		++bytes;	break;
	case tok_comma:		dputc(',');		++bytes;	break;
	case tok_ellipsis:	bytes = dputs("...", 3);		break;
	case tok_explicit:
	    bytes += print_tag(ast->l.ast->l.value, null, false);
	    dputc('.');		++bytes;
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_dot:
	    bytes += print_ast(ast->l.ast);
	    dputc('.');		++bytes;
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_set:
	    bytes += print_ast_binary("=", 1, ast);
	    break;
	case tok_andset:
	    bytes += print_ast_binary("&=", 2, ast);
	    break;
	case tok_orset:
	    bytes += print_ast_binary("|=", 2, ast);
	    break;
	case tok_xorset:
	    bytes += print_ast_binary("^=", 2, ast);
	    break;
	case tok_mul2set:
	    bytes += print_ast_binary("<<=", 3, ast);
	    break;
	case tok_div2set:
	    bytes += print_ast_binary(">>=", 3, ast);
	    break;
	case tok_shlset:
	    bytes += print_ast_binary("<<<=", 4, ast);
	    break;
	case tok_shrset:
	    bytes += print_ast_binary(">>>=", 4, ast);
	    break;
	case tok_addset:
	    bytes += print_ast_binary("+=", 2, ast);
	    break;
	case tok_subset:
	    bytes += print_ast_binary("-=", 2, ast);
	    break;
	case tok_mulset:
	    bytes += print_ast_binary("*=", 2, ast);
	    break;
	case tok_divset:
	    bytes += print_ast_binary("/=", 2, ast);
	    break;
	case tok_trunc2set:
	    bytes += print_ast_binary("\\=", 2, ast);
	    break;
	case tok_remset:
	    bytes += print_ast_binary("%=", 2, ast);
	    break;
	case tok_andand:
	    bytes += print_ast_binary("&&", 2, ast);
	    break;
	case tok_oror:
	    bytes += print_ast_binary("||", 2, ast);
	    break;
	case tok_lt:
	    bytes += print_ast_binary("<", 1, ast);
	    break;
	case tok_le:
	    bytes += print_ast_binary("<=", 2, ast);
	    break;
	case tok_eq:
	    bytes += print_ast_binary("==", 2, ast);
	    break;
	case tok_ge:
	    bytes += print_ast_binary(">=", 2, ast);
	    break;
	case tok_gt:
	    bytes += print_ast_binary(">", 1, ast);
	    break;
	case tok_ne:
	    bytes += print_ast_binary("!=", 2, ast);
	    break;
	case tok_and:
	    bytes += print_ast_binary("&", 1, ast);
	    break;
	case tok_or:
	    bytes += print_ast_binary("|", 1, ast);
	    break;
	case tok_xor:
	    bytes += print_ast_binary("^", 1, ast);
	    break;
	case tok_mul2:
	    bytes += print_ast_binary("<<", 2, ast);
	    break;
	case tok_div2:
	    bytes += print_ast_binary(">>", 2, ast);
	    break;
	case tok_shl:
	    bytes += print_ast_binary("<<<", 3, ast);
	    break;
	case tok_shr:
	    bytes += print_ast_binary(">>>", 3, ast);
	    break;
	case tok_add:
	    bytes += print_ast_binary("+", 1, ast);
	    break;
	case tok_sub:
	    bytes += print_ast_binary("-", 1, ast);
	    break;
	case tok_mul:
	    bytes += print_ast_binary("*", 1, ast);
	    break;
	case tok_div:
	    bytes += print_ast_binary("/", 1, ast);
	    break;
	case tok_trunc2:
	    bytes += print_ast_binary("\\", 1, ast);
	    break;
	case tok_rem:
	    bytes += print_ast_binary("%", 1, ast);
	    break;
	case tok_inc:
	    bytes += print_ast_unary("++", 2, ast);
	    break;
	case tok_dec:
	    bytes += print_ast_unary("--", 2, ast);
	    break;
	case tok_postinc:
	    bytes += print_ast(ast->l.ast);
	    bytes += dputs("++", 2);
	    break;
	case tok_postdec:
	    bytes += print_ast(ast->l.ast);
	    bytes += dputs("--", 2);
	    break;
	case tok_plus:
	    bytes += print_ast_unary("+", 1, ast);
	    break;
	case tok_neg:
	    bytes += print_ast_unary("-", 1, ast);
	    break;
	case tok_not:
	    bytes += print_ast_unary("!", 1, ast);
	    break;
	case tok_com:
	    bytes += print_ast_unary("~", 1, ast);
	    break;
	case tok_atan2:
	    bytes += print_ast_binary_function("atan2", 5, ast);
	    break;
	case tok_pow:
	    bytes += print_ast_binary_function("pow", 3, ast);
	    break;
	case tok_hypot:
	    bytes += print_ast_binary_function("hypot", 5, ast);
	    break;
	case tok_complex:
	    bytes += print_ast_binary_function("complex", 7, ast);
	    break;
	case tok_subtypeof:
	    bytes += print_ast_binary_function("subtypeof", 9, ast);
	    break;
	case tok_sizeof:
	    bytes += print_ast_unary_function("sizeof", 6, ast);
	    break;
	case tok_typeof:
	    bytes += print_ast_unary_function("typeof", 6, ast);
	    break;
	case tok_new:
	    bytes += dputs("new(", 4);
	    if (ast->l.ast->token == tok_type)
		bytes += print_tag(ast->l.ast->l.value, null, false);
	    else
		bytes += print_ast(ast->l.ast);
	    dputc(')');		++bytes;
	    break;
	case tok_integer_p:
	    bytes += print_ast_unary_function("integer_p", 9, ast);
	    break;
	case tok_rational_p:
	    bytes += print_ast_unary_function("rational_p", 10, ast);
	    break;
	case tok_float_p:
	    bytes += print_ast_unary_function("float_p", 7, ast);
	    break;
	case tok_real_p:
	    bytes += print_ast_unary_function("real_p", 6, ast);
	    break;
	case tok_complex_p:
	    bytes += print_ast_unary_function("complex_p", 9, ast);
	    break;
	case tok_number_p:
	    bytes += print_ast_unary_function("number_p", 8, ast);
	    break;
	case tok_finite_p:
	    bytes += print_ast_unary_function("finite_p", 8, ast);
	    break;
	case tok_inf_p:
	    bytes += print_ast_unary_function("inf_p", 6, ast);
	    break;
	case tok_nan_p:
	    bytes += print_ast_unary_function("nan_p", 6, ast);
	    break;
	case tok_num:
	    bytes += print_ast_unary_function("num", 3, ast);
	    break;
	case tok_den:
	    bytes += print_ast_unary_function("den", 3, ast);
	    break;
	case tok_real:
	    bytes += print_ast_unary_function("real", 4, ast);
	    break;
	case tok_imag:
	    bytes += print_ast_unary_function("imag", 4, ast);
	    break;
	case tok_signbit:
	    bytes += print_ast_unary_function("signbit", 7, ast);
	    break;
	case tok_abs:
	    bytes += print_ast_unary_function("abs", 3, ast);
	    break;
	case tok_goto:
	    bytes += dputs("goto ", 5);
	    bytes += print_obj(ast->l.ast->l.value);
	    dputc(';');		++bytes;
	    break;
	case tok_question:
	    bytes += print_ast(ast->t.ast);
	    bytes += dputs(" ? ", 3);
	    bytes += print_ast(ast->l.ast);
	    bytes += dputs(" : ", 3);
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_return:
	    if (ast->l.ast) {
		bytes += print_ast_unary("return ", 7, ast);
		dputc(';');	++bytes;
	    }
	    else
		bytes += dputs("return;", 7);
	    break;
	case tok_break:
	    bytes += dputs("break;", 6);
	    break;
	case tok_continue:
	    bytes += dputs("continue;", 9);
	    break;
	case tok_list:
	    bytes += print_ast_paren_comma_list(ast->l.ast);
	    break;
	case tok_vector:	case tok_vecdcl:
	case tok_vecnew:
	    bytes += print_ast(ast->l.ast);
	    dputc('[');		++bytes;
	    if (ast->r.ast)	bytes += print_ast(ast->r.ast);
	    dputc(']');		++bytes;
	    break;
	case tok_stat:
	    bytes += print_ast_newline();
	    bytes += print_ast_comma_list(ast->l.ast);
	    dputc(';');		++bytes;
	    break;
	case tok_call:
	    bytes += print_ast_call(ast->l.value);
	    bytes += print_ast_paren_comma_list(ast->r.ast);
	    break;
	case tok_proto:
	    bytes += print_ast_call(ast->l.value);
	    bytes += print_ast_paren_comma_list(ast->r.ast);
	    break;
	case tok_code:
	    dputc('{');		++bytes;
	    indent += 4;
	    if (ast->l.value)
		bytes += print_ast_list(ast->l.ast);
	    indent -= 4;
	    bytes += print_ast_newline();
	    dputc('}');		++bytes;
	    break;
	case tok_data:
	    dputc('{');		++bytes;
	    indent += 4;
	    bytes += print_ast_newline();
	    if (ast->l.value)
		bytes += print_ast_comma_list(ast->l.ast);
	    indent -= 4;
	    bytes += print_ast_newline();
	    dputc('}');		++bytes;
	    if (ast->r.value) {
		bytes += dputs(" /* ", 4);
		bytes += print_obj(ast->r.value);
		bytes += dputs(" */ ", 4);
	    }
	    break;
	case tok_if:
	    bytes += dputs("if ", 3);
	    bytes += print_ast_paren_comma_list(ast->t.ast);
	    if (ast->l.ast->token == tok_code) {
		dputc(' ');	++bytes;
	    }
	    else {
		indent += 4;
		if (ast->l.ast->token != tok_stat)
		    bytes += print_ast_newline();
	    }
	    bytes += print_ast(ast->l.ast);
	    if (ast->l.ast->token != tok_code)
		indent -= 4;
	    if (ast->r.ast) {
		bytes += print_ast_newline();
		bytes += dputs("else", 4);
		if (ast->r.ast->token == tok_code) {
		    dputc(' ');	++bytes;
		}
		else {
		    indent += 4;
		    if (ast->r.ast->token != tok_stat)
			bytes += print_ast_newline();
		}
		bytes += print_ast(ast->r.ast);
		if (ast->r.ast->token != tok_code)
		    indent -= 4;
	    }
	    break;
	case tok_while:
	    bytes += dputs("while ", 6);
	    bytes += print_ast_paren_comma_list(ast->t.ast);
	    if (ast->c.ast->token == tok_code) {
		dputc(' ');	++bytes;
	    }
	    else {
		indent += 4;
		if (ast->c.ast->token != tok_stat)
		    bytes += print_ast_newline();
	    }
	    bytes += print_ast(ast->c.ast);
	    if (ast->c.ast->token != tok_code)
		indent -= 4;
	    break;
	case tok_for:
	    bytes += dputs("for (", 5);
	    if (ast->l.ast)
		bytes += print_ast_comma_list(ast->l.ast);
	    if (ast->t.ast) {
		bytes += dputs("; ", 2);
		bytes += print_ast_comma_list(ast->t.ast);
	    }
	    else {
		dputc(';');	++bytes;
	    }
	    if (ast->r.ast) {
		bytes += dputs("; ", 2);
		bytes += print_ast_comma_list(ast->r.ast);
	    }
	    else {
		dputc(';');	++bytes;
	    }
	    dputc(')');		++bytes;
	    if (ast->c.ast->token == tok_code) {
		dputc(' ');	++bytes;
	    }
	    else {
		indent += 4;
		if (ast->c.ast->token != tok_stat)
		    bytes += print_ast_newline();
	    }
	    bytes += print_ast(ast->c.ast);
	    if (ast->c.ast->token != tok_code)
		indent -= 4;
	    break;
	case tok_do:
	    bytes += dputs("do", 2);
	    if (ast->c.ast->token == tok_code) {
		dputc(' ');	++bytes;
	    }
	    else {
		indent += 4;
		if (ast->c.ast->token != tok_stat)
		    bytes += print_ast_newline();
	    }
	    bytes += print_ast(ast->c.ast);
	    if (ast->c.ast->token != tok_code) {
		indent -= 4;
		bytes += print_ast_newline();
	    }
	    else {
		dputc(' ');	++bytes;
	    }
	    bytes += dputs("while ", 6);
	    bytes += print_ast_paren_comma_list(ast->t.ast);
	    dputc(';');		++bytes;
	    break;
	case tok_label:
	    bytes += print_obj(ast->l.value);
	    dputc(':');		++bytes;
	    break;
	case tok_switch:
	    bytes += dputs("switch ", 7);
	    bytes += print_ast_paren_comma_list(ast->t.ast);
	    dputc(' ');		++bytes;
	    bytes += print_ast(ast->c.ast);
	    break;
	case tok_case:
	    bytes += dputs("case ", 5);
	    acase = ast->l.value;
	    bytes += print_int(acase->lval);
	    if (acase->lval != acase->rval) {
		bytes += dputs(" ... ", 5);
		bytes += print_int(acase->rval);
	    }
	    dputc(':');		++bytes;
	    break;
	case tok_default:
	    bytes += dputs("default:", 8);
	    break;
	case tok_try:
	    bytes += dputs("try ", 4);
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_catch:
	    bytes += dputs("catch ", 6);
	    dputc('(');		++bytes;
	    symbol = ast->l.ast->r.ast->l.value;
	    bytes += print_tag(symbol->tag, symbol, false);
	    bytes += dputs(") ", 2);
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_finally:
	    bytes += dputs("finally ", 8);
	    bytes += print_ast(ast->l.ast);
	    break;
	case tok_throw:
	    if (ast->l.ast) {
		bytes += print_ast_unary("throw ", 6, ast);
		dputc(';');	++bytes;
	    }
	    else
		bytes += dputs("throw;", 6);
	    break;
	case tok_init:
	    bytes += print_ast(ast->l.ast);
	    bytes += dputs(": ", 2);
	    bytes += print_ast(ast->r.ast);
	    break;
	case tok_function:
	    /* Do not print twice if a class method
	      * defined outside class definition */
	    if (ast->r.ast->l.ast->token == tok_dot) {
		assert(ast->r.ast->l.ast->l.ast->token == tok_type);
		tag = ast->r.ast->l.ast->l.ast->l.value;
		if (current_record != tag->name)
		    break;
	    }
	    else if (ast->r.ast->l.ast->token == tok_ctor) {
		assert(ast->l.ast->token == tok_type);
		tag = ast->l.ast->l.value;
		if (current_record != tag->name)
		    break;
	    }
	    if (ast->l.ast->token == tok_type)
		bytes += print_tag(ast->l.ast->l.value, null, false);
	    else
		bytes += print_ast(ast->l.ast);
	    if (ast->r.ast->l.ast->token == tok_ctor)
		bytes += dputs("()", 2);
	    else {
		dputc(' ');	++bytes;
		if (ast->r.ast->l.ast->token == tok_dot)
		    bytes += print_ast(ast->r.ast->r.ast);
		else
		    bytes += print_ast(ast->r.ast);
	    }
	    dputc(' ');		++bytes;
	    bytes += print_ast(ast->c.ast);
	    break;
	default:
	    bytes += print_ptr(ast);
	    break;
    }
    if (cfg_verbose > 1) {
	dputc('>');	++bytes;
    }

    return (bytes);
}

static oword_t
write_ast_list(oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    for (bytes = 0; ast; ast = ast->next) {
	switch (ast->token) {
	    case tok_label:
	    case tok_case:
	    case tok_default:
		indent -= 4;
		bytes += print_ast_newline();
		indent += 4;
		break;
	    case tok_stat:
		break;
	    default:
		bytes += print_ast_newline();
		break;
	}
	bytes += print_ast(ast);
    }

    return (bytes);
}

static oword_t
write_ast_call(oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		 bytes;
    osymbol_t		*symbol;

    if (ast->token != tok_symbol) {
	assert(ast->token == tok_dot ||
	       ast->token == tok_explicit ||
	       ast->token == tok_ctor);
	bytes = print_ast(ast);
    }
    else {
	symbol = ast->l.value;
	bytes = print_sym(symbol);
    }

    return (bytes);
}

static oword_t
write_ast_unary(char *string, oword_t length,
		oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    bytes = dputs(string, length);
    bytes += print_ast(ast->l.ast);

    return (bytes);
}

static oword_t
write_ast_binary(char *string, oword_t length,
		 oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    bytes = print_ast(ast->l.ast);
    dputc(' ');		++bytes;
    bytes += dputs(string, length);
    dputc(' ');		++bytes;
    bytes += print_ast(ast->r.ast);

    return (bytes);
}

static oword_t
write_ast_unary_function(char *string, oword_t length,
			 oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    bytes = dputs(string, length);
    dputc('(');		++bytes;
    bytes += print_ast(ast->l.ast);
    dputc(')');		++bytes;

    return (bytes);
}

static oword_t
write_ast_binary_function(char *string, oword_t length,
			  oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    bytes = dputs(string, length);
    dputc('(');		++bytes;
    bytes += print_ast(ast->l.ast);
    bytes += dputs(", ", 2);
    bytes += print_ast(ast->r.ast);
    dputc(')');		++bytes;

    return (bytes);
}

static oword_t
write_ast_comma_list(oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    if (ast) {
	bytes = print_ast(ast);
	for (ast = ast->next; ast; ast = ast->next) {
	    bytes += dputs(", ", 2);
	    bytes += print_ast(ast);
	}
    }
    else
	bytes = 0;

    return (bytes);
}

static oword_t
write_ast_paren_comma_list(oast_t *ast, oint32_t indent, oformat_t *format)
{
    oword_t		bytes;

    dputc('(');		bytes = 1;
    if (ast)
	bytes += print_ast_comma_list(ast);
    dputc(')');		++bytes;

    return (bytes);
}

static oword_t
write_fields(orecord_t *record, oint32_t indent, oformat_t *format)
{
    orecord_t		*save;
    oword_t		 bytes;
    oword_t		 offset;
    osymbol_t		*symbol;
    ofunction_t		*function;

    save = current_record;
    current_record = record;
    bytes = dputs(" {", 2);
    indent += 4;
    for (offset = 0; offset < record->vector->offset; offset++) {
	symbol = record->vector->v.ptr[offset];
	if (symbol->tag && symbol->tag->type == tag_function) {
	    function = symbol->value;
	    if (function && function->ast &&
		function->name->record == record) {
		bytes += print_ast_newline();
		bytes += print_ast(function->ast);
	    }
	}
	else {
	    bytes += print_ast_newline();
	    bytes += print_tag(symbol->tag, symbol, false);
	    dputc(';');	++bytes;
	}
    }
    indent -= 4;
    bytes += print_ast_newline();
    dputc('}');		++bytes;
    current_record = save;

    return (bytes);
}

static oword_t
write_tag(otag_t *tag, osymbol_t *name, obool_t fields,
	  oint32_t indent, oformat_t *format)
{
    otag_t		*base;
    otag_t		*temp;
    oword_t		 bytes;
    oword_t		 length;
    orecord_t		*record;

    bytes = 0;
    if (fields && tag->type == tag_class)
	bytes = dputs("class ", 6);
    for (base = tag; base->base; base = base->base)
	assert(otag_p(base));
    if ((record = base->name))
	bytes += print_sym(record->name);
    else
	bytes += dputs("auto", 4);
    if (fields && tag->type == tag_class)
	bytes += print_fields(record);
    if (name) {
	dputc(' ');		++bytes;
	bytes += print_sym(name);
    }
    for (temp = tag; temp != base; temp = temp->base) {
	if (temp->type == tag_vector) {
	    dputc('[');		++bytes;
	    if ((length = *((ovector_t *)temp->name)->v.w))
		bytes += print_int(length);
	    dputc(']');		++bytes;
	}
    }

    return (bytes);
}

static oword_t
write_ast_newline(oint32_t indent)
{
    oword_t		bytes;

    dputc('\n');	bytes = 1;
    for (; indent >= 8; indent -= 8, ++bytes)
	dputc('\t');
    for (; indent >= 4; indent -= 4)
	bytes += dputs("    ", 4);

    return (bytes);
}
