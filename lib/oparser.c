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

#define top_ast()		ast_vector[parser_vector->offset - 1]
#define top_block()		block_vector->v.ptr[block_vector->offset - 1]

/*
 * Prototypes
 */
static otoken_t
statement_noeof(void);

static otoken_t
statement(void);

static void
label_define(oast_t *ast);

static oword_t
case_value(void);

static void
case_check(oast_t *ast);

static void
switch_case(oast_t *ast);

static void
switch_default(oast_t *ast);

static void
try_check(void);

static void
try_catch(oast_t *ast);

static void
try_finally(oast_t *ast);

static void
statement_paren_comma_list(void);

static oast_t *
get_block(otoken_t token, oast_t *ast);

/* Remember block to check for non allowed conditions of
 * leaving or entering exception handler code with goto,
 * break, continue and return. Must enter from catch/finally
 * normal flow, and leave with throw or normal flow. */
static void
get_try_block(oast_t *ast);

static osymbol_t *
declare(oast_t *type, oast_t *decl);

static otoken_t
declaration(void);

static ofunction_t *
prototype(otag_t *tag, oast_t *decl);

static void
enumeration(void);

static void
namespace(void);

static void
function(void);

static otoken_t
structure(void);

static otoken_t
definition(void);

static void
group_match(obool_t data, otoken_t token);

static otoken_t
group(obool_t data);

static otoken_t
precedence_noeof(void);

static otoken_t
precedence(void);

static otoken_t
expression_noeof(void);

static otoken_t
expression(void);

static otoken_t
binary_assign(void);

static otoken_t
binary_left(otoken_t token);

static void
binary_right(otoken_t token);

static otoken_t
unary_noeof(void);

static otoken_t
unary(void);

static otoken_t
unary_string(void);

static otoken_t
unary_loop(otoken_t token);

static otoken_t
unary_value(otoken_t token);

static otoken_t
unary_ctor(void);

static otoken_t
unary_decl(void);

static otoken_t
unary_list(void);

static otoken_t
unary_vector(void);

static otoken_t
unary_field(void);

static otoken_t
unary_namespace(void);

static otoken_t
unary_unary(otoken_t token);

static otoken_t
unary_binary(otoken_t token);

static otoken_t
primary_noeof(void);

static otoken_t
primary(void);

static otoken_t
lookahead(void);

static otoken_t
lookahead_noeof(void);

static otoken_t
primary(void);

static otoken_t
tokenize(void);

static void
consume(void);

static void
discard(void);

static void
push_ast(oast_t *ast);

static oast_t *
pop_ast(void);

static void
push_block(oast_t *ast);

static void
pop_block(void);

/*
 * Initialization
 */
osymbol_t		*symbol_new;
osymbol_t		*symbol_this;
osymbol_t		*symbol_token_vector[tok_ctor];

static struct {
    char	*name;
    otoken_t	 token;
} keywords[] = {
    /* macros */
    { "__FILE__",	tok_FILE	},
    { "__LINE__",	tok_LINE	},
    { "__FUNCTION__",	tok_FUNCTION	},
    { "__VA_ARGS__",	tok_VA_ARGS	},
    { "__DATE__",	tok_DATE	},
    { "__TIME__",	tok_TIME	},
    { "__COUNTER__",	tok_COUNTER	},
    { "#",		tok_stringify	},
    { "##",		tok_concat	},
    { "(",		tok_oparen	},
    { ")",		tok_cparen	},
    { "[",		tok_obrack	},
    { "]",		tok_cbrack	},
    { "{",		tok_obrace	},
    { "}",		tok_cbrace	},
    { ";",		tok_semicollon	},
    { ":",		tok_collon	},
    { ",",		tok_comma	},
    { ".",		tok_dot		},
    { "...",		tok_ellipsis	},
    { "=",		tok_set		},
    { "&=",		tok_andset	},
    { "|=",		tok_orset	},
    { "^=",		tok_xorset	},
    { "<<=",		tok_mul2set	},
    { ">>=",		tok_div2set	},
    { "<<<=",		tok_shlset	},
    { ">>>=",		tok_shrset	},
    { "+=",		tok_addset	},
    { "-=",		tok_subset	},
    { "*=",		tok_mulset	},
    { "/=",		tok_divset	},
    { "\\=",		tok_trunc2set	},
    { "%=",		tok_remset	},
    { "&&",		tok_andand	},
    { "||",		tok_oror	},
    { "!=",		tok_ne		},
    { "<",		tok_lt		},
    { "<=",		tok_le		},
    { "==",		tok_eq		},
    { ">=",		tok_ge		},
    { ">",		tok_gt		},
    { "&",		tok_and		},
    { "|",		tok_or		},
    { "^",		tok_xor		},
    { "<<",		tok_mul2	},
    { ">>",		tok_div2	},
    { "<<<",		tok_shl		},
    { "<<<",		tok_shr		},
    { "+",		tok_add		},
    { "-",		tok_sub		},
    { "*",		tok_mul		},
    { "/",		tok_div		},
    { "\\",		tok_trunc2	},
    { "%",		tok_rem		},
    { "!",		tok_not		},
    { "~",		tok_com		},
    { "++",		tok_inc		},
    { "--",		tok_dec		},
    { "?",		tok_question	},
    { "atan2", 		tok_atan2	},
    { "pow", 		tok_pow		},
    { "hypot",		tok_hypot	},
    { "complex",	tok_complex	},
    { "subtypeof",	tok_subtypeof	},
    { "renew",		tok_renew	},
    { "sizeof",		tok_sizeof	},
    { "typeof",		tok_typeof	},
    { "new",		tok_new		},
    { "thread",		tok_thread	},
    { "signbit",	tok_signbit	},
    { "signum",	 	tok_signum	},
    { "rational",	tok_rational	},
    { "integer_p",	tok_integer_p	},
    { "rational_p",	tok_rational_p	},
    { "float_p",	tok_float_p	},
    { "real_p",	 	tok_real_p	},
    { "complex_p",	tok_complex_p	},
    { "number_p",	tok_number_p	},
    { "finite_p",	tok_finite_p	},
    { "inf_p",	 	tok_inf_p	},
    { "nan_p",	 	tok_nan_p	},
    { "num", 		tok_num		},
    { "den", 		tok_den		},
    { "real", 		tok_real	},
    { "imag", 		tok_imag	},
    { "arg", 		tok_arg		},
    { "conj", 		tok_conj	},
    { "floor", 		tok_floor	},
    { "trunc", 		tok_trunc	},
    { "round", 		tok_round	},
    { "ceil", 		tok_ceil	},
    { "abs", 		tok_abs		},
    { "sqrt", 		tok_sqrt	},
    { "cbrt",		tok_cbrt	},
    { "sin",		tok_sin		},
    { "cos",		tok_cos		},
    { "tan",		tok_tan		},
    { "asin",		tok_asin	},
    { "acos",		tok_acos	},
    { "atan",		tok_atan	},
    { "sinh",		tok_sinh	},
    { "cosh",		tok_cosh	},
    { "tanh",		tok_tanh	},
    { "asinh",		tok_asinh	},
    { "acosh",		tok_acosh	},
    { "atanh",		tok_atanh	},
    { "proj",		tok_proj	},
    { "exp",		tok_exp		},
    { "log",		tok_log		},
    { "log2",		tok_log2	},
    { "log10",		tok_log10	},
    { "this",		tok_this	},
    { "if",		tok_if		},
    { "else",		tok_else	},
    { "goto",		tok_goto	},
    { "return",		tok_return	},
    { "switch",		tok_switch	},
    { "case",		tok_case	},
    { "default",	tok_default	},
    { "break",		tok_break	},
    { "for", 		tok_for		},
    { "do",		tok_do		},
    { "while",		tok_while	},
    { "continue",	tok_continue	},
    { "try", 		tok_try		},
    { "catch",		tok_catch	},
    { "throw",		tok_throw	},
    { "finally",	tok_finally	},
    { "class",		tok_class	},
    { "namespace",	tok_namespace	},
    { "enum",		tok_enum	},
};
static oast_t		 *root_ast;
static oast_t		 *head_ast;
static oast_t		**ast_vector;
static ovector_t	 *block_vector;
static ovector_t	 *parser_vector;
static ovector_t	 *except_vector;
static ovector_t	 *root_except_vector;

/*
 * Implemtantion
 */
void
init_parser(void)
{
    GET_THREAD_SELF()
    oint32_t		 offset;
    osymbol_t		*symbol;

    /* Declare language keywords / operators */
    for (offset = 0; offset < osize(keywords); offset++) {
	symbol = onew_identifier(oget_string((ouint8_t *)keywords[offset].name,
					     strlen(keywords[offset].name)));
	onew_word(&symbol->value, keywords[offset].token);
	symbol->keyword = 1;
	symbol_token_vector[keywords[offset].token] = symbol;
    }

    symbol = onew_identifier(oget_string((ouint8_t *)"null", 4));
    symbol->special = true;

    symbol = onew_identifier(oget_string((ouint8_t *)"true", 4));
    symbol->special = true;
    onew_word(&symbol->value, 1);

    symbol = onew_identifier(oget_string((ouint8_t *)"false", 5));
    symbol->special = true;
    onew_word(&symbol->value, 0);

    symbol = onew_identifier(oget_string((ouint8_t *)"nan", 3));
    symbol->special = true;
    onew_float(&symbol->value, NAN);

    symbol = onew_identifier(oget_string((ouint8_t *)"inf", 3));
    symbol->special = true;
    onew_float(&symbol->value, INFINITY);

    symbol = onew_identifier(oget_string((ouint8_t *)"I", 1));
    symbol->special = true;
    mpq_set_ui(thr_qr, 0, 1);
    mpq_set_ui(thr_qi, 1, 1);
    onew_cqq(&symbol->value, thr_qq);

    symbol_new = oget_identifier(oget_string((ouint8_t *)"new", 3));

    symbol_this = oget_identifier(oget_string((ouint8_t *)"this", 4));
    symbol_this->offset = THIS_OFFSET;

    oadd_root((oobject_t *)&root_ast);
    oadd_root((oobject_t *)&head_ast);
    oadd_root((oobject_t *)&block_vector);
    onew_vector((oobject_t *)&block_vector, t_void, 16);
    oadd_root((oobject_t *)&parser_vector);
    onew_vector((oobject_t *)&parser_vector, t_void, 16);
    ast_vector = parser_vector->v.obj;
    oadd_root((oobject_t *)&except_vector);
    onew_vector((oobject_t *)&except_vector, t_int32, 4);
    memset(except_vector->v.i32, -1, 16);
    oadd_root((oobject_t *)&root_except_vector);
    onew_vector((oobject_t *)&root_except_vector, t_int32, 4);
    memset(root_except_vector->v.i32, -1, 16);
}

void
finish_parser(void)
{
    orem_root((oobject_t *)&root_ast);
    orem_root((oobject_t *)&head_ast);
    orem_root((oobject_t *)&block_vector);
    orem_root((oobject_t *)&parser_vector);
    orem_root((oobject_t *)&except_vector);
    orem_root((oobject_t *)&root_except_vector);
}

oobject_t
oparser(void)
{
    oast_t		*ast;
    oast_t		*tail;

    for (;;) {
	if (statement() == tok_eof) {
	    assert(parser_vector->offset == 0);
	    return (root_ast);
	}
	ast = pop_ast();
	if (root_ast == null)
	    root_ast = tail = ast;
	else {
	    tail->next = ast;
	    tail = ast;
	}
    }

    return (null);
}

void
onew_ast(oobject_t *pointer, otoken_t token,
	 ovector_t *name, oint32_t lineno, oint32_t column)
{
    oast_t		*ast;

    ast = oget_ast(pointer);
    ast->token = token;
    ast->note.name = name;
    ast->note.lineno = lineno;
    ast->note.column = column;
}

static otoken_t
statement_noeof(void)
{
    otoken_t		token = statement();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
statement(void)
{
    oast_t		*ast;
    otoken_t		 token;
    ovector_t		*vector;
    oobject_t		*pointer;

    token = lookahead();
    switch (token) {
	case tok_eof:
	    return (token);
	case tok_obrace:
	    /* brace is code, not data */
	    primary();
	    break;
	default:
	    token = expression();
	    break;
    }
    ast = top_ast();
    switch (token) {
	case tok_label:
	    label_define(ast);
	    get_try_block(ast);
	    break;
	case tok_decl:		case tok_ctor:
	    token = declaration();
	    break;
	case tok_symbol:
	    vector = ((osymbol_t *)ast->l.value)->name;
	    if (oget_bound_symbol(vector) == null)
		oparse_error(ast, "undefined symbol '%p'", vector);
	case tok_number:
	    switch (lookahead()) {
		case tok_comma:
		    oparse_warn(ast, "expression has no effect");
		    discard();
		    consume();
		    token = statement();
		    break;
		case tok_semicollon:
		    oparse_warn(ast, "statement has no effect");
		    consume();
		    odel_object(&ast->l.value);
		    ast->token = token = tok_stat;
		    break;
		default:
		    oparse_error(ast, "expecting ',' or ';' %A", ast);
	    }
	    break;
	case tok_class:
	    token = definition();
	    break;
	case tok_enum:
	    enumeration();
	    token = tok_code;
	    break;
	case tok_namespace:
	    namespace();
	    token = tok_code;
	    break;
	case tok_list:			case tok_expr:
	    token = tok_stat;
	    gc_ref(pointer);
	    onew_ast(pointer, tok_stat, top_ast()->note.name,
		     top_ast()->note.lineno, top_ast()->note.column);
	    gc_pop(ast);
	    ast->l.ast = top_ast();
	    top_ast() = ast;
	    ast = ast->l.ast;
	    switch (lookahead()) {
		case tok_comma:
		    consume();
		    group_match(false, tok_semicollon);
		    ast->next = pop_ast();
		case tok_semicollon:
		    consume();
		    break;
		default:
		    oparse_error(ast, "expecting ',' or ';' %A", ast);
	    }
	    break;
	case tok_obrace:
	    token = ast->token = tok_code;
	    ast = null;
	    while (lookahead() != tok_cbrace) {
		statement_noeof();
		if (ast == null) {
		    ast = pop_ast();
		    top_ast()->l.value = ast;
		    ast = top_ast()->l.value;
		}
		else {
		    ast->next = pop_ast();
		    ast = ast->next;
		}
	    }
	    if (ast == null)
		top_ast()->l.value = null;
	    consume();
	    break;
	case tok_semicollon:
	    ast->token = token = tok_stat;
	    break;
	case tok_if:
	    statement_paren_comma_list();
	    ast->t.ast = pop_ast();
	    statement_noeof();
	    ast->l.ast = pop_ast();
	    if (lookahead() == tok_else) {
		consume();
		statement_noeof();
		ast->r.ast = pop_ast();
	    }
	    break;
	case tok_while:
	    statement_paren_comma_list();
	    ast->t.ast = pop_ast();
	    push_block(ast);
	    statement_noeof();
	    pop_block();
	    ast->c.ast = pop_ast();
	    break;
	case tok_for:
	    if (primary_noeof() != tok_oparen)
		oparse_error(top_ast(), "expecting '(' %A", top_ast());
	    discard();
	    group_match(false, tok_semicollon);
	    consume();
	    ast->l.ast = pop_ast();		/* init */
	    group_match(false, tok_semicollon);
	    consume();
	    ast->t.ast = pop_ast();
	    group_match(false, tok_cparen);
	    consume();
	    ast->r.ast = pop_ast();		/* increment */
	    push_block(ast);
	    statement_noeof();
	    pop_block();
	    ast->c.ast = pop_ast();
	    break;
	case tok_do:
	    push_block(ast);
	    statement_noeof();
	    pop_block();
	    ast->c.ast = pop_ast();
	    if (primary_noeof() != tok_while)
		oparse_error(top_ast(), "expecting 'while' %A", top_ast());
	    discard();
	    statement_paren_comma_list();
	    ast->t.ast = pop_ast();
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    break;
	case tok_switch:
	    statement_paren_comma_list();
	    ast->t.ast = pop_ast();
	    if (lookahead_noeof() != tok_obrace)
		oparse_error(head_ast, "expecting '{' %A", head_ast);
	    onew_vector(&ast->l.value, t_void, 8);
	    vector = ast->l.value;
	    vector->offset = 1;
	    push_block(ast);
	    statement_noeof();
	    pop_block();
	    ast->c.ast = pop_ast();
	    break;
	case tok_case:
	    switch_case(ast);
	    break;
	case tok_default:
	    if (primary_noeof() != tok_collon)
		oparse_error(top_ast(), "expecting ':' %A", top_ast());
	    discard();
	    switch_default(ast);
	    break;
	case tok_break:
	    ast->c.ast = get_block(tok_break, ast);
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    get_try_block(ast);
	    break;
	case tok_continue:
	    ast->c.ast = get_block(tok_continue, ast);
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    get_try_block(ast);
	    break;
	case tok_goto:
	    if (unary_noeof() != tok_symbol)
		oparse_error(top_ast(), "expecting label %A", top_ast());
	    ast->l.ast = pop_ast();
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    get_try_block(ast);
	    break;
	case tok_return:
	    if (lookahead() != tok_semicollon) {
		expression_noeof();
		ast->l.ast = pop_ast();
	    }
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    get_try_block(ast);
	    break;
	case tok_try:
	    vector = otype(current_record) == t_namespace ?
		root_except_vector : except_vector;
	    if (vector->offset >= vector->length) {
		orenew_vector(vector, vector->length + 4);
		memset(vector->v.i32 + vector->offset, -1, 16);
	    }
	    if (vector->v.i32[vector->offset] != -1)
		ast->offset = vector->v.i32[vector->offset++];
	    else {
		ast->offset = onew_exception(current_record);
		vector->v.i32[vector->offset++] = ast->offset;
	    }
	    if (lookahead_noeof() != tok_obrace)
		oparse_error(head_ast, "expecting '{' %A", head_ast);
	    push_block(ast);
	    statement_noeof();
	    onew_vector(&ast->l.value, t_void, 8);
	    ast->r.ast = pop_ast();
	    try_check();
	    break;
	case tok_catch:
	    try_catch(ast);
	    try_check();
	    break;
	case tok_finally:
	    try_finally(ast);
	    try_check();
	    break;
	case tok_throw:
	    if (lookahead() != tok_semicollon) {
		expression_noeof();
		ast->l.ast = pop_ast();
	    }
	    /* FIXME else must be inside a catch,
	     * and (re)throw the catch argument */
	    if (primary_noeof() != tok_semicollon)
		oparse_error(top_ast(), "expecting ';' %A", top_ast());
	    discard();
	    break;
	default:
	    oparse_error(ast, "expecting statement %A", ast);
    }

    return (token);
}

static void
label_define(oast_t *ast)
{
    oast_t		*block;
    oentry_t		*entry;
    olabel_t		*label;
    osymbol_t		*symbol;
    oobject_t		*pointer;
    ofunction_t		*function;

    if ((block = get_block(tok_label, ast)))
	function = block->t.value;
    else
	function = root_record->function;

    symbol = ast->l.value;
    if ((entry = oget_hash(function->labels, symbol->name))) {
	label = entry->value;
	block = label->ast;
	oparse_error(ast, "duplicate label '%p' also at %p:%d:%d",
		     ast->l.value, block->note.name,
		     block->note.lineno, block->note.column);
    }

    gc_ref(pointer);
    onew_entry(pointer, symbol->name, null);
    entry = *pointer;
    oput_hash(function->labels, entry);
    gc_dec();
    onew(&entry->value, label);
    label = entry->value;
    label->ast = ast;
}

static oword_t
case_value(void)
{
    oast_t		*ast;
    oword_t		 value;

    if (unary() != tok_number)
	oeval_ast(top_ast());
    ast = top_ast();
    if (ast->token != tok_number || otype(ast->l.value) != t_word)
	oparse_error(ast, "case value not a 32 bit integer %A", ast);
    value = *(oword_t *)ast->l.value;
#if __WORDSIZE == 64
    if (value < -2147483648 || value >  2147483647)
	oparse_error(ast, "case value not a 32 bit integer %A", ast);
#endif
    discard();

    return (value);
}

static void
case_check(oast_t *ast)
{
    oast_t		*block;
    ocase_t		*lcase;
    ocase_t		*rcase;
    oword_t		 offset;
    ovector_t		*vector;

    lcase = ast->l.value;
    block = ast->r.ast;
    vector = block->l.value;
    if (lcase->lval == lcase->rval) {
	for (offset = 1; offset < vector->offset; offset++) {
	    rcase = vector->v.ptr[offset];
	    if (lcase != rcase &&
		lcase->lval <= rcase->rval && lcase->lval >= rcase->lval)
		oparse_error(ast, "duplicate case %d also at %p:%d:%d",
			     lcase->lval, rcase->ast->note.name,
			     rcase->ast->note.lineno, rcase->ast->note.column);
	}
    }
    else {
	for (offset = 1; offset < vector->offset; offset++) {
	    rcase = vector->v.ptr[offset];
	    if (lcase != rcase &&
		((lcase->lval <= rcase->rval && lcase->lval >= rcase->lval) ||
		 (lcase->rval <= rcase->rval && lcase->rval >= rcase->lval)))
		oparse_error(ast, "case range overlaps at %p:%d:%d",
			     rcase->ast->note.name,
			     rcase->ast->note.lineno, rcase->ast->note.column);
	}
    }
}

static void
switch_case(oast_t *ast)
{
    oast_t		*block;
    ocase_t		*acase;
    oword_t		 value;
    ovector_t		*vector;

    block = get_block(tok_case, ast);
    value = case_value();
    vector = block->l.value;
    if (vector->offset >= vector->length)
	orenew_vector(vector, vector->length + 8);
    onew_object(vector->v.ptr + vector->offset, t_case, sizeof(ocase_t));
    acase = vector->v.ptr[vector->offset++];
    ast->l.value = acase;
    acase->lval = acase->rval = value;
    acase->ast = ast;
    ast->r.ast = block;
    switch (primary()) {
	case tok_ellipsis:
	    discard();
	    value = case_value();
	    if (acase->lval > value)
		oparse_error(ast, "left value larger than right in case range");
	    acase->lval = value;
	    if (primary_noeof() != tok_collon)
		oparse_error(top_ast(), "expecting ':' %A", top_ast());
	    discard();
	    break;
	case tok_collon:
	    discard();
	    break;
	default:
	    oparse_error(top_ast(), "expecting ':' %A", top_ast());
    }
    case_check(ast);
}

static void
switch_default(oast_t *ast)
{
    oast_t		*block;
    ocase_t		*acase;
    ovector_t		*vector;

    block = get_block(tok_default, ast);
    vector = block->l.value;
    if (vector->v.ptr[0]) {
	acase = vector->v.ptr[0];
	oparse_error(ast, "duplicate default also at %p:%d:%d",
		     acase->ast->note.name,
		     acase->ast->note.lineno, acase->ast->note.column);
    }
    onew_object(vector->v.ptr, t_case, sizeof(ocase_t));
    acase = vector->v.ptr[0];
    ast->l.value = acase;
    acase->ast = ast;
    ast->r.ast = block;
}

static void
try_check(void)
{
    switch (lookahead()) {
	case tok_catch:		case tok_finally:
	    break;
	default:
	    pop_block();
	    if (otype(current_record) == t_namespace)
		--root_except_vector->offset;
	    else
		--except_vector->offset;
	    break;
    }
}

static void
try_catch(oast_t *ast)
{
    otag_t		*tag;
    oast_t		*type;
    oast_t		*name;
    oast_t		*decl;
    ocase_t		*acase;
    oast_t		*block;
    oword_t		 value;
    oword_t		 offset;
    osymbol_t		*symbol;
    ovector_t		*vector;

    block = top_block();
    if (block->token != tok_try)
	oparse_error(ast, "'catch' not following 'try'");
    vector = block->l.value;
    statement_paren_comma_list();
    ast->l.ast = decl = pop_ast();
    if (decl == null || (type = decl->l.ast) == null || type->token != tok_type)
	oparse_error(ast, "expecting declaration");
    if (decl->next)
	oparse_error(decl->next, "expecting ')' %A", decl->next);
    tag = otag_ast(type->l.value, &decl->r.ast);
    name = decl->r.ast;
    if (name->token != tok_symbol)
	oparse_error(name, "expecting symbol %A", name);
    switch (value = otag_to_type(tag)) {
	case t_void:
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	case t_int64:		case t_uint64:
	case t_float32:		case t_float64:
	    /* For consistency when throw'ing an immediate value */
	    oparse_error(type, "only auto, class and vector types allowed");
	default:
	    break;
    }
    if (value == t_undef)
	value = t_void;
    offset = value == t_void ? 0 : vector->offset - 1;
    for (; offset >= 0; offset--) {
	if ((acase = vector->v.ptr[offset]) && acase->lval == value)
	    oparse_error(ast, "duplicate catch type, also at %p:%d:%d",
			 acase->ast->note.name,
			 acase->ast->note.lineno, acase->ast->note.column);
    }
    symbol = name->l.value;
    if (oget_symbol(current_record, symbol->name))
	oparse_error(decl, "symbol '%p' redefined", symbol);
    symbol = onew_symbol(current_record, symbol->name, tag);
    symbol->except = true;

    if (!vector->offset)
	vector->offset = 1;

    offset = value == t_void ? 0 : vector->offset;
    if (offset >= vector->length)
	orenew_vector(vector, vector->length + 8);
    onew_object(vector->v.ptr + offset, t_case, sizeof(ocase_t));
    acase = vector->v.ptr[offset];
    if (value != t_void)
	++vector->offset;
    ast->t.value = acase;
    acase->lval = value;
    acase->ast = ast;
    ast->c.ast = block;
    if (lookahead_noeof() != tok_obrace)
	oparse_error(head_ast, "expecting '{' %A", head_ast);
    push_block(ast);
    statement_noeof();
    pop_block();
    ast->r.ast = pop_ast();
}

static void
try_finally(oast_t *ast)
{
    oast_t		*block;

    block = top_block();
    if (block->token != tok_try)
	oparse_error(ast, "'finally' not following 'try'");
    if (block->t.ast)
	oparse_error(ast, "duplicated 'finally'");
    if (lookahead_noeof() != tok_obrace)
	oparse_error(head_ast, "expecting '{' %A", head_ast);
    push_block(ast);
    statement_noeof();
    pop_block();
    ast->l.ast = pop_ast();
    block->t.ast = ast;
}

static void
statement_paren_comma_list(void)
{
    if (primary_noeof() != tok_oparen)
	oparse_error(top_ast(), "expecting '(' %A", top_ast());
    discard();
    group_match(false, tok_cparen);
    consume();
    if (top_ast() == null)
	oread_error("expecting comma separated expression list");
}

static oast_t *
get_block(otoken_t token, oast_t *ast)
{
    oast_t		*block;
    oword_t		 offset;

    for (offset = block_vector->offset - 1; offset >= 0; offset--) {
	block = block_vector->v.ptr[offset];
	switch (token) {
	    case tok_case:		case tok_default:
		if (block->token == tok_switch)
		    return (block);
		break;
	    case tok_break:
		switch (block->token) {
		    case tok_for:	case tok_while:
		    case tok_switch:	case tok_do:
			return (block);
		    default:
			break;
		}
		break;
	    case tok_continue:
		switch (block->token) {
		    case tok_for:	case tok_while:
		    case tok_do:
			return (block);
		    default:
			break;
		}
		break;
	    default:
		if (block->token == tok_function)
		    return (block);
		break;
	}
    }

    switch (token) {
	case tok_case:
	    oparse_error(ast, "'case' not in 'switch'");
	case tok_default:
	    oparse_error(ast, "'default' not in 'switch'");
	case tok_break:
	    oparse_error(ast,
			 "'break' not in 'do', 'for', 'switch', or 'while'");
	case tok_continue:
	    oparse_error(ast, "'continue' not in 'do', 'for', or 'while'");
	default:
	    /* toplevel function */
	    return (null);
    }
}

/* Get try/catch/finally partial information of the current ast, so that
 * once all input is read, can check if attempting to leave or enter an
 * exception related block, what usually cannot be allowed. */
static void
get_try_block(oast_t *ast)
{
    oast_t		*block;
    oword_t		 offset;

    assert(ast->t.ast == null);
    for (offset = block_vector->offset - 1; offset >= 0; offset--) {
	block = block_vector->v.ptr[offset];
	switch (block->token) {
	    case tok_try:
	    case tok_catch:		case tok_finally:
		ast->t.ast = block;
		return;
	    case tok_switch:
		if (ast->token == tok_break)
		    return;
		break;
	    case tok_do:		case tok_while:
	    case tok_for:
		if (ast->token == tok_break || ast->token == tok_continue)
		    return;
		break;
	    default:
		break;
	}
    }
}

static osymbol_t *
declare(oast_t *type, oast_t *decl)
{
    otag_t		*tag;
    ovector_t		*name;
    oast_t		*vexp;
    osymbol_t		*symbol;

    if (decl == null)
	oparse_error(type, "expecting declaration %A", type);
    vexp = null;
    while (decl->token == tok_set) {
	vexp = decl->r.ast;
	decl = decl->l.ast;
    }
    tag = otag_ast(type->l.value, &decl);
    if (decl->token == tok_call) {
	(void)prototype(tag, decl);
	return (null);
    }
    if (decl->token != tok_symbol)
	oparse_error(decl, "expecting symbol %A", decl);
    symbol = decl->l.value;
    name = symbol->name;
    if (vexp && (tag->type == tag_vector || tag->type == tag_class))
	tag = otag_ast_data(tag, vexp);

    if (oget_symbol(current_record, name))
	oparse_error(decl, "symbol '%p' redefined", name);

    symbol = onew_symbol(current_record, name, tag);

    assert(symbol);

    return (symbol);
}

static otoken_t
declaration(void)
{
    oast_t		*type;
    oast_t		*decl;

    type = top_ast();
    assert(type->token == tok_declexpr);
    type->token = tok_decl;
    decl = type->r.value;
    type = type->l.value;
    assert(type->token == tok_type);
    switch (lookahead()) {
	case tok_comma:
	    consume();
	    group_match(false, tok_semicollon);
	    decl->next = pop_ast();
	case tok_semicollon:
	    consume();
	    break;
	case tok_obrace:
	    function();
	    return (tok_function);
	default:
	    oparse_error(head_ast, "expecting ',', ';', or '{' %A", head_ast);
    }

    for (; decl; decl = decl->next)
	(void)declare(type, decl);

    return (tok_decl);
}

static ofunction_t *
prototype(otag_t *base, oast_t *proto)
{
    oast_t		*ast;
    otag_t		*dot;
    otag_t		*tag;
    otype_t		 type;
    ovector_t		*name;
    oword_t		 length;
    orecord_t		*record;
    osymbol_t		*symbol;
    ofunction_t		*function;

    if (proto->token == tok_vector) {
	ast = proto->r.ast;
	if (ast == null)
	    length = 0;
	else if (ast->token != tok_number || otype(ast->l.value) != t_word) {
	    oparse_error(ast, "not an integer");
	    length = *(oword_t *)ast->l.value;
	    if (length < 0)
		oparse_error(ast, "negative length");
	}
	base = otag_vector(base, length);
	omove_ast_up_full(proto, proto->l.ast);
    }
    else if (proto->token != tok_call)
	oparse_error(proto, "expecting prototype %A", proto);
    proto->token = tok_proto;
    tag = otag_proto(base, proto);
    ast = proto->l.ast;

    if (ast->token == tok_symbol) {
	record = current_record;
	symbol = ast->l.value;
    }
    else if (ast->token == tok_ctor) {
	assert(base->type == tag_class);
	record = base->name;
	symbol = symbol_new;
    }
    else if (ast->token == tok_explicit) {
	assert(ast->l.ast->token == tok_type);
	dot = ast->l.ast->l.value;
	record = dot->name;
	symbol = ast->r.ast->l.value;
    }
    else
	oparse_error(ast, "expecting prototype %A", ast);

    assert(osymbol_p(symbol));
    name = symbol->name;
    type = otype(record);
    if (type != t_record && type != t_namespace)
	oparse_error(proto, "function '%p' cannot be declared here", name);
    if ((function = (ofunction_t *)oget_function(record, name))) {
	if (function->tag != tag)
	    oparse_error(proto, "'%p' redeclared as a different type", name);
    }
    /* if not overriding a virtual method */
    else if ((symbol = oget_symbol(record, name))) {
	if (!symbol->method)
	    oparse_error(proto, "'%p' redeclared as a different type", name);
	/* FIXME this can be allowed, but for consistency and language
	 * simplicity do not allow redefining virtual methods with a
	 * different prototype */
	else if (symbol->tag != tag)
	    oparse_error(proto, "'%p' virtual function prototype changed", name);
    }
    function = onew_function(record, name, tag);

    return (function);
}

static void
function(void)
{
    oast_t		*ast;
    oast_t		*type;
    oast_t		*list;
    oast_t		*proto;
    osymbol_t		*symbol;
    orecord_t		*record;
    ofunction_t		*function;

    if (otype(current_record) != t_record &&
	otype(current_record) != t_namespace)
	oparse_error(top_ast(), "function not allowed here");
    ast = top_ast();
    ast->token = tok_function;
    type = ast->l.ast;
    proto = ast->r.ast;
    function = prototype(type->l.value, proto);
    function->ast = ast;
    ast->t.value = function;
    record = current_record;
    current_record = function->record;

    ofunction_start_args(function);

    if ((list = proto->r.value)) {
	for (; list; list = list->next) {
	    if (list->token == tok_ellipsis) {
		assert(list->next == null);
		break;
	    }
	    else if (list->token != tok_declexpr)
		oparse_error(list, "expecting argument specifier %A", list);
	    symbol = declare(list->l.value, list->r.value);
	    if (symbol == null)
		oparse_error(list, "syntax error %A", list);
	}
    }

    ofunction_start_locs(function);

    memset(except_vector->v.i32, -1, except_vector->length * 4);

    push_block(ast);
    statement_noeof();
    pop_block();
    ast->c.ast = pop_ast();
    current_record = record;
}

static void
enumeration(void)
{
    oast_t		*ast;
    oast_t		*base;
    oast_t		*next;
    oword_t		 value;
    osymbol_t		*symbol;

    /* For now do not allow class specific namespace enums */
    if (otype(current_record) != t_namespace)
	oparse_error(top_ast(), "enum cannot be declared here");
    base = top_ast();
    next = null;
    /* For now only accept nameless enums, as there is no logic
     * to check for misuse of enums, that is, only use enums
     * to aid creating namespace specific (integer) constants */
    if (primary_noeof() != tok_obrace)
	oparse_error(top_ast(), "expecting '{' %A", top_ast());
    discard();

    value = 0;
    if (expression_noeof() == tok_cbrace)
	discard();
    else {
	for (;;) {
	    ast = top_ast();
	    switch (ast->token) {
		case tok_symbol:
		    symbol = ast->l.value;
		    if (oget_symbol(current_record, symbol->name))
			oparse_error(ast, "identifier '%p' redeclared", symbol);
		    onew_constant(current_record, symbol->name, value++);
		    break;
		case tok_set:
		    if (ast->l.ast->token == tok_symbol) {
			symbol = ast->l.ast->l.value;
			ast = ast->r.ast;
			if (ast->token != tok_number)
			    oeval_ast(ast);
			if (ast->token != tok_number ||
			    otype(ast->l.value) != t_word)
			    oparse_error(ast,
					 "enum value not a 32 bit integer %A",
				     ast);
			value = *(oword_t *)ast->l.value;
#if __WORDSIZE == 64
			if (value < -2147483648 || value >  2147483647)
			    oparse_error(ast,
					 "enum value not a 32 bit integer %A",
					 ast);
#endif
			onew_constant(current_record, symbol->name, value++);
			break;
		    }
		default:
		    oparse_error(ast, "expecting constant initializer %A", ast);
	    }
	    if (next == null)
		next = base->r.ast = pop_ast();
	    else {
		next->next = pop_ast();
		next = next->next;
	    }
	    if (lookahead_noeof() == tok_comma) {
		consume();
		(void)expression_noeof();
	    }
	    else if (lookahead_noeof() == tok_cbrace) {
		consume();
		break;
	    }
	}
    }
    if (lookahead_noeof() != tok_semicollon)
	oparse_error(head_ast, "expecting ';' %A", head_ast);
    consume();
}

static void
namespace(void)
{
    oast_t		*ast;
    osymbol_t		*name;
    osymbol_t		*symbol;
    orecord_t		*record;

    if (otype(current_record) != t_namespace)
	oparse_error(top_ast(), "namespace cannot be declared here");
    ast = top_ast();
    if (primary() == tok_symbol) {
	ast->l.ast = pop_ast();
	name = ast->l.ast->l.value;
	if ((symbol = oget_symbol(current_record, name->name)) == null) {
	    symbol = onew_symbol(current_record, name->name, null);
	    record = onew_namespace(symbol);
	}
	else
	    record = symbol->value;
	assert(symbol->namespace == true);
	ast->l.ast->l.value = symbol;
	switch (lookahead_noeof()) {
	    case tok_obrace:
		record->parent = current_record;
		current_record = record;
		push_block(top_ast());
		statement_noeof();
		pop_block();
		current_record = record->parent;
		if (ast->r.ast) {
		    ast->r.ast->next = pop_ast();
		    ast->r.ast = ast->r.ast->next;
		}
		else
		    ast->c.ast = ast->r.ast = pop_ast();
		break;
	    case tok_semicollon:
		consume();
		break;
	    default:
		oparse_error(head_ast, "expecting '{' or ';' %A", head_ast);
		break;
	}
    }
    else
	oparse_error(top_ast(), "expecting symbol %A", top_ast());
}

static otoken_t
structure(void)
{
    oast_t		*ast;
    oast_t		*top;
    otag_t		*tag;
    osymbol_t		*name;
    oast_t		*temp;
    orecord_t		*super;
    otoken_t		 token;
    orecord_t		*record;
    osymbol_t		*symbol;
    orecord_t		*current;
    oobject_t		*pointer;

    if (otype(current_record) != t_namespace)
	oparse_error(top_ast(), "class cannot be declared here");
    switch (primary()) {
	case tok_type:
	    ast = top_ast();
	    tag = ast->l.value;
	    record = tag->name;
	    goto check;
	case tok_symbol:
	    ast = top_ast();
	    symbol = ast->l.value;
	    if ((name = oget_symbol(current_record, symbol->name)))
		symbol = name;
	    if (!symbol->type) {
		record = onew_record(symbol);
		symbol = record->name;
	    }
	    tag = symbol->tag;
	    record = symbol->value;
	check:
	    if (tag == null || tag->type != tag_class)
		oparse_error(ast, "%p redeclared as a different type",
			     record->name);
	    if (record->length)
		oparse_error(ast, "class %p redefined", record->name);
	    token = lookahead();
	    if (token == tok_collon) {
		consume();
		(void)primary_noeof();
		top = pop_ast();
		if (top->token != tok_symbol)
		    oparse_error(ast, "expecting symbol %A", top);
		name = top->l.value;
		symbol = oget_bound_symbol(name->name);
		if (symbol == null || !symbol->type)
		    oparse_error(ast, "expecting base class %A", top);
		tag = symbol->tag;
		if (tag->type != tag_class)
		    oparse_error(ast, "type %p cannot be derived", tag);
		/* Can only derive an already defined class */
		super = tag->name;
		if (rtti_vector->v.ptr[super->type] == null) {
		    assert(super->type > t_mpc &&
			   super->type <= rtti_vector->offset);
		    oparse_error(ast, "class %p is not defined", tag);
		}
		oadd_record(record, super);
		token = lookahead_noeof();
		if (token != tok_obrace)
		    oparse_error(ast, "expecting '{' %A", top_ast());
	    }
	    else if (token != tok_obrace) {
		ast->l.value = otag_object(record);
		if (token == tok_comma || token == tok_semicollon)
		    return (ast->token = tok_defn);
		return (ast->token = tok_type);
	    }
	    break;
	default:
	    oparse_error(top_ast(), "expecting symbol or type %A", top_ast());
    }

    consume();
    ast->token = tok_type;
    current = current_record;
    current_record = record;
    while ((token = expression_noeof()) != tok_cbrace) {
	switch (token) {
	    case tok_expr:
		if (top_ast()->token != tok_vector)
		    oparse_error(top_ast(), "expecting declaration %A",
				 top_ast());
	    case tok_symbol:
		/* allow omitting type */
		gc_ref(pointer);
		onew_ast(pointer, tok_declexpr, top_ast()->note.name,
			 top_ast()->note.lineno, top_ast()->note.column);
		temp = *pointer;
		temp->r.ast = top_ast();
		top_ast() = temp;
		gc_dec();
		onew_ast(&temp->l.value, tok_type, top_ast()->note.name,
			 top_ast()->note.lineno, top_ast()->note.column);
		temp->l.ast->l.value = auto_tag;
		token = lookahead();
		if (token != tok_comma && token != tok_semicollon)
		    oparse_error(top_ast(), "expecting declaration %A",
				 top_ast());
	    case tok_ctor:	case tok_decl:
		if (declaration() != tok_function)
		    discard();
		else
		    (void)pop_ast();
		break;
	    default:
		oparse_error(top_ast(), "expecting declaration %A", top_ast());
	}
    }
    discard();
    current_record = current;
    oend_record(record);
    if (record->length == 0 &&
	record->function == null && record->methods->count == 0)
	oparse_error(top_ast(), "empty declaration");
    ast->l.value = otag_object(record);

    return (tok_type);
}

static otoken_t
definition(void)
{
    oast_t		*ast;
    otoken_t		 token;

    ast = top_ast();
    token = structure();
    ast->l.ast = pop_ast();
    switch (lookahead_noeof()) {
	case tok_comma:
	    if (token != tok_defn)
		oparse_error(head_ast, "expecting ';' %A", head_ast);
	    ast = ast->l.ast;
	    consume();
	    for (;;) {
		if (structure() != tok_defn)
		    oparse_error(top_ast(),
				 "type declaration must be single statement %A",
				 top_ast());
		ast->next = pop_ast();
		ast = ast->next;
		token = lookahead_noeof();
		consume();
		if (token == tok_semicollon)
		    break;
		if (token != tok_comma)
		    oparse_error(head_ast, "expecting ',' or ';' %A", head_ast);
	    }
	    break;
	case tok_semicollon:
	    consume();
	    break;
	default:
	    if (token == tok_defn)
		oparse_error(head_ast, "expecting ',' or ';' %A", head_ast);
	    oparse_error(head_ast, "expecting ';' %A", head_ast);
    }

    return (tok_class);
}

static void
group_match(obool_t data, otoken_t token)
{
    ouint8_t		 chr;
    oast_t		*ast;

    if (group(data) != token) {
	if ((ast = top_ast())) {
	    for (; ast->next; ast = ast->next)
		;
	}
	else
	    ast = head_ast;
	switch (token) {
	    case tok_semicollon:	chr = ';';	break;
	    case tok_cparen:		chr = ')';	break;
	    case tok_cbrace:		chr = '}';	break;
	    default:			abort();
	}
	oparse_error(ast, "expecting ',' or '%c' %A", chr, ast);
    }
}

static otoken_t
group(obool_t data)
{
    oast_t		*ast;
    oast_t		*init;
    otoken_t		 token;
    oobject_t		*pointer;

    ast = null;
    switch (token = lookahead()) {
	case tok_cparen:	case tok_cbrace:
	case tok_semicollon:
	    push_ast(null);
	    return (token);
	default:
	    break;
    }
    for (;;) {
	if  (expression_noeof() == tok_label && data) {
	    /* Rewrite "label, expr" to "init{ref, expr}" */
	    gc_ref(pointer);
	    onew_ast(pointer, tok_init, top_ast()->note.name,
		     top_ast()->note.lineno, top_ast()->note.column);
	    gc_pop(init);
	    init->l.ast = top_ast();
	    top_ast() = init;
	    if (otype(init->l.ast->l.value) == t_symbol)
		init->l.ast->token = tok_symbol;
	    else
		init->l.ast->token = tok_number;
	    (void)expression_noeof();
	    init->r.ast = pop_ast();
	}
	if (ast == null)
	    ast = top_ast();
	else {
	    ast->next = pop_ast();
	    ast = ast->next;
	}
	if ((token = lookahead()) == tok_comma) {
	    consume();
	    if (data && lookahead() == tok_cbrace) {
		if (ast == null)
		    push_ast(null);
		return (tok_cbrace);
	    }
	}
	else {
	    if (ast == null)
		push_ast(null);
	    return (token);
	}
    }
}

static otoken_t
precedence_noeof(void)
{
    otoken_t		token = precedence();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
precedence(void)
{
    oast_t		*ast;
    otoken_t		 next;
    otoken_t		 token;

    for (token = unary();;) {
	switch (token) {
	    case tok_obrace:
		ast = top_ast();
		group_match(true, tok_cbrace);
		consume();
		ast->token = tok_data;
		ast->l.ast = pop_ast();
		return (tok_expr);
	    case tok_list:
		ast = top_ast();
		if (ast->l.ast == null)
		    oparse_error(ast, "expecting expression after '(' %A", ast);
	    case tok_number:		case tok_symbol:
	    case tok_expr:		case tok_string:
		switch (next = lookahead()) {
		    case tok_set:	case tok_andset:
		    case tok_orset:	case tok_xorset:
		    case tok_mul2set:	case tok_div2set:
		    case tok_shlset:	case tok_shrset:
		    case tok_addset:	case tok_subset:
		    case tok_mulset:	case tok_divset:
		    case tok_trunc2set:	case tok_remset:
			token = binary_assign();
			break;
		    case tok_andand:	case tok_oror:
		    case tok_lt:	case tok_le:
		    case tok_eq:	case tok_ge:
		    case tok_gt:	case tok_ne:
		    case tok_and:	case tok_or:
		    case tok_xor:	case tok_mul2:
		    case tok_div2:	case tok_shl:
		    case tok_shr:	case tok_add:
		    case tok_sub:	case tok_mul:
		    case tok_div:	case tok_trunc2:
		    case tok_rem:
			token = binary_left(next);
			break;
		    case tok_question:
			(void)primary();
			ast = pop_ast();
			ast->t.ast = top_ast();
			top_ast() = ast;
			precedence_noeof();
			ast->l.ast = pop_ast();
			if (primary_noeof() != tok_collon)
			    oparse_error(top_ast(), "expecting ':' %A",
					 top_ast());
			discard();
			precedence_noeof();
			ast->r.ast = pop_ast();
			token = tok_expr;
			break;
		    default:
			return (token);
		}
		break;
	    default:
		return (token);
	}
    }
}

static otoken_t
expression_noeof(void)
{
    otoken_t		token = expression();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
expression(void)
{
    otoken_t		token;

    switch (token = precedence()) {
	case tok_symbol:		case tok_number:
	    if (lookahead() == tok_collon) {
		consume();
		top_ast()->token = token = tok_label;
	    }
	default:
	    break;
    }

    return (token);
}

static otoken_t
binary_assign(void)
{
    oast_t		*last;
    oast_t		*mast;
    oast_t		*rast;

    for (;;) {
	primary();
	switch (expression()) {
	    case tok_number:		case tok_string:
	    case tok_symbol:		case tok_this:
	    case tok_expr:		case tok_list:
		break;
	    default:
		oparse_error(top_ast(), "expecting expression %A", top_ast());
	}
	rast = pop_ast();
	mast = pop_ast();
	last = top_ast();
	mast->l.value = last;
	mast->r.value = rast;
	top_ast() = mast;
	switch (lookahead()) {
	    case tok_set:		case tok_andset:
	    case tok_orset:		case tok_xorset:
	    case tok_mul2set:		case tok_div2set:
	    case tok_shlset:		case tok_shrset:
	    case tok_addset:		case tok_subset:
	    case tok_mulset:		case tok_divset:
	    case tok_remset:
		break;
	    default:
		return (tok_expr);
	}
    }
}

static otoken_t
binary_left(otoken_t token)
{
    oast_t		*last;
    oast_t		*mast;
    oast_t		*rast;

    for (;;) {
	(void)primary();
	binary_right(token);
	rast = pop_ast();
	mast = pop_ast();
	last = top_ast();
	mast->l.value = last;
	mast->r.value = rast;
	top_ast() = mast;
	switch (lookahead()) {
	    case tok_andand ... tok_oror:
		if (token <= tok_oror)	break;
		return (tok_expr);
	    case tok_ne ... tok_gt:
		if (token <= tok_ne)	break;
		return (tok_expr);
	    case tok_and ... tok_xor:
		if (token <= tok_xor)	break;
		return (tok_expr);
	    case tok_mul2 ... tok_shr:
		if (token <= tok_shr)	break;
		return (tok_expr);
	    case tok_add ... tok_sub:
		if (token <= tok_sub)	break;
		return (tok_expr);
	    case tok_mul ... tok_rem:	break;
	    default:
		return (tok_expr);
	}
    }
}

static void
binary_right(otoken_t token)
{
    otoken_t		next;

    switch (unary()) {
	case tok_expr:		case tok_number:
	case tok_symbol:	case tok_this:
	case tok_list:
	    break;
	case tok_string:
	    if (token == tok_eq || token == tok_ne ||
		top_ast()->l.value == null)
		break;
	default:
	    oparse_error(top_ast(), "expecting expression %A", top_ast());
    }
    for (;;) {
	switch (next = lookahead()) {
	    case tok_ne ... tok_gt:
		if (token <= tok_oror)	break;
		return;
	    case tok_and ... tok_xor:
		if (token <= tok_gt)	break;
		return;
	    case tok_mul2 ... tok_shr:
		if (token <= tok_xor)	break;
		return;
	    case tok_add ... tok_sub:
		if (token <= tok_shr)	break;
		return;
	    case tok_mul ... tok_rem:
		if (token <= tok_sub)	break;
		return;
	    default:
		return;
	}
	binary_left(next);
    }
}

static otoken_t
unary_noeof(void)
{
    otoken_t		token = unary();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
unary(void)
{
    otoken_t		 token;

    switch (token = primary()) {
	case tok_type:
	    switch (lookahead()) {
		case tok_dot:
		    return (unary_loop(token));
		case tok_oparen:
		    return (unary_ctor());
		    break;
		default:
		    return (unary_decl());
	    }
	case tok_string:
	    token = unary_string();
	case tok_symbol:	case tok_this:
	    return (unary_loop(token));
	case tok_oparen:
	    return (unary_list());
	case tok_inc:		case tok_dec:
	case tok_add:		case tok_sub:
	case tok_not:		case tok_com:
	    return (unary_value(token));
	case tok_atan2:		case tok_pow:
	case tok_hypot:		case tok_complex:
	case tok_subtypeof:	case tok_renew:
	    return (unary_binary(token));
	case tok_sizeof:	case tok_new:
	case tok_typeof:	case tok_integer_p:
	case tok_rational_p:	case tok_float_p:
	case tok_real_p:	case tok_complex_p:
	case tok_number_p:	case tok_finite_p:
	case tok_inf_p:		case tok_nan_p:
	case tok_num:		case tok_den:
	case tok_real:		case tok_imag:
	case tok_signbit:	case tok_abs:
	case tok_signum:	case tok_rational:
	case tok_arg:		case tok_conj:
	case tok_floor:		case tok_trunc:
	case tok_round:		case tok_ceil:
	case tok_sqrt:		case tok_cbrt:
	case tok_sin:		case tok_cos:
	case tok_tan:		case tok_asin:
	case tok_acos:		case tok_atan:
	case tok_sinh:		case tok_cosh:
	case tok_tanh:		case tok_asinh:
	case tok_acosh:		case tok_atanh:
	case tok_proj:		case tok_exp:
	case tok_log:		case tok_log2:
	case tok_log10:
	    return (unary_unary(token));
	case tok_thread:
	    token = unary_unary(token);
	    if (top_ast()->l.ast->token != tok_call)
		oparse_error(top_ast()->l.ast, "'thread' argument must be a function call");
	    return (token);
	case tok_ellipsis:
	    if (lookahead() == tok_obrack)
		token = unary_vector();
	    return (token);
	case tok_FUNCTION:
	    if (current_record->function->name)
		top_ast()->l.value = current_record->function->name->name;
	    else
		top_ast()->l.value = null;
	    top_ast()->token = tok_string;
	    token = unary_string();
	    return (unary_loop(token));
	case tok_dot:
	    return (unary_namespace());
	default:
	    return (token);
    }
}

static otoken_t
unary_string(void)
{
    oast_t		*ast;
    oast_t		*top;
    obool_t		 first;
    oword_t		 offset;
    oword_t		 length;
    ovector_t		*append;
    ovector_t		*string;
    oobject_t		*pointer;

    ast = top_ast();
    if ((string = ast->l.value)) {
	first = true;
	offset = string->length;
	while (lookahead() == tok_string) {
	    primary();
	    top = top_ast();
	    if ((append = top->l.value) == null)
		oparse_error(top, "'null' not expected");
	    length = offset + append->length;
	    if (length < offset)
		oparse_error(top, "out of bounds");
	    if (first) {
		gc_ref(pointer);
		onew_vector(pointer, t_uint8, length);
		gc_pop(append);
		memcpy(append->v.u8, string->v.u8, string->length);
		string = ast->l.value = append;
		append = top->l.value;
	    }
	    else
		orenew_vector(string, length);
	    memcpy(string->v.u8 + offset, append->v.u8, append->length);
	    offset = length;
	    pop_ast();
	}
    }

    return (tok_string);
}

static otoken_t
unary_loop(otoken_t token)
{
    oast_t		*ast;
    otoken_t		 next;
    osymbol_t		*symbol;

    if (token == tok_symbol) {
	ast = top_ast();
	symbol = ast->l.value;
	symbol = oget_bound_symbol(symbol->name);
	if (symbol) {
	    if (symbol->type) {
		ast->token = tok_type;
		ast->l.value = symbol->tag;
		switch (lookahead()) {
		    case tok_dot:
			token = tok_type;
			break;
		    case tok_oparen:
			return (unary_ctor());
			break;
		    default:
			return (unary_decl());
		}
	    }
	    else if (symbol->constant) {
		ast->token = tok_number;
		assert(symbol->value && otype(symbol->value) == t_word);
		onew_word(&ast->l.value, *(oword_t *)symbol->value);
		return (tok_number);
	    }
	}
    }
    for (;;) {
	switch (next = lookahead()) {
	    case tok_obrack:
		token = tok_expr;
		(void)unary_vector();
		break;
	    case tok_oparen:
		token = tok_expr;
		(void)primary();
		discard();
		group_match(false, tok_cparen);
		ast = head_ast;
		head_ast = null;
		ast->token = tok_call;
		ast->r.ast = pop_ast();
		ast->l.ast = top_ast();
		top_ast() = ast;
		break;
	    case tok_dot:
		token = unary_field();
		break;
	    case tok_inc:	case tok_dec:
		token = tok_expr;
		(void)primary();
		ast = pop_ast();
		ast->token = next == tok_inc ? tok_postinc : tok_postdec;
		ast->l.ast = top_ast();
		top_ast() = ast;
		break;
	    default:
		return (token);
	}
    }
}

static otoken_t
unary_value(otoken_t token)
{
    oast_t		*ast;
    oast_t		*value;

    switch (lookahead()) {
	case tok_number:	case tok_symbol:
	case tok_string:	case tok_add:
	case tok_sub:		case tok_not:
	case tok_com:		case tok_integer_p:
	case tok_rational_p:	case tok_float_p:
	case tok_real_p:	case tok_complex_p:
	case tok_number_p:	case tok_finite_p:
	case tok_inf_p:		case tok_nan_p:
	case tok_num:		case tok_den:
	case tok_real:		case tok_imag:
	case tok_signbit:	case tok_abs:
	case tok_signum:	case tok_rational:
	case tok_arg:		case tok_conj:
	case tok_floor:		case tok_trunc:
	case tok_round:		case tok_ceil:
	case tok_sqrt:		case tok_cbrt:
	case tok_sin:		case tok_cos:
	case tok_tan:		case tok_asin:
	case tok_acos:		case tok_atan:
	case tok_sinh:		case tok_cosh:
	case tok_tanh:		case tok_asinh:
	case tok_acosh:		case tok_atanh:
	case tok_proj:		case tok_exp:
	case tok_log:		case tok_log2:
	case tok_log10:		case tok_atan2:
	case tok_pow:		case tok_hypot:
	case tok_complex:	case tok_ellipsis:
	case tok_oparen:	case tok_subtypeof:
	    break;
	default:
	    oparse_error(top_ast(), "expecting expression %A", top_ast());
    }

    unary();
    ast = pop_ast();
    value = top_ast();
    switch (token) {
	case tok_add:		value->token = tok_plus;	break;
	case tok_sub:		value->token = tok_neg;		break;
	case tok_inc:		case tok_dec:			break;
	case tok_not:		case tok_com:			break;
	default:		oparse_error(value, "internal error");
    }
    value->l.value = ast;

    return (unary_loop(tok_expr));
}

static otoken_t
unary_ctor(void)
{
    oast_t		*ast;
    oast_t		*top;
    otag_t		*tag;
    obool_t		 error;
    oobject_t		*pointer;

    ast = top_ast();
    assert(ast->token == tok_type);
    gc_ref(pointer);
    onew_ast(pointer, tok_declexpr, ast->note.name,
	     ast->note.lineno, ast->note.column);
    gc_pop(ast);
    ast->l.ast = top_ast();
    top_ast() = ast;
    ast->token = tok_declexpr;

    if (lookahead_noeof() != tok_oparen)
	oparse_error(head_ast, "expecting '(' %A", head_ast);
    ast->r.ast = head_ast;
    head_ast = null;
    ast = ast->r.ast;
    ast->token = tok_call;

    if (primary_noeof() != tok_cparen) {
	top = top_ast();
	error = true;
	if (top->token == tok_type) {
	    tag = top->l.value;
	    /* s/(void)/()/ */
	    if (tag->type == tag_basic && tag->size == 0) {
		consume();
		if (lookahead_noeof() != tok_cparen)
		    oparse_error(head_ast, "syntax error");
		error = false;
	    }
	}
	if (error)
	    oparse_error(top, "constructor cannot receive arguments");
    }
    else
	top = null;
    ast->l.ast = pop_ast();
    ast->l.ast->token = tok_ctor;

    if (lookahead_noeof() != tok_obrace)
	oparse_error(head_ast, "expecting '{'");

    return (tok_ctor);
}

static otoken_t
unary_decl(void)
{
    oast_t		*ast;
    otoken_t		 token;
    oobject_t		*pointer;

    ast = top_ast();
    assert(ast->token == tok_type);
    token = lookahead();
    if (token == tok_comma || token == tok_cparen || token == tok_semicollon)
	return (tok_type);
    else if (token == tok_obrack)
	return (unary_vector());
    gc_ref(pointer);
    onew_ast(pointer, tok_declexpr, ast->note.name,
	     ast->note.lineno, ast->note.column);
    gc_pop(ast);
    ast->l.ast = top_ast();
    top_ast() = ast;
    ast->token = tok_declexpr;
    switch ((token = expression_noeof())) {
	case tok_symbol:		case tok_expr:
	    break;
	default:
	    oparse_error(top_ast(), "expecting declaration %A", top_ast());
    }
    ast->r.ast = pop_ast();

    return (tok_decl);
}

static otoken_t
unary_list(void)
{
    oast_t		*ast, *list;

    group_match(false, tok_cparen);
    consume();
    list = pop_ast();
    ast = top_ast();
    ast->token = tok_list;
    ast->l.value = list;

    return (unary_loop(tok_list));
}

static otoken_t
unary_vector(void)
{
    oast_t		*ast;

    ast = head_ast;
    head_ast = null;
    ast->token = tok_vector;
    ast->l.ast = top_ast();
    top_ast() = ast;
    if (expression_noeof() == tok_cbrack)
	discard();
    else if (primary_noeof() != tok_cbrack)
	oparse_error(top_ast(), "expecting ']' %A", top_ast());
    else {
	discard();
	ast->r.ast = pop_ast();
    }

    return (unary_loop(tok_expr));
}

static otoken_t
unary_field(void)
{
    oast_t		*ast;
    oobject_t		 value;
    orecord_t		*record;
    osymbol_t		*symbol;

    ast = head_ast;
    head_ast = null;
    ast->l.ast = top_ast();
    top_ast() = ast;
    if (ast->l.ast->token == tok_type)
	ast->token = tok_explicit;
    if (primary_noeof() != tok_symbol) {
	if (top_ast()->token == tok_type) {
	    if (ast->l.ast->token == tok_type &&
		top_ast()->l.value == ast->l.ast->l.value) {
		/* constructor full specification or error */
		value = ast->l.ast->l.value;
		discard();
		ast = top_ast();
		odel_object(&ast->l.value);
		ast->token = tok_type;
		ast->l.value = value;
		return (unary_ctor());
	    }
	    if (lookahead() == tok_dot) {
		/* explicit call not from a class method or error */
		consume();
		if (primary_noeof() == tok_symbol) {
		    ast->token = tok_explicit;
		    ast->t.ast = ast->l.ast;
		    ast->r.ast = pop_ast();
		    ast->l.ast = pop_ast();
		    return (unary_loop(tok_expr));
		}
	    }
	}
	oparse_error(top_ast(), "expecting symbol %A", top_ast());
    }
    ast->r.ast = pop_ast();

    /* check for explicit namespace type access */
    if (ast->l.ast->token == tok_symbol) {
	symbol = ast->l.ast->l.value;
	/* ast may have been already replaced with final symbol */
	if (!symbol->namespace)
	    symbol = oget_namespace_symbol(symbol->name);
	if (symbol && symbol->namespace) {
	    record = current_record;
	    current_record = symbol->value;
	    symbol = ast->r.ast->l.value;
	    assert(osymbol_p(symbol));
	    symbol = oget_namespace_symbol(symbol->name);
	    current_record = record;
	    if (symbol == null)
		oparse_error(ast->r.ast, "namespace %p has no %p symbol",
			     ast->l.ast->l.value, ast->r.ast->l.value);
	    odel_object(&ast->l.value);
	    odel_object(&ast->r.value);
	    if (symbol->type) {
		ast->token = tok_type;
		ast->l.value = symbol->tag;
		switch (lookahead()) {
		    case tok_dot:
			break;
		    case tok_oparen:
			return (unary_ctor());
			break;
		    default:
			return (unary_decl());
		}
	    }
	    else if (symbol->constant) {
		ast->token = tok_number;
		assert(symbol->value && otype(symbol->value) == t_word);
		onew_word(&ast->l.value, *(oword_t *)symbol->value);
		return (tok_number);
	    }
	    else {
		ast->token = tok_symbol;
		ast->l.value = symbol;
	    }
	}
    }

    return (unary_loop(tok_expr));
}

static otoken_t
unary_namespace(void)
{
    oast_t		*ast;
    osymbol_t		*symbol;

    discard();
    if (primary_noeof() != tok_symbol)
	oparse_error(top_ast(), "expecting symbol %A", top_ast());
    ast = top_ast();
    symbol = ast->l.value;
    if ((symbol = oget_symbol(root_record, symbol->name)) == null)
	oparse_error(ast, "root namespace has no %p symbol", ast->l.value);
    ast->l.value = symbol;

    return (unary_loop(tok_symbol));
}

static otoken_t
unary_unary(otoken_t token)
{
    otag_t		*tag;
    oast_t		*ast;
    oast_t		*vast;
    obool_t		 paren;
    oword_t		 length;

    ast = top_ast();
    if ((paren = lookahead_noeof() == tok_oparen))
	consume();
    else if (token != tok_sizeof &&
	     token != tok_new &&
	     token != tok_typeof &&
	     token != tok_thread)
	oparse_error(ast, "expecting '(' %A", head_ast);
    expression_noeof();
    ast->l.ast = pop_ast();
    if (token == tok_new) {
	if (ast->l.ast->token != tok_type) {
	    if (ast->l.ast->token == tok_vector) {
		ast->l.ast->token = tok_vecnew;
		vast = ast->l.ast;
		if (vast->l.ast->token == tok_type) {
		    if (vast->r.ast == null)
			oparse_error(vast, "expecting vector length", vast);
		    if (vast->r.ast->token == tok_number) {
			if (otype(vast->r.ast->l.value) != t_word)
			    oparse_error(vast->r.ast,
					 "vector length not an integer");
			if ((length = *(oword_t *)vast->r.ast->l.value) < 0)
			    oparse_error(vast->r.ast, "invalid vector length");
		    }
		    else
			length = 0;
		    /* else a runtime vector length */
		    ast->r.value = otag_vector(vast->l.ast->l.value, length);
		    goto next;
		}
	    }
	    oparse_error(ast->l.ast, "expecting type %A", ast->l.ast);
	}
	else {
	    tag = ast->l.ast->l.value;
	    if (tag->type != tag_class)
		oparse_error(ast->l.ast, "expecting class %A", ast->l.ast);
	}
    }
next:
    if (paren) {
	if (primary_noeof() != tok_cparen)
	    oparse_error(ast, "expecting ')' %A", ast);
	discard();
    }

    return (tok_expr);
}

static otoken_t
unary_binary(otoken_t token)
{
    oast_t		*ast;

    ast = top_ast();
    if (primary_noeof() != tok_oparen)
	oparse_error(top_ast(), "expecting '(' %A", top_ast());
    discard();
    expression_noeof();
    ast->l.ast = pop_ast();
    if (primary_noeof() != tok_comma)
	oparse_error(top_ast(), "expecting ',' %A", top_ast());
    discard();
    expression_noeof();
    ast->r.ast = pop_ast();
    if (primary_noeof() != tok_cparen)
	oparse_error(top_ast(), "expecting ')' %A", top_ast());
    discard();

    return (tok_expr);
}

static otoken_t
primary_noeof(void)
{
    otoken_t		token = primary();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
primary(void)
{
    otoken_t		token;
    oobject_t		object;

    if (head_ast) {
	token = head_ast->token;
	push_ast(head_ast);
	head_ast = null;
	return (token);
    }

    if ((object = oread_object()) == object_eof)
	return (tok_eof);
    gc_push(object);

    return (tokenize());
}

static otoken_t
lookahead(void)
{
    if (head_ast == null) {
	if (primary() == tok_eof)
	    return (tok_eof);
	head_ast = pop_ast();
    }

    return (head_ast->token);
}

static otoken_t
lookahead_noeof(void)
{
    otoken_t		token = lookahead();

    if (token == tok_eof)
	oread_error("unexpected end of file");

    return (token);
}

static otoken_t
tokenize(void)
{
    oast_t		*ast;
    otoken_t		 token;
    oobject_t		 object;
    osymbol_t		*symbol;
    oobject_t		*pointer;

again:
    gc_top(object);
    switch (otype(object)) {
	case t_word:		case t_float:		case t_rat:
	case t_mpz:		case t_mpq:		case t_mpr:
	case t_cdd:		case t_cqq:		case t_mpc:
	    token = tok_number;
	    break;
	case t_string:
	    token = tok_string;
	    break;
	case t_symbol:
	    token = tok_symbol;
	    symbol = object;
	    if (symbol->special) {
		if (symbol->value == null) {
		    token = tok_string;
		    gc_set(null);
		}
		else {
		    switch (otype(symbol->value)) {
			case t_word:		case t_float:
			case t_rat:		case t_mpz:
			case t_mpq:		case t_mpr:
			case t_cdd:		case t_cqq:
			case t_mpc:
			    /* Must make a copy or cache and
			     * constant folding will corrupt
			     * input */
			    ocopy(gc_vector + gc_offset - 1, symbol->value);
			    break;
			default:
			    gc_set(symbol->value);
			    break;
		    }
		    goto again;
		}
	    }
	    break;
	case t_ast:
	    ast = object;
	    push_ast(ast);
	    gc_dec();
	    return (ast->token);
	default:
	    oread_error("internal error (unexpected object type)");
    }

    gc_ref(pointer);
    onew_ast(pointer, token, input_note.name, input->lineno, input->column);
    gc_pop(ast);
    gc_pop(ast->l.value);
    push_ast(ast);

    return (ast->token);
}

static void
consume(void)
{
    assert(head_ast);
    odel_object((oobject_t *)&head_ast);
}

static void
discard(void)
{
    assert(parser_vector->offset > 0);
    odel_object((oobject_t *)(ast_vector + --parser_vector->offset));
}

static void
push_ast(oast_t *ast)
{
    if (parser_vector->offset >= parser_vector->length) {
	orenew_vector(parser_vector, parser_vector->length + 16);
	ast_vector = parser_vector->v.obj;
    }
    ast_vector[parser_vector->offset++] = ast;
}

static oast_t *
pop_ast(void)
{
    assert(parser_vector->offset > 0);
    return (ast_vector[--parser_vector->offset]);
}

static void
push_block(oast_t *ast)
{
    if (block_vector->offset >= block_vector->length)
	orenew_vector(block_vector, 16);
    block_vector->v.ptr[block_vector->offset++] = ast;
}

static void
pop_block(void)
{
    assert(block_vector->offset > 0);
    block_vector->v.ptr[--block_vector->offset] = null;
}
