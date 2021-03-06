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

#define reset(offset)		unget(stack->offset - (offset))

/*
 * Prototypes
 */
static void
realize(oast_t *ast);

static void
statement(oast_t *ast);

static void
list(oast_t *ast);

static void
decl(oast_t *ast);

static void
call(oast_t *ast);

static oint32_t
get(void);

static void
unget(oint32_t count);

static oobject_t
literal(oobject_t object);

/*
 * Initialization
 */
static ohash_t		*hash;
static ovector_t	*stack;
static oint32_t		 call_size;
static oint32_t		 call_depth;

/*
 * Implementation
 */
void
init_realize(void)
{
    oadd_root((oobject_t *)&stack);
    onew_vector((oobject_t *)&stack, t_int32, 16);
    oadd_root((oobject_t *)&hash);
    onew_hash((oobject_t *)&hash, 32);
}

void
finish_realize(void)
{
    orem_root((oobject_t *)&stack);
    orem_root((oobject_t *)&hash);
}

void
orealize(void)
{
    ortti_t		*rtti;
    oentry_t		*entry;
    oword_t		 offset;
    orecord_t		*record;
    ofunction_t		*function;

    assert(current_record == root_record);
    assert(current_function == root_record->function);
    statement(root_record->function->ast->c.ast->l.ast);
    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function))) {
	    function = record->function;
	    if (function->ast) {
		current_record = record;
		current_function = function;
		stack->offset = 0;
		memset(stack->v.i32, 0, stack->length * sizeof(oint32_t));
		realize(function->ast->c.ast);
	    }
	}
    }
    current_function = function = root_record->function;
    current_record = root_record;

    /* Realize gc protected global variables storage */
    oend_record(root_record);
    onew_object((oobject_t *)&gd, t_root, root_record->length);

    /* Realize intial stack information */
    thread_main->type = t_root;
    thread_main->frame = THIS_OFFSET;
    thread_main->stack = 0;
    rtti = rtti_vector->v.ptr[t_root];
    rtti->frame = THIS_OFFSET;

    /* Realize gc protected stack frames information */
    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function))) {
	    oend_record(record);
	    rtti = rtti_vector->v.ptr[record->type];
	    record->function->stack = -record->offset;
	    rtti->frame = record->function->frame;
	    rtti->stack = record->function->stack;
	    if (record->function->framesize < record->function->stack)
		record->function->framesize = record->function->stack;
	    if (record->function->varargs)
		rtti->varargs = record->function->varargs;
	}
    }

    /* Move constant pool to runtime gc protected storage */
    onew_vector((oobject_t *)&cv, t_void, hash->count);
    for (offset = 0; offset < hash->size; offset++) {
	for (entry = hash->entries[offset]; entry; entry = entry->next)
	    cv->v.ptr[cv->offset++] = entry->value;
    }

#if 1
    if (cfg_verbose)
	owrite_object(root_record->function->ast->c.ast->l.ast);
#endif
}

/* Quickly simulate jit code generation to figure out how many slots
 * may be required to save expression temporaries, as function vm
 * stack frames and records cannot be resized [1], and record/hash all
 * constants.
 * [1]   Records are stored as direct pointers, not pointers to pointers,
 *     and a resize may need to move the base pointer.
 *       Stack frames need to be fixed size because a valid gc description
 *     information must always be available, and during stack frame
 *     construction it must use its final layout, just that may have
 *     several slots filled with null.
 */
static void
realize(oast_t *ast)
{
    orecord_t		*record;
    osymbol_t		*symbol;

    switch (ast->token) {
	case tok_number:
	    ast->offset = get();
	    switch (otype(ast->l.value)) {
		case t_word:	case t_float:
		    break;
		default:
		    ast->l.value = literal(ast->l.value);
		    break;
	    }
	    break;
	case tok_this:
	    if (current_function->name == null ||
		!current_function->name->method)
		oparse_error(ast, "'this' not allowed here");
	    ast->t.value = current_record->parent;
	    assert(ast->t.value && otype(ast->t.value) == t_record);
	    ast->offset = get();
	    break;
	case tok_symbol:	case tok_ellipsis:
	    ast->offset = get();
	    break;
	case tok_string:
	    ast->offset = get();
	    ast->l.value = literal(ast->l.value);
	    break;
	case tok_data:
	    ast->offset = get();
	    /* gc protect constant initializers */
	    ast->r.value = literal(ast->r.value);
	    /* parse dynamic intializers */
	    statement(ast->l.ast);
	    break;

	case tok_dot:
	    realize(ast->l.ast);
	    ast->offset = ast->l.ast->offset;
	    break;

	case tok_new:
	    ast->offset = get();
	    statement(ast->l.ast);
	    break;
	case tok_vecnew:
	    if (ast->r.ast->token != tok_number)
		realize(ast->r.ast);
	    break;
	case tok_renew:
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    unget(2);
	    break;
	case tok_vector:	case tok_hash:
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    ast->offset = ast->r.ast->offset;
	    break;

	case tok_not:		case tok_com:
	case tok_plus:		case tok_neg:
	case tok_inc:		case tok_dec:
	case tok_sizeof:	case tok_typeof:
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
	case tok_thread:
	    realize(ast->l.ast);
	    ast->offset = ast->l.ast->offset;
	    break;
	case tok_postinc:	case tok_postdec:
	    ast->offset = get();
	    realize(ast->l.ast);
	    break;
	case tok_andand:	case tok_oror:
	case tok_lt:		case tok_le:
	case tok_eq:		case tok_ge:
	case tok_gt:		case tok_ne:
	case tok_and:		case tok_or:
	case tok_xor:
	case tok_mul2:		case tok_div2:
	case tok_shl:		case tok_shr:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_div:
	case tok_trunc2:	case tok_rem:
	case tok_complex:	case tok_atan2:
	case tok_pow:		case tok_hypot:
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    unget(1);
	    ast->offset = ast->l.ast->offset;
	    break;
	case tok_set:
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    ast->offset = ast->l.ast->offset;
	    break;
	case tok_andset:	case tok_orset:
	case tok_xorset:
	case tok_mul2set:	case tok_div2set:
	case tok_shlset:	case tok_shrset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	case tok_trunc2set:	case tok_remset:
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    if (ast->l.ast->token == tok_symbol)
		/* FIXME should also work for records, but for safety on
		 * complex (complicated) lvalues... */
		ast->offset = ast->l.ast->offset;
	    else
		ast->offset = get();
	    break;
	case tok_code:		case tok_stat:
	case tok_finally:
	    statement(ast->l.ast);
	    break;
	case tok_list:
	    list(ast->l.ast);
	    break;
	case tok_init:
	    realize(ast->r.ast);
	    break;
	case tok_decl:
	    decl(ast->r.ast);
	    break;
	case tok_question:
	    realize(ast->t.ast);
	    realize(ast->l.ast);
	    realize(ast->r.ast);
	    ast->offset = ast->l.ast->offset;
	    break;
	case tok_if:
	    statement(ast->t.ast);
	    statement(ast->l.ast);
	    statement(ast->r.ast);
	    break;
	case tok_while:		case tok_do:
	    statement(ast->c.ast);
	    statement(ast->t.ast);
	    break;
	case tok_for:
	    statement(ast->l.ast);
	    statement(ast->t.ast);
	    statement(ast->r.ast);
	    statement(ast->c.ast);
	    break;
	case tok_switch:
	    statement(ast->t.ast);
	    statement(ast->c.ast);
	    break;
	case tok_return:	case tok_throw:
	    if (ast->l.ast)
		realize(ast->l.ast);
	    break;
	case tok_call:
	    call(ast);
	    ast->offset = get();
	    break;
	case tok_try:		case tok_catch:
	    statement(ast->r.ast);
	    break;

	case tok_namespace:
	    record = current_record;
	    symbol = ast->l.ast->l.value;
	    current_record = symbol->value;
	    statement(ast->c.ast);
	    current_record = record;
	    break;

	case tok_break:		case tok_continue:
	case tok_case:		case tok_default:
	case tok_function:	case tok_class:
	case tok_vecdcl:	case tok_label:
	case tok_goto:		case tok_type:
	case tok_enum:
	    break;

	default:
#if DEBUG
	    oparse_warn(ast, "realize: not handling %s",
			otoken_to_charp(ast->token));
#endif
	    break;
    }
}

static void
statement(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    for (; ast; ast = ast->next) {
	realize(ast);
	reset(offset);
    }
}

static void
list(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    while (ast->next) {
	realize(ast);
	reset(offset);
	ast = ast->next;
    }
    realize(ast);
}

static void
decl(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    for (; ast; ast = ast->next) {
	if (ast->token == tok_set) {
	    realize(ast);
	    reset(offset);
	}
    }
}

static void
call(oast_t *ast)
{
    oast_t		*list;
    oast_t		*prev;
    oint32_t		 size;
    obool_t		 vcall;
    obool_t		 ecall;
    oword_t		 offset;
    osymbol_t		*symbol;
    ovector_t		*vector;
    obool_t		 builtin;
    ofunction_t		*function;

    vcall = ast->l.ast->token == tok_dot;
    ecall = ast->l.ast->token == tok_explicit;
    if (vcall) {
	realize(ast->l.ast->l.ast);
	assert(ast->l.ast->r.ast->token == tok_symbol);
	symbol = ast->l.ast->r.ast->l.value;
    }
    else if (ecall) {
	if (ast->t.ast)
	    realize(ast->t.ast);
	assert(ast->l.ast->r.ast->token == tok_symbol);
	symbol = ast->l.ast->r.ast->l.value;
    }
    else {
	assert(ast->l.ast->token == tok_symbol);
	symbol = ast->l.ast->l.value;
	if (symbol->tag->type != tag_function)
	    oparse_error(ast, "called object '%p' is not a function", symbol);
    }

    if (!(builtin = symbol->builtin))
	assert(symbol->function == true);
    function = symbol->value;

    offset = 0;
    vector = function->record->vector;
    for (prev = ast, list = ast->r.ast; offset < vector->offset; offset++) {
	symbol = vector->v.ptr[offset];
	if (!symbol->argument)
	    break;
	if (list == null)
	    oparse_error(prev, "too few arguments to '%p'", function->name);
	prev = list;
	list = list->next;
    }

    for (; list; offset++, list = list->next) {
	if (offset >= vector->length ||
	    (symbol = vector->v.ptr[offset]) == null ||
	    !symbol->argument) {
	    if (function->varargs)
		break;
	    oparse_error(list, "too many arguments to '%p'", function->name);
	}
    }

    ++call_depth;

    size = function->frame;
    for (; list; list = list->next)
	size += sizeof(oobject_t);

    call_size += size;
    assert(call_size >= 0);

    statement(ast->r.ast);

    if (--call_depth == 0) {
	if (!builtin) {
	    call_size += function->stack;
	    assert(call_size >= 0);
	    if (function->framesize < call_size)
		function->framesize = call_size;
	}
    }
    else
	assert(call_depth > 0);
}

static oint32_t
get(void)
{
    oword_t		offset;

    offset = stack->v.i32[stack->offset];
    if (offset == 0) {
	offset = onew_slot(current_record);
	stack->v.i32[stack->offset] = offset;
    }
    if (++stack->offset >= stack->length)
	orenew_vector(stack, stack->length + 16);

    return (offset);
}

static void
unget(oint32_t count)
{
    assert(stack->offset >= count);
    stack->offset -= count;
}

static oobject_t
literal(oobject_t object)
{
    oentry_t		*entry;
    oobject_t		*pointer;

    if (object == null)
	return (object);
    if ((entry = oget_hash(hash, object)) == null) {
	gc_ref(pointer);
	onew(pointer, entry);
	entry = *pointer;
	entry->name = object;
	switch (otype(object)) {
	    case t_word:	case t_float:
	    case t_rat:		case t_mpz:
	    case t_mpq:		case t_mpr:
	    case t_cdd:		case t_cqq:
	    case t_mpc:
		ocopy(&entry->value, object);
		break;
	    default:
		entry->value = object;
	}
	oput_hash(hash, entry);
	gc_dec();
    }

    return (entry->value);
}
