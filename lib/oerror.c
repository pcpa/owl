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

#include <stdarg.h>
#include <stdlib.h>

/*
 * Prototypes
 */
static void
explain(oast_t *ast);

static void
message(ovector_t *name, oint32_t lineno, oint32_t column, const char *prefix,
	const char *format, va_list ap);

static oword_t
print(const char *format, ...);

static oword_t
vprint(const char *format, va_list ap);

/*
 * Implementation
 */
void
oerror(const char *format, ...)
{
    va_list		ap;

    if (likely(std_error && std_error->fileno >= 0)) {
	va_start(ap, format);
	message(null, 0, 0, "error", format, ap);
	va_end(ap);
    }
    exit(-1);
}

void
onote_error(onote_t *note, const char *format, ...)
{
    va_list		ap;

    if (likely(std_error && std_error->fileno >= 0)) {
	va_start(ap, format);
	message(note->name, note->lineno, note->column, "error", format, ap);
	va_end(ap);
    }
    exit(-1);
}

void
owarn(const char *format, ...)
{
    va_list		ap;

    if (likely(std_error && std_error->fileno >= 0)) {
	va_start(ap, format);
	message(null, 0, 0, "warning", format, ap);
    }
    va_end(ap);
}

void
onote_warn(onote_t *note, const char *format, ...)
{
    va_list		ap;

    if (likely(std_error && std_error->fileno >= 0)) {
	va_start(ap, format);
	message(note->name, note->lineno, note->column, "warning", format, ap);
	va_end(ap);
    }
}

void
oabort(oint8_t *code, const char *string)
{
    GET_THREAD_SELF()
    oint32_t		 i;
    oint8_t		*fp;
    oint8_t		*prev;
    char		*func;
    char		*name;
    jit_int32_t		 line;
    static obool_t	 aborting;

    if (aborting)
	abort();

    aborting = true;
    owrite(std_error, "error: unhandled '", 18);
    owrite(std_error, (oobject_t)string, strlen(string));
    owrite(std_error, "' exception\n", 12);

    i = 0;
    if (jit_get_note(code, &func, &name, &line)) {
	print("#0 %s:%d: %s\n", name, line, func);
	++i;
    }
    /* Search remaining stack frames */
    fp = thread_self->fp;
    for (prev = *(oint8_t **)(fp + PREV_OFFSET);
	 prev;
	 fp = prev, prev = *(oint8_t **)(fp + PREV_OFFSET)) {
	code = *(oint8_t **)(fp + RET_OFFSET);
	if (jit_get_note(code, &func, &name, &line)) {
	    print("#%d %s:%d: %s\n", i, name, line, func);
	    ++i;
	}
    }

    oflush(std_error);
    exit(-1);
}

char *
otoken_to_charp(otoken_t token)
{
    switch (token) {
	case tok_eof:		return ("<eof>");
	case tok_none:		return ("<none>");
	case tok_type:		return ("<type>");
	case tok_symbol:	return ("<symbol>");
	case tok_number:	return ("<number>");
	case tok_string:	return ("<string>");
	case tok_FILE:		return ("'__FILE__'");
	case tok_LINE:		return ("'__LINE__'");
	case tok_FUNCTION:	return ("'__FUNCTION__'");
	case tok_VA_ARGS:	return ("'__VA_ARGS__'");
	case tok_DATE:		return ("'__DATE__'");
	case tok_TIME:		return ("'__TIME__'");
	case tok_COUNTER:	return ("'__COUNTER__'");
	case tok_stringify:	return ("'#'");
	case tok_concat:	return ("'##'");
	case tok_oparen:	return ("'('");
	case tok_cparen:	return ("')'");
	case tok_obrack:	return ("'['");
	case tok_cbrack:	return ("']'");
	case tok_obrace:	return ("'{'");
	case tok_cbrace:	return ("'}'");
	case tok_semicollon:	return ("';'");
	case tok_collon:	return ("':'");
	case tok_comma:		return ("','");
	case tok_dot:		return ("'.'");
	case tok_ellipsis:	return ("'...'");
	case tok_set:		return ("'='");
	case tok_andset:	return ("'&='");
	case tok_orset:		return ("'|='");
	case tok_xorset:	return ("'^='");
	case tok_mul2set:	return ("'<<='");
	case tok_div2set:	return ("'>>='");
	case tok_shlset:	return ("'<<<='");
	case tok_shrset:	return ("'>>>='");
	case tok_addset:	return ("'+='");
	case tok_subset:	return ("'-='");
	case tok_mulset:	return ("'*='");
	case tok_divset:	return ("'/='");
	case tok_trunc2set:	return ("'\\='");
	case tok_remset:	return ("'%='");
	case tok_andand:	return ("'&&'");
	case tok_oror:		return ("'||'");
	case tok_ne:		return ("'!='");
	case tok_lt:		return ("'<'");
	case tok_le:		return ("'<='");
	case tok_eq:		return ("'=='");
	case tok_ge:		return ("'>='");
	case tok_gt:		return ("'>'");
	case tok_and:		return ("'&'");
	case tok_or:		return ("'|'");
	case tok_xor:		return ("'^'");
	case tok_mul2:		return ("'<<'");
	case tok_div2:		return ("'>>'");
	case tok_shl:		return ("'<<<'");
	case tok_shr:		return ("'>>>'");
	case tok_add:		return ("'+'");
	case tok_sub:		return ("'-'");
	case tok_mul:		return ("'*'");
	case tok_div:		return ("'/'");
	case tok_trunc2:	return ("'\\'");
	case tok_rem:		return ("'%'");
	case tok_atan2:		return ("'atan2'");
	case tok_pow:		return ("'pow'");
	case tok_hypot:		return ("'hypot'");
	case tok_complex:	return ("'complex'");
	case tok_subtypeof:	return ("'subtypeof'");
	case tok_renew:		return ("'renew'");
	case tok_sizeof:	return ("'sizeof'");
	case tok_typeof:	return ("'typeof'");
	case tok_new:		return ("'new'");
	case tok_thread:	return ("'thread'");
	case tok_not:		return ("'!'");
	case tok_com:		return ("'~'");
	case tok_plus:		return ("'+'");
	case tok_neg:		return ("'-'");
	case tok_signbit:	return ("'signbit'");
	case tok_signum:	return ("'signum'");
	case tok_rational:	return ("'rational'");
	case tok_integer_p:	return ("'integer_p'");
	case tok_rational_p:	return ("'rational_p'");
	case tok_float_p:	return ("'float_p'");
	case tok_real_p:	return ("'real_p'");
	case tok_complex_p:	return ("'complex_p'");
	case tok_number_p:	return ("'number_p'");
	case tok_finite_p:	return ("'finite_p'");
	case tok_inf_p:		return ("'inf_p'");
	case tok_nan_p:		return ("'nan_p'");
	case tok_num:		return ("'num'");
	case tok_den:		return ("'den'");
	case tok_real:		return ("'real'");
	case tok_imag:		return ("'imag'");
	case tok_arg:		return ("'arg'");
	case tok_conj:		return ("'conj'");
	case tok_floor:		return ("'floor'");
	case tok_trunc:		return ("'trunc'");
	case tok_round:		return ("'round'");
	case tok_ceil:		return ("'ceil'");
	case tok_abs:		return ("'abs'");
	case tok_sqrt:		return ("'sqrt'");
	case tok_cbrt:		return ("'cbrt'");
	case tok_sin:		return ("'sin'");
	case tok_cos:		return ("'cos'");
	case tok_tan:		return ("'tan'");
	case tok_asin:		return ("'asin'");
	case tok_acos:		return ("'acos'");
	case tok_atan:		return ("'atan'");
	case tok_sinh:		return ("'sinh'");
	case tok_cosh:		return ("'cosh'");
	case tok_tanh:		return ("'tanh'");
	case tok_asinh:		return ("'asinh'");
	case tok_acosh:		return ("'acosh'");
	case tok_atanh:		return ("'atanh'");
	case tok_proj:		return ("'proj'");
	case tok_exp:		return ("'exp'");
	case tok_log:		return ("'log'");
	case tok_log2:		return ("'log2'");
	case tok_log10:		return ("'log10'");
	case tok_inc:		return ("'++'");
	case tok_dec:		return ("'--'");
	case tok_postinc:	return ("'++'");
	case tok_postdec:	return ("'--'");
	case tok_question:	return ("'?'");
	case tok_this:		return ("'this'");
	case tok_if:		return ("'if'");
	case tok_else:		return ("'else'");
	case tok_goto:		return ("'goto'");
	case tok_return:	return ("'return'");
	case tok_switch:	return ("'switch'");
	case tok_case:		return ("'case'");
	case tok_default:	return ("'default'");
	case tok_break:		return ("'break'");
	case tok_continue:	return ("'continue'");
	case tok_try:		return ("'try'");
	case tok_catch:		return ("'catch'");
	case tok_throw:		return ("'throw'");
	case tok_finally:	return ("'finally'");
	case tok_for:		return ("'for'");
	case tok_do:		return ("'do'");
	case tok_while:		return ("'while'");
	case tok_class:		return ("'class'");
	case tok_namespace:	return ("'namespace'");
	case tok_enum:		return ("'enum'");
	case tok_label:		return ("<label>");
	case tok_list:		return ("<list>");
	case tok_vector:	return ("<vector>");
	case tok_vecdcl:	return ("<vecdcl>");
	case tok_vecnew:	return ("<vecnew>");
	case tok_hash:		return ("<hash>");
	case tok_code:		return ("<code>");
	case tok_data:		return ("<data>");
	case tok_new_type_data:	return ("<new_type_data>");
	case tok_function:	return ("<function>");
	case tok_declexpr:	return ("<declaration_expression>");
	case tok_decl:		return ("<declaration>");
	case tok_defn:		return ("<definition>");
	case tok_expr:		return ("<expression>");
	case tok_stat:		return ("<statement>");
	case tok_offset:	return ("<offset>");
	case tok_explicit:	return ("<explicit>");
	case tok_call:		return ("<call>");
	case tok_proto:		return ("<proto>");
	case tok_init:		return ("<initializer>");
	case tok_ctor:		return ("<constructor>");
    }
    abort();
}

static void
explain(oast_t *ast)
{
    char		*s;
    otype_t		 type;
    oformat_t		 format;
    union {
	oword_t		*wrd;
	ofloat_t	*flt;
	orat_t		 rat;
	ompz_t		 mpz;
	ompq_t		 mpq;
	ompr_t		 mpr;
	ocdd_t		*cdd;
	ocqq_t		 cqq;
	ompc_t		 mpc;
	osymbol_t	*sym;
	ovector_t	*vec;
	oobject_t	 ptr;
    } data;

    owrite(std_error, "(got ", 5);
    switch (ast->token) {
	case tok_number:
	    memset(&format, 0, sizeof(oformat_t));
	    format.radix = 10;
	    format.read = true;
	    data.ptr = ast->l.value;
	    switch (type = otype(data.ptr)) {
		case t_word:
		    oprint_wrd(std_error, &format, *data.wrd);
		    break;
		case t_float:
		    oprint_flt(std_error, &format, *data.flt);
		    break;
		case t_rat:
		    oprint_rat(std_error, &format, data.rat);
		    break;
		case t_mpz:
		    oprint_mpz(std_error, &format, data.mpz);
		    break;
		case t_mpq:
		    oprint_mpq(std_error, &format, data.mpq);
		    break;
		case t_mpr:
		    oprint_mpr(std_error, &format, data.mpr);
		    break;
		case t_cdd:
		    oprint_cdd(std_error, &format, data.cdd);
		    break;
		case t_cqq:
		    oprint_cqq(std_error, &format, data.cqq);
		    break;
		default:
		    assert(type == t_mpc);
		    oprint_mpc(std_error, &format, data.mpc);
		break;
	    }
	    break;
	case tok_string:
	    memset(&format, 0, sizeof(oformat_t));
	    format.read = true;
	    oprint_str(std_error, &format, ast->l.value);
	    break;
	case tok_symbol:
	    data.ptr = ast->l.value;
	    oputc(std_error, '\'');
	    owrite(std_error, data.sym->name->v.ptr, data.sym->name->length);
	    oputc(std_error, '\'');
	    break;
	default:
	    s = otoken_to_charp(ast->token);
	    owrite(std_error, s, strlen(s));
	    break;
    }
    oputc(std_error, ')');
}

static void
message(ovector_t *name, oint32_t lineno, oint32_t column, const char *prefix,
	const char *format, va_list ap)
{
    if (name)
	print("%p:%d:%d:", name, lineno, column);
    print("%s: ", prefix);
    vprint(format, ap);
    oputc(std_error, '\n');
    oflush(std_error);
}

static oword_t
print(const char *format, ...)
{
    va_list	ap;
    oword_t	result;

    va_start(ap, format);
    result = vprint(format, ap);
    va_end(ap);

    return (result);
}

static oword_t
vprint(const char *format, va_list ap)
{
    int			 i;
    char		*s;
    oast_t		*ast;
    oformat_t		 fmt;
    char		*f, *t;
    oobject_t		 object;
    oword_t		 length;
    oword_t		 result;
    ovector_t		*vector;

    result = length = 0;
    memset(&fmt, 0, sizeof(oformat_t));
    fmt.radix = 10;
    f = t = (char *)format;
    while (*t) {
	if (*t == '%') {
	    owrite(std_error, f, t - f);
	    result += t - f;
	    switch (*++t) {
		case '%':
		    oputc(std_error, '%');
		    break;
		case 'c':
		    i = va_arg(ap, int);
		    result += oputc(std_error, i);
		    break;
		case 'd':
		    i = va_arg(ap, int);
		    result += oprint_wrd(std_error, &fmt, i);
		    break;
		case 's':
		    s = va_arg(ap, char*);
		    length = strlen(s);
		    owrite(std_error, s, length);
		    result += length;
		    break;
		case 'A':
		    ast = va_arg(ap, oast_t*);
		    explain(ast);
		    break;
		case 'p':
		default:
		    object = va_arg(ap, oobject_t);
		    if (object == null)
			owrite(std_error, "null", 4);
		    else {
			switch (otype(object)) {
			    case t_word:
				length = oprint_wrd(std_error, &fmt,
						    *(oword_t *)object);
				break;
			    case t_string:
				vector = object;
				length = vector->length;
				owrite(std_error, vector->v.obj, length);
				break;
			    case t_symbol:
				vector = ((osymbol_t *)object)->name;
				length = vector->length;
				owrite(std_error, vector->v.obj, length);
				break;
			    default:
				length = oprint_ptr(std_error, &fmt, object);
				break;
			}
			result += length;
			break;
		    }
	    }
	    t++;
	    f = t;
	}
	else
	    t++;
    }
    owrite(std_error, f, t - f);

    return (result + t - f);
}
