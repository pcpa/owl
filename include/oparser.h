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

#ifndef _oparser_h
#define _oparser_h

#define oparse_error(ast, ...)		onote_error(&((ast)->note), __VA_ARGS__)
#define oparse_warn(ast, ...)		onote_warn(&((ast)->note), __VA_ARGS__)

/*
 * Types
 */
enum otoken {
    tok_eof		= -1,
    tok_none,
    tok_type,
    tok_symbol,
    tok_number,
    tok_string,
    tok_FILE,
    tok_LINE,
    tok_FUNCTION,
    tok_VA_ARGS,
    tok_DATE,
    tok_TIME,
    tok_COUNTER,
    tok_stringify,
    tok_concat,
    tok_oparen,
    tok_cparen,
    tok_obrack,
    tok_cbrack,
    tok_obrace,
    tok_cbrace,
    tok_semicollon,
    tok_collon,
    tok_comma,
    tok_dot,
    tok_ellipsis,
    tok_set,
    tok_andset,
    tok_orset,
    tok_xorset,
    tok_mul2set,
    tok_div2set,
    tok_shlset,
    tok_shrset,
    tok_addset,
    tok_subset,
    tok_mulset,
    tok_divset,
    tok_trunc2set,
    tok_remset,
    tok_andand,
    tok_oror,
    tok_ne,
    tok_lt,
    tok_le,
    tok_eq,
    tok_ge,
    tok_gt,
    tok_and,
    tok_or,
    tok_xor,
    tok_mul2,
    tok_div2,
    tok_shl,
    tok_shr,
    tok_add,
    tok_sub,
    tok_mul,
    tok_div,
    tok_trunc2,
    tok_rem,
    tok_atan2,
    tok_pow,
    tok_hypot,
    tok_complex,
    tok_subtypeof,
    tok_renew,
    tok_sizeof,
    tok_typeof,
    tok_new,
    tok_rankof,
    tok_not,
    tok_com,
    tok_plus,
    tok_neg,
    tok_signbit,
    tok_signum,
    tok_rational,
    tok_integer_p,
    tok_rational_p,
    tok_float_p,
    tok_real_p,
    tok_complex_p,
    tok_number_p,
    tok_finite_p,
    tok_inf_p,
    tok_nan_p,
    tok_num,
    tok_den,
    tok_real,
    tok_imag,
    tok_arg,
    tok_conj,
    tok_floor,
    tok_trunc,
    tok_round,
    tok_ceil,
    tok_abs,
    tok_sqrt,
    tok_cbrt,
    tok_sin,
    tok_cos,
    tok_tan,
    tok_asin,
    tok_acos,
    tok_atan,
    tok_sinh,
    tok_cosh,
    tok_tanh,
    tok_asinh,
    tok_acosh,
    tok_atanh,
    tok_proj,
    tok_exp,
    tok_log,
    tok_log2,
    tok_log10,
    tok_inc,
    tok_dec,
    tok_postinc,
    tok_postdec,
    tok_question,
    tok_this,
    tok_if,
    tok_else,
    tok_goto,
    tok_return,
    tok_switch,
    tok_case,
    tok_default,
    tok_break,
    tok_continue,
    tok_try,
    tok_catch,
    tok_throw,
    tok_finally,
    tok_for,
    tok_do,
    tok_while,
    tok_class,
    tok_label,
    tok_list,
    tok_vector,
    tok_vecdcl,
    tok_code,		/* '{' in code context */
    tok_data,		/* '{' in data context */
    tok_new_type_data,
    tok_function,
    tok_declexpr,
    tok_decl,
    tok_defn,
    tok_expr,
    tok_stat,
    tok_offset,
    tok_explicit,	/* explicit class method call */
    tok_call,
    tok_proto,
    tok_fieldref,
    tok_elemref,
    tok_ctor,
};

struct onote {
    ovector_t		*name;
    oint32_t		 lineno;
    oint32_t		 column;
};

typedef union oast_union {
    oast_t		*ast;
    oobject_t		 value;
} oast_union_t;

struct oast {
    oast_t		*next;
    otoken_t		 token;
    oint32_t		 offset;
    onote_t		 note;
    oast_union_t	 l;		/* (l)eft */
    oast_union_t	 r;		/* (r)ight */
    oast_union_t	 t;		/* (t)est */
    oast_union_t	 c;		/* (c)ode */
};

struct olabel {
    oast_t		*ast;
    jit_node_t		*jit;
};

struct ocase {
    oint32_t		 lval;		/* (l)eft (val)ue */
    oint32_t		 rval;		/* (r)ight (val)ue */
    oast_t		*ast;		/* back pointer */
    jit_node_t		*jit;		/* jit generic pointer */
};

/*
 * Prototypes
 */
extern void
init_parser(void);

extern void
finish_parser(void);

extern oobject_t
oparser(void);

extern void
onew_ast(oobject_t *pointer, otoken_t token,
	 ovector_t *name, oint32_t lineno, oint32_t column);

/*
 * Externs
 */
extern osymbol_t	*symbol_new;

#endif /* _oparser_h */
