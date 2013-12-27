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
emit_binary(oast_t *ast);

static void
emit_binary_setup(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_binary_next(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_and_or_xor(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_mul2(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_div2(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_shl(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_shr(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_add_sub(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_mul(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_div(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_trunc2(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_rem(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_complex(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_atan2_hypot(ooperand_t *lop, otoken_t tok, ooperand_t *rop);

static void
emit_pow(ooperand_t *lop, otoken_t tok, ooperand_t *rop);
#endif

#if defined(CODE)
static void
emit_binary(oast_t *ast)
{
    ooperand_t		*lop;
    otoken_t		 tok;
    ooperand_t		*rop;

    tok = get_token(ast);
    emit(ast->l.ast);
    lop = operand_top();
    /* FIXME only need emit_load(lop) if rop may change lop,
     * e.g. "a = b + (b = a);" */
    emit_load(lop);
    emit(ast->r.ast);
    rop = operand_top();
    emit_binary_next(lop, tok, rop);
}

static void
emit_binary_setup(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		lty;
    otype_t		rty;
    oword_t		lreg;
    oword_t		rreg;

    lreg = lop->u.w;

    /* Delay constant load in case a register is not required */
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    emit_load(rop);
	    rreg = rop->u.w;
	    break;
    }

    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word)
	    ;
	else if (tok != tok_mul2 && tok != tok_div2 &&
		 tok != tok_shl  && tok != tok_shr) {
	    if (rty == t_single || rty == t_float) {
		if (rty == t_single) {
		    jit_extr_f_d(FPR[rreg], FPR[rreg]);
		    emit_set_type(rop, t_float);
		}
		jit_extr_d(FPR[lreg], GPR[lreg]);
		emit_set_type(lop, t_float);
	    }
	    else
		sync_w(lreg);
	}
	else {
	    sync_w(lreg);
	    if (rty == t_single || rty == t_float) {
		if (rreg == -1) {
		    emit_load(rop);
		    rreg = rop->u.w;
		}
		if (rty == t_single)
		    jit_extr_f_d(FPR[rreg], FPR[rreg]);
		sync_d(rreg);
		emit_set_type(rop, t_void);
	    }
	}
    }
    else if (lty == t_single) {
	if (rty == t_half || rty == t_word) {
	    if (tok != tok_mul2 && tok != tok_div2 &&
		tok != tok_shl  && tok != tok_shr) {
		if (rreg == -1) {
		    emit_load(rop);
		    rreg = rop->u.w;
		}
		jit_extr_f_d(FPR[lreg], FPR[lreg]);
		emit_set_type(lop, t_float);
		jit_extr_d(FPR[rreg], GPR[rreg]);
		emit_set_type(rop, t_float);
	    }
	}
	else if (rty == t_single)
	    assert(rop->t == (t_single|t_register));
	else if (rty == t_float) {
	    if (rreg != -1) {
		jit_extr_f_d(FPR[lreg], FPR[lreg]);
		emit_set_type(lop, t_float);
	    }
	}
	else {
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    sync_d(lreg);
	    emit_set_type(lop, t_float);
	}
    }
    else if (lty == t_float) {
	if (rty == t_half || rty == t_word) {
	    if (tok != tok_mul2 && tok != tok_div2 &&
		tok != tok_shl  && tok != tok_shr) {
		if (rreg == -1) {
		    emit_load(rop);
		    rreg = rop->u.w;
		}
		jit_extr_d(FPR[rreg], GPR[rreg]);
		emit_set_type(rop, t_float);
	    }
	}
	else if (rty == t_single) {
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	    emit_set_type(rop, t_float);
	}
	else if (rty == t_float) {
	    if (rreg != -1)
		assert(rop->t == (t_float|t_register));
	}
	else
	    sync_d(lreg);
    }
    else {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(rreg);
	}
	else if (rty == t_single) {
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	    sync_d(rreg);
	    emit_set_type(rop, t_float);
	}
	else if (rty == t_float) {
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    assert(rop->t = (t_float|t_register));
	    sync_d(rreg);
	}
	else
	    ;
    }
}

static void
emit_binary_next(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    emit_load(lop);
    emit_binary_setup(lop, tok, rop);

    switch (tok) {
	case tok_and:		case tok_or:
	case tok_xor:
	    emit_and_or_xor(lop, tok, rop);
	    break;
	case tok_mul2:
	    emit_mul2(lop, tok, rop);
	    break;
	case tok_div2:
	    emit_div2(lop, tok, rop);
	    break;
	case tok_shl:
	    emit_shl(lop, tok, rop);
	    break;
	case tok_shr:
	    emit_shr(lop, tok, rop);
	    break;
	case tok_add:		case tok_sub:
	    emit_add_sub(lop, tok, rop);
	    break;
	case tok_mul:
	    emit_mul(lop, tok, rop);
	    break;
	case tok_div:
	    emit_div(lop, tok, rop);
	    break;
	case tok_trunc2:
	    emit_trunc2(lop, tok, rop);
	    break;
	case tok_rem:
	    emit_rem(lop, tok, rop);
	    break;
	case tok_complex:
	    emit_complex(lop, tok, rop);
	    break;
	case tok_atan2:		case tok_hypot:
	    emit_atan2_hypot(lop, tok, rop);
	    break;
	case tok_pow:
	    emit_pow(lop, tok, rop);
	    break;
	default:
	    abort();
    }
}

static void
emit_and_or_xor(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if      (tok == tok_and)
		    jit_andi(GPR[lreg], GPR[lreg], rop->u.w);
		else if (tok == tok_or)
		    jit_ori(GPR[lreg], GPR[lreg], rop->u.w);
		else
		    jit_xori(GPR[lreg], GPR[lreg], rop->u.w);
	    }
	    else {
		if      (tok == tok_and)
		    jit_andr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else if (tok == tok_or)
		    jit_orr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else
		    jit_xorr(GPR[lreg], GPR[lreg], GPR[rreg]);
	    }
	    if (lty == t_half && rty == t_half)
		emit_set_type(lop, t_half);
	    else
		emit_set_type(lop, t_word);
	    goto finish;
	}
	assert(rty == t_void);
	if (tok == tok_and)	function = ovm_w_and;
	else if (tok == tok_or)	function = ovm_w_or;
	else			function = ovm_w_xor;
    }
    else {
	if (rty != t_half && rty != t_word)
	    assert(rty == t_void);
	if (tok == tok_and)	function = ovm_o_and;
	else if (tok == tok_or)	function = ovm_o_or;
	else			function = ovm_o_xor;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_mul2(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*jump;
    jit_node_t		*done;
    obool_t		 reload;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    reload = false;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		assert(rop->u.w >= 0);
		if (rop->u.w == 0)
		    /* no shift */
		    goto finish;
		if (rop->u.w < __WORDSIZE) {
		    jit_lshi(JIT_R0, GPR[lreg], rop->u.w);
		    jit_rshi(JIT_R0, JIT_R0, rop->u.w);
		    jump = jit_bner(JIT_R0, GPR[lreg]);
		    jit_lshi(GPR[lreg], GPR[lreg], rop->u.w);
		    sync_w(lreg);
		    done = jit_jmpi();
		    jit_patch(jump);
		    emit_load(rop);
		    rreg = rop->u.w;
		    sync_w(lreg);
		    sync_w(rreg);
		}
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(lreg);
	    sync_w(rreg);
	}
	else
	    assert(rty == t_void);
    }
    else if (lty == t_single) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w) {
		    assert(rop->u.w > 0);
		    assert(rty == t_half);
		    jit_prepare();
		    jit_pushargr_f(FPR[lreg]);
		    jit_pushargi(rop->u.w);
		    emit_finish(ldexpf, mask1(lreg));
		    jit_retval_f(FPR[lreg]);
		}
		goto finish;
	    }
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    emit_set_type(lop, t_float);
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_d(lreg);
	    sync_w(rreg);
	}
	else {
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    sync_d(lreg);
	    emit_set_type(lop, t_float);
	    if (rty == t_single) {
		jit_extr_f_d(FPR[rreg], FPR[rreg]);
		sync_d(rreg);
	    }
	    else
		assert(rty == t_void);
	}
	reload = true;
    }
    else if (lty == t_float) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w) {
		    assert(rop->u.w > 0);
		    assert(rty == t_half);
		    jit_prepare();
		    jit_pushargr_f(FPR[lreg]);
		    jit_pushargi(rop->u.w);
		    emit_finish(ldexp, mask1(lreg));
		    jit_retval_d(FPR[lreg]);
		}
		goto finish;
	    }
	    else {
		sync_d(lreg);
		sync_w(rreg);
	    }
	}
	else {
	    sync_d(lreg);
	    if (rty == t_float)
		sync_d(rreg);
	    else
		assert(rty == t_void);
	}
	reload = true;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(ovm_o_mul2, mask1(lreg));
    if (done)
	jit_patch(done);
    if (reload)
	load_d(lreg);
    else
	emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_div2(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*jump;
    jit_node_t		*done;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		assert(rop->u.w >= 0);
		if (rop->u.w == 0)
		    /* no shift */
		    goto finish;
		if (rop->u.w < __WORDSIZE) {
		    jit_rshi(JIT_R0, GPR[lreg], rop->u.w);
		    jit_rshi(JIT_R0, JIT_R0, rop->u.w);
		    jump = jit_bner(JIT_R0, GPR[lreg]);
		    jit_rshi(GPR[lreg], GPR[lreg], rop->u.w);
		    sync_w(lreg);
		    done = jit_jmpi();
		    jit_patch(jump);
		}
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(lreg);
	    sync_w(rreg);
	}
	else
	    assert(rty == t_void);
    }
    else if (lty == t_single) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w) {
		    assert(rop->u.w > 0);
		    assert(rty == t_half);
		    jit_prepare();
		    jit_pushargr_f(FPR[lreg]);
		    jit_pushargi(-rop->u.w);
		    emit_finish(ldexpf, mask1(lreg));
		    jit_retval_f(FPR[lreg]);
		}
		goto finish;
	    }
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    emit_set_type(lop, t_float);
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_d(lreg);
	    sync_w(rreg);
	}
	else {
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    sync_d(lreg);
	    emit_set_type(lop, t_float);
	    if (rty == t_single) {
		jit_extr_f_d(FPR[rreg], FPR[rreg]);
		sync_d(rreg);
	    }
	    else
		assert(rty == t_void);
	}
    }
    else if (lty == t_float) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w) {
		    assert(rop->u.w > 0);
		    assert(rty == t_half);
		    jit_prepare();
		    jit_pushargr_f(FPR[lreg]);
		    jit_pushargi(-rop->u.w);
		    emit_finish(ldexp, mask1(lreg));
		    jit_retval_d(FPR[lreg]);
		}
		goto finish;
	    }
	    else {
		sync_d(lreg);
		sync_w(rreg);
	    }
	}
	else {
	    sync_d(lreg);
	    if (rty == t_float)
		sync_d(rreg);
	    else
		assert(rty == t_void);
	}
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(ovm_o_div2, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_shl(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*jump;
    jit_node_t		*done;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		assert(rop->u.w >= 0);
		if (rop->u.w == 0)
		    /* no shift */
		    goto finish;
		if (rop->u.w < __WORDSIZE) {
		    jit_lshi(JIT_R0, GPR[lreg], rop->u.w);
		    jit_rshi(JIT_R0, JIT_R0, rop->u.w);
		    jump = jit_bner(JIT_R0, GPR[lreg]);
		    jit_lshi(GPR[lreg], GPR[lreg], rop->u.w);
		    sync_w(lreg);
		    done = jit_jmpi();
		    jit_patch(jump);
		}
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    /* must check sign and range of shift */
	    sync_w(lreg);
	    sync_w(rreg);
	}
	else
	    assert(rty == t_void);
    }
    else if (lty == t_single || lty == t_float) {
	if (lty == t_single)
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	sync_d(lreg);
	if (rreg == -1) {
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	if (rty == t_half || rty == t_word)
	    sync_w(rreg);
	else if (rty == t_single) {
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	    sync_d(rreg);
	}
	else if (rty == t_float)
	    sync_d(rreg);
	else
	    assert(rty == t_void);
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(ovm_o_shl, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_shr(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*jump;
    oword_t		 fshr;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		assert(rop->u.w >= 0);
		if (rop->u.w < __WORDSIZE)
		    jit_rshi(GPR[lreg], GPR[lreg], rop->u.w);
		else
		    jit_rshi(GPR[lreg], GPR[lreg], __WORDSIZE - 1);
		/* no type change and no overflow possible */
		goto finish;
	    }
	    else {
		/* must check sign and range of shift */
		sync_w(lreg);
		sync_w(rreg);
	    }
	}
	else
	    assert(rty == t_void);
    }
    else if (lty == t_single || lty == t_float) {
	if (lty == t_single)
	    jit_extr_f_d(FPR[lreg], FPR[lreg]);
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		fshr = rop->u.w;
		emit_load(rop);
		rreg = rop->u.w;
		sync_d(lreg);
		sync_w(rreg);
		jit_prepare();
		jit_pushargr_d(FPR[lreg]);
		jit_pushargi(-fshr);
		emit_finish(ldexp, mask2(lreg, rreg));
		jit_retval_d(FPR[lreg]);
		jit_prepare();
		jit_pushargr_d(FPR[lreg]);
		jit_finishi(floor);
		jit_retval_d(FPR[lreg]);
		jit_truncr_d(GPR[lreg], FPR[lreg]);
		jit_extr_d(FPR[rreg], GPR[lreg]);
		jump = jit_bner_d(FPR[lreg], FPR[rreg]);
		sync_w(lreg);
		done = jit_jmpi();
		jit_patch(jump);
	    }
	    else {
		sync_d(lreg);
		sync_w(rreg);
	    }
	}
	else {
	    if (rty == t_single || rty == t_float) {
		sync_d(lreg);
		if (rreg == -1) {
		    assert(rty == t_float);
		    emit_load(rop);
		}
		else if (rty == t_single)
		    jit_extr_f_d(FPR[rreg], FPR[rreg]);
		sync_d(rreg);
	    }
	    else
		assert(rty == t_void);
	}
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(ovm_o_shr, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_add_sub(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*jump;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half) {
	if (rty == t_half) {
	    if (rreg == -1) {
		if (tok == tok_add)
		    jit_addi(GPR[lreg], GPR[lreg], rop->u.w);
		else
		    jit_subi(GPR[lreg], GPR[lreg], rop->u.w);
	    }
	    else {
		if (tok == tok_add)
		    jit_addr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else
		    jit_subr(GPR[lreg], GPR[lreg], GPR[rreg]);
	    }
	    emit_set_type(lop, t_word);
	    goto finish;
	}
	else if (rty == t_word) {
	w_w:
	    jit_movr(JIT_R0, GPR[lreg]);
	    if (rreg == -1) {
		if (tok == tok_add)
		    jump = jit_boaddi(GPR[lreg], rop->u.w);
		else
		    jump = jit_bosubi(GPR[lreg], rop->u.w);
	    }
	    else {
		if (tok == tok_add)
		    jump = jit_boaddr(GPR[lreg], GPR[rreg]);
		else
		    jump = jit_bosubr(GPR[lreg], GPR[rreg]);
	    }
	    sync_w(lreg);
	    done = jit_jmpi();
	    jit_patch(jump);
	    sync_w_w(lreg, JIT_R0);
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(rreg);
	    function = tok == tok_add ? ovm_w_add : ovm_w_sub;
	}
	else {
	    assert(rty == t_void);
	    function = tok == tok_add ? ovm_w_add : ovm_w_sub;
	}
    }
    else if (lty == t_word) {
	if (rty == t_half || rty == t_word)
	    goto w_w;
	assert(rty == t_void);
	function = tok == tok_add ? ovm_w_add : ovm_w_sub;
    }
    else if (lty == t_single) {
	if (rreg == -1) {
	    assert(rty == t_float);
	    if (tok == tok_add)
		jit_addi_f(FPR[lreg], FPR[lreg], rop->u.d);
	    else
		jit_subi_f(FPR[lreg], FPR[lreg], rop->u.d);
	}
	else {
	    assert(rty == t_single);
	    if (tok == tok_add)
		jit_addr_f(FPR[lreg], FPR[lreg], FPR[rreg]);
	    else
		jit_subr_f(FPR[lreg], FPR[lreg], FPR[rreg]);
	}
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_float) {
	    if (rreg == -1) {
		if (tok == tok_add)
		    jit_addi_d(FPR[lreg], FPR[lreg], rop->u.d);
		else
		    jit_subi_d(FPR[lreg], FPR[lreg], rop->u.d);
	    }
	    else {
		if (tok == tok_add)
		    jit_addr_d(FPR[lreg], FPR[lreg], FPR[rreg]);
		else
		    jit_subr_d(FPR[lreg], FPR[lreg], FPR[rreg]);
	    }
	    goto finish;
	}
	assert(rty == t_void);
	function = tok == tok_add ? ovm_d_add : ovm_d_sub;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
	function = tok == tok_add ? ovm_o_add : ovm_o_sub;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_mul(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*zjmp;
    jit_node_t		*njmp;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half) {
	if (rty == t_half) {
	    if (rreg == -1)
		jit_muli(GPR[lreg], GPR[lreg], rop->u.w);
	    else
		jit_mulr(GPR[lreg], GPR[lreg], GPR[rreg]);
	    emit_set_type(lop, t_word);
	    goto finish;
	}
	if (rty == t_word) {
	w_w:
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(lreg);
	    sync_w(rreg);
	    jit_qmulr(GPR[lreg], JIT_R0, GPR[lreg], GPR[rreg]);
	    /* if top word is 0 or -1 did not overflow */
	    zjmp = jit_bnei(JIT_R0, 0);
	    njmp = jit_bnei(JIT_R0, -1);
	    spill_w(lreg);
	    done = jit_jmpi();
	    jit_patch(zjmp);
	    jit_patch(njmp);
	}
	else
	    assert(rty == t_void);
	function = ovm_w_mul;
    }
    else if (lty == t_word) {
	if (rty == t_half || rty == t_word)
	    goto w_w;
	else
	    assert(rty == t_void);
	function = ovm_w_mul;
    }
    else if (lty == t_single) {
	if (rreg == -1) {
	    assert(rty == t_float);
	    jit_muli_f(FPR[lreg], FPR[lreg], rop->u.d);
	}
	else {
	    assert(rty == t_single);
	    jit_mulr_f(FPR[lreg], FPR[lreg], FPR[rreg]);
	}
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_float) {
	    if (rreg == -1)
		jit_muli_d(FPR[lreg], FPR[lreg], rop->u.d);
	    else
		jit_mulr_d(FPR[lreg], FPR[lreg], FPR[rreg]);
	    goto finish;
	}
	assert(rty == t_void);
	function = ovm_d_mul;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
	function = ovm_o_mul;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_div(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*jump;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(lreg);
	    sync_w(rreg);
	    jit_qdivr(GPR[lreg], JIT_R0, GPR[lreg], GPR[rreg]);
	    /* if top word is 0 division is exact */
	    jump = jit_bnei(JIT_R0, 0);
	    spill_w(lreg);
	    done = jit_jmpi();
	    jit_patch(jump);
	}
	else
	    assert(rty == t_void);
	function = ovm_w_div;
    }
    else if (lty == t_single) {
	if (rreg == -1) {
	    assert(rty == t_float);
	    jit_divi_f(FPR[lreg], FPR[lreg], rop->u.d);
	}
	else {
	    assert(rty == t_single);
	    jit_divr_f(FPR[lreg], FPR[lreg], FPR[rreg]);
	}
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_float) {
	    if (rreg == -1)
		jit_divi_d(FPR[lreg], FPR[lreg], rop->u.d);
	    else
		jit_divr_d(FPR[lreg], FPR[lreg], FPR[rreg]);
	    goto finish;
	}
	assert(rty == t_void);
	function = ovm_d_div;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
	function = ovm_o_div;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_trunc2(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*jump;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1)
		jit_divi(GPR[lreg], GPR[lreg], rop->u.w);
	    else
		jit_divr(GPR[lreg], GPR[lreg], GPR[rreg]);
	    goto finish;
	}
	assert(rty == t_void);
	function = ovm_w_trunc2;
    }
    else if (lty == t_single || lty == t_float) {
	if (rreg == -1) {
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	if (rty == t_single || rty == t_float) {
	    if (lty == t_single)
		jit_extr_f_d(FPR[lreg], FPR[lreg]);
	    if (rty == t_single)
		jit_extr_f_d(FPR[rreg], FPR[rreg]);
	    sync_d(lreg);
	    sync_d(rreg);
	    jit_divr_d(FPR[lreg], FPR[lreg], FPR[rreg]);
	    jit_addi(JIT_R0, JIT_FP, scratch);
	    jit_prepare();
	    jit_pushargr_d(FPR[lreg]);
	    jit_pushargr(JIT_R0);
	    emit_finish(modf, mask2(lreg, rreg));
	    jit_ldxi_d(FPR[lreg], JIT_FP, scratch);
	    jit_truncr_d(GPR[lreg], FPR[lreg]);
	    jit_extr_d(FPR[rreg], GPR[lreg]);
	    /* Done if result fits in a word */
	    jump = jit_bner_d(FPR[lreg], FPR[rreg]);
	    sync_w(lreg);
	    done = jit_jmpi();
	    jit_patch(jump);
	}
	function = ovm_d_trunc2;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
	function = ovm_o_trunc2;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_rem(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1)
		jit_remi(GPR[lreg], GPR[lreg], rop->u.w);
	    else
		jit_remr(GPR[lreg], GPR[lreg], GPR[rreg]);
	    goto finish;
	}
	assert(rty == t_void);
	function = ovm_w_rem;
    }
    else if (lty == t_single) {
	if (rreg != -1 && (rty == t_half || rty == t_word))
	    jit_extr_f(FPR[rreg], GPR[rreg]);
	jit_prepare();
	jit_pushargr_f(FPR[lreg]);
	if (rreg == -1)
	    jit_pushargi_f(rty == t_half || rty == t_word ?
			   (jit_float64_t)rop->u.w : rop->u.d);
	else
	    jit_pushargr_f(FPR[rreg]);
	jit_finishi(fmodf);
	jit_retval_f(FPR[lreg]);
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_half || rty == t_word || rty == t_float) {
	    jit_prepare();
	    jit_pushargr_d(FPR[lreg]);
	    if (rreg == -1)
		jit_pushargi_d(rty == t_half || rty == t_word ?
			       (jit_float64_t)rop->u.w : rop->u.d);
	    else
		jit_pushargr_d(FPR[rreg]);
	    jit_finishi(fmod);
	    jit_retval_d(FPR[lreg]);
	    goto finish;
	}
	assert(rty == t_void);
	function = ovm_d_rem;
    }
    else {
	if (rty != t_half && rty != t_word && rty != t_float)
	    assert(rty == t_void);
	function = ovm_o_rem;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_complex(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	sync_w(lreg);
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w == 0)
		    goto finish;
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(rreg);
	}
	else
	    assert(rty == t_void);
	function = ovm_w_complex;
    }
    else if (lty == t_single) {
	if (rreg == -1) {
	    if (rop->u.d == 0)
		goto finish;
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	jit_extr_f_d(FPR[lreg], FPR[lreg]);
	sync_d(lreg);
	if (rty == t_single)
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	else
	    assert(rty == t_float);
	sync_d(rreg);
	function = ovm_d_complex;
    }
    else if (lty == t_float) {
	if (rreg == -1) {
	    if (rop->u.d == 0)
		goto finish;
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	sync_d(lreg);
	if (rty == t_float)
	    sync_d(rreg);
	function = ovm_d_complex;
    }
    else {
	assert(lty == t_void);
	function = ovm_o_complex;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_atan2_hypot(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	if (!cfg_float_format) {
	    if (rty == t_half || rty == t_word) {
		jit_extr_d(FPR[lreg], GPR[lreg]);
		if (rreg == -1) {
		    rop->u.d = rop->u.w;
		    emit_set_type(rop, t_float);
		    emit_load(rop);
		    rreg = rop->u.w;
		}
		else
		    jit_extr_d(FPR[rreg], GPR[rreg]);
		jit_prepare();
		jit_pushargr_d(FPR[lreg]);
		jit_pushargr_d(FPR[rreg]);
		if (tok == tok_atan2)
		    emit_finish(atan2, mask1(lreg));
		else
		    emit_finish(hypot, mask1(lreg));
		jit_retval_d(FPR[lreg]);
		emit_set_type(lop, t_float);
		goto finish;
	    }
	}
	sync_w(lreg);
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (rop->u.w == 0)
		    goto finish;
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_w(rreg);
	}
	else
	    assert(rty == t_void);
	if (tok == tok_atan2)
	    function = ovm_w_atan2;
	else
	    function = ovm_w_hypot;
    }
    else if (lty == t_single) {
	jit_extr_f_d(FPR[lreg], FPR[lreg]);
	emit_set_type(lop, t_float);
	if (rreg == -1) {
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	else {
	    assert(rty == t_single);
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	}
	jit_prepare();
	jit_pushargr_d(FPR[lreg]);
	jit_pushargr_d(FPR[rreg]);
	if (tok == tok_atan2)
	    emit_finish(atan2, mask1(lreg));
	else
	    emit_finish(hypot, mask1(lreg));
	jit_retval_d(FPR[lreg]);
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_float) {
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    jit_prepare();
	    jit_pushargr_d(FPR[lreg]);
	    jit_pushargr_d(FPR[rreg]);
	    if (tok == tok_atan2)
		emit_finish(atan2, mask1(lreg));
	    else
		emit_finish(hypot, mask1(lreg));
	    jit_retval_d(FPR[lreg]);
	    goto finish;
	}
	sync_d(lreg);
	if (rty == t_float)
	    sync_d(rreg);
	if (tok == tok_atan2)
	    function = ovm_d_atan2;
	else
	    function = ovm_d_hypot;
    }
    else {
	assert(lty == t_void);
	if (tok == tok_atan2)
	    function = ovm_o_atan2;
	else
	    function = ovm_o_hypot;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    emit_set_type(lop, t_void);
finish:
    operand_unget(1);
}

static void
emit_pow(ooperand_t *lop, otoken_t tok, ooperand_t *rop)
{
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*done;
    jit_node_t		*jump;
    oobject_t		 function;

    lreg = lop->u.w;
    switch (rop->t) {
	case t_half:	case t_word:
	case t_single:	case t_float:
	    rreg = -1;
	    break;
	default:
	    rreg = rop->u.w;
	    break;
    }
    done = jump = null;
    lty = emit_get_type(lop);
    rty = emit_get_type(rop);
    if (lty == t_half || lty == t_word) {
	jit_extr_d(FPR[lreg], GPR[lreg]);
	sync_d(lreg);
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		rop->u.d = rop->u.w;
		emit_set_type(rop, t_float);
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    else
		jit_extr_d(FPR[rreg], GPR[rreg]);
	    sync_d(rreg);
	    jump = jit_blti(GPR[lreg], 0);
	    jit_prepare();
	    jit_pushargr_d(FPR[lreg]);
	    jit_pushargr_d(FPR[rreg]);
	    emit_finish(pow, mask1(lreg));
	    jit_retval_d(FPR[lreg]);
	    sync_d(lreg);
	    done = jit_jmpi();
	}
	function = ovm_d_pow;
    }
    else if (lty == t_single) {
	jit_extr_f_d(FPR[lreg], FPR[lreg]);
	sync_d(lreg);
	if (rreg == -1) {
	    emit_load(rop);
	    rreg = rop->u.w;
	}
	else
	    jit_extr_f_d(FPR[rreg], FPR[rreg]);
	sync_d(rreg);
	jump = jit_blti_d(FPR[lreg], 0.0);
	jit_prepare();
	jit_pushargr_d(FPR[lreg]);
	jit_pushargr_d(FPR[rreg]);
	emit_finish(pow, mask1(lreg));
	jit_retval_d(FPR[lreg]);
	sync_d(lreg);
	done = jit_jmpi();
	function = ovm_d_pow;
    }
    else if (lty == t_float) {
	sync_d(lreg);
	if (rty == t_float) {
	    if (rreg == -1) {
		emit_load(rop);
		rreg = rop->u.w;
	    }
	    sync_d(rreg);
	    jump = jit_blti_d(FPR[lreg], 0.0);
	    jit_prepare();
	    jit_pushargr_d(FPR[lreg]);
	    jit_pushargr_d(FPR[rreg]);
	    emit_finish(pow, mask1(lreg));
	    jit_retval_d(FPR[lreg]);
	    sync_d(lreg);
	    done = jit_jmpi();
	}
	function = ovm_d_pow;
    }
    else {
	assert(lty == t_void);
	function = ovm_o_pow;
    }
    if (jump)
	jit_patch(jump);
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    if (done)
	jit_patch(done);
    emit_set_type(lop, t_void);
    operand_unget(1);
}
#endif
