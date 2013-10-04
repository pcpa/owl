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

static void
update_symbol(oast_t *ast);

/*
 * Implementation
 */
void
init_code(void)
{
    current_record = root_record;
}

void
finish_code(void)
{
}

void
ocode(void)
{
    if ((root_record->function->ast->c.ast->l.ast = oparser())) {
	eval_root();
#if 1
	if (cfg_verbose)
	    owrite_object(root_record->function->ast->c.ast->l.ast);
#endif
    }
}

void
oeval_ast(oast_t *ast)
{
    oast_t		*temp;

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
	    oeval_ast(ast->r.ast);
	    /* validate element reference */
	    oeval_ast_tag(ast);
	    break;
	case tok_dot:
	    oeval_ast(ast->l.ast);
	    /* validate field reference */
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
	case tok_elemref:
	    oeval_ast(ast->l.ast);
	    break;
	case tok_com:		case tok_plus:
	case tok_neg:		case tok_not:
	    oeval_ast(ast->l.ast);
	    ofold(ast);
	    break;
	case tok_atan2:		case tok_pow:
	case tok_hypot:		case tok_subtypeof:
	    oeval_ast(ast->l.ast);
	    oeval_ast(ast->r.ast);
	    break;
	case tok_sizeof:
	    eval_ast_sizeof(ast);
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
	    assert(ast->l.ast->token == tok_symbol);
	    eval_ast_stat(ast->r.ast);
	    break;
	case tok_stat:		case tok_code:
	case tok_data:
	    eval_ast_stat(ast->l.ast);
	    break;
	case tok_decl:
	    oeval_ast(ast->l.ast);
	    eval_ast_decl(ast->r.ast);
	    break;
	case tok_symbol:
	    update_symbol(ast);
	    break;
	case tok_goto:
	    eval_ast_goto(ast);
	    break;
	case tok_type:		case tok_number:	case tok_string:
	case tok_class:		case tok_label:		case tok_break:
	case tok_continue:	case tok_case:		case tok_default:
	case tok_function:	case tok_fieldref:
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
    orecord_t		*record;
    osymbol_t		*symbol;
    ovector_t		*vector;

    switch (ast->token) {
	case tok_symbol:
	    update_symbol(ast);
	    symbol = ast->l.value;
	    return (symbol->tag);
	case tok_vector:
	    tag = oeval_ast_tag(ast->l.ast);
	    if (tag->type != tag_vector) {
		if (ast->l.ast->token == tok_symbol) {
		    symbol = ast->l.ast->l.value;
		    oparse_error(ast, "'%p' is not a vector %A",
				 symbol->name, ast);
		}
		oparse_error(ast, "not a vector reference %A", ast);
	    }
	    return (tag->base);
	case tok_dot:
	    tag = oeval_ast_tag(ast->l.ast);
	    if (tag->type != tag_class) {
		if (ast->l.ast->token == tok_symbol) {
		    symbol = ast->l.ast->l.value;
		    oparse_error(ast, "'%p' is not a class %A",
				 symbol->name, ast);
		}
		oparse_error(ast, "not a class reference %A", ast);
	    }
	    record = tag->name;
	    ast = ast->r.ast;
	    assert(ast->token == tok_symbol);
	    symbol = ast->l.value;
	    vector = symbol->name;
	    if ((symbol = oget_symbol(record, vector)) == null)
		oparse_error(ast, "'%p' has no field named '%p'",
			     record->name, vector);
	    ast->l.value = symbol;
	    return (symbol->tag);
	case tok_string:
	    return (string_tag);
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
	    ast = function->ast;
	    oeval_ast(ast->l.ast);
	    eval_ast_declexpr(ast->r.ast->r.ast);
	    oeval_ast(ast->c.ast);
	    odata(function->ast->c.ast);
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
	    case tok_set:	case tok_proto:
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
	update_symbol(ast);
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
    oentry_t		*entry;
    osymbol_t		*symbol;

    symbol = ast->l.ast->l.value;
    entry = oget_hash(current_function->labels, symbol->name);
    if (entry == null)
	oparse_error(ast, "undefined label '%p'", symbol->name);
    /*   Use ast->c.ast to match other break, case, continue and default. */
    ast->c.ast = entry->value;
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

static void
update_symbol(oast_t *ast)
{
    osymbol_t		*symbol;
    ovector_t		*vector;

    symbol = ast->l.value;
    if (!symbol->bound) {
	vector = symbol->name;
	symbol = oget_bound_symbol(vector);
	if (symbol == null)
	    oparse_error(ast, "unbound symbol '%p'", vector);
	/*   This also happens to error out if using a now shadowed variable. */
	if (!symbol->bound)
	    oparse_error(ast, "symbol '%p' used before definition", vector);
	ast->l.value = symbol;
    }
}
