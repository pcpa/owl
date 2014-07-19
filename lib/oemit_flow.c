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
emit_question(oast_t *ast);

static void
emit_if(oast_t *ast);

static void
emit_while(oast_t *ast);

static void
emit_do(oast_t *ast);

static void
emit_for(oast_t *ast);

static void
emit_break(oast_t *ast);

static void
emit_continue(oast_t *ast);

static void
emit_switch(oast_t *ast);

static void
emit_case(oast_t *ast);

static void
emit_function(ofunction_t *function);

static void
emit_return(oast_t *ast);

static void
emit_thread(oast_t *ast);

static void
emit_call(oast_t *ast);

static void
emit_call_next(ooperand_t *rop,
	       ofunction_t *function, oast_t *alist,
	       ooperand_t *top,
	       obool_t vcall, obool_t ecall, obool_t builtin, obool_t thread);
#endif

#if defined(CODE)
/* Spill/reload registers used as temporaries without overwriting
 * actual language values storing in the thread state, and not the
 * stack */
#define stack_spill_w(r)	jit_stxi(SPL[r], JIT_FP, GPR[r])
#define stack_load_w(r)		jit_ldxi(GPR[r], JIT_FP, SPL[r])
static void
emit_question(oast_t *ast)
{
    oword_t		 lty;
    oword_t		 rty;
    ooperand_t		*lop;
    ooperand_t		*rop;
    oword_t		 lreg;
    oword_t		 rreg;
    ojump_t		*jump;
    jit_node_t		*node;
    jit_node_t		*fixup;
    jit_node_t		*label;
    obool_t		 match;

    jump = jump_get(tok_question);
    emit_test(ast->t.ast, false, 0);

    /* true test short circuit */
    if (jump->t->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }
    /* true expression */
    emit(ast->l.ast);
    lop = operand_top();
    emit_load(lop);
    lty = emit_get_type(lop);
    node = jit_jmpi();

    /* false test jump target */
    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    /* Pretend the operand is spilled to "release" the register
     * so that other code does not need to understand that the
     * execution of left and right expressions are exclusive */
    lop->t |= t_spill;

    /* false expression */
    emit(ast->r.ast);
    rop = operand_top();
    emit_load(rop);
    rty = emit_get_type(rop);
    lreg = lop->u.w;
    rreg = rop->u.w;

    /*   Due to type promotion rules, cannot really make the language
     * cause a fatal error if true and false expressions type result
     * differ. Nevertheless, try the best to avoid the need of sync/load
     * of expression results. If type mismatch was not allowed, could
     * arrange to not need to move the result of the false expression
     * to the same register as the true expression result. */
    match = true;
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word)
	    jit_movr(GPR[lreg], GPR[rreg]);
	else {
	    if (rty == t_single) {
		jit_extr_f_d(FPR[lreg], FPR[rreg]);
		sync_d(lreg);
	    }
	    else if (rty == t_float)
		sync_d_w(lreg, FPR[rreg]);
	    match = false;
	}
    }
    else if (lty == t_single) {
	if (rty == t_single)
	    jit_movr_f(FPR[lreg], FPR[rreg]);
	else if (rty == t_half || rty == t_word)
	    jit_extr_f(FPR[lreg], GPR[rreg]);
	else {
	    if (rty == t_float) {
		jit_extr_f_d(FPR[lreg], FPR[rreg]);
		sync_d(lreg);
	    }
	    match = false;
	}
    }
    else if (lty == t_float) {
	if (rty == t_float)
	    jit_movr_d(FPR[lreg], FPR[rreg]);
	else if (rty == t_half || rty == t_word)
	    jit_extr_d(FPR[lreg], GPR[rreg]);
	else if (rty == t_single)
	    jit_extr_f_d(FPR[lreg], FPR[rreg]);
	else
	    match = false;
    }
    else {
	if (rty == t_half || rty == t_word)
	    sync_w_w(lreg, GPR[rreg]);
	else if (rty == t_single) {
	    jit_extr_f_d(FPR[lreg], FPR[rreg]);
	    sync_d(lreg);
	}
	else if (rty == t_float)
	    sync_d_w(lreg, FPR[rreg]);
	match = false;
    }

    /* If type conversion/coercion is complex */
    if (match == false) {
	if (rty == t_void && lreg != rreg) {
	    load_r(lreg);
	    load_r_w(rreg, JIT_R0);
	    jit_prepare();
	    jit_pushargr(GPR[lreg]);
	    jit_pushargr(JIT_R0);
	    emit_finish(ovm_move, mask1(lreg));
	}
	if (lty == t_half || lty == t_word ||
	    lty == t_single || lty == t_float)
	    /* Need to sync true expression result to memory due to type
	     * mismatch (or possible type mismatch due to overflow
	     * prevention) */
	    fixup = jit_jmpi();
	else
	    fixup = null;
    }

    /* True expression lands here */
    label = jit_label();
    jit_patch_at(node, label);

    /* If type conversion/coercion is complex */
    if (match == false) {
	if (fixup) {
	    if (lty == t_half || lty == t_word)
		sync_w(lreg);
	    else {
		if (lty == t_single)
		    jit_extr_f_d(FPR[lreg], FPR[lreg]);
		else
		    assert(lty == t_float);
		sync_d(lreg);
	    }
	    jit_patch(fixup);
	}
	emit_set_type(lop, t_void);
    }

    /* Do not get confused with the spill flag, that could attempt
     * to reload the value in another register. */
    lop->t &= ~t_spill;

    operand_unget(1);
    jump_unget(1);
}

static void
emit_if(oast_t *ast)
{
    ojump_t		*jump;
    oast_t		*test;
    jit_node_t		*node;
    jit_node_t		*label;
    oword_t		 offset;

    node = null;

    /* test */
    offset = stack->offset;
    for (test = ast->t.ast; test->next; test = test->next) {
	emit(test);
	operand_reset(offset);
    }
    jump = jump_get(tok_if);
    emit_test(test, false, 0);
    operand_reset(offset);

    /* true test condition short circuit */
    if (jump->t->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }

    /* true condition code */
    emit_stat(ast->l.ast);

    if (ast->r.ast)
	/* jump past false condition code */
	node = jit_jmpi();

    /* false test condition target */
    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    if (ast->r.ast) {
	/* false condition code */
	emit_stat(ast->r.ast);
	jit_patch(node);
    }

    jump_unget(1);
}

static void
emit_while(oast_t *ast)
{
    ojump_t		*jump;
    oast_t		*test;
    jit_node_t		*node;
    jit_node_t		*label;
    oword_t		 offset;

    /* loop start, before test */
    label = jit_label();
    offset = stack->offset;
    for (test = ast->t.ast; test->next; test = test->next) {
	emit(test);
	operand_reset(offset);
    }

    /* test */
    jump = jump_get(tok_while);
    emit_test(test, false, 0);
    operand_reset(offset);

    /* test true short circuit */
    if (jump->t->offset) {
	node = jit_label();
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], node);
	while (jump->t->offset);
    }

    /* body */
    emit_stat(ast->c.ast);

    /* restart loop */
    node = jit_jmpi();
    jit_patch_at(node, label);

    /* continue */
    if (jump->t->offset) {
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }

    /* break and false test condition */
    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    jump_unget(1);
}

static void
emit_do(oast_t *ast)
{
    ojump_t		*jump;
    oast_t		*test;
    jit_node_t		*node;
    jit_node_t		*label;
    oword_t		 offset;

    /* loop start */
    label = jit_label();
    offset = stack->offset;
    jump = jump_get(tok_do);

    /* body */
    emit_stat(ast->c.ast);

    /* test */
    for (test = ast->t.ast; test->next; test = test->next) {
	emit(test);
	operand_reset(offset);
    }
    emit_test(test, false, 0);
    operand_reset(offset);

    /* test true short circuit */
    if (jump->t->offset) {
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }

    /* restart loop */
    node = jit_jmpi();
    jit_patch_at(node, label);

    /* continue */
    if (jump->t->offset) {
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }

    /* break and false test condition */
    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    jump_unget(1);
}

static void
emit_for(oast_t *ast)
{
    ojump_t		*jump;
    oast_t		*test;
    jit_node_t		*node;
    jit_node_t		*label;
    oword_t		 offset;

    node = null;

    /* init expression */
    emit_stat(ast->l.ast);

    /* If there is an increment expression, jump over it after the init one */
    if (ast->r.ast)
	node = jit_jmpi();

    /* loop start */
    label = jit_label();

    /* increment expression */
    if (ast->r.ast) {
	emit_stat(ast->r.ast);
	/* actual loop start if there are init and increment expressions */
	jit_patch(node);
    }

    /* test */
    offset = stack->offset;
    if ((test = ast->t.ast)) {
	for (; test->next; test = test->next) {
	    emit(test);
	    operand_reset(offset);
	}
    }
    jump = jump_get(tok_for);
    /* test is optional */
    if (test) {
	emit_test(test, false, 0);
	operand_reset(offset);
	/* test true short circuit */
	if (jump->t->offset) {
	    node = jit_label();
	    do
		jit_patch_at(jump->t->v.ptr[--jump->t->offset], node);
	    while (jump->t->offset);
	}
    }

    /* body */
    emit_stat(ast->c.ast);

    /* restart loop */
    node = jit_jmpi();
    jit_patch_at(node, label);

    /* continue */
    if (jump->t->offset) {
	do
	    jit_patch_at(jump->t->v.ptr[--jump->t->offset], label);
	while (jump->t->offset);
    }

    /* break and false test condition */
    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    jump_unget(1);
}

static void
emit_break(oast_t *ast)
{
    jit_node_t		*node;
    ojump_t		*jump;
    oword_t		 offset;

    for (offset = branch->offset - 1; offset >= 0; offset--) {
	jump = branch->v.ptr[offset];
	switch (jump->k) {
	    case tok_do:	case tok_for:
	    case tok_while:	case tok_switch:
		node = jit_jmpi();
		add_jump(jump->f, node);
		return;
	    default:
		break;
	}
    }
    /* break outside switch or loop already tested */
    abort();
}

static void
emit_continue(oast_t *ast)
{
    jit_node_t		*node;
    ojump_t		*jump;
    oword_t		 offset;

    for (offset = branch->offset - 1; offset >= 0; offset--) {
	jump = branch->v.ptr[offset];
	switch (jump->k) {
	    case tok_do:	case tok_for:
	    case tok_while:
		node = jit_jmpi();
		add_jump(jump->t, node);
		return;
	    default:
		break;
	}
    }
    /* continue outside loop already tested */
    abort();
}

static void
emit_switch(oast_t *ast)
{
    ooperand_t		*op;
    otype_t		 type;
    ojump_t		*jump;
    oast_t		*test;
    jit_node_t		*ljmp;
    jit_node_t		*rjmp;
    jit_node_t		*done;
    ocase_t		*acase;
    jit_node_t		*label;
    oword_t		 regno;
    oword_t		 offset;
    ovector_t		*vector;

    /* test */
    offset = stack->offset;
    for (test = ast->t.ast; test->next; test = test->next) {
	emit(test);
	operand_reset(offset);
    }
    emit(test);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    type = emit_get_type(op);
    if (type != t_half && type != t_word) {
	if (type == t_single) {
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	    sync_d(regno);
	}
	else if (type == t_float)
	    sync_d(regno);
	load_r(regno);
	jit_prepare();
	jit_pushargr(GPR[regno]);
	emit_finish(ovm_case, mask1(regno));
	/* exception should be generated if value is not a 32 bit integer */
	load_w(regno);
	emit_set_type(op, t_word);
    }

    /* dumb mode, compare every case and case range */
    vector = ast->l.value;
    for (offset = 1; offset < vector->offset; offset++) {
	acase = vector->v.ptr[offset];
	if (acase->lval == acase->rval)
	    acase->jit = jit_beqi(GPR[regno], acase->lval);
	else {
	    ljmp = jit_blti(GPR[regno], acase->lval);
	    rjmp = jit_bgti(GPR[regno], acase->rval);
	    acase->jit = jit_jmpi();
	    jit_patch(ljmp);
	    jit_patch(rjmp);
	}
    }

    /* free register */
    operand_unget(1);

    done = jit_jmpi();
    /* if every check failed and there is a default entry */
    if ((acase = vector->v.ptr[0]))
	acase->jit = done;

    /* body */
    jump = jump_get(tok_switch);
    emit(ast->c.ast);

    /* only break allowed */
    assert(jump->t->offset == 0);

    /* break */
    if (jump->f->offset) {
	label = jit_label();
	if (acase == null)
	    jit_patch_at(done, label);
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }
    else if (acase == null) {
	label = jit_label();
	if (acase == null)
	    jit_patch_at(done, label);
    }

    jump_unget(1);
}

static void
emit_case(oast_t *ast)
{
    ojump_t		*jump;
    ocase_t		*acase;
    jit_node_t		*label;
    oword_t		 offset;

    for (offset = branch->offset - 1; offset >= 0; offset--) {
	jump = branch->v.ptr[offset];
	if (jump->k == tok_switch) {
	    label = jit_label();
	    acase = ast->l.value;
	    jit_patch_at(acase->jit, label);
	    return;
	}
    }
    /* case outside switch already tested */
    abort();
}

static void
emit_function(ofunction_t *function)
{
    oast_t		*ast;
    jit_int32_t		 ret;
    ojump_t		*jump;
    otype_t		 type;
    jit_int32_t		 save;
    jit_int32_t		 frame;
    jit_int32_t		 stack;
    jit_node_t		*label;
    oword_t		 length;
    ofunction_t		*parent;
    ovector_t		*vector;
    jit_node_t		*overflow;
    char		 name[1024];

    vector = oget_namespace_string(function->name);
    if ((length = vector->offset) >= sizeof(name) - 1)
	length = sizeof(name) - 1;
    memcpy(name, vector->v.obj, length);
    name[length] = '\0';
    function->address = jit_name(name);

    /* Entry point */
    jit_link(function->patch);

    ast = function->ast->l.ast;
    if (ast->note.name != note.name || ast->note.lineno != note.lineno)
	emit_note(&ast->note);

    current_function = function;
    current_record = function->record;

    if ((frame = GPR[FRAME]) == JIT_NOREG) {
	frame = GPR[0];
	jit_ldxi(frame, JIT_V0, offsetof(othread_t, fp));
    }
    if ((stack = GPR[STACK]) == JIT_NOREG) {
	stack = GPR[1];
	jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));
    }
    jit_stxi(PREV_OFFSET, stack, frame);
    jit_movr(frame, stack);

    /* Stack frame set, no stack frame being built */
    jit_movi(JIT_R0, 0);
    jit_stxi(NEXT_OFFSET, frame, JIT_R0);

    if (function->framesize) {
	/* Check if stack may overflow during function execution */
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, bp));
	jit_addi(JIT_R0, JIT_R0, function->framesize);
	overflow = jit_bltr_u(JIT_R0, stack);
	jit_prepare();
	jit_pushargi(except_stack_overflow);
	jit_finishi(ovm_raise);
	jit_patch(overflow);

	if (function->stack) {
	    /* Update stack pointer */
	    jit_subi(stack, stack, function->stack);
	    jit_stxi(offsetof(othread_t, sp), JIT_V0, stack);
	}
    }

    jit_stxi(offsetof(othread_t, fp), JIT_V0, frame);
    if (GPR[THIS] != JIT_NOREG && function->record->parent != root_record)
	jit_ldxi(GPR[THIS], frame, THIS_OFFSET);

    jump = jump_get(tok_function);

    /* If a constructor, explicitly call parent constructor first */
    if (function->name->ctor && function->name->record->super) {
	if ((parent = oget_constructor(function->name->record->super)))
	    emit_call_next(null, parent, null, null, false, true, false, false);
    }

    emit(function->ast->c.ast);

    assert(jump->t->offset == 0);

    if (jump->f->offset) {
	label = jit_label();
	do
	    jit_patch_at(jump->f->v.ptr[--jump->f->offset], label);
	while (jump->f->offset);
    }

    /* Constructors always 'return this;' (and only this) */
    if (function->name->ctor) {
	if (GPR[FRAME] == JIT_NOREG)
	    jit_ldxi(frame, JIT_V0, offsetof(othread_t, fp));
	if (GPR[THIS] == JIT_NOREG) {
	    jit_ldxi(GPR[0], frame, THIS_OFFSET);
	    sync_r(0, function->name->record->type);
	}
	else
	    sync_r_w(0, GPR[THIS], function->name->record->type);
    }

    vector = function->name->tag->name;
    type = otag_to_type(vector->v.ptr[0]);

    save = GPR[0];
    switch (type) {
	case t_int8:	case t_uint8:
	case t_int16:	case t_uint16:
	case t_int32:	case t_uint32:
#if __WORDSIZE == 64
	case t_int64:	case t_uint64:
#endif
	    stack_spill_w(0);
	    break;
	default:
	    break;
    }

    /* All live registers must have been saved and at exit,
     * only register 0 could/should be live for return value */
    ret = GPR[1];
    if (GPR[FRAME] != JIT_NOREG)
	frame = GPR[FRAME];
    else {
	frame = GPR[2];
	jit_ldxi(frame, JIT_V0, offsetof(othread_t, fp));
    }

    /* Restore stack pointer */
    if (GPR[STACK] != JIT_NOREG) {
	jit_movr(save, GPR[STACK]);
	if (function->varargs) {
	    jit_ldxi_i(JIT_R0, frame, SIZE_OFFSET);
	    jit_addr(GPR[STACK], GPR[STACK], JIT_R0);
	}
	jit_addi(GPR[STACK], GPR[STACK], function->frame + function->stack);
	jit_stxi(offsetof(othread_t, sp), JIT_V0, GPR[STACK]);
    }
    else {
	jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, sp));
	jit_movr(save, JIT_R0);
	if (function->varargs) {
	    jit_ldxi_i(GPR[3], frame, SIZE_OFFSET);
	    jit_addi(GPR[3], GPR[3], function->frame + function->stack);
	    jit_addi(JIT_R0, JIT_R0, GPR[3]);
	}
	else
	    jit_addi(JIT_R0, JIT_R0, function->frame + function->stack);
	jit_stxi(offsetof(othread_t, sp), JIT_V0, JIT_R0);
    }

    jit_ldxi(ret, frame, RET_OFFSET);
    jit_ldxi(frame, frame, PREV_OFFSET);
    jit_stxi(offsetof(othread_t, fp), JIT_V0, frame);

    if ((type == t_single || type == t_float) && !jit_callee_save_p(FPR[0]))
	spill_d(0);
    jit_prepare();
    jit_pushargr(save);
    jit_pushargi(0);
    if (function->varargs) {
	if (GPR[STACK] != JIT_NOREG)
	    jit_subr(JIT_R0, GPR[STACK], save);
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, sp));
	    jit_subr(JIT_R0, JIT_R0, save);
	}
	jit_pushargr(JIT_R0);
    }
    else
	jit_pushargi(function->frame + function->stack);
    jit_finishi(memset);
    if ((type == t_single || type == t_float) && !jit_callee_save_p(FPR[0]))
	load_d(0);
    if (save == GPR[0]) {
	switch (type) {
	    case t_int8:	case t_uint8:
	    case t_int16:	case t_uint16:
	    case t_int32:	case t_uint32:
#if __WORDSIZE == 64
	    case t_int64:	case t_uint64:
#endif
		stack_load_w(0);
		break;
	    default:
		break;
	}
    }

    jit_jmpr(ret);

    jump_unget(1);
}

static void
emit_return(oast_t *ast)
{
    ooperand_t		*op;
    ooperand_t		*rop;
    otag_t		*tag;
    otype_t		 type;
    jit_node_t		*node;
    ojump_t		*jump;
    oword_t		 offset;
    ovector_t		*vector;

    assert(current_record->function->tag->type == tag_function);
    vector = current_record->function->tag->name;
    tag = vector->v.ptr[0];
    type = otag_to_type(tag);
    if (ast->l.ast) {
	if (current_function->name && current_function->name->ctor)
	    oparse_error(ast, "constructor cannot return a value");

	assert(type != t_void);
	if (type > t_float32)
	    type = t_void;
	/* Return must be in a top level statement */
	assert(stack->offset == 0);
	emit(ast->l.ast);
	op = operand_top();
	emit_load(op);
	/* Return must be in register 0 */
	if (op->u.w != 0) {
	    /* In some complex expressions it may be left in another register */
	    rop = operand_get(op->s);
	    operand_copy(rop, op);
	    rop->u.w = get_register(true);
	    assert(rop->u.w == 0);
	}
	else
	    rop = op;
	emit_coerce(type, rop, op);
	operand_unget(rop == op ? 1 : 2);
    }

    for (offset = branch->offset - 1; offset >= 0; offset--) {
	jump = branch->v.ptr[offset];
	if (jump->k == tok_function) {
	    node = jit_jmpi();
	    add_jump(jump->f, node);
	    return;
	}
    }
    /* return outside function already tested */
    abort();
}

/*

frame = @arguments + @stackframe
framesize = frame + word * varargs
next = %sp - framesize

if (depth == 0)
	%fp[NEXT] = next
else
	%sp[NEXT] = next
%sp = next
%sp[TYPE] = type
if (varargs)
	%sp[SIZE] = framesize - frame
++depth
<< parse and push arguments >>
--depth

 */

static void
emit_thread(oast_t *ast)
{
    ooperand_t		*rop;
    ooperand_t		*top;
    oast_t		*list;
    obool_t		 vcall;
    obool_t		 ecall;
    osymbol_t		*symbol;
    obool_t		 builtin;
    ofunction_t		*function;

    ast = ast->l.ast;
    rop = operand_get(ast->offset);
    rop->u.w = get_register(true);
    top = null;
    vcall = ast->l.ast->token == tok_dot;
    ecall = ast->l.ast->token == tok_explicit;

    if (vcall) {
	emit(ast->l.ast->l.ast);
	top = operand_top();
	symbol = ast->l.ast->r.ast->l.value;
    }
    else if (ecall) {
	if (ast->t.ast) {
	    emit(ast->t.ast);
	    top = operand_top();
	}
	symbol = ast->l.ast->r.ast->l.value;
    }
    else
	symbol = ast->l.ast->l.value;

    if (!(builtin = symbol->builtin))
	assert(symbol->function == true);

    function = symbol->value;

    list = ast->r.ast;
    emit_call_next(rop, function, list, top, vcall, ecall, builtin, true);
}

static void
emit_call(oast_t *ast)
{
    ooperand_t		*rop;
    ooperand_t		*top;
    oast_t		*list;
    obool_t		 vcall;
    obool_t		 ecall;
    osymbol_t		*symbol;
    obool_t		 builtin;
    ofunction_t		*function;

    rop = operand_get(ast->offset);
    rop->u.w = get_register(true);
    top = null;
    vcall = ast->l.ast->token == tok_dot;
    ecall = ast->l.ast->token == tok_explicit;

    if (vcall) {
	emit(ast->l.ast->l.ast);
	top = operand_top();
	symbol = ast->l.ast->r.ast->l.value;
    }
    else if (ecall) {
	if (ast->t.ast) {
	    emit(ast->t.ast);
	    top = operand_top();
	}
	symbol = ast->l.ast->r.ast->l.value;
    }
    else
	symbol = ast->l.ast->l.value;

    if (!(builtin = symbol->builtin))
	assert(symbol->function == true);
    function = symbol->value;

    list = ast->r.ast;
    emit_call_next(rop, function, list, top, vcall, ecall, builtin, false);
}

static void
emit_call_next(ooperand_t *rop,
	       ofunction_t *function, oast_t *alist,
	       ooperand_t *top,
	       obool_t vcall, obool_t ecall, obool_t builtin, obool_t thread)
{
    ooperand_t		*op;
    jit_node_t		*call;
    oword_t		 mask;
    otype_t		 type;
    oast_t		*list;
    obool_t		 mcall;
    jit_int32_t		 frame;
    jit_int32_t		 stack;
    oword_t		 offset;
    osymbol_t		*symbol;
    ovector_t		*vector;
    jit_node_t		*address;
    oint32_t		 framesize;

    address = null;

    /* Too few or too many arguments already checked */
    vector = function->record->vector;
    for (offset = 0, list = alist; offset < vector->offset; offset++) {
	symbol = vector->v.ptr[offset];
	if (!symbol->argument)
	    break;
	list = list->next;
    }

    framesize = function->frame;
    for (; list; list = list->next)
	framesize += sizeof(oobject_t);

    if (top)
	emit_load(top);

    /* FIXME It should be worth the cost of spill/reloading registers
     * if they were holding mp*_t objects instead of using this error
     * prone logic */
    mask = 0;
    if (GPR[FRAME] == JIT_NOREG) {
	if (GPR[TMP0] != JIT_NOREG) {
	    frame = GPR[TMP0];
	    protect_tmp(0);
	}
	else {
	    if (top == null || top->u.w != 0)
		frame = 0;
	    else
		frame = 1;
	    mask |= 1 << frame;
	    stack_spill_w(frame);
	    frame = GPR[frame];
	}
	jit_ldxi(frame, JIT_V0, offsetof(othread_t, fp));
    }
    else
	frame = GPR[FRAME];
    if (GPR[STACK] == JIT_NOREG) {
	if (GPR[TMP1] != JIT_NOREG) {
	    stack = GPR[TMP1];
	    protect_tmp(1);
	}
	else {
	    if (top == null || top->u.w > 1)
		stack = 1;
	    else
		stack = 2;
	    mask |= 1 << stack;
	    stack_spill_w(stack);
	    stack = GPR[stack];
	}
	jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));
    }
    else
	stack = GPR[STACK];

    /* Update "next" frame pointer */
    jit_subi(JIT_R0, stack, framesize);
    if (depth == 0)
	jit_stxi(NEXT_OFFSET, frame, JIT_R0);
    else
	jit_stxi(NEXT_OFFSET, stack, JIT_R0);

    /* Update stack pointer */
    jit_movr(stack, JIT_R0);
    jit_stxi(offsetof(othread_t, sp), JIT_V0, stack);

    if (!builtin) {
	/* Update return address */
	address = jit_movi(JIT_R0, 0);
	jit_stxi(RET_OFFSET, stack, JIT_R0);
    }

    /* Update "next" type */
    jit_movi(JIT_R0, function->record->type);
    jit_stxi_i(TYPE_OFFSET, stack, JIT_R0);

    /* Update "next" size */
    if (framesize != function->frame) {
	jit_movi(JIT_R0, framesize - function->frame);
	jit_stxi_i(SIZE_OFFSET, stack, JIT_R0);
    }

    mcall = false;
    if (top) {
	emit_load(top);
	load_w(top->u.w);

	/* Only null or a compatible object type should be stored */
	call = jit_bnei(GPR[top->u.w], 0);
	jit_prepare();
	jit_pushargi(except_null_dereference);
	jit_finishi(ovm_raise);
	jit_patch(call);

	jit_stxi(THIS_OFFSET, stack, GPR[top->u.w]);
	operand_unget(1);
    }
    else if (!builtin && function->name->method) {
	mcall = !ecall && function->name->offset != 0;
	if (GPR[THIS] != JIT_NOREG)
	    jit_stxi(THIS_OFFSET, stack, GPR[THIS]);
	else {
	    jit_ldxi(JIT_R0, frame, THIS_OFFSET);
	    jit_stxi(THIS_OFFSET, stack, JIT_R0);
	}
    }

    if (frame == GPR[TMP0])
	release_tmp(0);
    if (stack == GPR[TMP1])
	release_tmp(1);
    if (mask & 1)
	stack_load_w(0);
    if (mask & 2)
	stack_load_w(1);
    if (mask & 4)
	stack_load_w(2);

    ++depth;

    vector = function->record->vector;
    for (offset = 0, list = alist; list; offset++, list = list->next) {
	/* Invalid argument list already checked */
	if (offset >= vector->length ||
	    (symbol = vector->v.ptr[offset]) == null ||
	    !symbol->argument)
	    break;
	emit(list);
	op = operand_top();
	emit_load(op);
	type = otag_to_type(symbol->tag);
	if (GPR[STACK] != JIT_NOREG)
	    storei(op, type, GPR[STACK], symbol->offset);
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, sp));
	    storei(op, type, JIT_R0, symbol->offset);
	}
	operand_unget(1);
    }
    offset = function->frame;
    for (; list; offset += sizeof(oobject_t), list = list->next) {
	emit(list);
	op = operand_top();
	emit_load(op);
	if (GPR[STACK] != JIT_NOREG)
	    storei(op, t_void, GPR[STACK], offset);
	else {
	    jit_ldxi(JIT_R0, JIT_V0, offsetof(othread_t, sp));
	    storei(op, t_void, JIT_R0, offset);
	}
	operand_unget(1);
    }

    --depth;

    /* Any live operand must be spilled */
    emit_save_operands();

    vector = function->name->tag->name;
    type = otag_to_type(vector->v.ptr[0]);
    if (type && rop)
	rop->k = vector->v.ptr[0];

    /* FIXME For the sake of keeping builtins as simple as possible,
     * generate a not so simple call sequence, that arguably would
     * be better done as a builtin prolog/epilog sequence. */
    if (builtin) {
	if (thread) {
	    jit_prepare();
	    jit_pushargi(function->record->type);
	    jit_pushargi((oword_t)function->address);
	    jit_pushargi(true);
	    jit_pushargi(type != t_void);
	    jit_finishi(ovm_thread);
	}
	else {
	    /*   Pass first builtin argument as pointer to first language
	     * specific argument, so that casting to a structure is easy. */
	    if (GPR[STACK] != JIT_NOREG)
		stack = GPR[STACK];
	    else {
		stack = JIT_R0;
		jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));
	    }
	    jit_addi(JIT_R0, stack, THIS_OFFSET);
	    jit_prepare();
	    jit_pushargr(JIT_R0);

	    /*   Pass second builtin argument as number of void* pointers
	     * following declared arguments, in case it is a varargs. */
	    assert((framesize - function->frame) % sizeof(oobject_t) == 0);
	    jit_pushargi((framesize - function->frame) / sizeof(oobject_t));

	    /*   Execute builtin. */
	    jit_finishi(function->address);

	    /*   Stack memory not being used must be zero'ed or gc may
	     * inspect garbage when the memory is recast to a different
	     * layout, where previously word/double data could be mapped
	     * to collectable memory on a newer stack frame. */
	    if (GPR[STACK] != JIT_NOREG)
		stack = GPR[STACK];
	    else {
		stack = JIT_R0;
		jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));
	    }
	    jit_prepare();
	    jit_pushargr(stack);
	    jit_pushargi(0);
	    jit_pushargi(framesize);
	    jit_finishi(memset);
	    if (GPR[STACK] == JIT_NOREG)
		jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));

	    /*   Restore stack pointer */
	    jit_addi(stack, stack, framesize);
	    jit_stxi(offsetof(othread_t, sp), JIT_V0, stack);
	}
    }

    /*   Non builtins (and non virtual methods) are cheaper to call
     * and have a proper prolog/epilog. */
    else {
	/* Start executing function */
	if (vcall || mcall) {
	    /* All methods are virtual, this is not always cheap but
	     * do the expected thing for a dynamically typed language */
	    if (GPR[STACK] != JIT_NOREG)
		stack = GPR[STACK];
	    else {
		stack = JIT_R0;
		jit_ldxi(stack, JIT_V0, offsetof(othread_t, sp));
	    }
	    /* All operand registers were saved */
	    if (mcall && GPR[THIS] != JIT_NOREG)
		jit_ldxi_i(GPR[1], GPR[THIS], -4);	/* typeof(this) */
	    else {
		jit_ldxi(GPR[0], stack, THIS_OFFSET);	/* this */
		jit_ldxi_i(GPR[1], GPR[0], -4);		/* typeof(this) */
	    }
#if DEBUG
	    /* Ensure it is not a bad pointer; only if some bug and/or
	     * memory corruption happened this would be possible */
	    call = jit_blei_u(GPR[1], rtti_vector->offset);
	    jit_calli(abort);
	    jit_patch(call);
#endif
	    jit_muli(GPR[1], GPR[1], sizeof(oobject_t));
	    jit_movi(JIT_R0, (oword_t)rtti_vector->v.ptr);
	    jit_ldxr(GPR[1], JIT_R0, GPR[1]);		/* rrti[typeof(this)] */
	    jit_ldxi(JIT_R0, GPR[1], offsetof(ortti_t, mdinfo));
	    jit_ldxi(GPR[0], JIT_R0,
		     function->name->offset * sizeof(oobject_t));
	    if (thread) {
		jit_prepare();
		jit_pushargi(function->record->type);
		jit_pushargr(GPR[0]);
		jit_pushargi(false);
		jit_pushargi(type != t_void);
		jit_finishi(ovm_thread);
	    }
	    else
		jit_jmpr(GPR[0]);
	}
	else {
	    if (thread) {
		call = jit_movi(GPR[0], 0);
		jit_patch_at(call, function->patch);
		jit_prepare();
		jit_pushargi(function->record->type);
		jit_pushargr(GPR[0]);
		jit_pushargi(false);
		jit_pushargi(type != t_void);
		jit_finishi(ovm_thread);
	    }
	    else {
		call = jit_jmpi();
		jit_patch_at(call, function->patch);
	    }
	}

	/* Return address */
	jit_patch(address);
    }

    /* Get result type */
    if (thread)
	rop->t = t_void | t_register;
    else if (rop) {
	switch (type) {
	    case t_int8:	case t_uint8:
	    case t_int16:	case t_uint16:
#if __WORDSIZE == 64
	    case t_int32:	case t_uint32:
#endif
		if (builtin)
		    load_w(0);
		if (rop->u.w)
		    jit_movr(GPR[rop->u.w], GPR[0]);
		rop->t = t_half | t_register;
		break;
#if __WORDSIZE == 32
	    case t_int32:
#else
	    case t_int64:
#endif
		if (builtin)
		    load_w(0);
		if (rop->u.w)
		    jit_movr(GPR[rop->u.w], GPR[0]);
		rop->t = t_word | t_register;
		break;
	    case t_single:
		assert(!builtin);
		if (rop->u.w)
		    jit_movr_f(FPR[rop->u.w], FPR[0]);
		rop->t = t_single | t_register;
		break;
	    case t_float:
		if (builtin)
		    load_d(0);
		if (rop->u.w)
		    jit_movr_d(FPR[rop->u.w], FPR[0]);
		rop->t = t_float | t_register;
		break;
	    default:
		if (rop->u.w) {
		    load_r(rop->u.w);
		    load_r_w(0, JIT_R0);
		    jit_prepare();
		    jit_pushargr(GPR[rop->u.w]);
		    jit_pushargr(JIT_R0);
		    emit_finish(ovm_move, mask1(rop->u.w));
		}
		rop->t = t_void | t_register;
		break;
	}
    }
}
#endif
