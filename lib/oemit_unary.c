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
emit_plus(oast_t *ast);

static void
emit_neg(oast_t *ast);

static void
emit_inc_dec(oast_t *ast);

static void
emit_integer_p(oast_t *ast);

static void
emit_rational_p(oast_t *ast);

static void
emit_float_p(oast_t *ast);

static void
emit_real_p(oast_t *ast);

static void
emit_complex_p(oast_t *ast);

static void
emit_number_p(oast_t *ast);

static void
emit_finite_p(oast_t *ast);

static void
emit_inf_p(oast_t *ast);

static void
emit_nan_p(oast_t *ast);

static void
emit_num(oast_t *ast);

static void
emit_den(oast_t *ast);

static void
emit_real(oast_t *ast);

static void
emit_imag(oast_t *ast);

static void
emit_signbit(oast_t *ast);

static void
emit_abs(oast_t *ast);

static void
emit_signum(oast_t *ast);

static void
emit_rational(oast_t *ast);

static void
emit_arg(oast_t *ast);

static void
emit_conj(oast_t *ast);

static void
emit_floor(oast_t *ast);

static void
emit_trunc(oast_t *ast);

static void
emit_round(oast_t *ast);

static void
emit_ceil(oast_t *ast);

static void
emit_sqrt(oast_t *ast);

static void
emit_cbrt(oast_t *ast);

static void
emit_sin(oast_t *ast);

static void
emit_cos(oast_t *ast);

static void
emit_tan(oast_t *ast);

static void
emit_asin(oast_t *ast);

static void
emit_acos(oast_t *ast);

static void
emit_atan(oast_t *ast);

static void
emit_sinh(oast_t *ast);

static void
emit_cosh(oast_t *ast);

static void
emit_tanh(oast_t *ast);

static void
emit_asinh(oast_t *ast);

static void
emit_acosh(oast_t *ast);

static void
emit_atanh(oast_t *ast);

static void
emit_proj(oast_t *ast);

static void
emit_exp(oast_t *ast);

static void
emit_log(oast_t *ast);

static void
emit_log2(oast_t *ast);

static void
emit_log10(oast_t *ast);
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
	case tok_plus:
	    emit_plus(ast);
	    break;
	case tok_neg:
	    emit_neg(ast);
	    break;
	case tok_integer_p:
	    emit_integer_p(ast);
	    break;
	case tok_rational_p:
	    emit_rational_p(ast);
	    break;
	case tok_float_p:
	    emit_float_p(ast);
	    break;
	case tok_real_p:
	    emit_real_p(ast);
	    break;
	case tok_complex_p:
	    emit_complex_p(ast);
	    break;
	case tok_number_p:
	    emit_number_p(ast);
	    break;
	case tok_finite_p:
	    emit_finite_p(ast);
	    break;
	case tok_inf_p:
	    emit_inf_p(ast);
	    break;
	case tok_nan_p:
	    emit_nan_p(ast);
	    break;
	case tok_num:
	    emit_num(ast);
	    break;
	case tok_den:
	    emit_den(ast);
	    break;
	case tok_real:
	    emit_real(ast);
	    break;
	case tok_imag:
	    emit_imag(ast);
	    break;
	case tok_signbit:
	    emit_signbit(ast);
	    break;
	case tok_abs:
	    emit_abs(ast);
	    break;
	case tok_signum:
	    emit_signum(ast);
	    break;
	case tok_rational:
	    emit_rational(ast);
	    break;
	case tok_arg:
	    emit_arg(ast);
	    break;
	case tok_conj:
	    emit_conj(ast);
	    break;
	case tok_floor:
	    emit_floor(ast);
	    break;
	case tok_trunc:
	    emit_trunc(ast);
	    break;
	case tok_round:
	    emit_round(ast);
	    break;
	case tok_ceil:
	    emit_ceil(ast);
	    break;
	case tok_sqrt:
	    emit_sqrt(ast);
	    break;
	case tok_cbrt:
	    emit_cbrt(ast);
	    break;
	case tok_sin:
	    emit_sin(ast);
	    break;
	case tok_cos:
	    emit_cos(ast);
	    break;
	case tok_tan:
	    emit_tan(ast);
	    break;
	case tok_asin:
	    emit_asin(ast);
	    break;
	case tok_acos:
	    emit_acos(ast);
	    break;
	case tok_atan:
	    emit_atan(ast);
	    break;
	case tok_sinh:
	    emit_sinh(ast);
	    break;
	case tok_cosh:
	    emit_cosh(ast);
	    break;
	case tok_tanh:
	    emit_tanh(ast);
	    break;
	case tok_asinh:
	    emit_asinh(ast);
	    break;
	case tok_acosh:
	    emit_acosh(ast);
	    break;
	case tok_atanh:
	    emit_atanh(ast);
	    break;
	case tok_proj:
	    emit_proj(ast);
	    break;
	case tok_exp:
	    emit_exp(ast);
	    break;
	case tok_log:
	    emit_log(ast);
	    break;
	case tok_log2:
	    emit_log2(ast);
	    break;
	case tok_log10:
	    emit_log10(ast);
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
emit_plus(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	case t_single:		case t_float:
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_plus, mask1(regno));
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
	    sync_uw_z(regno);
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

static void
emit_integer_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 1);
	    break;
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 0);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_integer_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_rational_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 1);
	    break;
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 0);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_rational_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_float_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 0);
	    break;
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 1);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_float_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_real_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 1);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_real_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_complex_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 0);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_complex_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_number_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	case t_single:		case t_float:
	    jit_movi(GPR[regno], 1);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_number_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_finite_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 1);
	    break;
	case t_single:
	    jit_prepare();
	    jit_pushargr_f(FPR[regno]);
	    emit_finish(finitef, mask1(regno));
	    jit_retval(GPR[regno]);
	    break;
	case t_float:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(finite, mask1(regno));
	    jit_retval(GPR[regno]);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_finite_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_inf_p(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*njmp;
    jit_node_t		*ijmp;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 0);
	    break;
	case t_single:
	    jit_stxi_f(scratch, JIT_FP, FPR[regno]);
	    jit_prepare();
	    jit_pushargr_f(FPR[regno]);
	    emit_finish(isinff, mask1(regno));
	    jit_retval(GPR[regno]);
	    ijmp = jit_beqi(GPR[regno], 0);
	    /* signbit */
	    jit_ldxi_i(GPR[regno], JIT_FP, scratch);
	    jit_rshi(GPR[regno], GPR[regno], 31);
	    /* if not zero must be -1 after shift */
	    njmp = jit_bnei(GPR[regno], 0);
	    jit_movi(GPR[regno], 1);
	    jit_patch(ijmp);
	    jit_patch(njmp);
	    break;
	case t_float:
	    jit_stxi_d(scratch, JIT_FP, FPR[regno]);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(isinf, mask1(regno));
	    jit_retval(GPR[regno]);
	    ijmp = jit_beqi(GPR[regno], 0);
	    jit_ldxi_i(GPR[regno], JIT_FP, scratch
#if __BYTE_ORDER == __LITTLE_ENDIAN
		       + 4
#endif
		       );
	    jit_rshi(GPR[regno], GPR[regno], 31);
	    /* if not zero must be -1 after shift */
	    njmp = jit_bnei(GPR[regno], 0);
	    jit_movi(GPR[regno], 1);
	    jit_patch(ijmp);
	    jit_patch(njmp);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_inf_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_nan_p(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 0);
	    break;
	case t_single:
	    jit_prepare();
	    jit_pushargr_f(FPR[regno]);
	    emit_finish(isnanf, mask1(regno));
	    jit_retval(GPR[regno]);
	    break;
	case t_float:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(isnan, mask1(regno));
	    jit_retval(GPR[regno]);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_nan_p, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_signbit(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_lti(GPR[regno], GPR[regno], 0);
	    break;
	case t_single:
	    jit_stxi_f(scratch, JIT_FP, FPR[regno]);
	    jit_ldxi_i(GPR[regno], JIT_FP, scratch);
	    jit_rshi(GPR[regno], GPR[regno], 31);
	    break;
	case t_float:
	    jit_stxi_d(scratch, JIT_FP, FPR[regno]);
	    jit_ldxi_i(GPR[regno], JIT_FP, scratch
#if __BYTE_ORDER == __LITTLE_ENDIAN
		       + 4
#endif
		       );
	    jit_rshi(GPR[regno], GPR[regno], 31);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_signbit, mask1(regno));
	    load_w(regno);
	    break;
    }
    emit_set_type(op, t_half);
}

static void
emit_abs(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jump;
    jit_node_t		*zjmp;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:
	    jump = jit_bgei(GPR[regno], 0);
	    jit_negr(GPR[regno], GPR[regno]);
	    jit_patch(jump);
	    emit_set_type(op, t_word);
	    break;
	case t_word:
	    jump = jit_bgei(GPR[regno], 0);
	    zjmp = jit_beqi(GPR[regno], MININT);
	    jit_negr(GPR[regno], GPR[regno]);
	    jit_patch(jump);
	    sync_w(regno);
	    jump = jit_jmpi();
	    jit_patch(zjmp);
	    sync_uw_z(regno);
	    jit_patch(jump);
	    emit_set_type(op, t_void);
	    break;
	case t_single:
	    jit_absr_f(FPR[regno], FPR[regno]);
	    break;
	case t_float:
	    jit_absr_d(FPR[regno], FPR[regno]);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_abs, mask1(regno));
	    break;
    }
}

static void
emit_num(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_num, mask1(regno));
	    break;
    }
}

static void
emit_den(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 1);
	    emit_set_type(op, t_half);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_den, mask1(regno));
	    break;
    }
}

static void
emit_real(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	case t_float:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_real, mask1(regno));
	    break;
    }
}

static void
emit_imag(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jit_movi(GPR[regno], 0);
	    emit_set_type(op, t_half);
	    break;
	case t_single:		case t_float:
	    jit_movi_d(FPR[regno], 0.0);
	    emit_set_type(op, t_float);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_imag, mask1(regno));
	    break;
    }
}

static void
emit_signum(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*done;
    jit_node_t		*jump;
    jit_node_t		*zjmp;
    jit_node_t		*njmp;
    jit_node_t		*pjmp;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    jump = jit_bgti(GPR[regno], 0);
	    jit_rshi(GPR[regno], GPR[regno], __WORDSIZE - 1);
	    done = jit_jmpi();
	    jit_patch(jump);
	    jit_movi(GPR[regno], 1);
	    jit_patch(done);
	    emit_set_type(op, t_half);
	    break;
	case t_single:
	    jit_prepare();
	    jit_pushargr_f(FPR[regno]);
	    emit_finish(isnanf, 0);
	    jit_retval(GPR[regno]);
	    done = jit_bnei(GPR[regno], 0);
	    /* could "f = f / fabs(f)" but that does not work for +-inf */
	    zjmp = jit_bnei_f(FPR[regno], 0.0);
	    jit_movi_f(FPR[regno], 0.0);
	    jump = jit_jmpi();
	    jit_patch(zjmp);
	    njmp = jit_blti_f(FPR[regno], 0.0);
	    jit_movi_f(FPR[regno], 1.0);
	    pjmp = jit_jmpi();
	    jit_patch(njmp);
	    jit_movi_f(FPR[regno], -1.0);
	    jit_patch(pjmp);
	    jit_patch(jump);
	    jit_patch(done);
	    break;
	case t_float:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(isnan, 0);
	    jit_retval(GPR[regno]);
	    done = jit_bnei(GPR[regno], 0);
	    zjmp = jit_bnei_d(FPR[regno], 0.0);
	    jit_movi_d(FPR[regno], 0.0);
	    jump = jit_jmpi();
	    jit_patch(zjmp);
	    njmp = jit_blti_d(FPR[regno], 0.0);
	    jit_movi_d(FPR[regno], 1.0);
	    pjmp = jit_jmpi();
	    jit_patch(njmp);
	    jit_movi_d(FPR[regno], -1.0);
	    jit_patch(pjmp);
	    jit_patch(jump);
	    jit_patch(done);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_signum, mask1(regno));
	    break;
    }
}

static void
emit_rational(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_rational, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_arg(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 0);
		jit_movi_d(FPR[regno], 0.0);
		done = jit_jmpi();
		jit_patch(jump);
		jit_movi_d(FPR[regno], M_PI);
		jit_patch(done);
		emit_set_type(op, t_float);
	    }
	    else {
		sync_w(regno);
		goto next;
	    }
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 0.0);
	    jit_movi_d(FPR[regno], 0.0);
	    done = jit_jmpi();
	    jit_patch(jump);
	    jit_movi_d(FPR[regno], M_PI);
	    jit_patch(done);
	    jit_patch(jord);
	    emit_set_type(op, t_float);
	    break;
	default:
	next:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_arg, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_conj(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	case t_float:
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_conj, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_floor(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*done;
    jit_node_t		*jump;
    oword_t		 regno;

    done = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(floor, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    jit_truncr_d(GPR[regno], FPR[regno]);
	    jit_extr_d(JIT_F0, GPR[regno]);
	    jump = jit_bner_d(FPR[regno], JIT_F0);
	    sync_w(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_floor, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    break;
    }
}

static void
emit_trunc(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*done;
    jit_node_t		*jump;
    oword_t		 regno;

    done = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(trunc, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    jit_truncr_d(GPR[regno], FPR[regno]);
	    jit_extr_d(JIT_F0, GPR[regno]);
	    jump = jit_bner_d(FPR[regno], JIT_F0);
	    sync_w(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_trunc, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    break;
    }
}

static void
emit_round(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*done;
    jit_node_t		*jump;
    oword_t		 regno;

    done = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(round, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    jit_truncr_d(GPR[regno], FPR[regno]);
	    jit_extr_d(JIT_F0, GPR[regno]);
	    jump = jit_bner_d(FPR[regno], JIT_F0);
	    sync_w(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_round, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    break;
    }
}

static void
emit_ceil(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*done;
    jit_node_t		*jump;
    oword_t		 regno;

    done = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(ceil, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    jit_truncr_d(GPR[regno], FPR[regno]);
	    jit_extr_d(JIT_F0, GPR[regno]);
	    jump = jit_bner_d(FPR[regno], JIT_F0);
	    sync_w(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_ceil, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    break;
    }
}

static void
emit_sqrt(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 0);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(sqrt, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 0.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(sqrt, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_sqrt, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_cbrt(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(cbrt, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_cbrt, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_sin(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(sin, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_sin, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_cos(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(cos, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_cos, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_tan(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(tan, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_tan, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_asin(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*njmp;
    jit_node_t		*pjmp;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		njmp = jit_blti(GPR[regno], -1);
		pjmp = jit_bgti(GPR[regno],  1);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(asin, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(njmp);
		jit_patch(pjmp);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jit_absr_d(JIT_F0, FPR[regno]);
	    pjmp = jit_bgei_d(JIT_F0, 1.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(asin, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(pjmp);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_asin, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_acos(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*njmp;
    jit_node_t		*pjmp;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		njmp = jit_blti(GPR[regno], -1);
		pjmp = jit_bgti(GPR[regno],  1);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(acos, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(njmp);
		jit_patch(pjmp);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jit_absr_d(JIT_F0, FPR[regno]);
	    pjmp = jit_bgei_d(JIT_F0, 1.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(acos, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(pjmp);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_acos, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_atan(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(atan, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_atan, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_sinh(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(sinh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_sinh, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_cosh(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(cosh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_cosh, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_tanh(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(tanh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_tanh, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_asinh(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(asinh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_asinh, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_acosh(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 1);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(acosh, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 1.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(acosh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_acosh, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_atanh(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_bnei(GPR[regno], 0);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(atanh, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jit_absr_d(JIT_F0, FPR[regno]);
	    jump = jit_bgei_d(JIT_F0, 1.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(atanh, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_atanh, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_proj(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    break;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_bnei_d(FPR[regno], -INFINITY);
	    jit_movi_d(FPR[regno], INFINITY);
	    jit_patch(jord);
	    jit_patch(jump);
	    emit_set_type(op, t_float);
	    break;
	default:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_proj, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_exp(oast_t *ast)
{
    ooperand_t		*op;
    oword_t		 regno;

    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jit_extr_d(FPR[regno], GPR[regno]);
		goto jit;
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	jit:
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(exp, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    emit_set_type(op, t_float);
	    break;
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_exp, mask1(regno));
	    emit_set_type(op, t_void);
	    break;
    }
}

static void
emit_log(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 0);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(log, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 0.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(log, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_log, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_log2(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 0);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(log2, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 0.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(log2, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_log2, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}

static void
emit_log10(oast_t *ast)
{
    ooperand_t		*op;
    jit_node_t		*jord;
    jit_node_t		*jump;
    jit_node_t		*done;
    oword_t		 regno;

    done = jord = null;
    emit(ast->l.ast);
    op = operand_top();
    emit_load(op);
    regno = op->u.w;
    switch (emit_get_type(op)) {
	case t_half:		case t_word:
	    if (!cfg_float_format) {
		jump = jit_blti(GPR[regno], 0);
		jit_extr_d(FPR[regno], GPR[regno]);
		jit_prepare();
		jit_pushargr_d(FPR[regno]);
		emit_finish(log10, mask1(regno));
		jit_retval_d(FPR[regno]);
		sync_d(regno);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    sync_w(regno);
	    goto vm;
	case t_single:
	    jit_extr_f_d(FPR[regno], FPR[regno]);
	case t_float:
	    sync_d(regno);
	    jord = jit_bunordr_d(FPR[regno], FPR[regno]);
	    jump = jit_blti_d(FPR[regno], 0.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[regno]);
	    emit_finish(log10, mask1(regno));
	    jit_retval_d(FPR[regno]);
	    sync_d(regno);
	    done = jit_jmpi();
	    jit_patch(jump);
	default:
	vm:
	    load_r(regno);
	    jit_prepare();
	    jit_pushargr(GPR[regno]);
	    emit_finish(ovm_log10, mask1(regno));
	    emit_set_type(op, t_void);
	    if (done)
		jit_patch(done);
	    if (jord)
		jit_patch(jord);
	    break;
    }
}
#endif
