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

#define mask1(a)			 (1 << (a))
#define mask2(a, b)			((1 << (a)) | (1 << (b)))
#define mask3(a, b, c)			((1 << (a)) | (1 << (b)) | (1 << (c)))
#define emit_get_type(op)		((op->t) & 0x0fffffff)
#define emit_set_type(op, ty)		 (op)->t = ((op->t) & 0xf0000000) | ty
#define emit_set_mode(op, md)		 (op)->t = ((op->t) & 0x0fffffff) | md
#define add_t_jump(node)		add_jump(jump_top()->t, node)
#define add_f_jump(node)		add_jump(jump_top()->f, node)
#define operand_copy(lop, rop)		memcpy(lop, rop, sizeof(ooperand_t))
#define gc_offset(op)			(op)->s + 8
#define wf_offset(op)			(op)->s
/* To be used when a pure temporary is required, but that may
 * need to be live after an ovm_* call */
#define protect_tmp(N)							\
    do {								\
	assert(!(tmp_mask & (1 << (N))));				\
	tmp_mask |= 1 << (N);						\
    } while (0);
#define release_tmp(N)							\
    do {								\
	assert(tmp_mask & (1 << (N)));					\
	tmp_mask &= ~(1 << (N));					\
    } while (0)

/* Avoid double spill, usually with the second spill being
 * done with the wrong value to be restored */
#define protect_reg(N)							\
    do {								\
	assert(!(reg_mask & (1 << (N))));				\
	reg_mask |= 1 << (N);						\
    } while (0);
#define release_reg(N)							\
    do {								\
	assert(reg_mask & (1 << (N)));					\
	reg_mask &= ~(1 << (N));					\
    } while (0)

/*
 * Prototypes
 */
static void
resolve_methods(orecord_t *record);

static ooperand_t *
operand_get(oword_t offset);

static ooperand_t *
operand_top(void);

static void
operand_reset(oword_t offset);

static void
operand_unget(oword_t count);

static ojump_t *
jump_get(otoken_t token);

static ojump_t *
jump_top(void);

static void
jump_unget(oword_t count);

static void
add_jump(ovector_t *vector, jit_node_t *node);

static otoken_t
get_token(oast_t *ast);

static void
emit(oast_t *ast);

static void
emit_note(onote_t *n);

static void
emit_stat(oast_t *ast);

static void
emit_list(oast_t *ast);

static void
emit_decl(oast_t *ast);

static void
emit_set(oast_t *ast);

static void
emit_sizeof(oast_t *ast);

static void
emit_typeof(oast_t *ast);

static void
emit_new(oast_t *ast);

static void
emit_data(oast_t *ast);

static void
data_record(oast_t *ast);

static void
data_vector(oast_t *ast);

static void
emit_symbol(oast_t *last, oast_t *ast, oast_t *rast);

static void
emit_record(oast_t *last, oast_t *ast, oast_t *rast);

static void
emit_vector(oast_t *last, oast_t *ast, oast_t *rast);

static void
emit_vararg(oast_t *last, oast_t *ast, oast_t *rast);

static void
emit_label(oast_t *ast);

static void
emit_goto(oast_t *ast);

#define PROTO
#  include "oemit_unary.c"
#  include "oemit_binary.c"
#  include "oemit_cond.c"
#  include "oemit_except.c"
#  include "oemit_flow.c"
#undef PROTO

static void
loadi(ooperand_t *op, otype_t type, jit_int32_t base, oword_t offset);

static void
loadr(ooperand_t *op, otype_t type, jit_int32_t base, jit_int32_t offset);

static void
storei(ooperand_t *op, otype_t type, jit_int32_t base, oword_t offset);

static void
storer(ooperand_t *op, otype_t type, jit_int32_t base, jit_int32_t offset);

static void
emit_coerce(otype_t type, ooperand_t *lop, ooperand_t *rop);

static void
emit_load(ooperand_t *op);

static void
load_symbol(osymbol_t *symbol, ooperand_t *op);

static void
store_symbol(osymbol_t *symbol, ooperand_t *op);

static void
load_record(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop);

static void
store_record(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop);

static void
load_vector(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop);

static void
store_vector(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop);

static void
load_vararg(ooperand_t *lop, ooperand_t *rop);

static void
store_vararg(ooperand_t *lop, ooperand_t *rop);

static void
emit_reload(ooperand_t *op, obool_t same);

static void
emit_save(ooperand_t *op, obool_t force);

static void
load_w(oword_t regno);

static void
load_w_w(oword_t regno, oword_t regval);

static void
loadif_w(oword_t regno);

static void
load_d(oword_t regno);

static void
loadif_d(oword_t regno);

static void
loadif_f(oword_t regno);

static void
load_r(oword_t regno);

static void
load_r_w(oword_t regno, oword_t regval);

#if __WORDSIZE == 32
static void
sync_ww(oword_t regno);

static void
sync_uwuw(oword_t regno);
#endif

static void
sync_w(oword_t regno);

static void
spill_w(oword_t regno);

static void
sync_w_w(oword_t regno, oword_t regval);

static void
sync_d(oword_t regno);

static void
spill_d(oword_t regno);

static void
sync_d_w(oword_t regno, oword_t regval);

static void
sync_uw(oword_t regno);

static void
sync_uw_z(oword_t regno);

static void
sync_r(oword_t regno, oint32_t type);

static void
sync_r_w(oword_t regno, oword_t regval, oint32_t type);

static oword_t
get_register(obool_t spill);

static void
emit_finish(oobject_t code, oword_t mask);

/* There is not an emit_reload_operands call because saved
 * operands are reloaded on demand. */
static void
emit_save_operands(void);

/* The mask argument is the mask of values that can be overwritten. */
static oword_t
sync_registers(oword_t mask);

static void
load_registers(oword_t mask);

/*
 * Initialization
 */
oint8_t			*jit_code_start;
oint8_t			*jit_code_end;

static onote_t		 note;
static oint32_t		 depth;
static ovector_t	*stack;
static ovector_t	*branch;
static jit_int32_t	 scratch;
static jit_int32_t	 GPR[10];
static jit_int32_t	 FPR[4];
static jit_int32_t	 SPL[6];
/* FIXME a bit ugly usage but good to avoid double spills at least
 * in {load/store}_vector */
static jit_word_t	 tmp_mask;
static jit_word_t	 reg_mask;

static char		*exceptions[] = {
    "nothing",
    "out of memory",
    "floating point error",
    "out of bounds",
    "null dereference",
    "type mismatch",
    "input output error",
    "invalid argument",
    "not a number",
    "not an integer",
    "not a real number",
    "not a rational number",
    "not a 32 bits integer",
    "not a finite real number",
    "shift too large",
};

/*
 * Implementation
 */
void
init_emit(void)
{
    oadd_root((oobject_t *)&stack);
    onew_vector((oobject_t *)&stack, t_void, 16);
    oadd_root((oobject_t *)&branch);
    onew_vector((oobject_t *)&branch, t_void, 16);

    GPR[0] = JIT_V1;
    GPR[1] = JIT_V2;
    GPR[2] = JIT_R1;
    GPR[3] = JIT_R2;

#define TMP0			4
    GPR[TMP0] = JIT_NOREG;
#  if defined(JIT_R3)
    if (!jit_callee_save_p(JIT_R3))
	GPR[TMP0] = JIT_R3;
#  endif
#define TMP1			5
    GPR[TMP1] = JIT_NOREG;
#  if defined(JIT_R4)
    if (!jit_callee_save_p(JIT_R4))
	GPR[TMP1] = JIT_R4;
#  endif

#define FRAME			6
    GPR[FRAME] = JIT_NOREG;
#if defined(JIT_V3)
    GPR[FRAME] = JIT_V3;
#endif

#define GLOBAL			7
    GPR[GLOBAL] = JIT_NOREG;
#if defined(JIT_V4)
    GPR[GLOBAL] = JIT_V4;
#else
#  if defined(JIT_R3)
    if (jit_callee_save_p(JIT_R3))
	GPR[GLOBAL] = JIT_R3;
#  endif
#endif

#define THIS			8
    GPR[THIS] = JIT_NOREG;
#if defined(JIT_V5)
    GPR[THIS] = JIT_V5;
#else
#  if defined(JIT_R4)
    if (jit_callee_save_p(JIT_R4))
	GPR[THIS] = JIT_R4;
#  endif
#endif

#define STACK			9
    GPR[STACK] = JIT_NOREG;
#if defined(JIT_V6)
    GPR[STACK] = JIT_V6;
#else
#  if defined(JIT_R5)
    if (jit_callee_save_p(JIT_R5))
	GPR[STACK] = JIT_R5;
#  endif
#endif

    FPR[0] = JIT_F1;
    FPR[1] = JIT_F2;
    FPR[2] = JIT_F3;
    FPR[3] = JIT_F4;
}

void
finish_emit(void)
{
    orem_root((oobject_t *)&stack);
    orem_root((oobject_t *)&branch);
}

void
oemit(void)
{
    oint32_t		 offset;
    orecord_t		*record;
    jit_node_t		*thread;
    jit_node_t		*execute;
    jit_node_t		*handler;
    jit_node_t		*reenter;
    jit_node_t		*exception;

    assert(current_record == root_record);
    assert(current_function == root_record->function);

    jit_prolog();

    thread = jit_arg();
    jit_getarg(JIT_V0, thread);

    /* Fast access small scratch buffer */
    scratch = jit_allocai(8);

    for (offset = 0; offset < 6; offset++) {
	if (GPR[offset] != JIT_NOREG)
	    SPL[offset] = jit_allocai(sizeof(jit_word_t));
    }

    /* Initialize before any exception handling */
    if (GPR[GLOBAL] != JIT_NOREG)
	jit_movi(GPR[GLOBAL], (oword_t)gd);

    /* Call sigsetjmp when starting thread to set exception handler */
    jit_prepare();
    jit_addi(JIT_R0, JIT_V0, offsetof(othread_t, env));
    jit_pushargr(JIT_R0);
    jit_pushargi(1);
#  if defined(__linux__)
    jit_finishi(__sigsetjmp);
#  else
    jit_finishi(sigsetjmp);
#  endif

    jit_retval_i(JIT_R0);

    /* If sigsetjmp returns non zero, it is reentering from exception */
    exception = jit_bnei(JIT_R0, 0);

    /* Unlock threads */
    jit_prepare();
    jit_pushargi((oword_t)&othread_mutex);
    jit_finishi(pthread_mutex_unlock);

    /* If not coming from exception */
    execute = jit_jmpi();

    /* Handle exception */
    jit_patch(exception);
    jit_ldxi_i(JIT_R1, JIT_V0, offsetof(othread_t, xcpt));

    /* Try to call handler if not a SIGSEGV */
    handler = jit_bnei(JIT_R1, except_segmentation_violation);

    /* Cannot handle exception */
    jit_prepare();
    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ip));
    jit_pushargr(JIT_R0);
    jit_pushargi((oword_t)"segmentation violation");
    jit_finishi(oabort);

    jit_patch(handler);

    /* Get pointer to exception handler (GPR[1] must be callee save) */
    jit_ldxi(GPR[1], JIT_V0, offsetof(othread_t, ex));
    /* Prepare to reenter if there is a handler */
    reenter = jit_bnei(GPR[1], 0);

    exception = jit_bgei_u(JIT_R1, except_unhandled_exception);
    jit_prepare();
    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ip));
    jit_pushargr(JIT_R0);
    jit_muli(JIT_R0, JIT_R1, sizeof(char *));
    jit_movi(JIT_R2, (oword_t)exceptions);
    jit_ldxr(JIT_R0, JIT_R2, JIT_R0);
    jit_pushargr(JIT_R0);
    jit_finishi(oabort);
    /* Maybe exception on exception due to invalid exception number... */
    jit_patch(exception);
    jit_prepare();
    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ip));
    jit_pushargr(JIT_R0);
    jit_pushargi((oword_t)"unknown");
    jit_finishi(oabort);

    /* Attempt to reenter from vm exception */
    jit_patch(reenter);
    /* ovm_raise sets r0.t == t_word and r0.v.w == exception_value */
    load_r(0);
    jit_addi(JIT_R0, JIT_V0, offsetof(othread_t, ev));
    jit_prepare();
    jit_pushargr(GPR[0]);
    jit_pushargr(JIT_R0);
    jit_pushargi(t_void);
    jit_finishi(ovm_store);
    /* Jump to try catch type switch */
    jit_ldxi(JIT_R0, GPR[1], offsetof(oexception_t, ip));
    jit_jmpr(JIT_R0);

    /* Not reentering from exception, dispatch to thread start */
    jit_patch(execute);

    /* Jump to thread code start */
    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ip));
    jit_jmpr(JIT_R0);

    /* First pass, create the symbolic function entry point */
    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function)))
	    record->function->patch = jit_forward();
    }

    /* Secod pass, actually generate jit */
    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function))) {
	    if (record->function->ast)
		emit_function(record->function);
	}
    }
    current_function = root_record->function;
    current_record = root_record;

    /* Root function start point */
    current_function->address = jit_name("");
    if (GPR[FRAME] != JIT_NOREG)
	jit_ldxi(GPR[FRAME], JIT_V0, offsetof(othread_t, fp));
    if (GPR[STACK] != JIT_NOREG)
	jit_ldxi(GPR[STACK], JIT_V0, offsetof(othread_t, sp));
    emit_stat(root_record->function->ast->c.ast->l.ast);

    jit_ret();
    jit_epilog();

    jit_main = jit_emit();

    /* Third pass, resolve functions addresses */
    for (offset = type_vector->offset - 1; offset > t_root; --offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_prototype &&
	    likely(record->function && ofunction_p(record->function))) {
	    if (record->function->address)
		record->function->address =
		    jit_address(record->function->address);
	}
    }

    /* Fourth pass, resolve virtual methods addresses */
    for (offset = t_root + 1; offset < type_vector->offset; ++offset) {
	record = type_vector->v.ptr[offset];
	if (otype(record) == t_record)
	    resolve_methods(record);
    }

    current_function->address = jit_address(current_function->address);
    thread_main->ip = current_function->address;
}

/* *MUST* be called from lower to higher address so that super methods are
 * already in place when resolving virtual methods of a derived class */
static void
resolve_methods(orecord_t *record)
{
    ortti_t		*rtti;
    ortti_t		*stti;
    oword_t		 offset;
    ofunction_t		*function;

    rtti = rtti_vector->v.ptr[record->type];
    /* Initialize to superclass methods, if any */
    if (record->super) {
	stti = rtti_vector->v.ptr[record->super->type];
	memcpy(rtti->mdinfo, stti->mdinfo, stti->mdsize * sizeof(oobject_t));
    }
    /* Overwrite pointers to redefined methods, if any */
    for (offset = 0; offset < record->methods->size; offset++) {
	function = (ofunction_t *)record->methods->entries[offset];
	for (; function; function = function->next)
	    rtti->mdinfo[function->name->offset] = function->address;
    }
}

static ooperand_t *
operand_get(oword_t offset)
{
    ooperand_t		*op;

    if (stack->offset >= stack->length)
	orenew_vector(stack, stack->length + 16);
    if ((op = stack->v.ptr[stack->offset]) == null) {
	onew_object(stack->v.ptr + stack->offset,
		    t_operand, sizeof(ooperand_t));
	op = stack->v.ptr[stack->offset];
    }
    else
	memset(op, 0, sizeof(ooperand_t));
    op->s = offset;
    ++stack->offset;

    return (op);
}

static ooperand_t *
operand_top(void)
{
    assert(stack->offset > 0);
    return (stack->v.ptr[stack->offset - 1]);
}

static void
operand_reset(oword_t offset)
{
    operand_unget(stack->offset - offset);
}

static void
operand_unget(oword_t count)
{
    assert(stack->offset >= count);
    stack->offset -= count;
}

static ojump_t *
jump_get(otoken_t token)
{
    ojump_t		*jump;

    if (branch->offset >= branch->length)
	orenew_vector(branch, branch->length + 16);
    if ((jump = branch->v.ptr[branch->offset]) == null) {
	onew_object(branch->v.ptr + branch->offset, t_jump, sizeof(ojump_t));
	jump = branch->v.ptr[branch->offset];
    }
    ++branch->offset;
    jump->k = token;
    if (jump->t == null)
	onew_vector((oobject_t *)&jump->t, t_word, 4);
    else
	jump->t->offset = 0;
    if (jump->f == null)
	onew_vector((oobject_t *)&jump->f, t_word, 4);
    else
	jump->f->offset = 0;

    return (jump);
}

static ojump_t *
jump_top(void)
{
    assert(branch->offset > 0);
    return (branch->v.ptr[branch->offset - 1]);
}

static void
jump_unget(oword_t count)
{
#if DEBUG
    ojump_t		*jump;

    assert(branch->offset >= count);
    while (count--) {
	jump = branch->v.ptr[--branch->offset];
	assert(jump->t->offset == 0);
	assert(jump->f->offset == 0);
    }
#else
    branch->offset -= count;
#endif
}

static void
add_jump(ovector_t *vector, jit_node_t *node)
{
    if (vector->offset >= vector->length)
	orenew_vector(vector, vector->length + 4);
    vector->v.w[vector->offset++] = (oword_t)node;
}

static otoken_t
get_token(oast_t *ast)
{
    switch (ast->token) {
	case tok_not:		case tok_com:
	case tok_plus:		case tok_neg:
	case tok_integer_p:	case tok_rational_p:
	case tok_float_p:	case tok_real_p:
	case tok_complex_p:	case tok_number_p:
	case tok_finite_p:	case tok_inf_p:
	case tok_nan_p:		case tok_num:
	case tok_den:		case tok_real:
	case tok_imag:		case tok_signbit:
	case tok_abs:		case tok_andand:
	case tok_oror:		case tok_lt:
	case tok_le:		case tok_eq:
	case tok_ge:		case tok_gt:
	case tok_ne:		case tok_and:
	case tok_or:		case tok_xor:
	case tok_shl:		case tok_shr:
	case tok_mul2:		case tok_div2:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_div:
	case tok_trunc2:	case tok_rem:
	    return (ast->token);
	case tok_andset:	case tok_orset:
	case tok_xorset:
	case tok_mul2set:
	case tok_div2set:
	case tok_shlset:
	case tok_shrset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	case tok_trunc2set:	case tok_remset:
	    return ((otoken_t)(ast->token + (tok_and - tok_andset)));
	default:
	    return (tok_none);
    }
}

static void
emit(oast_t *ast)
{
    ooperand_t		*op;

    if (ast->note.name != note.name || ast->note.lineno != note.lineno)
	emit_note(&ast->note);

    switch (ast->token) {
	case tok_number:
	    op = operand_get(ast->offset);
	    switch (otype(ast->l.value)) {
		case t_word:
		    op->u.w = *(oword_t *)ast->l.value;
#if __WORDSIZE == 32
		    if ((oint16_t)op->u.w == op->u.w)
			op->t = t_half;
#else
		    if ((oint32_t)op->u.w == op->u.w)
			op->t = t_half;
#endif
		    else
			op->t = t_word;
		    op->k = word_tag;
		    break;
		case t_float:
		    op->t = t_float;
		    op->u.d = *(ofloat_t *)ast->l.value;
		    op->k = float_tag;
		    break;
		default:
		    op->t = t_undef;
		    op->u.o = ast->l.value;
		    op->k = otag_object(ast->l.value);
		    break;
	    }
	    break;
	case tok_symbol:
	    op = operand_get(ast->offset);
	    op->t = t_symbol;
	    op->u.o = ast->l.value;
	    op->k = ((osymbol_t *)op->u.o)->tag;
	    break;
	case tok_string:
	    op = operand_get(ast->offset);
	    op->t = t_undef;
	    op->u.o = ast->l.value;
	    op->k = string_tag;
	    break;
	case tok_sizeof:
	    emit_sizeof(ast);
	    break;
	case tok_typeof:
	    emit_typeof(ast);
	    break;
	case tok_new:
	    emit_new(ast);
	    break;
	case tok_data:
	    emit_data(ast);
	    break;
	case tok_dot:
	    emit_record(ast, null, null);
	    break;
	case tok_vector:
	    if (ast->l.ast->token == tok_ellipsis)
		emit_vararg(ast, null, null);
	    else
		emit_vector(ast, null, null);
	    break;
	case tok_not:		case tok_com:
	case tok_neg:		case tok_plus:
	case tok_integer_p:	case tok_rational_p:
	case tok_float_p:	case tok_real_p:
	case tok_complex_p:	case tok_number_p:
	case tok_finite_p:	case tok_inf_p:
	case tok_nan_p:		case tok_num:
	case tok_den:		case tok_real:
	case tok_imag:		case tok_signbit:
	case tok_abs:
	    emit_unary(ast);
	    break;
	case tok_inc:		case tok_dec:
	case tok_postinc: 	case tok_postdec:
	    emit_inc_dec(ast);
	    break;
	case tok_andand:	case tok_oror:
	    emit_cond(ast);
	    break;
	case tok_lt:		case tok_le:
	case tok_eq:		case tok_ge:
	case tok_gt:		case tok_ne:
	    emit_cmp(ast);
	    break;
	case tok_and:		case tok_or:
	case tok_xor:
	case tok_mul2:		case tok_div2:
	case tok_shl:		case tok_shr:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_div:
	case tok_trunc2:	case tok_rem:
	    emit_binary(ast);
	    break;
	case tok_set:
	case tok_andset:	case tok_orset:
	case tok_xorset:
	case tok_mul2set:	case tok_div2set:
	case tok_shlset:	case tok_shrset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	case tok_trunc2set:	case tok_remset:
	    emit_set(ast);
	    break;
	case tok_code:		case tok_stat:
	    emit_stat(ast->l.ast);
	    break;
	case tok_list:
	    emit_list(ast->l.ast);
	    break;
	case tok_decl:
	    emit_decl(ast->r.ast);
	    break;
	case tok_question:
	    emit_question(ast);
	    break;
	case tok_if:
	    emit_if(ast);
	    break;
	case tok_while:
	    emit_while(ast);
	    break;
	case tok_do:
	    emit_do(ast);
	    break;
	case tok_for:
	    emit_for(ast);
	    break;
	case tok_break:
	    emit_break(ast);
	    break;
	case tok_continue:
	    emit_continue(ast);
	    break;
	case tok_switch:
	    emit_switch(ast);
	    break;
	case tok_case:		case tok_default:
	    emit_case(ast);
	    break;
	case tok_return:
	    emit_return(ast);
	    break;
	case tok_call:
	    emit_call(ast);
	    break;
	case tok_label:
	    emit_label(ast);
	    break;
	case tok_goto:
	    emit_goto(ast);
	    break;
	case tok_try:
	    emit_try(ast);
	    break;
	case tok_throw:
	    emit_throw(ast);
	    break;
	case tok_function:	case tok_class:
	case tok_catch:		case tok_finally:
	    break;
	default:
#if DEBUG
	    oparse_warn(ast,
			"emit: not handling %s", otoken_to_charp(ast->token));
#endif
	    break;
    }
}

static void
emit_note(onote_t *n)
{
    oword_t		length;
    char		name[1024];

    if ((length = n->name->length) >= sizeof(name) - 1)
	length = sizeof(name) - 1;
    memcpy(name, n->name->v.obj, length);
    name[length] = '\0';
    note.name = n->name;
    note.lineno = n->lineno;
    jit_note(name, note.lineno);
}

static void
emit_stat(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    for (; ast; ast = ast->next) {
	emit(ast);
	operand_reset(offset);
    }
}

static void
emit_list(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    if (ast) {
	for (; ast->next; ast = ast->next) {
	    emit(ast);
	    operand_reset(offset);
	}
	emit(ast);
    }
}

static void
emit_decl(oast_t *ast)
{
    oword_t		offset;

    offset = stack->offset;
    for (; ast; ast = ast->next) {
	if (ast->token == tok_set) {
	    emit_set(ast);
	    operand_reset(offset);
	}
    }
}

static void
emit_set(oast_t *ast)
{
    oast_t		*last;

    switch (ast->l.ast->token) {
	case tok_symbol:
	    emit_symbol(ast->l.ast, ast, ast->r.ast);
	    break;
	case tok_dot:
	    emit_record(ast->l.ast, ast, ast->r.ast);
	    break;
	case tok_vector:
	    if (ast->l.ast->l.ast->token == tok_ellipsis)
		emit_vararg(ast->l.ast, ast, ast->r.ast);
	    else
		emit_vector(ast->l.ast, ast, ast->r.ast);
	    break;
	case tok_vecdcl:
	    for (last = ast->l.ast;
		 last->token == tok_vecdcl;
		 last = last->l.ast)
		;
	    emit_symbol(last, ast, ast->r.ast);
	    break;
	default:
	    abort();
    }
}

static void
emit_sizeof(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jvec;
    jit_node_t		*jump;

    if (ast->l.ast->token == tok_ellipsis) {
	if (!current_function->varargs)
	    oparse_error(ast->l.ast, "function is not varargs");
	op = operand_get(ast->l.ast->offset);
	op->u.w = get_register(true);
	if (GPR[FRAME] != JIT_NOREG)
	    jit_ldxi_i(GPR[op->u.w], GPR[FRAME], SIZE_OFFSET);
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    jit_ldxi_i(GPR[op->u.w], JIT_R0, SIZE_OFFSET);
	}
#if __WORDSIZE == 32
	jit_rshi(GPR[op->u.w], GPR[op->u.w], 2);
#else
	jit_rshi(GPR[op->u.w], GPR[op->u.w], 3);
#endif
    }
    else {
	emit(ast->l.ast);
	op = operand_top();
	emit_load(op);
	assert(emit_get_type(op) == t_void);
	load_r(op->u.w);
	jit_ldxi_i(JIT_R0, GPR[op->u.w], offsetof(oregister_t, t));
	/* if not a vector object */
	jvec = jit_bmci(JIT_R0, t_vector);
	jit_ldxi(JIT_R0, GPR[op->u.w], offsetof(oregister_t, v.o));
	jit_ldxi(GPR[op->u.w], JIT_R0, offsetof(ovector_t, length));
	jump = jit_jmpi();
	jit_patch(jvec);
	jit_movi(GPR[op->u.w], 0);
	jit_patch(jump);
    }
    op->t = t_register|t_word;
}

static void
emit_typeof(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 offset;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    switch (emit_get_type(op)) {
	case t_half:	case t_word:
	    jit_movi(GPR[op->u.w], t_word);
	    break;
	case t_single:
	    jit_movi(GPR[op->u.w], t_float32);
	    break;
	case t_float:
	    jit_movi(GPR[op->u.w], t_float64);
	    break;
	default:
	    switch (op->u.w) {
		case 0:		offset = offsetof(othread_t, r0);	break;
		case 1:		offset = offsetof(othread_t, r1);	break;
		case 2:		offset = offsetof(othread_t, r2);	break;
		case 3:		offset = offsetof(othread_t, r3);	break;
		default:	abort();
	    }
	    jit_ldxi_i(GPR[op->u.w], JIT_V0, offset + offsetof(oregister_t, t));
	    break;
    }
    emit_set_type(op, t_word);
}

static void
emit_new(oast_t *ast)
{
    ooperand_t		*bop;
    ooperand_t		*rop;
    otype_t		 type;
    orecord_t		*record;

    bop = operand_get(ast->offset);
    bop->t = t_void|t_register;
    /* new class */
    if (ast->l.ast->token == tok_type)
	bop->k = ast->l.ast->l.value;
    /* new vector[...] */
    else
	bop->k = ast->r.value;
    assert(otag_p(bop->k));
    type = otag_to_type(bop->k);
    bop->u.w = get_register(true);
    if (current_record == root_record) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[GLOBAL], gc_offset(bop));
	else {
	    jit_movi(GPR[bop->u.w], (oword_t)gd + gc_offset(bop));
	}
    }
    else {
	if (GPR[STACK] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[STACK], gc_offset(bop));
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    jit_addi(GPR[bop->u.w], JIT_R0, + gc_offset(bop));
	}
    }
    /* new class */
    if (bop->k->type == tag_class) {
	record = bop->k->name;
	assert(orecord_p(record));
	jit_prepare();
	jit_pushargr(GPR[bop->u.w]);
	jit_pushargi(type);
	jit_pushargi(record->length);
	emit_finish(onew_object, 0);
    }
    /* new vector[immediate] */
    else if (bop->k->size) {
	jit_prepare();
	jit_pushargr(GPR[bop->u.w]);
	jit_pushargi(type & ~t_vector);
	jit_pushargi(type);
	jit_pushargi(bop->k->size);
	jit_pushargi(sizeof(ovector_t));
	emit_finish(onew_vector_base, 0);
    }
    /* new vector[variable] */
    else {
	emit(ast->l.ast->r.ast);
	rop = operand_top();
	emit_load(rop);
	switch (emit_get_type(rop)) {
	    case t_half:	case t_word:
		break;
	    case t_single:
		jit_extr_f_d(FPR[rop->u.w], FPR[rop->u.w]);
	    case t_float:
		sync_d(rop->u.w);
	    case t_void:
		load_r(rop->u.w);
		jit_prepare();
		jit_pushargr(GPR[rop->u.w]);
		emit_finish(ovm_offset, mask1(rop->u.w));
		load_w(rop->u.w);
		emit_set_type(rop, t_word);
		break;
	    default:
		abort();
	}
	jit_prepare();
	jit_pushargr(GPR[bop->u.w]);
	jit_pushargi(type & ~t_vector);
	jit_pushargi(type);
	jit_pushargr(GPR[rop->u.w]);
	jit_pushargi(sizeof(ovector_t));
	emit_finish(onew_vector_base, 0);
	operand_unget(1);
    }
    jit_ldr(GPR[bop->u.w], GPR[bop->u.w]);
    sync_r(bop->u.w, type);
}

static void
emit_data(oast_t *ast)
{
    if (otype(ast->r.ast) & t_vector)
	data_vector(ast);
    else
	data_record(ast);
}

static void
data_record(oast_t *ast)
{
    ooperand_t		*bop;
    ooperand_t		*lop;
    ooperand_t		*rop;
    oast_t		*ref;
    ofunction_t		*ctor;
    otype_t		 type;
    oword_t		 index;
    oword_t		 offset;
    orecord_t		*record;
    osymbol_t		*symbol;
    ovector_t		*vector;

    type = otype(ast->r.value);
    record = type_vector->v.ptr[type];
    bop = operand_get(ast->offset);
    bop->t = t_void|t_register;
    bop->u.w = get_register(true);
    bop->k = ast->t.value;
    if (current_record == root_record) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[GLOBAL], gc_offset(bop));
	else {
	    jit_movi(GPR[bop->u.w], (oword_t)gd + gc_offset(bop));
	}
    }
    else {
	if (GPR[STACK] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[STACK], gc_offset(bop));
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    jit_addi(GPR[bop->u.w], JIT_R0, + gc_offset(bop));
	}
    }
    jit_prepare();
    jit_pushargr(GPR[bop->u.w]);
    jit_pushargi(record->type);
    jit_pushargi(record->length);
    emit_finish(onew_object, 0);
    jit_ldr(GPR[bop->u.w], GPR[bop->u.w]);
    sync_r(bop->u.w, type);
    jit_prepare();
    jit_pushargr(GPR[bop->u.w]);
    jit_pushargi((oword_t)ast->r.value);
    jit_pushargi(record->length);
    jit_finishi(memcpy);
    vector = record->vector;
    offset = 0;
    ast = ast->l.ast;
    for (; ast; offset++) {
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
	    ref = ast->r.ast;
	}
	else {
	    assert(offset < vector->offset);
	    symbol = vector->v.ptr[offset];
	    if (!symbol->field)
		continue;
	    ref = ast;
	}
	if (ref->token != tok_number) {
	    lop = operand_get(ref->offset);
	    lop->t = t_symbol;
	    lop->u.o = symbol;
	    emit(ref);
	    rop = operand_top();
	    store_record(bop, lop, rop);
	    operand_unget(2);
	}
	ast = ast->next;
    }

    /* Constructor may override any static initialization of use it as input */
    if ((ctor = oget_constructor(record))) {
	lop = operand_get(0);
	operand_copy(lop, bop);
	/* op argument is implicitly unget */
	emit_call_next(null, ctor, null, lop, false, true, false);
    }
}

static void
data_vector(oast_t *ast)
{
    ooperand_t		*bop;
    ooperand_t		*lop;
    ooperand_t		*rop;
    oast_t		*ref;
    otype_t		 type;
    oword_t		 offset;
    ovector_t		*vector;

    vector = ast->r.value;
    type = otype(vector);
    bop = operand_get(ast->offset);
    bop->t = t_void|t_register;
    bop->u.w = get_register(true);
    bop->k = ast->t.value;
    if (current_record == root_record) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[GLOBAL], gc_offset(bop));
	else {
	    jit_movi(GPR[bop->u.w], (oword_t)gd + gc_offset(bop));
	}
    }
    else {
	if (GPR[STACK] != JIT_NOREG)
	    jit_addi(GPR[bop->u.w], GPR[STACK], gc_offset(bop));
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    jit_addi(GPR[bop->u.w], JIT_R0, + gc_offset(bop));
	}
    }
    jit_prepare();
    jit_pushargr(GPR[bop->u.w]);
    jit_pushargi(type & ~t_vector);
    jit_pushargi(type);
    jit_pushargi(vector->length);
    jit_pushargi(sizeof(ovector_t));
    emit_finish(onew_vector_base, 0);
    jit_ldr(GPR[bop->u.w], GPR[bop->u.w]);
    sync_r(bop->u.w, type);
    jit_ldxi(JIT_R0, GPR[bop->u.w], offsetof(ovector_t, v.ptr));
    jit_prepare();
    jit_pushargr(JIT_R0);
    jit_pushargi((oword_t)vector->v.ptr);
    switch (type & ~t_vector) {
	case t_int8:		case t_uint8:
	    jit_pushargi(vector->length);
	    break;
	case t_int16:		case t_uint16:
	    jit_pushargi(vector->length << 1);
	    break;
	case t_int32:		case t_uint32:
	case t_float32:
#if __WORDSIZE == 32
	default:
#endif
	    jit_pushargi(vector->length << 2);
	    break;
	case t_int64:		case t_uint64:
	case t_float64:
#if __WORDSIZE == 64
	default:
#endif
	    jit_pushargi(vector->length << 3);
	    break;
    }
    jit_finishi(memcpy);
    offset = 0;
    for (ast = ast->l.ast; ast; ast = ast->next, offset++) {
	if (ast->token == tok_init) {
	    offset = *(oword_t *)ast->l.ast->l.value;
	    ref = ast->r.ast;
	}
	else
	    ref = ast;
	if (ref->token != tok_number) {
	    assert(offset >= 0 && offset < vector->length);
	    lop = operand_get(ref->offset);
#if __WORDSIZE == 32
	    if ((oint16_t)offset == offset)
		lop->t = t_half;
#else
	    if ((oint32_t)offset == offset)
		lop->t = t_half;
#endif
	    lop->u.w = offset;
	    emit(ref);
	    rop = operand_top();
	    store_vector(bop, lop, rop);
	    operand_unget(2);
	}
    }
}

static void
emit_symbol(oast_t *last, oast_t *ast, oast_t *rast)
{
    ooperand_t		*op;
    osymbol_t		*symbol;

    symbol = last->l.value;
    if (rast) {
	if (get_token(ast)) {
	    emit_binary(ast);
	    op = operand_top();
	    store_symbol(symbol, op);
	}
	else {
	    emit(ast->r.ast);
	    op = operand_top();
	    emit_load(op);
	    store_symbol(symbol, op);
	}
    }
    else {
	emit(last);
	op = operand_top();
	assert(op->t == t_symbol);
	/* Short circuit due to assertion */
	load_symbol(symbol, op);
    }
}

static void
emit_record(oast_t *last, oast_t *ast, oast_t *rast)
{
    ooperand_t		*bop;
    ooperand_t		*lop;
    otoken_t		 tok;
    ooperand_t		*rop;
    orecord_t		*record;
    osymbol_t		*symbol;

    emit(last->l.ast);
    bop = operand_top();
    if (bop->k == null || bop->k->type != tag_class)
	oparse_error(last, "expecting class %A", last->l.ast);
    record = bop->k->name;
    lop = operand_get(0);
    lop->t = t_symbol;
    lop->u.o = symbol = last->r.ast->l.value;
    /* FIXME should have been already checked */
    symbol = oget_symbol(record, symbol->name);
    if (symbol == null)
	oparse_error(last, "no field '%p' in '%p'", lop->u.o, record->name);

    if (rast) {
	tok = get_token(ast);
	if (tok) {
	    rop = operand_get(bop->s);
	    load_record(bop, lop, rop);
	    emit(rast);
	    emit_binary_next(rop, tok, operand_top());
	    if (ast->token >= tok_andset && ast->token <= tok_remset)
		store_record(bop, lop, rop);
	}
	else {
	    emit(rast);
	    rop = operand_top();
	    store_record(bop, lop, rop);
	}
    }
    else {
	rop = operand_get(bop->s);
	load_record(bop, lop, rop);
    }
    operand_copy(bop, rop);
    operand_unget(2);
}

static void
emit_vector(oast_t *last, oast_t *ast, oast_t *rast)
{
    ooperand_t		*bop;
    ooperand_t		*lop;
    otoken_t		 tok;
    ooperand_t		*rop;

    emit(last->l.ast);
    bop = operand_top();
    if (bop->k == null || bop->k->type != tag_vector)
	oparse_error(last, "expecting vector %A", last->l.ast);
    emit(last->r.ast);
    lop = operand_top();

    if (rast) {
	tok = get_token(ast);
	if (tok) {
	    rop = operand_get(bop->s);
	    load_vector(bop, lop, rop);
	    emit(rast);
	    emit_binary_next(rop, tok, operand_top());
	    if (ast->token >= tok_andset && ast->token <= tok_remset)
		store_vector(bop, lop, rop);
	}
	else {
	    emit(rast);
	    rop = operand_top();
	    store_vector(bop, lop, rop);
	}
    }
    else {
	rop = operand_get(bop->s);
	load_vector(bop, lop, rop);
    }
    operand_copy(bop, rop);
    operand_unget(2);
}

static void
emit_vararg(oast_t *last, oast_t *ast, oast_t *rast)
{
    ooperand_t		*lop;
    ooperand_t		*rop;
    otoken_t		 tok;

    if (!current_function->varargs)
	oparse_error(last, "function is not varargs");
    emit(last->r.ast);
    lop = operand_top();

    if (rast) {
	tok = get_token(ast);
	if (tok) {
	    rop = operand_get(rast->offset);
	    load_vararg(lop, rop);
	    emit(rast);
	    emit_binary_next(rop, tok, operand_top());
	    if (ast->token >= tok_andset && ast->token <= tok_remset)
		store_vararg(lop, rop);
	}
	else {
	    emit(rast);
	    rop = operand_top();
	    store_vararg(lop, rop);
	}
    }
    else {
	rop = operand_get(lop->s);
	load_vararg(lop, rop);
    }
    operand_copy(lop, rop);
    operand_unget(1);
}

static void
emit_label(oast_t *ast)
{
    oentry_t		*entry;
    olabel_t		*label;
    osymbol_t		*symbol;

    symbol = ast->l.value;
    assert(osymbol_p(symbol));
    entry = oget_hash(current_function->labels, symbol->name);
    label = entry->value;
    if (label->jit == null)
	label->jit = jit_label();
    else
	jit_link(label->jit);
}

static void
emit_goto(oast_t *ast)
{
    jit_node_t		*jump;
    oentry_t		*entry;
    olabel_t		*label;
    osymbol_t		*symbol;

    symbol = ast->l.ast->l.value;
    assert(osymbol_p(symbol));
    entry = oget_hash(current_function->labels, symbol->name);
    label = entry->value;
    if (label->jit == null)
	label->jit = jit_forward();
    jump = jit_jmpi();
    jit_patch_at(jump, label->jit);
}

#define CODE
#  include "oemit_unary.c"
#  include "oemit_binary.c"
#  include "oemit_cond.c"
#  include "oemit_except.c"
#  include "oemit_flow.c"
#undef CODE

static void
emit_load(ooperand_t *op)
{
    oword_t		 regno;

    if (!(op->t & t_register)) {
	switch (op->t) {
	    case t_half:	case t_word:
		regno = get_register(true);
		jit_movi(GPR[regno], op->u.w);
		break;
	    case t_float:
		regno = get_register(true);
		jit_movi_d(FPR[regno], op->u.d);
		break;
	    case t_symbol:
		load_symbol(op->u.o, op);
		return;
	    case t_undef:
		/* Load from constant pool */
		regno = get_register(true);
		load_r(regno);
		jit_movi(JIT_R0, (oword_t)op->u.o);
		jit_prepare();
		jit_pushargr(GPR[regno]);
		jit_pushargr(JIT_R0);
		emit_finish(ovm_copy, mask1(regno));
		op->t = t_void;
		break;
	    default:
		abort();
	}
	op->t |= t_register;
	op->u.w = regno;
    }
    else if (op->t & t_spill)
	emit_reload(op, false);
}

static void
loadi(ooperand_t *op, otype_t type, jit_int32_t base, oword_t offset)
{
    oword_t		regno;

    regno = op->u.w;
    switch (type) {
	case t_int8:	case t_uint8:	case t_int16:
#if __WORDSIZE == 32
	    op->t = t_half|t_register;
	    break;
#endif
	case t_uint16:	case t_int32:
#if __WORDSIZE == 32
	    op->t = t_word|t_register;
#else
	    op->t = t_half|t_register;
#endif
	    break;
	case t_uint32:	case t_int64:
#if __WORDSIZE == 64
	    op->t = t_word|t_register;
	    break;
#endif
	case t_uint64:
	    op->t = t_void|t_register;
	    break;
	case t_single:	case t_float:
	    op->t = type|t_register;
	    break;
	default:
	    op->t = t_void|t_register;
	    jit_addi(JIT_R0, base, offset);
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    jit_pushargr(JIT_R0);
	    emit_finish(ovm_load, mask1(regno));
	    return;
    }

    switch (type) {
	case t_int8:
	    jit_ldxi_c(GPR[regno], base, offset);
	    break;
	case t_uint8:
	    jit_ldxi_uc(GPR[regno], base, offset);
	    break;
	case t_int16:
	    jit_ldxi_s(GPR[regno], base, offset);
	    break;
	case t_uint16:
	    jit_ldxi_us(GPR[regno], base, offset);
	    break;
	case t_int32:
	    jit_ldxi_i(GPR[regno], base, offset);
	    break;
#if __WORDSIZE == 32
	case t_uint32:
	    jit_ldxi(GPR[regno], base, offset);
	    sync_uw(regno);
	    break;
	case t_int64:
	    jit_addi(GPR[regno], base, offset);
	    sync_ww(regno);
	    break;
	case t_uint64:
	    jit_addi(GPR[regno], base, offset);
	    sync_uwuw(regno);
	    break;
#else
	case t_uint32:
	    jit_ldxi_ui(GPR[regno], base, offset);
	    break;
	case t_int64:
	    jit_ldxi(GPR[regno], base, offset);
	    break;
	case t_uint64:
	    jit_ldxi(GPR[regno], base, offset);
	    sync_uw(regno);
	    break;
#endif
	case t_single:
	    jit_ldxi_f(FPR[regno], base, offset);
	    break;
	case t_float:
	    jit_ldxi_d(FPR[regno], base, offset);
	    break;
	default:
	    abort();
    }
}

static void
loadr(ooperand_t *op, otype_t type, jit_int32_t base, jit_int32_t offset)
{
    oword_t		regno;

    regno = op->u.w;
    switch (type) {
	case t_int8:	case t_uint8:	case t_int16:
#if __WORDSIZE == 32
	    op->t = t_half|t_register;
	    break;
#endif
	case t_uint16:	case t_int32:
#if __WORDSIZE == 32
	    op->t = t_word|t_register;
#else
	    op->t = t_half|t_register;
#endif
	    break;
	case t_uint32:	case t_int64:
#if __WORDSIZE == 64
	    op->t = t_word|t_register;
	    break;
#endif
	case t_uint64:
	    op->t = t_void|t_register;
	    break;
	case t_single:	case t_float:
	    op->t = type|t_register;
	    break;
	default:
	    op->t = t_void|t_register;
	    jit_addr(JIT_R0, base, offset);
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    jit_pushargr(JIT_R0);
	    emit_finish(ovm_load, mask1(regno));
	    return;
    }

    switch (type) {
	case t_int8:
	    jit_ldxr_c(GPR[regno], base, offset);
	    break;
	case t_uint8:
	    jit_ldxr_uc(GPR[regno], base, offset);
	    break;
	case t_int16:
	    jit_ldxr_s(GPR[regno], base, offset);
	    break;
	case t_uint16:
	    jit_ldxr_us(GPR[regno], base, offset);
	    break;
	case t_int32:
	    jit_ldxr_i(GPR[regno], base, offset);
	    break;
#if __WORDSIZE == 32
	case t_uint32:
	    jit_ldxr(GPR[regno], base, offset);
	    sync_uw(regno);
	    break;
	case t_int64:
	    jit_addr(GPR[regno], base, offset);
	    sync_ww(regno);
	    break;
	case t_uint64:
	    jit_addr(GPR[regno], base, offset);
	    sync_uwuw(regno);
	    break;
#else
	case t_uint32:
	    jit_ldxr_ui(GPR[regno], base, offset);
	    break;
	case t_int64:
	    jit_ldxr(GPR[regno], base, offset);
	    break;
	case t_uint64:
	    jit_ldxr(GPR[regno], base, offset);
	    sync_uw(regno);
	    break;
#endif
	case t_single:
	    jit_ldxr_f(FPR[regno], base, offset);
	    break;
	case t_float:
	    jit_ldxr_d(FPR[regno], base, offset);
	    break;
	default:
	    abort();
    }
}

static void
storei(ooperand_t *op, otype_t type, jit_int32_t base, oword_t offset)
{
    otype_t		otype;
    oword_t		regno;
    obool_t		use_f0;

    use_f0 = false;
    emit_load(op);
    assert((op->t & (t_spill|t_register)) == t_register);
    otype = emit_get_type(op);
    regno = op->u.w;
    switch (type) {
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	    if (otype == t_single)
		jit_truncr_f_i(GPR[regno], FPR[regno]);
	    else if (otype == t_float)
		jit_truncr_d_i(GPR[regno], FPR[regno]);
	    break;
#if __WORDSIZE == 64
	case t_int64:		case t_uint64:
	    if (otype == t_single)
		jit_truncr_f(GPR[regno], FPR[regno]);
	    else if (otype == t_float)
		jit_truncr_d(GPR[regno], FPR[regno]);
	    break;
#else
	case t_int64:		case t_uint64:
	    if (otype == t_single || otype == t_float) {
		if (otype == t_single)
		    jit_extr_f_d(FPR[regno], FPR[regno]);
		jit_addi(GPR[regno], base, offset);
		sync_d(regno);
		load_r_w(regno, JIT_R0);
		jit_prepare();
		jit_pushargr(JIT_R0);
		jit_pushargr(GPR[regno]);
		emit_finish(ovm_store_i64, 0);
		if (otype == t_single)
		    jit_extr_d_f(FPR[regno], FPR[regno]);
		return;
	    }
	    break;
#endif
	case t_single:
	    if (otype == t_half || otype == t_word)
		jit_extr_f(FPR[regno], GPR[regno]);
	    else if (otype == t_float) {
		use_f0 = true;
		jit_extr_d_f(JIT_F0, FPR[regno]);
	    }
	    break;
	case t_float:
	    if (otype == t_half || otype == t_word)
		jit_extr_d(FPR[regno], GPR[regno]);
	    else if (otype == t_single) {
		use_f0 = true;
		jit_extr_f_d(JIT_F0, FPR[regno]);
	    }
	    break;
	default:
	    if (otype == t_single) {
		use_f0 = true;
		jit_extr_f_d(JIT_F0, FPR[regno]);
	    }
	    jit_addi(JIT_R0, base, offset);
	    jit_prepare();
	    if (otype == t_half || otype == t_word) {
		jit_pushargr(GPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store_w, 0);
	    }
	    else if (otype == t_single || otype == t_float) {
		if (use_f0)
		    jit_pushargr_d(JIT_F0);
		else
		    jit_pushargr_d(FPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store_d, 0);
	    }
	    else {
		load_r(regno);
		jit_pushargr(GPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store, mask1(regno));
	    }
	    return;
    }

    if (otype == t_void) {
	jit_addi(JIT_R0, base, offset);
	load_r(regno);
	jit_prepare();
	jit_pushargr(GPR[regno]);
	jit_pushargr(JIT_R0);
	switch (type) {
	    case t_int8:		case t_uint8:
		emit_finish(ovm_store_i8, mask1(regno));
		break;
	    case t_int16:		case t_uint16:
		emit_finish(ovm_store_i16, mask1(regno));
		break;
	    case t_int32:		case t_uint32:
		emit_finish(ovm_store_i32, mask1(regno));
		break;
	    case t_int64:		case t_uint64:
		emit_finish(ovm_store_i64, mask1(regno));
		break;
	    case t_single:
		emit_finish(ovm_store_f32, mask1(regno));
		break;
	    default:
		assert(type == t_float);
		emit_finish(ovm_store_f64, mask1(regno));
		break;
	}
    }
    else {
	switch (type) {
	    case t_int8:		case t_uint8:
		jit_stxi_c(offset, base, GPR[regno]);
		break;
	    case t_int16:		case t_uint16:
		jit_stxi_s(offset, base, GPR[regno]);
		break;
	    case t_int32:		case t_uint32:
		jit_stxi_i(offset, base, GPR[regno]);
		break;
	    case t_int64:		case t_uint64:
#if __WORDSIZE == 32
		if (GPR[TMP0] != JIT_NOREG && !(tmp_mask & 1))
		    jit_rshi(GPR[TMP0], GPR[regno], 31);
		else if (GPR[TMP1] != JIT_NOREG && !(tmp_mask & 2))
		    jit_rshi(GPR[TMP1], GPR[regno], 31);
		else {
		    spill_w(regno);
		    jit_rshi(GPR[regno], GPR[regno], 31);
		}

#  if __BYTE_ORDER == __LITTLE_ENDIAN
		if (GPR[TMP0] != JIT_NOREG && !(tmp_mask & 1))
		    jit_stxi_i(offset + 4, base, GPR[TMP0]);
		else if (GPR[TMP1] != JIT_NOREG && !(tmp_mask & 2))
		    jit_stxi_i(offset + 4, base, GPR[TMP1]);
		else {
		    jit_stxi_i(offset + 4, base, GPR[regno]);
		    load_w(regno);
		}
		jit_stxi_i(offset, base, GPR[regno]);
#  else
		if (GPR[TMP0] != JIT_NOREG && !(tmp_mask & 1))
		    jit_stxi_i(offset, base, GPR[TMP0]);
		else if (GPR[TMP1] != JIT_NOREG && !(tmp_mask & 2))
		    jit_stxi_i(offset, base, GPR[TMP1]);
		else {
		    jit_stxi_i(offset, base, GPR[regno]);
		    load_w(regno);
		}
		jit_stxi_i(offset + 4, base, GPR[regno]);
#  endif
#else
		jit_stxi_l(offset, base, GPR[regno]);
#endif
		break;
	    case t_single:
		if (use_f0)
		    jit_stxi_f(offset, base, JIT_F0);
		else
		    jit_stxi_f(offset, base, FPR[regno]);
		    break;
	    default:
		assert(type == t_float);
		if (use_f0)
		    jit_stxi_d(offset, base, JIT_F0);
		else
		    jit_stxi_d(offset, base, FPR[regno]);
		    break;
	}
    }
}

static void
storer(ooperand_t *op, otype_t type, jit_int32_t base, jit_int32_t offset)
{
    otype_t		otype;
    oword_t		regno;
    obool_t		use_f0;

    use_f0 = false;
    emit_load(op);
    assert((op->t & (t_spill|t_register)) == t_register);
    otype = emit_get_type(op);
    regno = op->u.w;
    switch (type) {
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	    if (otype == t_single)
		jit_truncr_f_i(GPR[regno], FPR[regno]);
	    else if (otype == t_float)
		jit_truncr_d_i(GPR[regno], FPR[regno]);
	    break;
#if __WORDSIZE == 64
	case t_int64:		case t_uint64:
	    if (otype == t_single)
		jit_truncr_f(GPR[regno], FPR[regno]);
	    else if (otype == t_float)
		jit_truncr_d(GPR[regno], FPR[regno]);
	    break;
#else
	case t_int64:		case t_uint64:
	    if (otype == t_single || otype == t_float) {
		if (otype == t_single)
		    jit_extr_f_d(FPR[regno], FPR[regno]);
		jit_addi(GPR[regno], base, offset);
		sync_d(regno);
		load_r_w(regno, JIT_R0);
		jit_prepare();
		jit_pushargr(JIT_R0);
		jit_pushargr(GPR[regno]);
		emit_finish(ovm_store_i64, 0);
		if (otype == t_single)
		    jit_extr_d_f(FPR[regno], FPR[regno]);
		return;
	    }
	    break;
#endif
	case t_single:
	    if (otype == t_half || otype == t_word)
		jit_extr_f(FPR[regno], GPR[regno]);
	    else if (otype == t_float) {
		use_f0 = true;
		jit_extr_d_f(JIT_F0, FPR[regno]);
	    }
	    break;
	case t_float:
	    if (otype == t_half || otype == t_word)
		jit_extr_d(FPR[regno], GPR[regno]);
	    else if (otype == t_single) {
		use_f0 = true;
		jit_extr_f_d(JIT_F0, FPR[regno]);
	    }
	    break;
	default:
	    if (otype == t_single) {
		use_f0 = true;
		jit_extr_f_d(JIT_F0, FPR[regno]);
	    }
	    jit_addr(JIT_R0, base, offset);
	    jit_prepare();
	    if (otype == t_half || otype == t_word) {
		jit_pushargr(GPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store_w, 0);
	    }
	    else if (otype == t_single || otype == t_float) {
		if (use_f0)
		    jit_pushargr_d(JIT_F0);
		else
		    jit_pushargr_d(FPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store_d, 0);
	    }
	    else {
		load_r(regno);
		jit_pushargr(GPR[regno]);
		jit_pushargr(JIT_R0);
		jit_pushargi(type == t_undef ? t_void : type);
		emit_finish(ovm_store, mask1(regno));
	    }
	    return;
    }

    if (otype == t_void) {
	jit_addr(JIT_R0, base, offset);
	load_r(regno);
	jit_prepare();
	jit_pushargr(GPR[regno]);
	jit_pushargr(JIT_R0);
	switch (type) {
	    case t_int8:		case t_uint8:
		emit_finish(ovm_store_i8, mask1(regno));
		break;
	    case t_int16:		case t_uint16:
		emit_finish(ovm_store_i16, mask1(regno));
		break;
	    case t_int32:		case t_uint32:
		emit_finish(ovm_store_i32, mask1(regno));
		break;
	    case t_int64:		case t_uint64:
		emit_finish(ovm_store_i64, mask1(regno));
		break;
	    case t_single:
		emit_finish(ovm_store_f32, mask1(regno));
		break;
	    default:
		assert(type == t_float);
		emit_finish(ovm_store_f64, mask1(regno));
		break;
	}
    }
    else {
	switch (type) {
	    case t_int8:		case t_uint8:
		jit_stxr_c(offset, base, GPR[regno]);
		break;
	    case t_int16:		case t_uint16:
		jit_stxr_s(offset, base, GPR[regno]);
		break;
	    case t_int32:		case t_uint32:
		jit_stxr_i(offset, base, GPR[regno]);
		break;
	    case t_int64:		case t_uint64:
#if __WORDSIZE == 32
		if (GPR[TMP0] != JIT_NOREG)
		    jit_rshi(GPR[TMP0], GPR[regno], 31);
		else {
		    spill_w(regno);
		    jit_rshi(GPR[regno], GPR[regno], 31);
		}
#  if __BYTE_ORDER == __LITTLE_ENDIAN
		if (GPR[TMP0] != JIT_NOREG)
		    jit_stxi_i(offset + 4, base, GPR[TMP0]);
		else {
		    jit_stxi_i(offset + 4, base, GPR[regno]);
		    load_w(regno);
		}
		jit_stxi_i(offset, base, GPR[regno]);
#  else
		if (GPR[TMP0] != JIT_NOREG)
		    jit_stxi_i(offset, base, GPR[TMP0]);
		else {
		    jit_stxi_i(offset, base, GPR[regno]);
		    load_w(regno);
		}
		jit_stxi_i(offset + 4, base, GPR[regno]);
#  endif
#else
		jit_stxr_l(offset, base, GPR[regno]);
#endif
		break;
	    case t_single:
		if (use_f0)
		    jit_stxr_f(offset, base, JIT_F0);
		else
		    jit_stxr_f(offset, base, FPR[regno]);
		break;
	    default:
		assert(type == t_float);
		if (use_f0)
		    jit_stxr_d(offset, base, JIT_F0);
		else
		    jit_stxr_d(offset, base, FPR[regno]);
		break;
	}
    }
}

static void
emit_coerce(otype_t type, ooperand_t *lop, ooperand_t *rop)
{
    otype_t		rty;
    oword_t		lreg;
    oword_t		rreg;

    rty = emit_get_type(rop);
    lreg = lop->u.w;
    rreg = rop->u.w;
    lop->t = rop->t;
    if (rty == t_half || rty == t_word) {
    word:
	switch (type) {
	    case t_int8:
		jit_extr_c(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_half);
		break;
	    case t_uint8:
		jit_extr_uc(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_half);
		break;
	    case t_int16:
		jit_extr_s(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_half);
		break;
	    case t_uint16:
		jit_extr_us(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_half);
		break;
	    case t_int32:
#if __WORDSIZE == 32
		jit_movr(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_word);
#else
		jit_extr_i(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_half);
#endif
		break;
	    case t_uint32:
#if __WORDSIZE == 32
		jit_movr(GPR[lreg], GPR[rreg]);
		sync_uw(lreg);
		loadif_w(lreg);
		emit_set_type(lop, t_void);
#else
		jit_extr_ui(GPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_word);
#endif
		break;
	    case t_int64:
		jit_movr(GPR[lreg], GPR[rreg]);
#if __WORDSIZE == 32
		sync_w(lreg);
		emit_set_type(lop, t_void);
#else
		emit_set_type(lop, t_word);
#endif
		break;
	    case t_uint64:
		jit_movr(GPR[lreg], GPR[rreg]);
		sync_uw(lreg);
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
	    case t_float32:
		jit_extr_f(FPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_single);
		break;
	    case t_float64:
		jit_extr_d(FPR[lreg], GPR[rreg]);
		emit_set_type(lop, t_float);
		break;
	    default:
		jit_movr(GPR[lreg], GPR[rreg]);
		sync_w(lreg);
		emit_set_type(lop, t_void);
		break;
	}
    }
    else if (rty == t_single) {
	switch (type) {
	    case t_int8:	case t_uint8:
	    case t_int16:	case t_uint16:
	    case t_int32:	case t_uint32:
		jit_truncr_f(GPR[lreg], FPR[rreg]);
		rreg = lreg;
		goto word;
	    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
		load_r(lreg);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr_f(FPR[rreg]);
		emit_finish(ovm_truncr_f, mask1(lreg));
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
#else
		jit_truncr_f(GPR[lreg], FPR[rreg]);
		rreg = lreg;
		goto word;
#endif
	    case t_float32:
		jit_movr_f(FPR[lreg], FPR[rreg]);
		break;
	    case t_float64:
		jit_extr_f_d(FPR[lreg], FPR[rreg]);
		emit_set_type(lop, t_float);
		break;
	    default:
		jit_extr_f_d(FPR[lreg], FPR[rreg]);
		sync_d(lreg);
		emit_set_type(lop, t_void);
		break;
	}
    }
    else if (rty == t_float) {
	switch (type) {
	    case t_int8:	case t_uint8:
	    case t_int16:	case t_uint16:
	    case t_int32:	case t_uint32:
		jit_truncr_d(GPR[lreg], FPR[rreg]);
		rreg = lreg;
		goto word;
	    case t_int64:	case t_uint64:
#if __WORDSIZE == 32
		load_r(lreg);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr_d(FPR[rreg]);
		emit_finish(ovm_truncr_d, mask1(lreg));
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
#else
		jit_truncr_d(GPR[lreg], FPR[rreg]);
		rreg = lreg;
		goto word;
#endif
	    case t_float32:
		jit_extr_d_f(FPR[lreg], FPR[rreg]);
		emit_set_type(lop, t_single);
		break;
	    case t_float64:
		jit_movr_d(FPR[lreg], FPR[rreg]);
		break;
	    default:
		jit_movr_d(FPR[lreg], FPR[rreg]);
		sync_d(lreg);
		emit_set_type(lop, t_void);
		break;
	}
    }
    else {
	switch (type) {
	    case t_int8:	case t_uint8:
	    case t_int16:	case t_uint16:
	    case t_int32:
#if __WORDSIZE == 64
	    case t_uint32:	case t_int64:
#endif
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_w, mask1(lreg));
		load_w(lreg);
		rreg = lreg;
		goto word;
#if __WORDSIZE == 32
	    case t_uint32:
#else
	    case t_uint64:
#endif
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_uw, mask1(lreg));
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
#if __WORDSIZE == 32
	    case t_int64:
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_ww, mask1(lreg));
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
	    case t_uint64:
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_uwuw, mask1(lreg));
		loadif_w(lreg);
		emit_set_type(lop, t_void);
		break;
#endif
	    case t_float32:
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_d, mask1(lreg));
		load_d(lreg);
		jit_extr_d_f(FPR[lreg], FPR[lreg]);
		break;
	    case t_float64:
		load_r(lreg);
		if (lreg != rreg)		load_r_w(rreg, JIT_R0);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		jit_pushargr(lreg != rreg ? JIT_R0 : GPR[lreg]);
		emit_finish(ovm_coerce_d, mask1(lreg));
		load_d(lreg);
		break;
	    default:
		if (lreg != rreg) {
		    load_r(lreg);
		    load_r_w(rreg, JIT_R0);
		    jit_prepare();
		    jit_pushargr(GPR[lreg]);
		    jit_pushargr(JIT_R0);
		    emit_finish(ovm_move, mask1(lreg));
		}
		break;
	}
    }
}

static void
load_symbol(osymbol_t *symbol, ooperand_t *op)
{
    otype_t		type;
    jit_int32_t		regval;

    op->u.w = get_register(true);
    type = otag_to_type(symbol->tag);
    if (symbol->global) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    regval = GPR[GLOBAL];
	else {
	    regval = JIT_R0;
	    jit_movi(regval, (oword_t)gd);
	}
    }
    else if (symbol->field) {
	if (GPR[THIS] != JIT_NOREG)
	    regval = GPR[THIS];
	else if (GPR[FRAME] != JIT_NOREG) {
	    regval = JIT_R0;
	    jit_ldxi(regval, GPR[FRAME], THIS_OFFSET);
	}
	else {
	    regval = JIT_R0;
	    jit_ldxi(regval, JIT_V0, offsetof(othread_t, fp));
	    jit_ldxi(regval, regval, THIS_OFFSET);
	}
    }
    else {
	if (GPR[FRAME] != JIT_NOREG)
	    regval = GPR[FRAME];
	else {
	    regval = JIT_R0;
	    jit_ldxi(regval, JIT_V0, offsetof(othread_t, fp));
	}
    }
    loadi(op, type, regval, symbol->offset);
}

static void
store_symbol(osymbol_t *symbol, ooperand_t *op)
{
    otype_t		type;
    jit_int32_t		regval;

    type = otag_to_type(symbol->tag);
    if (symbol->global) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    regval = GPR[GLOBAL];
	else {
	    regval = JIT_R0;
	    jit_movi(regval, (oword_t)gd);
	}
    }
    else if (symbol->field) {
	if (GPR[THIS] != JIT_NOREG)
	    regval = GPR[THIS];
	else if (GPR[FRAME] != JIT_NOREG) {
	    regval = JIT_R0;
	    jit_ldxi(regval, GPR[FRAME], THIS_OFFSET);
	}
	else {
	    regval = JIT_R0;
	    jit_ldxi(regval, JIT_V0, offsetof(othread_t, fp));
	    jit_ldxi(regval, regval, THIS_OFFSET);
	}
    }
    else {
	if (GPR[FRAME] != JIT_NOREG)
	    regval = GPR[FRAME];
	else {
	    regval = JIT_R0;
	    jit_ldxi(regval, JIT_V0, offsetof(othread_t, fp));
	}
    }
    storei(op, type, regval, symbol->offset);
}

static void
load_record(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop)
{
    jit_node_t		*jump;
    osymbol_t		*symbol;

    emit_load(bop);
    assert(lop->t == t_symbol);
    symbol = lop->u.o;
    rop->u.w = get_register(true);
    rop->k = symbol->tag;
    assert(emit_get_type(bop) == t_void);
    /* Only null or a compatible object type should be stored */
    load_w(bop->u.w);
    jump = jit_bnei(GPR[bop->u.w], 0);
    jit_prepare();
    jit_pushargi(except_null_dereference);
    jit_finishi(ovm_raise);
    jit_patch(jump);
    loadi(rop, otag_to_type(rop->k), GPR[bop->u.w], symbol->offset);
}

static void
store_record(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop)
{
    jit_node_t		*jump;
    osymbol_t		*symbol;

    emit_load(bop);
    assert(lop->t == t_symbol);
    symbol = lop->u.o;
    assert(emit_get_type(bop) == t_void);
    /* Only null or a compatible object type should be stored */
    load_w(bop->u.w);
    jump = jit_bnei(GPR[bop->u.w], 0);
    jit_prepare();
    jit_pushargi(except_null_dereference);
    jit_finishi(ovm_raise);
    jit_patch(jump);
    storei(rop, otag_to_type(symbol->tag), GPR[bop->u.w], symbol->offset);
}

static void
load_vector(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop)
{
    obool_t		 imm;
    obool_t		 byte;
    otype_t		 type;
    jit_node_t		*jnul;
    jit_node_t		*jump;
    oword_t		 offset;

    emit_load(bop);

    imm = lop->t == t_half || lop->t == t_word;
    if (imm)
	offset = lop->u.w;
    else {
	/* Load now or rop may allocate the same register */
	emit_load(lop);
	offset = 0;
    }

    rop->u.w = get_register(true);
    rop->k = bop->k->base;
    type = otag_to_type(rop->k);
    byte = type == t_int8 || type == t_uint8;

    /* Only null or a compatible object type should be stored */
    load_w(bop->u.w);
    /* Could just except invalid argument, but prefer better error
     * description at the cost of (at least) two more instructions */
    jit_movi(JIT_R0, except_null_dereference);
    jnul = jit_beqi(GPR[bop->u.w], 0);
    /* Vector allocation does not allow allocating non addressable
     * memory, so, no need to check if the offset overflows */
    if (imm) {
	jit_ldxi(JIT_R0, GPR[bop->u.w], offsetof(ovector_t, length));
	jump = jit_bgti_u(JIT_R0, offset);
    }
    else {
	switch (emit_get_type(lop)) {
	    case t_half:	case t_word:
		break;
	    case t_single:
		jit_extr_f_d(FPR[lop->u.w], FPR[lop->u.w]);
	    case t_float:
		sync_d(lop->u.w);
	    case t_void:
		load_r(lop->u.w);
		jit_prepare();
		jit_pushargr(GPR[lop->u.w]);
		emit_finish(ovm_offset, mask1(lop->u.w));
		load_w(lop->u.w);
		emit_set_type(lop, t_word);
		break;
	    default:
		abort();
	}
	jit_ldxi(JIT_R0, GPR[bop->u.w], offsetof(ovector_t, length));
	jump = jit_bgtr_u(JIT_R0, GPR[lop->u.w]);
    }
    jit_movi(JIT_R0, except_out_of_bounds);
    jit_patch(jnul);
    jit_prepare();
    jit_pushargr(JIT_R0);
    jit_finishi(ovm_raise);
    jit_patch(jump);

    if (!byte) {
	if (imm) {
	    /* Overflow after shift must have been already tested */
	    switch (type) {
		case t_int16:	case t_uint16:
		    offset <<= 1;
		    break;
		case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
		default:
#endif
		    offset <<= 2;
		    break;
		case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
		default:
#endif
		    offset <<= 3;
		    break;
	    }
	}
	else {
	    protect_reg(lop->u.w);
	    spill_w(lop->u.w);
	    switch (type) {
		case t_int16:	case t_uint16:
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 1);
		    break;
		case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
		default:
#endif
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 2);
		    break;
		case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
		default:
#endif
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 3);
		    break;
	    }
	}
    }

    if (GPR[TMP0] == JIT_NOREG) {
	spill_w(bop->u.w);
	jit_ldxi(GPR[bop->u.w], GPR[bop->u.w], offsetof(ovector_t, v.ptr));
	if (imm)
	    loadi(rop, type, GPR[bop->u.w], offset);
	else
	    loadr(rop, type, GPR[bop->u.w], GPR[lop->u.w]);
	load_w(bop->u.w);
    }
    else {
	protect_tmp(0);
	jit_ldxi(GPR[TMP0], GPR[bop->u.w], offsetof(ovector_t, v.ptr));
	if (imm)
	    loadi(rop, type, GPR[TMP0], offset);
	else
	    loadr(rop, type, GPR[TMP0], GPR[lop->u.w]);
	release_tmp(0);
    }
    if (!imm && !byte) {
	load_w(lop->u.w);
	release_reg(lop->u.w);
    }
}

static void
store_vector(ooperand_t *bop, ooperand_t *lop, ooperand_t *rop)
{
    obool_t		 imm;
    obool_t		 byte;
    otype_t		 type;
    jit_node_t		*jnul;
    jit_node_t		*jump;
    oword_t		 offset;

    emit_load(bop);
    type = otag_to_type(bop->k->base);
    byte = type == t_int8 || type == t_uint8;

    imm = lop->t == t_half || lop->t == t_word;
    if (imm)
	offset = lop->u.w;
    else {
	/* Load now or rop may allocate the same register */
	emit_load(lop);
	offset = 0;
    }

    /* Only null or a compatible object type should be stored */
    load_w(bop->u.w);
    /* Could just except invalid argument, but prefer better error
     * description at the cost of (at least) two more instructions */
    jit_movi(JIT_R0, except_null_dereference);
    jnul = jit_beqi(GPR[bop->u.w], 0);
    /* Vector allocation does not allow allocating non addressable
     * memory, so, no need to check if the offset overflows */
    if (imm) {
	jit_ldxi(JIT_R0, GPR[bop->u.w], offsetof(ovector_t, length));
	jump = jit_bgti_u(JIT_R0, offset);
    }
    else {
	switch (emit_get_type(lop)) {
	    case t_half:	case t_word:
		break;
	    case t_single:
		jit_extr_f_d(FPR[lop->u.w], FPR[lop->u.w]);
	    case t_float:
		sync_d(lop->u.w);
	    case t_void:
		load_r(lop->u.w);
		jit_prepare();
		jit_pushargr(GPR[lop->u.w]);
		emit_finish(ovm_offset, mask1(lop->u.w));
		load_w(lop->u.w);
		emit_set_type(lop, t_word);
		break;
	    default:
		abort();
	}
	jit_ldxi(JIT_R0, GPR[bop->u.w], offsetof(ovector_t, length));
	jump = jit_bgtr_u(JIT_R0, GPR[lop->u.w]);
    }
    jit_movi(JIT_R0, except_out_of_bounds);
    jit_patch(jnul);
    jit_prepare();
    jit_pushargr(JIT_R0);
    jit_finishi(ovm_raise);
    jit_patch(jump);

    if (!byte) {
	if (imm) {
	    switch (type) {
		case t_int16:	case t_uint16:
		    offset <<= 1;
		    break;
		case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
		default:
#endif
		    offset <<= 2;
		    break;
		case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
		default:
#endif
		    offset <<= 3;
		    break;
	    }
	}
	else {
	    protect_reg(lop->u.w);
	    spill_w(lop->u.w);
	    switch (type) {
		case t_int16:	case t_uint16:
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 1);
		    break;
		case t_int32:	case t_uint32:	case t_float32:
#if __WORDSIZE == 32
		default:
#endif
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 2);
		    break;
		case t_int64:	case t_uint64:	case t_float64:
#if __WORDSIZE == 64
		default:
#endif
		    jit_lshi(GPR[lop->u.w], GPR[lop->u.w], 3);
		    break;
	    }
	}
    }

    if (GPR[TMP0] == JIT_NOREG) {
	spill_w(bop->u.w);
	jit_ldxi(GPR[bop->u.w], GPR[bop->u.w], offsetof(ovector_t, v.ptr));
	if (imm)
	    storei(rop, type, GPR[bop->u.w], offset);
	else
	    storer(rop, type, GPR[bop->u.w], GPR[lop->u.w]);
	load_w(bop->u.w);
    }
    else {
	protect_tmp(0);
	jit_ldxi(GPR[TMP0], GPR[bop->u.w], offsetof(ovector_t, v.ptr));
	if (imm)
	    storei(rop, type, GPR[TMP0], offset);
	else
	    storer(rop, type, GPR[TMP0], GPR[lop->u.w]);
	release_tmp(0);
    }
    if (!imm && !byte) {
	load_w(lop->u.w);
	release_reg(lop->u.w);
    }
}

static void
load_vararg(ooperand_t *lop, ooperand_t *rop)
{
    obool_t		 imm;
    jit_node_t		*jovr;
    jit_node_t		*jump;
    oword_t		 offset;

    jovr = null;
    imm = lop->t == t_half || lop->t == t_word;
    offset = imm ? lop->u.w * sizeof(oword_t) : 0;

    /* Load now or rop may allocate the same register */
    if (!imm)
	emit_load(lop);

    rop->u.w = get_register(true);
    rop->t = t_void|t_register;
    rop->k = auto_tag;

    if (!imm) {
	switch (emit_get_type(lop)) {
	    case t_half:		case t_word:
		break;
	    case t_single:
		jit_extr_f_d(FPR[lop->u.w], FPR[lop->u.w]);
	    case t_float:
		sync_d(lop->u.w);
	    case t_void:
		load_r(lop->u.w);
		jit_prepare();
		jit_pushargr(GPR[lop->u.w]);
		emit_finish(ovm_offset, mask1(lop->u.w));
		load_w(lop->u.w);
		emit_set_type(lop, t_word);
		break;
	    default:
		abort();
	}
	protect_reg(lop->u.w);
	spill_w(lop->u.w);
#if __WORDSIZE == 32
	jit_qmuli(GPR[lop->u.w], JIT_R0, GPR[lop->u.w], 4);
#else
	jit_qmuli(GPR[lop->u.w], JIT_R0, GPR[lop->u.w], 8);
#endif
	jovr = jit_bnei(JIT_R0, 0);
    }

    if (GPR[FRAME] != JIT_NOREG)
	jit_ldxi_i(JIT_R0, GPR[FRAME], SIZE_OFFSET);
    else {
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	jit_ldxi_i(JIT_R0, JIT_R0, SIZE_OFFSET);
    }

    if (imm)
	jump = jit_bgti_u(JIT_R0, offset);
    else
	jump = jit_bgtr_u(JIT_R0, GPR[lop->u.w]);
    if (jovr)
	jit_patch(jovr);

    jit_prepare();
    jit_pushargi(except_out_of_bounds);
    jit_finishi(ovm_raise);
    jit_patch(jump);

    if (imm) {
	if (GPR[FRAME] != JIT_NOREG)
	    jit_addi(JIT_R0, GPR[FRAME], current_function->varargs + offset);
	else {
	    jit_ldxi(GPR[rop->u.w], JIT_V0, offsetof(othread_t, fp));
	    jit_addi(JIT_R0, GPR[rop->u.w], current_function->varargs + offset);
	}
    }
    else {
	if (GPR[FRAME] != JIT_NOREG)
	    jit_addi(JIT_R0, GPR[FRAME], current_function->varargs);
	else {
	    jit_ldxi(GPR[rop->u.w], JIT_V0, offsetof(othread_t, fp));
	    jit_addi(JIT_R0, GPR[rop->u.w], current_function->varargs);
	}
	jit_addr(JIT_R0, JIT_R0, GPR[lop->u.w]);
    }
    load_r(rop->u.w);
    jit_prepare();
    jit_pushargr(GPR[rop->u.w]);
    jit_pushargr(JIT_R0);
    emit_finish(ovm_load, mask1(rop->u.w));
    if (jovr) {
	load_w(lop->u.w);
	release_reg(lop->u.w);
    }
}

static void
store_vararg(ooperand_t *lop, ooperand_t *rop)
{
    obool_t		 imm;
    jit_node_t		*jovr;
    jit_node_t		*jump;
    oword_t		 offset;

    jovr = null;
    imm = lop->t == t_half || lop->t == t_word;
    offset = imm ? lop->u.w * sizeof(oword_t) : 0;

    if (!imm) {
	emit_load(lop);
	switch (emit_get_type(lop)) {
	    case t_half:		case t_word:
		break;
	    case t_single:
		jit_extr_f_d(FPR[lop->u.w], FPR[lop->u.w]);
	    case t_float:
		sync_d(lop->u.w);
	    case t_void:
		load_r(lop->u.w);
		jit_prepare();
		jit_pushargr(GPR[lop->u.w]);
		emit_finish(ovm_offset, mask1(lop->u.w));
		load_w(lop->u.w);
		emit_set_type(lop, t_word);
		break;
	    default:
		abort();
	}
	protect_reg(lop->u.w);
	spill_w(lop->u.w);
#if __WORDSIZE == 32
	jit_qmuli(GPR[lop->u.w], JIT_R0, GPR[lop->u.w], 4);
#else
	jit_qmuli(GPR[lop->u.w], JIT_R0, GPR[lop->u.w], 8);
#endif
	jovr = jit_bnei(JIT_R0, 0);
    }

    if (GPR[FRAME] != JIT_NOREG)
	jit_ldxi_i(JIT_R0, GPR[FRAME], SIZE_OFFSET);
    else {
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	jit_ldxi_i(JIT_R0, JIT_R0, SIZE_OFFSET);
    }

    if (imm)
	jump = jit_bgti_u(JIT_R0, offset);
    else
	jump = jit_bgtr_u(JIT_R0, GPR[lop->u.w]);
    if (jovr)
	jit_patch(jovr);

    jit_prepare();
    jit_pushargi(except_out_of_bounds);
    jit_finishi(ovm_raise);
    jit_patch(jump);

    if (imm) {
	if (GPR[FRAME] != JIT_NOREG)
	    jit_addi(JIT_R0, GPR[FRAME], current_function->varargs + offset);
	else {
	    jit_ldxi(GPR[rop->u.w], JIT_V0, offsetof(othread_t, fp));
	    jit_addi(JIT_R0, GPR[rop->u.w], current_function->varargs + offset);
	}
    }
    else {
	if (GPR[FRAME] != JIT_NOREG)
	    jit_addi(JIT_R0, GPR[FRAME], current_function->varargs);
	else {
	    jit_ldxi(GPR[rop->u.w], JIT_V0, offsetof(othread_t, fp));
	    jit_addi(JIT_R0, GPR[rop->u.w], current_function->varargs);
	}
	jit_addr(JIT_R0, JIT_R0, GPR[lop->u.w]);
    }
    load_r(rop->u.w);
    jit_prepare();
    jit_pushargr(GPR[rop->u.w]);
    jit_pushargr(JIT_R0);
    jit_pushargi(t_void);
    emit_finish(ovm_store, mask1(rop->u.w));
    if (jovr) {
	load_w(lop->u.w);
	release_reg(lop->u.w);
    }
}

static void
emit_reload(ooperand_t *op, obool_t same)
{
    oword_t		regno;
    obool_t		global;
    oword_t		regval;

    if (current_record == root_record) {
	global = true;
	if (GPR[GLOBAL] != JIT_NOREG)
	    regval = GPR[GLOBAL];
	else
	    regval = JIT_R0;
    }
    else {
	global = false;
	if (GPR[FRAME] != JIT_NOREG)
	    regval = GPR[FRAME];
	else
	    regval = JIT_R0;
    }

    /* When reloading, the base register may change */
    assert((op->t & (t_spill|t_register)) == (t_spill|t_register));
    op->t &= ~t_spill;
    /* Force usage of same register, or register set to op */
    if (same)
	regno = op->u.w;
    /* Cannot spill any register */
    else {
	regno = get_register(false);
	op->u.w = regno;
    }
    switch (emit_get_type(op)) {
	case t_void:
	    load_r(regno);
	    if (regval == JIT_R0 && global)
		jit_movi(JIT_R0, (oword_t)gd + gc_offset(op));
	    else {
		if (regval == JIT_R0)
		    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
		jit_addi(JIT_R0, regval, gc_offset(op));
	    }
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    jit_pushargr(JIT_R0);
	    jit_finishi(ovm_load);
	    break;
	case t_half:	case t_word:
	    if (regval == JIT_R0) {
		if (global)
		    jit_movi(JIT_R0, (oword_t)gd);
		else
		    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    }
	    jit_ldxi(GPR[regno], regval, wf_offset(op));
	    break;
	case t_single:	case t_float:
	    if (regval == JIT_R0) {
		if (global)
		    jit_movi(JIT_R0, (oword_t)gd);
		else
		    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	    }
	    jit_ldxi_d(FPR[regno], regval, wf_offset(op));
	    break;
	default:
	    abort();
    }
}

static void
emit_save(ooperand_t *op, obool_t force)
{
    oword_t		regno;
    obool_t		global;
    oword_t		regval;

    if (current_record == root_record) {
	global = true;
	if (GPR[GLOBAL] != JIT_NOREG)
	    regval = GPR[GLOBAL];
	else
	    regval = JIT_R0;
    }
    else {
	global = false;
	if (GPR[FRAME] != JIT_NOREG)
	    regval = GPR[FRAME];
	else
	    regval = JIT_R0;
	assert(op->s != 0);
    }

    assert((op->t & (t_register|t_spill)) == t_register);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_void:
	    load_r(regno);
	    if (regval == JIT_R0 && global)
		jit_movi(JIT_R0, (oword_t)gd + gc_offset(op));
	    else {
		if (regval == JIT_R0)
		    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
		jit_addi(JIT_R0, regval, gc_offset(op));
	    }
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    jit_pushargr(JIT_R0);
	    jit_pushargi(t_void);
	    emit_finish(ovm_store, mask1(regno));
	    op->t |= t_spill;
	    break;
	case t_half:	case t_word:
	    if (force || !jit_callee_save_p(regno)) {
		if (regval == JIT_R0) {
		    if (global)
			jit_movi(JIT_R0, (oword_t)gd);
		    else
			jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
		}
		jit_stxi(wf_offset(op), regval, GPR[regno]);
		op->t |= t_spill;
	    }
	    break;
	case t_single:	case t_float:
	    if (force || !jit_callee_save_p(regno)) {
		if (regval == JIT_R0) {
		    if (global)
			jit_movi(JIT_R0, (oword_t)gd);
		    else
			jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
		}
		jit_stxi_d(wf_offset(op), regval, FPR[regno]);
		op->t |= t_spill;
	    }
	    break;
	default:
	    abort();
    }
}

static void
load_w(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi(GPR[regno], JIT_V0, offset + offsetof(oregister_t, v.w));
}

static void
load_w_w(oword_t regno, oword_t regval)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi(GPR[regval], JIT_V0, offset + offsetof(oregister_t, v.w));
}

static void
loadif_w(oword_t regno)
{
    jit_node_t		*node;
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi_i(JIT_R0, JIT_V0, offset + offsetof(oregister_t, t));
    node = jit_bnei(JIT_R0, t_word);
    jit_ldxi(GPR[regno], JIT_V0, offset + offsetof(oregister_t, v.w));
    jit_patch(node);
}

static void
load_d(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi_d(FPR[regno], JIT_V0, offset + offsetof(oregister_t, v.d));
}

static void
loadif_d(oword_t regno)
{
    jit_node_t		*node;
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi_i(JIT_R0, JIT_V0, offset + offsetof(oregister_t, t));
    node = jit_bnei(JIT_R0, t_word);
    jit_ldxi_d(FPR[regno], JIT_V0, offset + offsetof(oregister_t, v.w));
    jit_patch(node);
}

static void
loadif_f(oword_t regno)
{
    jit_node_t		*node;
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_ldxi_i(JIT_R0, JIT_V0, offset + offsetof(oregister_t, t));
    node = jit_bnei(JIT_R0, t_word);
    jit_ldxi_d(FPR[regno], JIT_V0, offset + offsetof(oregister_t, v.w));
    jit_extr_d_f(FPR[regno], FPR[regno]);
    jit_patch(node);
}

static void
load_r(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_addi(GPR[regno], JIT_V0, offset);
}

static void
load_r_w(oword_t regno, oword_t regval)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_addi(regval, JIT_V0, offset);
}

static void
sync_w(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, t_word);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_stxi(offset + offsetof(oregister_t, v.w), JIT_V0, GPR[regno]);
}

static void
spill_w(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_stxi(offset + offsetof(oregister_t, v.w), JIT_V0, GPR[regno]);
}

static void
sync_w_w(oword_t regno, oword_t regval)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_stxi(offset + offsetof(oregister_t, v.w), JIT_V0, regval);
    jit_movi(JIT_R0, t_word);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
}

static void
sync_d(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, t_float);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_stxi_d(offset + offsetof(oregister_t, v.w), JIT_V0, FPR[regno]);
}

static void
spill_d(oword_t regno)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_stxi_d(offset + offsetof(oregister_t, v.w), JIT_V0, FPR[regno]);
}

static void
sync_d_w(oword_t regno, oword_t regval)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, t_float);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_stxi_d(offset + offsetof(oregister_t, v.w), JIT_V0, regval);
}

static void
sync_uw(oword_t regno)
{
    jit_node_t		*jump;

    sync_w(regno);
    jump = jit_bgei(GPR[regno], 0);
    sync_uw_z(regno);
    jit_patch(jump);
}

static void
sync_uw_z(oword_t regno)
{
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, t_mpz);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_addi(JIT_R0, JIT_V0,
	     offset + offsetof(oregister_t, qq) +
	     offsetof(__cqq_struct, re) + offsetof(__mpq_struct, _mp_num));
    jit_prepare();
    jit_pushargr(JIT_R0);
    jit_pushargr(GPR[regno]);
    emit_finish(mpz_set_ui, mask1(regno));
}

#if __WORDSIZE == 32
static void
sync_ww(oword_t regno)
{
    jit_node_t		*jump;
    jit_node_t		*zhjmp;
    jit_node_t		*pljmp;
    jit_node_t		*nhjmp;
    jit_node_t		*nljmp;
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    if (!cfg_optsize) {
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  else
	jit_ldr(JIT_R0, GPR[regno]);
#  endif
	zhjmp = jit_bnei(JIT_R0, 0);
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldr(JIT_R0, GPR[regno]);
#  else
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  endif
	/* done if top word is zero and low word is positive */
	pljmp = jit_bgei(JIT_R0, 0);
	/* convert to mpz */
	jump = jit_jmpi();
	/* top word is not zero */
	jit_patch(zhjmp);
	nhjmp = jit_bnei(JIT_R0, -1);
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  else
	jit_ldr(JIT_R0, GPR[regno]);
#  endif
	/* done if top word is -1 and low word is negative */
	nljmp = jit_bgei(JIT_R0, 0);
	/* Convert to mpz */
	jit_patch(jump);
	jit_patch(nhjmp);
    }

    jit_addi(JIT_R0, JIT_V0, offset);
    jit_prepare();
    jit_pushargr(JIT_R0);
    jit_pushargr(GPR[regno]);
    emit_finish(ovm_load_ww, mask1(regno));

    if (!cfg_optsize) {
	jump = jit_jmpi();
	/* fits in a word */
	jit_patch(pljmp);
	jit_patch(nljmp);
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldr (GPR[regno], GPR[regno]);
#  else
	jit_ldxi(GPR[regno], GPR[regno], 4);
#  endif
	sync_w(regno);
	jit_patch(jump);
    }
}

static void
sync_uwuw(oword_t regno)
{
    jit_node_t		*jump;
    jit_node_t		*zjmp;
    jit_node_t		*njmp;
    oword_t		 offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    if (cfg_optsize) {
	jit_addi(JIT_R0, JIT_V0, offset);
	jit_prepare();
	jit_pushargr(JIT_R0);
	jit_pushargr(GPR[regno]);
	emit_finish(ovm_load_uwuw, mask1(regno));
    }
    else {
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  else
	jit_ldr (JIT_R0, GPR[regno]);
#  endif
	zjmp = jit_bnei(JIT_R0, 0);
	/* fits in an unsigned word */
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldr (JIT_R0, GPR[regno]);
#  else
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  endif
	njmp = jit_blti(JIT_R0, 0);
	jit_movr(GPR[regno], JIT_R0);
	sync_w(regno);
	jump = jit_jmpi();
	jit_patch(zjmp);
	jit_patch(njmp);
	/* does not fit in a signed word */
	jit_addi(JIT_R0, JIT_V0,
		 offset + offsetof(oregister_t, qq) +
		 offsetof(__cqq_struct, re) + offsetof(__mpq_struct, _mp_num));
	jit_prepare();
	jit_pushargr(JIT_R0);
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldr (JIT_R0, GPR[regno]);
#  else
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  endif
	jit_pushargr(JIT_R0);
#  if __BYTE_ORDER == __LITTLE_ENDIAN
	jit_ldxi(JIT_R0, GPR[regno], 4);
#  else
	jit_ldr (JIT_R0, GPR[regno]);
#  endif
	jit_pushargr(JIT_R0);
	emit_finish(ompz_set_uiui, mask1(regno));
	jit_movi(JIT_R0, t_mpz);
	jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
	jit_patch(jump);
    }
}
#endif

static void
sync_r(oword_t regno, oint32_t type)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, type);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_stxi(offset + offsetof(oregister_t, v.w), JIT_V0, GPR[regno]);
}

static void
sync_r_w(oword_t regno, oword_t regval, oint32_t type)
{
    oword_t		offset;
    switch (regno) {
	case 0:		offset = offsetof(othread_t, r0);	break;
	case 1:		offset = offsetof(othread_t, r1);	break;
	case 2:		offset = offsetof(othread_t, r2);	break;
	case 3:		offset = offsetof(othread_t, r3);	break;
	default:	abort();
    }
    jit_movi(JIT_R0, type);
    jit_stxi_i(offset + offsetof(oregister_t, t), JIT_V0, JIT_R0);
    jit_stxi(offset + offsetof(oregister_t, v.w), JIT_V0, regval);
}

static oword_t
get_register(obool_t spill)
{
    oword_t		 regno;
    oword_t		 offset;
    ooperand_t		*operand;

    for (regno = 0; regno < 4; regno++) {
	for (offset = 0; offset < stack->offset; offset++) {
	    operand = stack->v.ptr[offset];
	    if (( operand->t & t_register) &&
		  operand->u.w == regno &&
		!(operand->t & t_spill))
		/* Register is live and not spilled */
		break;
	}
	/* Register is free */
	if (offset == stack->offset)
	    return (regno);
    }

    /* Should never happen */
    if (!spill)
	oerror("internal error: out of registers");

    /*  No register found; spill first match */
    for (offset = 0; offset < stack->offset; offset++) {
	operand = stack->v.ptr[offset];
	if ((operand->t & (t_spill|t_register)) == t_register) {
	    regno = operand->u.w;
	    break;
	}
    }

    assert(offset < stack->offset);
    emit_save(operand, false);

    return (regno);
}

static void
emit_finish(oobject_t code, oword_t mask)
{
    mask = sync_registers(mask);
    jit_finishi(code);
    load_registers(mask);
}

static void
emit_save_operands(void)
{
    oword_t		 offset;
    ooperand_t		*operand;

    for (offset = 0; offset < stack->offset; offset++) {
	operand = stack->v.ptr[offset];
	if ((operand->t & (t_register|t_spill)) == t_register)
	    emit_save(operand, true);
    }
}

static oword_t
sync_registers(oword_t mask)
{
    ooperand_t		*op;
    otype_t		 type;
    oword_t		 regno;
    oword_t		 offset;
    oword_t		 result;

    result = 0;
    mask |= reg_mask;
    assert((mask & ~0xf) == 0);
    mask ^= 0xf;
    for (offset = stack->offset - 1; mask && offset >= 0; offset--) {
	op = stack->v.ptr[offset];
	if ((op->t & (t_spill|t_register)) == t_register) {
	    regno = op->u.w;
	    assert(!(regno & ~0xf));
	    if (mask & (0x01 << regno)) {
		type = emit_get_type(op);
		if (type == t_half || type == t_word) {
		    sync_w(regno);
		    result |= 0x001 << regno;
		}
		else if (type == t_single || type == t_float) {
		    sync_d(regno);
		    result |= 0x010 << regno;
		}
		else if (SPL[regno]) {
		    jit_stxi(SPL[regno], JIT_FP, GPR[regno]);
		    result |= 0x100 << regno;
		}
		mask &= ~(1 << regno);
	    }
	}
    }
    if (tmp_mask & 1)
	jit_stxi(SPL[TMP0], JIT_FP, GPR[TMP0]);
    if (tmp_mask & 2)
	jit_stxi(SPL[TMP1], JIT_FP, GPR[TMP1]);

    return (result);
}

static void
load_registers(oword_t mask)
{
    oword_t		regno;

    for (regno = 0; regno < 4; regno++) {
	if (mask & (0x001 << regno))
	    load_w(regno);
	if (mask & (0x010 << regno))
	    load_d(regno);
	if (mask & (0x100 << regno))
	    jit_ldxi(GPR[regno], JIT_FP, SPL[regno]);
    }
    if (tmp_mask & 1)
	jit_ldxi(GPR[TMP0], JIT_FP, SPL[TMP0]);
    if (tmp_mask & 2)
	jit_ldxi(GPR[TMP1], JIT_FP, SPL[TMP1]);
}
