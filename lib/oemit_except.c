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

#if defined(PROTO)
static void
load_exception(jit_int32_t regval, oword_t offset);

static void
store_exception(jit_int32_t regval, jit_int32_t ipreg);

static void
unwind_exception(jit_int32_t regval, obool_t unsafe);

static void
emit_catch(oast_t *ast, jit_node_t *final);

static void
emit_try(oast_t *ast);

static void
emit_throw(oast_t *ast);
#endif

#if defined(CODE)
static void
load_exception(jit_int32_t regval, oword_t offset)
{
    if (current_record == root_record) {
	if (GPR[GLOBAL] != JIT_NOREG)
	    jit_addi(regval, GPR[GLOBAL], offset);
	else
	    jit_movi(regval, (oword_t)gd + offset);
    }
    else {
	if (GPR[FRAME] != JIT_NOREG)
	    jit_addi(regval, GPR[FRAME], offset);
	else {
	    jit_ldxi(regval, JIT_V0, offsetof(othread_t, fp));
	    jit_addi(regval, regval, offset);
	}
    }
}

static void
store_exception(jit_int32_t regval, jit_int32_t ipreg)
{
    jit_stxi(offsetof(oexception_t, ip), regval, ipreg);
    /* Current fp */
    if (GPR[FRAME] != JIT_NOREG)
	jit_stxi(offsetof(oexception_t, fp), regval, GPR[FRAME]);
    else {
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, fp));
	jit_stxi(offsetof(oexception_t, fp), regval, JIT_R0);
    }
    /* Current sp */
    if (GPR[STACK] != JIT_NOREG)
	jit_stxi(offsetof(oexception_t, sp), regval, GPR[STACK]);
    else {
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, sp));
	jit_stxi(offsetof(oexception_t, sp), regval, JIT_R0);
    }
    /* Current this (if there is a this register) */
    if (GPR[THIS] != JIT_NOREG)
	jit_stxi(offsetof(oexception_t, th), regval, GPR[THIS]);
    /* Previous exception frame */
    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ex));
    jit_stxi(offsetof(oexception_t, ex), regval, JIT_R0);
    /* Update current exception frame */
    jit_stxi(offsetof(othread_t, ex), JIT_V0, regval);
}

static void
unwind_exception(jit_int32_t regval, obool_t unsafe)
{
    jit_int32_t		 frame;
    jit_int32_t		 stack;
    jit_node_t		*equal;

    /* Need to restore and resync thread state */
    frame = GPR[FRAME] != JIT_NOREG ? GPR[FRAME] : JIT_R0;
    stack = GPR[STACK] != JIT_NOREG ? GPR[STACK] : JIT_R0;
    /* Restore fp */
    jit_ldxi(frame, regval, offsetof(oexception_t, fp));
    jit_stxi(offsetof(othread_t, fp), JIT_V0, frame);
    /* Restore sp */
    if (unsafe) {
	/* If throw may be from another vm stack frame, need to
	 * zero memory, or it may later be cast to the wrong object;
	 * the logic is somewhat optimized, to zero memory before
	 * function return instead of function call, but exceptions
	 * break all rules. */
	if (GPR[STACK] != JIT_NOREG) {
	    jit_ldxi(GPR[STACK], regval, offsetof(oexception_t, sp));
	    jit_ldxi(JIT_R1, JIT_V0, offsetof(othread_t, sp));
	    equal = jit_beqr(GPR[STACK], JIT_R1);
	    jit_subr(JIT_R0, GPR[STACK], JIT_R1);
	    jit_prepare();
	    jit_pushargr(JIT_R1);
	    jit_pushargi(0);
	    jit_pushargr(JIT_R0);
	    jit_finishi(memset);
	    jit_patch(equal);
	    jit_stxi(offsetof(othread_t, sp), JIT_V0, GPR[STACK]);
	}
	else {
	    jit_ldxi(JIT_R0, regval, offsetof(oexception_t, sp));
	    jit_ldxi(JIT_R1, JIT_V0, offsetof(othread_t, sp));
	    equal = jit_beqr(JIT_R0, JIT_R1);
	    jit_subr(JIT_R0, JIT_R0, JIT_R1);
	    jit_prepare();
	    jit_pushargr(JIT_R1);
	    jit_pushargi(0);
	    jit_pushargr(JIT_R0);
	    jit_finishi(memset);
	    jit_ldxi(JIT_R0, regval, offsetof(oexception_t, sp));
	    jit_patch(equal);
	    jit_stxi(offsetof(othread_t, sp), JIT_V0, JIT_R0);
	}
    }
    else {
	jit_ldxi(stack, regval, offsetof(oexception_t, sp));
	jit_stxi(offsetof(othread_t, sp), JIT_V0, stack);
    }
    /* Restore this (if there is a this register) */
    if (GPR[THIS] != JIT_NOREG)
	jit_ldxi(GPR[THIS], regval, offsetof(oexception_t, th));
    /* Restore exception frame */
    jit_ldxi(JIT_R0, regval, offsetof(oexception_t, ex));
    jit_stxi(offsetof(othread_t, ex), JIT_V0, JIT_R0);
}

static void
emit_catch(oast_t *ast, jit_node_t *final)
{
    ooperand_t		*op;
    oast_t		*decl;
    jit_node_t		*jump;

    decl = ast->l.ast->r.ast;

    /* Load exception object from thread state */
    op = operand_get(0);
    op->t = t_register|t_void;
    op->u.w = 0;
    load_r(op->u.w);
    jit_addi(JIT_R0, JIT_V0, offsetof(othread_t, ev));
    jit_prepare();
    jit_pushargr(GPR[op->u.w]);
    jit_pushargr(JIT_R0);
    jit_finishi(ovm_load);

    /* Store exception object on catch block symbol */
    assert(decl->token == tok_symbol);
    store_symbol(decl->l.value, op);
    operand_unget(1);

    emit_stat(ast->r.ast);
    jump = jit_jmpi();
    jit_patch_at(jump, final);
}

static void
emit_try(oast_t *ast)
{
    jit_node_t		*jmp;
    jit_node_t		*hnd;
    jit_node_t		*final;
    ocase_t		*acase;
    oword_t		 offset;
    ovector_t		*vector;

    /* All registers are dead */

    /* Build new exception frame */
    load_exception(GPR[0], ast->offset);
    /* Where throw will land */
    hnd = jit_movi(JIT_R0, 0);
    store_exception(GPR[0], JIT_R0);

    /* Emit try body code */
    emit(ast->r.ast);

    /* Unwind if no exception happened */
    load_exception(GPR[0], ast->offset);
    unwind_exception(GPR[0], false);
    /* Done */
    jmp = jit_jmpi();
    final = jit_forward();
    jit_patch_at(jmp, final);

    /* Throw lands here */
    jit_patch(hnd);

    /* Unwind before proceeding */
    jit_ldxi(GPR[0], JIT_V0, offsetof(othread_t, ex));
    unwind_exception(GPR[0], true);

    /* Dispatch exception */
    vector = ast->l.value;
    if (vector->offset > 1) {
	/* Get exception object type */
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, ev));
	jit_ldxi_i(JIT_R0, JIT_R0, OBJECT_TYPE_OFFSET);
	for (offset = 1; offset < vector->offset; offset++) {
	    acase = vector->v.ptr[offset];
	    acase->jit = jit_beqi(JIT_R0, acase->lval);
	}
	/* Fallback if all typed catchers failed */
	jmp = jit_jmpi();
	/* Emit catch handlers */
	for (offset = 1; offset < vector->offset; offset++) {
	    acase = vector->v.ptr[offset];
	    jit_patch(acase->jit);
	    emit_catch(acase->ast, final);
	}
	jit_patch(jmp);
    }
    else
	jmp = null;

    /* If there is an "auto" exception catcher */
    if ((acase = vector->v.ptr[0])) {
	acase->jit = jmp;
	emit_catch(acase->ast, final);
    }
    else {
	jit_ldxi(GPR[0], JIT_V0, offsetof(othread_t, ex));
	/* Check if there is a previous exception handler */
	hnd = jit_beqi(GPR[0], 0);

	/* Rethrow to previous exception frame */
	jit_ldxi(JIT_R0, GPR[0], offsetof(oexception_t, ip));
	jit_jmpr(JIT_R0);

	jit_patch(hnd);
	jit_prepare();
	jit_pushargi(except_unhandled_exception);
	jit_finishi(ovm_raise);
    }

    /* Finally */
    jit_link(final);
    if (ast->t.ast)
	emit_stat(ast->t.ast->l.ast);
}

static void
emit_throw(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jmp;

    if (ast->l.ast) {
	/* Throw argument */
	emit(ast->l.ast);
	op = operand_top();
	emit_load(op);
	/* Operand stack should be empty */
	assert(op->u.w == 0);
	switch (emit_get_type(op)) {
	    case t_half:	case t_word:
		sync_w(op->u.w);
		break;
	    case t_single:
		jit_extr_f_d(FPR[op->u.w], FPR[op->u.w]);
	    case t_float:
		sync_d(op->u.w);
		break;
	    default:
		break;
	}
	operand_unget(1);

	/* Store exception object in thread state */
	load_r(op->u.w);
	jit_addi(JIT_R0, JIT_V0, offsetof(othread_t, ev));
	jit_prepare();
	jit_pushargr(GPR[op->u.w]);
	jit_pushargr(JIT_R0);
	jit_pushargi(t_void);
	jit_finishi(ovm_store);
    }
    /* else rethrow current exception */

    /* Get pointer to exception handler */
    jit_ldxi(GPR[0], JIT_V0, offsetof(othread_t, ex));

    /* Check there is a handler */
    jmp = jit_beqi(GPR[0], 0);

    /* Dispatch */
    jit_ldxi(JIT_R0, GPR[0], offsetof(oexception_t, ip));
    jit_jmpr(JIT_R0);

    /* If no exception handler */
    jit_patch(jmp);
    jit_prepare();
    jit_pushargi(except_unhandled_exception);
    jit_finishi(ovm_raise);
}
#endif
