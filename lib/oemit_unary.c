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
emit_unary(oast_t *ast);

static void
emit_not(oast_t *ast);

static void
emit_com(oast_t *ast);

static void
emit_neg(oast_t *ast);

static void
emit_inc_dec(oast_t *ast);
#endif

#if defined(CODE)
static void
emit_unary(oast_t *ast)
{
    switch (get_token(ast)) {
	case tok_not:
	    emit_not(ast);
	    break;
	case tok_com:
	    emit_com(ast);
	    break;
	case tok_neg:
	    emit_neg(ast);
	    break;
	default:
	    abort();
    }
}

static void
emit_not(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_eqi(GPR[regno], GPR[regno], 0);
	    break;
	case t_single:
	    jit_eqi_f(GPR[regno], FPR[regno], 0.0);
	    break;
	case t_float:
	    jit_eqi_d(GPR[regno], FPR[regno], 0.0);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_not, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_com(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:	case t_word:
	    jit_comr(GPR[regno], GPR[regno]);
	    emit_set_type(op, t_word);
	    break;
	case t_single:	case t_float:
	    oparse_error(ast, "not an integer");
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_com, mask1(regno));
	    break;
    }
}

static void
emit_neg(oast_t *ast)
{
    ooperand_t		*op;
    otype_t		 type;
    jit_node_t		*ijmp;
    jit_node_t		*zjmp;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (type = emit_get_type(op)) {
	case t_half:
	    jit_negr(GPR[regno], GPR[regno]);
	    emit_set_type(op, t_word);
	    break;
	case t_word:
	    zjmp = jit_beqi(GPR[regno], MININT);
	    jit_negr(GPR[regno], GPR[regno]);
	    sync_w(regno);
	    ijmp = jit_jmpi();
	    jit_patch(zjmp);
	    sync_w(regno);
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_neg, mask1(regno));
	    jit_patch(ijmp);
	    emit_set_type(op, t_void);
	    break;
	case t_single:
	    jit_negr_f(FPR[regno], FPR[regno]);
	    break;
	case t_float:
	    jit_negr_d(FPR[regno], FPR[regno]);
	    break;
	default:
	    assert(type == t_void);
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_neg, mask1(regno));
	    break;
    }
}

static void
emit_inc_dec(oast_t *ast)
{
    ooperand_t		*bop;
    ooperand_t		*lop;
    ooperand_t		*rop;
    ooperand_t		*pop;
    obool_t		 inc;
    jit_node_t		*jump;
    jit_node_t		*done;
    obool_t		 post;
    oword_t		 offset;
    orecord_t		*record;
    osymbol_t		*symbol;
    obool_t		 vararg;

    switch (ast->token) {
	case tok_inc:		inc = true;	post = false;	break;
	case tok_dec:		inc = false;	post = false;	break;
	case tok_postinc:	inc = true;	post = true;	break;
	default:		inc = false;	post = true;	break;
    }
    offset = ast->offset;
    ast = ast->l.ast;
    vararg = ast->token == tok_vector && ast->l.ast->token == tok_ellipsis;
    switch (ast->token) {
	case tok_symbol:
	    emit(ast);
	    rop = operand_top();
	    symbol = rop->u.o;
	    load_symbol(symbol, rop);
	    break;
	case tok_dot:
	    emit(ast->l.ast);
	    bop = operand_top();
	    if (bop->k == null || bop->k->type != tag_class)
		oparse_error(ast, "expecting class %A", ast->l.ast);
	    record = bop->k->name;
	    lop = operand_get(0);
	    lop->t = t_symbol;
	    lop->u.o = symbol = ast->r.ast->l.value;
	    /* FIXME should have been already checked */
	    symbol = oget_symbol(record, symbol->name);
	    if (symbol == null)
		oparse_error(ast, "no field '%p' in '%p'",
			     lop->u.o, record->name);
	    rop = operand_get(bop->s);
	    load_record(bop, lop, rop);
	    break;
	case tok_vector:
	    if (vararg) {
		emit(ast->r.ast);
		lop = operand_top();
		rop = operand_get(ast->offset);
		load_vararg(lop, rop);
	    }
	    else {
		emit(ast->l.ast);
		bop = operand_top();
		if (bop->k == null || bop->k->type != tag_vector)
		    oparse_error(ast, "expecting vector %A", ast->l.ast);
		emit(ast->r.ast);
		lop = operand_top();
		rop = operand_get(bop->s);
		load_vector(bop, lop, rop);
	    }
	    break;
	default:
	    oparse_error(ast, "not a lvalue %A", ast);
    }

    if (post) {
	pop = operand_get(0);
	operand_copy(pop, rop);
	pop->s = offset;
	switch (emit_get_type(rop)) {
	    case t_half:	case t_word:
		pop->u.w = get_register(true);
		jit_movr(GPR[pop->u.w], GPR[rop->u.w]);
		break;
	    case t_single:
		pop->u.w = get_register(true);
		jit_movr_f(FPR[pop->u.w], FPR[rop->u.w]);
		break;
	    case t_float:
		pop->u.w = get_register(true);
		jit_movr_d(FPR[pop->u.w], FPR[rop->u.w]);
		break;
	    default:
		emit_save(pop, false);
		pop->u.w = get_register(true);
		break;
	}
    }

    switch (emit_get_type(rop)) {
	case t_half:
	    if (inc)	jit_addi(GPR[rop->u.w], GPR[rop->u.w], 1);
	    else	jit_subi(GPR[rop->u.w], GPR[rop->u.w], 1);
	    break;
	case t_word:
	    jit_movr(JIT_R0, GPR[rop->u.w]);
	    if (inc)	jump = jit_boaddi(GPR[rop->u.w], 1);
	    else	jump = jit_bosubi(GPR[rop->u.w], 1);
	    sync_w(rop->u.w);
	    done = jit_jmpi();
	    jit_patch(jump);
	    sync_w_w(rop->u.w, JIT_R0);
	    load_r(rop->u.w);
	    jit_prepare();
	    jit_pushargr(GPR[rop->u.w]);
	    if (inc)	emit_finish(ovm_inc, mask1(rop->u.w));
	    else	emit_finish(ovm_dec, mask1(rop->u.w));
	    jit_patch(done);
	    emit_set_type(rop, t_void);
	    break;
	case t_single:
	    if (inc)	jit_addi_f(FPR[rop->u.w], FPR[rop->u.w], 1.0);
	    else	jit_subi_f(FPR[rop->u.w], FPR[rop->u.w], 1.0);
	    break;
	case t_float:
	    if (inc)	jit_addi_d(FPR[rop->u.w], FPR[rop->u.w], 1.0);
	    else	jit_subi_d(FPR[rop->u.w], FPR[rop->u.w], 1.0);
	    break;
	default:
	    load_r(rop->u.w);
	    jit_prepare();
	    jit_pushargr(GPR[rop->u.w]);
	    if (inc)	emit_finish(ovm_inc, mask1(rop->u.w));
	    else	emit_finish(ovm_dec, mask1(rop->u.w));
	    break;
    }

    switch (ast->token) {
	case tok_symbol:
	    store_symbol(symbol, rop);
	    break;
	case tok_dot:
	    store_record(bop, lop, rop);
	    operand_copy(bop, rop);
	    operand_unget(2);
	    break;
	default:
	    if (vararg) {
		store_vararg(lop, rop);
		operand_copy(lop, rop);
		operand_unget(1);
	    }
	    else {
		store_vector(bop, lop, rop);
		operand_copy(bop, rop);
		operand_unget(2);
	    }
	    break;
    }

    if (post) {
	if (ast->token == tok_symbol || vararg)
	    operand_copy(rop, pop);
	else
	    operand_copy(bop, pop);
	operand_unget(1);
    }
}
#endif
