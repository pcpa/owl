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
 * Initialization
 */
orecord_t		*current_record;

/*
 * Prototypes
 */
static void
eval_root(void);

static void
eval_ast_stat(oast_t *ast);

static void
eval_ast_decl(oast_t *decl);

static void
eval_ast_declexpr(oast_t *decl);

static void
eval_ast_goto(oast_t *ast);

static void
eval_ast_sizeof(oast_t *ast);

static osymbol_t *
ast_call_symbol(oast_t *ast);

static void
eval_ast_typeof(oast_t *ast);

static void
eval_ast_renew(oast_t *ast);

static void
update_symbol(oast_t *ast, obool_t function);

static void
check_exception(oast_t *ast);

/*
 * Implementation
 */
void
init_code(void)
{
}

void
finish_code(void)
{
}

void
ocode(void)
{
    if ((root_record->function->ast->c.ast->l.ast = oparser()))
	eval_root();
}

void
oeval_ast(oast_t *ast)
{
    oast_t		*temp;
    orecord_t		*record;
    osymbol_t		*symbol;

    switch (ast->token) {
	case tok_set:		case tok_andset:
	case tok_orset:		case tok_xorset:
	case tok_mul2set:	case tok_div2set:
	case tok_shlset:	case tok_shrset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	case tok_trunc2set:	case tok_remset:
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    break;
	case tok_vector:
	    oeval_ast(ast->l.ast);
	    if (ast->r.ast == null)
		oparse_error(ast, "expecting offset");
	    oeval_ast(ast->r.ast);
	    /* validate element reference */
	    oeval_ast_tag(ast);
	    break;
	case tok_dot:
	    oeval_ast(ast->l.ast);
	    /* validate field reference */
	    oeval_ast_tag(ast);
	    break;
	case tok_explicit:
	    oeval_ast_tag(ast);
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
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    ofold(ast);
	    break;
	case tok_inc:		case tok_dec:
	case tok_postinc:	case tok_postdec:
	case tok_new:		case tok_thread:
	    oeval_ast(ast->l.ast);
	    break;
	case tok_init:		case tok_vecnew:
	    oeval_ast(ast->r.ast);
	    break;
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
	    oeval_ast(ast->l.ast);
	    ofold(ast);
	    break;
	case tok_atan2:		case tok_pow:
	case tok_hypot:
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    ofold(ast);
	    break;
	case tok_subtypeof:
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    break;
	case tok_sizeof:
	    eval_ast_sizeof(ast);
	    break;
	case tok_typeof:
	    eval_ast_typeof(ast);
	    break;
	case tok_renew:
	    eval_ast_renew(ast);
	    break;
	case tok_question:
	    oeval_ast(ast->t.ast);
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    break;
	case tok_if:
	    eval_ast_stat(ast->t.ast);
	    eval_ast_stat(ast->l.ast);
	    eval_ast_stat(ast->r.ast);
	    break;
	case tok_return:
	    check_exception(ast);
	case tok_throw:
	    if (ast->l.ast)
		oeval_ast(ast->l.ast);
	    break;
	case tok_switch:
	    eval_ast_stat(ast->t.ast);
	    eval_ast_stat(ast->c.ast);
	    break;
	case tok_for:
	    eval_ast_stat(ast->l.ast);
	    eval_ast_stat(ast->t.ast);
	    eval_ast_stat(ast->r.ast);
	    eval_ast_stat(ast->c.ast);
	    break;
	case tok_do:		case tok_while:
	    eval_ast_stat(ast->c.ast);
	    eval_ast_stat(ast->t.ast);
	    break;
	case tok_list:
	    temp = ast->l.ast;
	    eval_ast_stat(temp);
	    /*   Check inner fields for the sake of clean debug output. */
	    if (temp->next == null && temp->r.value == null &&
		temp->t.value == null && temp->c.value == null)
		omove_ast_up_full(ast, temp);
	    break;
	case tok_call:
	    if (ast->l.ast->token != tok_symbol)
		oeval_ast(ast->l.ast);
	    else
		update_symbol(ast->l.ast, true);
	    eval_ast_stat(ast->r.ast);
	    break;
	case tok_stat:		case tok_code:
	case tok_data:		case tok_finally:
	    eval_ast_stat(ast->l.ast);
	    break;
	case tok_decl:
	    oeval_ast(ast->l.ast);
	    eval_ast_decl(ast->r.ast);
	    break;
	case tok_symbol:
	    update_symbol(ast, false);
	    break;
	case tok_goto:
	    eval_ast_goto(ast);
	    break;
	case tok_try:
	    eval_ast_stat(ast->r.ast);
	    break;
	case tok_catch:
	    eval_ast_decl(ast->l.ast->r.ast);
	    eval_ast_stat(ast->r.ast);
	    break;
	case tok_break:		case tok_continue:
	    check_exception(ast);
	case tok_type:		case tok_number:	case tok_string:
	case tok_class:		case tok_label:		case tok_case:
	case tok_default:	case tok_function:	case tok_ellipsis:
	case tok_this:		case tok_enum:
	    break;
	case tok_namespace:
	    record = current_record;
	    assert(ast->l.ast->token == tok_symbol);
	    symbol = ast->l.ast->l.value;
	    current_record = symbol->value;
	    assert(otype(current_record) == t_namespace);
	    eval_ast_stat(ast->c.ast);
	    current_record = record;
	    break;
	default:
#if DEBUG
	    oparse_warn(ast,
			"code: not handling %s", otoken_to_charp(ast->token));
#endif
	    break;
    }
}

otag_t *
oeval_ast_tag(oast_t *ast)
{
    otag_t		*tag;
    oast_t		*rast;
    orecord_t		*record;
    osymbol_t		*symbol;
    ovector_t		*vector;

    switch (ast->token) {
	case tok_symbol:
	    update_symbol(ast, false);
	    symbol = ast->l.value;
	    return (symbol->tag);
	case tok_vector:
	    tag = oeval_ast_tag(ast->l.ast);
	    switch (tag->type) {
		case tag_vector:
		case tag_varargs:
		    break;
		case tag_hash:
		    /* Patch token to simplify later parsing */
		    ast->token = tok_hash;
		    break;
		default:
		    if (ast->l.ast->token == tok_symbol) {
			symbol = ast->l.ast->l.value;
			oparse_error(ast, "'%p' is not a vector %A",
				     symbol->name, ast);
		    }
		    oparse_error(ast, "not a vector reference %A", ast);
		    break;
	    }
	    return (tag->base);
	case tok_dot:
	    if (ast->l.ast->token == tok_this) {
		for (record = current_record; record; record = record->parent)
		    if (otype(record) == t_record)
			break;
		if (record == null)
		    oparse_error(ast,
				 "'this' now allowed outside 'class' method");
		tag = record->name->tag;
	    }
	    else
		tag = oeval_ast_tag(ast->l.ast);
	    rast = ast->r.ast;
	    assert(rast->token == tok_symbol);
	    if (tag->type == tag_class && ast->l.ast->token == tok_this) {
		omove_ast_up_full(ast, rast);
		rast = ast;
	    }
	    else {
		while (tag->type == tag_function) {
		    vector = tag->name;
		    tag = vector->v.ptr[0];
		}
		if (tag->type != tag_class) {
		    if (ast->l.ast->token == tok_symbol) {
			symbol = ast->l.ast->l.value;
			oparse_error(ast, "'%p' is not a class or namespace %A",
				     symbol->name, ast);
		    }
		    oparse_error(ast, "not a class or namespace %A", ast);
		}
	    }
	    record = tag->name;
	    symbol = rast->l.value;
	    vector = symbol->name;
	    if ((symbol = oget_symbol(record, vector)) == null)
		oparse_error(ast, "'%p' has no %s named '%p'",
			     record->name,
			     tag->type == tag_class ? "field" : "symbol",
			     vector);
	    rast->l.value = symbol;
	    return (symbol->tag);
	case tok_explicit:
	    tag = ast->l.ast->l.value;
	    if (tag->type != tag_class)
		oparse_error(ast, "not a class reference");
	    record = tag->name;
	    rast = ast->r.ast;
	    assert(rast->token == tok_symbol);
	    symbol = rast->l.value;
	    vector = symbol->name;
	    if ((symbol = oget_symbol(record, vector)) == null)
		oparse_error(ast, "'%p' has no field named '%p'",
			     record->name, vector);
	    rast->l.value = symbol;
	    return (symbol->tag);
	case tok_string:
	    return (string_tag);
	case tok_ellipsis:
	    return (varargs_tag);
	case tok_call:
	    return (oeval_ast_tag(ast->l.ast));
	default:
	    oparse_error(ast, "not a type or symbol reference %A", ast);
    }
}

void
omove_left_ast_up(oast_t *ast)
{
    assert(ast->r.ast == null);
    ast->r.ast = ast->l.ast;
    ast->token = ast->l.ast->token;
    ast->l.ast = ast->l.ast->l.ast;
    ast->r.ast->l.value = null;
    odel_object(&ast->r.value);
}

void
omove_ast_up_full(oast_t *ast, oast_t *move)
{
    assert(move == ast->l.ast || move == ast->r.ast ||
	   move == ast->t.ast || move == ast->c.ast);
    gc_push(move);
    ast->l.ast = move->l.ast;
    ast->r.ast = move->r.ast;
    ast->t.ast = move->t.ast;
    ast->c.ast = move->c.ast;
    ast->token = move->token;
    move->l.value = move->r.value = move->t.value = move->c.value = null;
    odel_object((oobject_t *)&move);
    gc_dec();
}

static void
eval_root(void)
{
    oast_t		*ast;
    oword_t		 offset;
    orecord_t		*record;
    ofunction_t		*function;

    assert(current_record == root_record);
    assert(current_function == root_record->function);
    oeval_ast(root_record->function->ast->c.ast);
    odata(root_record->function->ast->c.ast);

    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function))) {
	    function = record->function;
	    current_record = record;
	    current_function = function;
	    /* Error later if function is actually called but never defined */
	    if ((ast = function->ast)) {
		oeval_ast(ast->l.ast);
		eval_ast_declexpr(ast->r.ast->r.ast);
		oeval_ast(ast->c.ast);
		odata(function->ast->c.ast);
	    }
	}
    }
    current_function = root_record->function;
    current_record = root_record;
}

static void
eval_ast_stat(oast_t *ast)
{
    for (; ast; ast = ast->next)
	oeval_ast(ast);
}

static void
eval_ast_decl(oast_t *decl)
{
    oast_t		*ast;
    oast_t		*list;
    osymbol_t		*symbol;
    ovector_t		*vector;

    for (list = decl; list; list = list->next) {
	ast = list;
	switch (ast->token) {
	    case tok_symbol:
		break;
	    case tok_vecdcl:
		if (ast->r.ast)
		    oeval_ast(ast->r.ast);
		goto eval_left;
	    case tok_proto:
		if (ast->r.ast)
		    oeval_ast(ast->r.ast);
		goto eval_left;
	    case tok_set:
		oeval_ast(ast->r.ast);
	    eval_left:
		for (ast = ast->l.ast;
		     ast->token != tok_symbol; ast = ast->l.ast)
		    ;
		break;
	    default:
		abort();
	}

	/* make symbol bound */
	symbol = ast->l.value;
	vector = symbol->name;
	symbol = oget_symbol(current_record, vector);
	assert(symbol);
	ast->l.value = symbol;
	symbol->bound = true;

	/* remember symbol is referenced */
	update_symbol(ast, false);
    }
}

static void
eval_ast_declexpr(oast_t *decl)
{
    oast_t		*list;

    for (list = decl; list; list = list->next) {
	if (list->token == tok_ellipsis) {
	    assert(list->next == null);
	    break;
	}
	assert(list->token == tok_declexpr);
	eval_ast_decl(list->r.ast);
    }
}

static void
eval_ast_goto(oast_t *ast)
{
    obool_t		 fail;
    olabel_t		*label;
    oentry_t		*entry;
    osymbol_t		*symbol;
    char		*opcode;
    char		*string;

    symbol = ast->l.ast->l.value;
    entry = oget_hash(current_function->labels, symbol->name);
    if (entry == null)
	oparse_error(ast, "undefined label '%p'", symbol->name);
    /*   Use ast->c.ast to match other break, case, continue and default. */
    ast->c.value = label = entry->value;

    fail = false;
    if (ast->t.ast != label->ast->t.ast) {
	fail = true;
	if (ast->t.ast) {
	    string = "leaves";
	    if (ast->t.ast->token == tok_try)
		opcode = "try";
	    else if (ast->t.ast->token == tok_catch)
		opcode = "catch";
	    else {
		assert(ast->t.ast->token == tok_finally);
		opcode = "finally";
	    }
	}
	else {
	    assert(label->ast->t.ast);
	    string = "enters";
	    if (label->ast->t.ast->token == tok_try)
		opcode = "try";
	    else if (label->ast->t.ast->token == tok_catch)
		opcode = "catch";
	    else {
		assert(label->ast->t.ast->token == tok_finally);
		opcode = "finally";
	    }
	}
    }
    if (fail)
	oparse_error(ast, "goto %s '%s' block", string, opcode);
}

static void
eval_ast_sizeof(oast_t *ast)
{
    oword_t		 size;
    ovector_t		*vector;

    oeval_ast(ast->l.ast);
    switch (ast->l.ast->token) {
	case tok_number:
	    size = 0;
	    break;
	case tok_string:
	    vector = ast->l.ast->l.value;
	    size = vector->length;
	    break;
	default:
	    return;
    }
    odel_object(&ast->l.value);
    ast->token = tok_number;
    onew_word(&ast->l.value, size);
}

static osymbol_t *
ast_call_symbol(oast_t *ast)
{
    osymbol_t		*name;
    osymbol_t		*symbol;

    assert(ast->token == tok_call);
    if (ast->l.ast->token == tok_dot ||
	ast->l.ast->token == tok_explicit)
	symbol = ast->l.ast->r.ast->l.value;
    else {
	name = ast->l.ast->l.value;
	if ((symbol = oget_bound_symbol(name->name)) == null)
	    oparse_error(ast, "undefined function '%p'", name);
	else if (symbol->tag->type != tag_function)
	    oparse_error(ast, "called object '%p' is not a function",
			 name);
    }

    return (symbol);
}

static void
eval_ast_typeof(oast_t *ast)
{
    otype_t		 type;
    osymbol_t		*symbol;
    ofunction_t		*function;

    oeval_ast(ast->l.ast);
    switch (ast->l.ast->token) {
	case tok_number:
	    type = otype(ast->l.ast->l.value);
	    break;
	case tok_string:
	    type = t_string;
	    break;
	case tok_symbol:
	    symbol = ast->l.ast->l.value;
	    type = otag_to_type(symbol->tag);
	    break;
	case tok_call:
	    symbol = ast_call_symbol(ast->l.ast);
	    function = symbol->value;
	    type = otag_to_type(function->tag) & ~t_function;
	    break;
	case tok_type:
	    type = otag_to_type(ast->l.ast->l.value);
	    break;
	default:
	    return;
    }

    /* Return "normalized" type, not variable type, because
     * otherwise there would be inconsistencies with actual
     * runtime type information for "unsigned word" if it
     * does not fit in a "signed word", as well as being
     * simply tagged as "word" if moved to a vm register */
    switch ((oword_t)type) {
	case t_undef:
	    if (ast->l.ast->token != tok_type)
		return;
	    type = t_void;
	    break;
	case t_void:
	    break;
	case t_int8:			case t_uint8:
	case t_int16:			case t_uint16:
	case t_int32:
#if __WORDSIZE == 64
	case t_uint32:			case t_int64:
#endif
	    type = t_word;
	    break;
#if __WORDSIZE == 32
	case t_uint32:			case t_int64:
#endif
	case t_uint64:
	    if (ast->l.ast->token != tok_type)
		return;
	    type = t_word;
	    break;
	case t_float32:			case t_float64:
	    break;
	case t_vector|t_int8:		case t_vector|t_uint8:
	case t_vector|t_int16:		case t_vector|t_uint16:
	case t_vector|t_int32:		case t_vector|t_uint32:
	case t_vector|t_int64:		case t_vector|t_uint64:
	case t_vector|t_float32:	case t_vector|t_float64:
	    break;
	case t_vector|t_undef:
	    type = t_vector;
	    break;
	default:
	    if (type > rtti_vector->offset)
		return;
	    break;
    }
    odel_object(&ast->l.value);
    ast->token = tok_number;
    onew_word(&ast->l.value, type);
}

static void
eval_ast_renew(oast_t *ast)
{
    otag_t		*tag;

    oeval_ast(ast->l.ast);
    tag = oeval_ast_tag(ast->l.ast);
    if (tag == null || tag->type != tag_vector)
	oparse_error(ast->l.ast, "expecting vector %A", ast->l.ast);
    oeval_ast(ast->r.ast);
    if (ast->r.ast->token == tok_number) {
	if (ast->r.ast->l.value == null || otype(ast->r.ast->l.value) != t_word)
	    oparse_error(ast->r.ast, "expecting integer %A", ast->r.ast);
	if (*(oword_t *)ast->r.ast->l.value < 0)
	    oparse_error(ast->r.ast, "not a positive integer");
    }
}

static void
update_symbol(oast_t *ast, obool_t function)
{
    osymbol_t		*symbol;
    ovector_t		*vector;

    symbol = ast->l.value;
    if (!symbol->bound && symbol->record == (orecord_t *)language_table) {
	vector = symbol->name;
	symbol = oget_bound_symbol(vector);
	if (symbol == null)
	    oparse_error(ast, "unbound symbol '%p'", vector);
	/*   This also happens to error out if using a now shadowed variable. */
	if (!symbol->bound) {
	    if (symbol->function || symbol->method || symbol->builtin) {
		/*  More descriptive error message when attempting to use a
		 * function as a variable. */
		if (function)
		    goto done;
		oparse_error(ast, "symbol '%p' is a function", vector);
	    }
	    oparse_error(ast, "symbol '%p' used before definition", vector);
	}
    done:
	ast->l.value = symbol;
    }
}

static void
check_exception(oast_t *ast)
{
    char	*string;
    char	*opcode;

    if (ast->t.ast) {
	if (ast->t.ast->token == tok_try)
	    opcode = "try";
	else if (ast->t.ast->token == tok_catch)
	    opcode = "catch";
	else {
	    assert(ast->t.ast->token == tok_finally);
	    opcode = "finally";
	}
	if (ast->token == tok_break)
	    string = "break";
	else if (ast->token == tok_continue) {
	    string = "continue";
	}
	else {
	    assert(ast->token == tok_return);
	    string = "return";
	}
	oparse_error(ast, "'%s' leaves '%s' block", string, opcode);
    }
}
