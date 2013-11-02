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
emit_cond(oast_t *ast);

static void
emit_cmp(oast_t *ast);

static void
emit_test(oast_t *ast, obool_t jmpif, oword_t level);
#endif

#if defined(CODE)
static void
emit_cond(oast_t *ast)
{
    ooperand_t		*lop;
    ooperand_t		*rop;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*jump;
    otoken_t		 token;

    token = get_token(ast);
    emit(ast->l.ast);
    lop = operand_top();
    emit_load(lop);
    lreg = lop->u.w;
    switch (emit_get_type(lop)) {
	case t_half:		case t_word:
	    jit_nei(GPR[lreg], GPR[lreg], 0);
	    break;
	case t_single:
	    jit_nei_f(GPR[lreg], FPR[lreg], 0.0);
	    break;
	case t_float:
	    jit_nei_d(GPR[lreg], FPR[lreg], 0.0);
	    break;
	default:
	    load_r(lreg);
	    jit_prepare();
	    jit_pushargr(GPR[lreg]);
	    emit_finish(ovm_bool, mask1(lreg));
	    load_w(lreg);
	    break;
    }
    emit_set_type(lop, t_half);
    if (token == tok_andand)
	jump = jit_beqi(GPR[lreg], 0);
    else {
	assert(token == tok_oror);
	jump = jit_bnei(GPR[lreg], 0);
    }

    emit(ast->r.ast);
    rop = operand_top();
    emit_load(rop);
    rreg = rop->u.w;
    switch (emit_get_type(rop)) {
	case t_half:		case t_word:
	    jit_nei(GPR[lreg], GPR[rreg], 0);
	    break;
	case t_single:
	    jit_nei_f(GPR[lreg], FPR[rreg], 0.0);
	    break;
	case t_float:
	    jit_nei_d(GPR[lreg], FPR[rreg], 0.0);
	    break;
	default:
	    load_r_w(rreg, JIT_R0);
	    jit_prepare();
	    jit_pushargr(JIT_R0);
	    emit_finish(ovm_bool, mask1(lreg));
	    load_w_w(rreg, lreg);
	    break;
    }

    jit_patch(jump);

    assert((lop->t & (t_half|t_register)) == (t_half|t_register));
    /*   If it was spilled (when evaluating right operand),
     * does not need to reload as value was overwritten */
    lop->t = t_half | t_register;
    operand_unget(1);
}

static void
emit_cmp(oast_t *ast)
{
    ooperand_t		*lop;
    ooperand_t		*rop;
    otype_t		 lty;
    otype_t		 rty;
    oword_t		 lreg;
    oword_t		 rreg;
    otoken_t		 token;
    oobject_t		 function;

    token = get_token(ast);
    emit(ast->l.ast);
    lop = operand_top();
    emit(ast->r.ast);
    rop = operand_top();
    emit_load(lop);
    emit_binary_setup(lop, token, rop);
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
	if (rty == t_half || rty == t_word) {
	    if (rreg == -1) {
		if (token == tok_lt)
		    jit_lti(GPR[lreg], GPR[lreg], rop->u.w);
		else if (token == tok_le)
		    jit_lei(GPR[lreg], GPR[lreg], rop->u.w);
		else if (token == tok_eq)
		    jit_eqi(GPR[lreg], GPR[lreg], rop->u.w);
		else if (token == tok_ge)
		    jit_gei(GPR[lreg], GPR[lreg], rop->u.w);
		else if (token == tok_gt)
		    jit_gti(GPR[lreg], GPR[lreg], rop->u.w);
		else
		    jit_nei(GPR[lreg], GPR[lreg], rop->u.w);
	    }
	    else {
		if (token == tok_lt)
		    jit_ltr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else if (token == tok_le)
		    jit_ler(GPR[lreg], GPR[lreg], GPR[rreg]);
		else if (token == tok_eq)
		    jit_eqr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else if (token == tok_ge)
		    jit_ger(GPR[lreg], GPR[lreg], GPR[rreg]);
		else if (token == tok_gt)
		    jit_gtr(GPR[lreg], GPR[lreg], GPR[rreg]);
		else
		    jit_ner(GPR[lreg], GPR[lreg], GPR[rreg]);
	    }
	    goto finish;
	}
	assert(rty == t_void);
	if (token == tok_lt)		function = ovm_w_lt;
	else if (token == tok_le)	function = ovm_w_le;
	else if (token == tok_eq)	function = ovm_w_eq;
	else if (token == tok_ge)	function = ovm_w_ge;
	else if (token == tok_gt)	function = ovm_w_gt;
	else				function = ovm_w_ne;
    }
    else if (lty == t_single) {
	assert(rty == t_single);
	if (rreg == -1) {
	    if (token == tok_lt)
		jit_lti_f(GPR[lreg], FPR[lreg], rop->u.d);
	    else if (token == tok_le)
		jit_lei_f(GPR[lreg], FPR[lreg], rop->u.d);
	    else if (token == tok_eq)
		jit_eqi_f(GPR[lreg], FPR[lreg], rop->u.d);
	    else if (token == tok_ge)
		jit_gei_f(GPR[lreg], FPR[lreg], rop->u.d);
	    else if (token == tok_gt)
		jit_gti_f(GPR[lreg], FPR[lreg], rop->u.d);
	    else
		jit_nei_f(GPR[lreg], FPR[lreg], rop->u.d);
	}
	else {
	    if (token == tok_lt)
		jit_ltr_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	    else if (token == tok_le)
		jit_ler_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	    else if (token == tok_eq)
		jit_eqr_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	    else if (token == tok_ge)
		jit_ger_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	    else if (token == tok_gt)
		jit_gtr_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	    else
		jit_ner_f(GPR[lreg], FPR[lreg], FPR[rreg]);
	}
	goto finish;
    }
    else if (lty == t_float) {
	if (rty == t_float) {
	    if (rreg == -1) {
		if (token == tok_lt)
		    jit_lti_d(GPR[lreg], FPR[lreg], rop->u.d);
		else if (token == tok_le)
		    jit_lei_d(GPR[lreg], FPR[lreg], rop->u.d);
		else if (token == tok_eq)
		    jit_eqi_d(GPR[lreg], FPR[lreg], rop->u.d);
		else if (token == tok_ge)
		    jit_gei_d(GPR[lreg], FPR[lreg], rop->u.d);
		else if (token == tok_gt)
		    jit_gti_d(GPR[lreg], FPR[lreg], rop->u.d);
		else
		    jit_nei_d(GPR[lreg], FPR[lreg], rop->u.d);
	    }
	    else {
		if (token == tok_lt)
		    jit_ltr_d(GPR[lreg], FPR[lreg], FPR[rreg]);
		else if (token == tok_le)
		    jit_ler_d(GPR[lreg], FPR[lreg], FPR[rreg]);
		else if (token == tok_eq)
		    jit_eqr_d(GPR[lreg], FPR[lreg], FPR[rreg]);
		else if (token == tok_ge)
		    jit_ger_d(GPR[lreg], FPR[lreg], FPR[rreg]);
		else if (token == tok_gt)
		    jit_gtr_d(GPR[lreg], FPR[lreg], FPR[rreg]);
		else
		    jit_ner_d(GPR[lreg], FPR[lreg], FPR[rreg]);
	    }
	    goto finish;
	}
	assert(rty == t_void);
	if (token == tok_lt)		function = ovm_d_lt;
	else if (token == tok_le)	function = ovm_d_le;
	else if (token == tok_eq)	function = ovm_d_eq;
	else if (token == tok_ge)	function = ovm_d_ge;
	else if (token == tok_gt)	function = ovm_d_gt;
	else				function = ovm_d_ne;
    }
    else {
	assert(lty == t_void);
	if (token == tok_lt)		function = ovm_o_lt;
	else if (token == tok_le)	function = ovm_o_le;
	else if (token == tok_eq)	function = ovm_o_eq;
	else if (token == tok_ge)	function = ovm_o_ge;
	else if (token == tok_gt)	function = ovm_o_gt;
	else				function = ovm_o_ne;
    }
    load_r(lreg);
    load_r_w(rreg, JIT_R0);
    jit_prepare();
    jit_pushargr(GPR[lreg]);
    jit_pushargr(JIT_R0);
    emit_finish(function, mask1(lreg));
    load_w(lreg);
finish:
    emit_set_type(lop, t_half);
    operand_unget(1);
}

static void
emit_test(oast_t *ast, obool_t jmpif, oword_t level)
{
    otype_t		 lty;
    otype_t		 rty;
    ooperand_t		*lop;
    ooperand_t		*rop;
    oword_t		 lreg;
    oword_t		 rreg;
    jit_node_t		*node;
    ojump_t		*ajmp;
    ojump_t		*bjmp;
    ovector_t		*avec;
    ovector_t		*bvec;
    oobject_t		 function;

    switch (ast->token) {
	case tok_andand:	case tok_oror:
	    if (branch->offset)
		bjmp = jump_top();
	    else
		bjmp = null;
	    ajmp = jump_get(ast->token);
	    emit_test(ast->l.ast, ast->token == tok_oror, level + 1);
	    avec = ajmp->t;
	    if (avec->offset) {
		if (ast->token == tok_oror) {
		    /* Lift true test jumps down for oror */
		    bvec = bjmp->t;
		    do
			add_jump(bvec, avec->v.ptr[--avec->offset]);
		    while (avec->offset);
		}
		else {
		    /* Must check next test for andand */
		    node = jit_label();
		    do
			jit_patch_at(avec->v.ptr[--avec->offset], node);
		    while (avec->offset);
		}
	    }
	    avec = ajmp->f;
	    if (avec->offset) {
		if (ast->token == tok_andand) {
		    /* Lift false test jumps down for andand */
		    bvec = bjmp->f;
		    do
			add_jump(bvec, avec->v.ptr[--avec->offset]);
		    while (avec->offset);
		}
		else {
		    /* Must check next test for oror */
		    node = jit_label();
		    do
			jit_patch_at(avec->v.ptr[--avec->offset], node);
		    while (avec->offset);
		}
	    }
	    /* Evaluate right operand */
	    emit_test(ast->r.ast, false, level + 1);
	    if (bjmp == null) {
		assert(branch->offset > 1);
		bjmp = branch->v.ptr[branch->offset - 2];
	    }
	    avec = ajmp->t;
	    if (avec->offset) {
		if (level > 0) {
		    /* Lift true test jumps down if nested */
		    bvec = bjmp->t;
		    do
			add_jump(bvec, avec->v.ptr[--avec->offset]);
		    while (avec->offset);
		}
		else {
		    /* Add label before true code if at toplevel */
		    node = jit_label();
		    do
			jit_patch_at(avec->v.ptr[--avec->offset], node);
		    while (avec->offset);
		}
	    }
	    avec = ajmp->f;
	    if (avec->offset) {
		/* Always lift down rigt operand evaluating to false */
		bvec = bjmp->f;
		do
		    add_jump(bvec, avec->v.ptr[--avec->offset]);
		while (avec->offset);
	    }
	    jump_unget(1);
	    break;
	case tok_lt:	case tok_le:	case tok_eq:
	case tok_ge:	case tok_gt:	case tok_ne:
	    emit(ast->l.ast);
	    lop = operand_top();
	    emit(ast->r.ast);
	    rop = operand_top();
	    emit_load(lop);
	    lreg = lop->u.w;
	    emit_binary_setup(lop, ast->token, rop);
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
		if (rty == t_half || rty == t_word) {
		    if (rreg == -1) {
			if (jmpif) {
			    if (ast->token == tok_lt)
				node = jit_blti(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_le)
				node = jit_blei(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_eq)
				node = jit_beqi(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_ge)
				node = jit_bgei(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_gt)
				node = jit_bgti(GPR[lreg], rop->u.w);
			    else
				node = jit_bnei(GPR[lreg], rop->u.w);
			}
			else {
			    if (ast->token == tok_lt)
				node = jit_bgei(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_le)
				node = jit_bgti(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_eq)
				node = jit_bnei(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_ge)
				node = jit_blti(GPR[lreg], rop->u.w);
			    else if (ast->token == tok_gt)
				node = jit_blei(GPR[lreg], rop->u.w);
			    else
				node = jit_beqi(GPR[lreg], rop->u.w);
			}
		    }
		    else {
			if (jmpif) {
			    if (ast->token == tok_lt)
				node = jit_bltr(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_le)
				node = jit_bler(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_eq)
				node = jit_beqr(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_ge)
				node = jit_bger(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_gt)
				node = jit_bgtr(GPR[lreg], GPR[rreg]);
			    else
				node = jit_bner(GPR[lreg], GPR[rreg]);
			}
			else {
			    if (ast->token == tok_lt)
				node = jit_bger(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_le)
				node = jit_bgtr(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_eq)
				node = jit_bner(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_ge)
				node = jit_bltr(GPR[lreg], GPR[rreg]);
			    else if (ast->token == tok_gt)
				node = jit_bler(GPR[lreg], GPR[rreg]);
			    else
				node = jit_beqr(GPR[lreg], GPR[rreg]);
			}
		    }
		    goto finish;
		}
		if (ast->token == tok_lt)	function = ovm_w_lt;
		else if (ast->token == tok_le)	function = ovm_w_le;
		else if (ast->token == tok_eq)	function = ovm_w_eq;
		else if (ast->token == tok_ge)	function = ovm_w_ge;
		else if (ast->token == tok_gt)	function = ovm_w_gt;
		else				function = ovm_w_ne;
	    }
	    else if (lty == t_single) {
		if (rreg == -1) {
		    assert(rty == t_float);
		    if (jmpif) {
			if (ast->token == tok_lt)
			    node = jit_blti_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_le)
			    node = jit_blei_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_eq)
			    node = jit_beqi_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_ge)
			    node = jit_bgei_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_gt)
			    node = jit_bgti_f(FPR[lreg], rop->u.d);
			else
			    node = jit_bnei_f(FPR[lreg], rop->u.d);
		    }
		    else {
			if (ast->token == tok_lt)
			    node = jit_bgei_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_le)
			    node = jit_bgti_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_eq)
			    node = jit_bnei_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_ge)
			    node = jit_blti_f(FPR[lreg], rop->u.d);
			else if (ast->token == tok_gt)
			    node = jit_blei_f(FPR[lreg], rop->u.d);
			else
			    node = jit_beqi_f(FPR[lreg], rop->u.d);
			}
		}
		else {
		    assert(rty == t_single);
		    if (jmpif) {
			if (ast->token == tok_lt)
			    node = jit_bltr_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_le)
			    node = jit_bler_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_eq)
			    node = jit_beqr_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_ge)
			    node = jit_bger_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_gt)
			    node = jit_bgtr_f(FPR[lreg], FPR[rreg]);
			else
			    node = jit_bner_f(FPR[lreg], FPR[rreg]);
		    }
		    else {
			if (ast->token == tok_lt)
			    node = jit_bger_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_le)
			    node = jit_bgtr_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_eq)
			    node = jit_bner_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_ge)
			    node = jit_bltr_f(FPR[lreg], FPR[rreg]);
			else if (ast->token == tok_gt)
			    node = jit_bler_f(FPR[lreg], FPR[rreg]);
			else
			    node = jit_beqr_f(FPR[lreg], FPR[rreg]);
		    }
		}
		goto finish;
	    }
	    else if (lty == t_float) {
		if (rty == t_float) {
		    if (rreg == -1) {
			if (jmpif) {
			    if (ast->token == tok_lt)
				node = jit_blti_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_le)
				node = jit_blei_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_eq)
				node = jit_beqi_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_ge)
				node = jit_bgei_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_gt)
				node = jit_bgti_d(FPR[lreg], rop->u.d);
			    else
				node = jit_bnei_d(FPR[lreg], rop->u.d);
			}
			else {
			    if (ast->token == tok_lt)
				node = jit_bgei_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_le)
				node = jit_bgti_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_eq)
				node = jit_bnei_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_ge)
				node = jit_blti_d(FPR[lreg], rop->u.d);
			    else if (ast->token == tok_gt)
				node = jit_blei_d(FPR[lreg], rop->u.d);
			    else
				node = jit_beqi_d(FPR[lreg], rop->u.d);
			}
		    }
		    else {
			if (jmpif) {
			    if (ast->token == tok_lt)
				node = jit_bltr_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_le)
				node = jit_bler_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_eq)
				node = jit_beqr_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_ge)
				node = jit_bger_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_gt)
				node = jit_bgtr_d(FPR[lreg], FPR[rreg]);
			    else
				node = jit_bner_d(FPR[lreg], FPR[rreg]);
			}
			else {
			    if (ast->token == tok_lt)
				node = jit_bger_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_le)
				node = jit_bgtr_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_eq)
				node = jit_bner_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_ge)
				node = jit_bltr_d(FPR[lreg], FPR[rreg]);
			    else if (ast->token == tok_gt)
				node = jit_bler_d(FPR[lreg], FPR[rreg]);
			    else
				node = jit_beqr_d(FPR[lreg], FPR[rreg]);
			}
		    }
		    goto finish;
		}
		assert(rty == t_void);
		if (ast->token == tok_lt)	function = ovm_d_lt;
		else if (ast->token == tok_le)	function = ovm_d_le;
		else if (ast->token == tok_eq)	function = ovm_d_eq;
		else if (ast->token == tok_ge)	function = ovm_d_ge;
		else if (ast->token == tok_gt)	function = ovm_d_gt;
		else				function = ovm_d_ne;
	    }
	    else {
		assert(lty == t_void);
		if (ast->token == tok_lt)	function = ovm_o_lt;
		else if (ast->token == tok_le)	function = ovm_o_le;
		else if (ast->token == tok_eq)	function = ovm_o_eq;
		else if (ast->token == tok_ge)	function = ovm_o_ge;
		else if (ast->token == tok_gt)	function = ovm_o_gt;
		else				function = ovm_o_ne;
	    }
	    load_r(lreg);
	    load_r_w(rreg, JIT_R0);
	    jit_prepare();
	    jit_pushargr(GPR[lreg]);
	    jit_pushargr(JIT_R0);
	    emit_finish(function, mask1(lreg));
	    load_w(lreg);
	    if (jmpif)		node = jit_bnei(GPR[lreg], 0);
	    else		node = jit_beqi(GPR[lreg], 0);
	finish:
	    if (jmpif)		add_t_jump(node);
	    else		add_f_jump(node);
	    operand_unget(2);
	    break;
	case tok_number:
	    node = jit_jmpi();
	    if (ofalse_p(ast->l.value) ^ jmpif)
		add_f_jump(node);
	    else
		add_t_jump(node);
	    break;
	default:
	    emit(ast);
	    lop = operand_top();
	    emit_load(lop);
	    lreg = lop->u.w;
	    lty = emit_get_type(lop);
	    if (lty == t_half || lty == t_word) {
		if (jmpif)	node = jit_bnei(GPR[lreg], 0);
		else		node = jit_beqi(GPR[lreg], 0);
	    }
	    else if (lty == t_single) {
		if (jmpif)	node = jit_bnei_f(FPR[lreg], 0.0);
		else		node = jit_beqi_f(FPR[lreg], 0.0);
	    }
	    else if (lty == t_float) {
		if (jmpif)	node = jit_bnei_d(FPR[lreg], 0.0);
		else		node = jit_beqi_d(FPR[lreg], 0.0);
	    }
	    else {
		load_r(lreg);
		jit_prepare();
		jit_pushargr(GPR[lreg]);
		emit_finish(ovm_bool, mask1(lreg));
		load_w(lreg);
		if (jmpif)	node = jit_bnei(GPR[lreg], 0);
		else		node = jit_beqi(GPR[lreg], 0);
	    }
	    if (jmpif)		add_t_jump(node);
	    else		add_f_jump(node);
	    operand_unget(1);
	    break;
    }
}
#endif
