/*
 * Copyright (C) 2008-2012  Paulo Cesar Pereira de Andrade.
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

#define symbol_char_p(ch)	(symbol_set[(ch) >> 5] & (1 << ((ch) & 31)))
#define space_p(ch)		(spaces_set[(ch) >> 5] & (1 << ((ch) & 31)))

#define macro_lookahead(token, offset, vector)				\
    do {								\
	if (macro_next_token == tok_none)				\
	    macro_next_token = macro_do_lookahead(offset, vector);	\
	token = macro_next_token;					\
    } while (0)

#define macro_consume(offset)						\
    do {								\
	if (macro_next_token != tok_none) {				\
	    macro_next_token = tok_none;				\
	    ++*offset;							\
	}								\
	else								\
	    macro_cond_error();						\
    } while (0)

#define macro_skip_line()						\
    for (ch = macro_skip(); ch != eof && ch != '\n'; ch = macro_skip())

#define macro_state_inc()						\
    do {								\
	if (++macro_state->offset >= macro_state->length)				\
	    orenew_vector(macro_state, macro_state->length + 256);	\
    } while (0)

#define macro_get_else()	(macro_state->v.i8[macro_state->offset] &  1)
#define macro_set_else()	 macro_state->v.i8[macro_state->offset] |= 1
#define macro_get_true()	(macro_state->v.i8[macro_state->offset] &  2)
#define macro_set_true()	 macro_state->v.i8[macro_state->offset] |= 2
#define macro_get_skip()	(macro_state->v.i8[macro_state->offset] &  4)
#define macro_set_skip()	 macro_state->v.i8[macro_state->offset] |= 4
#define macro_clr_bits()	 macro_state->v.i8[macro_state->offset]  = 0

/*
 * Types
 */
typedef enum {
    mac_define,
    mac_undef,
    mac_include,
    mac_ifdef,
    mac_ifndef,
    mac_if,
    mac_elif,
    mac_else,
    mac_endif,
    mac_error,
    mac_warning,
} macro_t;

/*
 * Prototypes
 */
static oword_t
scan_chr(void);

static oword_t
scan_str(void);

static oword_t
scan_num(oint32_t ch, oint32_t radix, obool_t uppr);

static otype_t
scan_num_real(oint32_t ch, oint32_t radix, obool_t uppr);

static otype_t
scan_num_radix(oword_t offset, oint32_t radix, obool_t uppr);

static ovector_t *
read_ident(oint32_t ch);

static oobject_t
read_object(void);

static oobject_t
read_character(void);

static ovector_t *
read_string(void);

static oobject_t
read_number(oint32_t ch);

static oobject_t
read_symbol(oint32_t ch);

static oobject_t
expand_symbol(osymbol_t *symbol);

static oobject_t
read_keyword(oint32_t ch);

static oint32_t
getc_noeof(void);

static oint32_t
getc(void);

static void
ungetc(oint32_t ch);

static oint32_t
skip(void);

static oint32_t
skip_comment(void);

static oint32_t
getc_quoted(void);

static oint32_t
char_value(oint32_t ch, oint32_t base);

static void
macro_date(oobject_t *pointer);

static void
macro_time(oobject_t *pointer);

static oobject_t
macro_string(ovector_t *vector);

static void
macro_insert_vector(ovector_t *vector);

static void
macro_expand_object(ovector_t *expand, oobject_t object);

static void
macro_expand_vector(ovector_t *expand, ovector_t *vector);

static void
macro_function(omacro_t *macro, ovector_t *expand, ovector_t *alist);

static void
macro_evaluate(ovector_t *final, ovector_t *expand);

static otoken_t
macro_vector_token(ovector_t *vector);

static oobject_t
macro_expand(omacro_t *macro);

static oobject_t
macro_read(void);

static oobject_t
macro_object(obool_t define);

static osymbol_t *
macro_ident(void);

static oint32_t
macro_skip(void);

static omacro_t *
macro_define(osymbol_t *symbol);

static void
macro_undef(omacro_t *macro);

static void
macro_check(void);

static void
macro_ignore(void);

static otoken_t
macro_unary(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_do_lookahead(oword_t *offset, ovector_t *expand);

static otoken_t
macro_muldivrem(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_addsub(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_shift(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_bitop(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_logcmp(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static otoken_t
macro_logop(oword_t *offset, oword_t *lvalue, ovector_t *expand);

static void
macro_cond_error(void);

static obool_t
macro_cond(void);

static oword_t
macro_value(omacro_t *macro);

static void
macro_defined(ovector_t *expand);

static void
macro_get_object(oobject_t *pointer, oobject_t object);

/*
 * Initialization
 */
oinput_t		*input;
onote_t			 input_note;
oobject_t		 object_eof;

static struct {
    char		*name;
    macro_t		 macro;
    osymbol_t		*symbol;
} macros[] = {
    { "define",		 mac_define	},
    { "undef",		 mac_undef	},
    { "include",	 mac_include	},
    { "ifdef",		 mac_ifdef	},
    { "ifndef",		 mac_ifndef	},
    { "if",		 mac_if		},
    { "elif",		 mac_elif	},
    { "else",		 mac_else	},
    { "endif",		 mac_endif	},
    { "error",		 mac_error	},
    { "warning",	 mac_warning	},
};
static oint32_t		 spaces_set[256 / 4];
static oint32_t		 string_set[256 / 4];
static oint32_t		 symbol_set[256 / 4];
static ovector_t	*input_vector;
static ovector_t	*macro_vector;
static ovector_t	*save_vector;
static ovector_t	*expand_vector;
static ovector_t	*alist_vector;
static ohash_t		*macro_table;
static ovector_t	*macro_state;
static ovector_t	*macro_name;
static oint32_t		 macro_line;
static oint32_t		 macro_counter;
static obool_t		 macro_conditional;
static oint32_t		 macro_cond_lineno;
static oint32_t		 macro_cond_column;
static osymbol_t	*symbol_defined;
static osymbol_t	*symbol_concat;
static osymbol_t	*symbol_oparen;
static osymbol_t	*symbol_cparen;
static osymbol_t	*symbol_counter;
static oword_t		 macro_offset;
static oint32_t		 macro_expand_level;
static otoken_t		 macro_next_token;

/*
 * Implementation
 */
void
init_read(void)
{
    oword_t		 offset;
    char		*string;
    osymbol_t		*symbol;

    oadd_root((oobject_t *)&input_vector);
    onew_vector((oobject_t *)&input_vector, t_stream, 16);

    oadd_root((oobject_t *)&macro_vector);
    onew_vector((oobject_t *)&macro_vector, t_void, 16);

    oadd_root((oobject_t *)&save_vector);
    onew_vector((oobject_t *)&save_vector, t_void, 4);

    oadd_root((oobject_t *)&expand_vector);
    onew_vector((oobject_t *)&expand_vector, t_void, 4);

    oadd_root((oobject_t *)&alist_vector);
    onew_vector((oobject_t *)&alist_vector, t_void, 4);

    oadd_root((oobject_t *)&macro_table);
    onew_hash((oobject_t *)&macro_table, 256);

    /* conditional state bits */
    oadd_root((oobject_t *)&macro_state);
    onew_vector((oobject_t *)&macro_state, t_int8, 256);

    for (offset = 0; offset < osize(macros); offset++) {
	symbol = onew_identifier(oget_string((ouint8_t *)macros[offset].name,
					     strlen(macros[offset].name)));
	macros[offset].symbol = symbol;
    }

    object_eof = onew_identifier(oget_string((ouint8_t *)"*eof*", 5));

    symbol_concat = onew_identifier(oget_string((ouint8_t *)"##", 2));
    symbol_oparen = onew_identifier(oget_string((ouint8_t *)"(", 1));
    symbol_cparen = onew_identifier(oget_string((ouint8_t *)")", 1));
    symbol_counter = onew_identifier(oget_string((ouint8_t *)"__COUNTER__", 11));

    symbol_defined = onew_identifier(oget_string((ouint8_t *)"defined", 7));

    memset(string_set, 0xff, sizeof(string_set));
    for (string = " \t\r\n\f\v"; *string; string++) {
	spaces_set[*string >> 5] |= 1 << (*string & 31);
	string_set[*string >> 5] &= ~(1 << (*string & 31));
    }

    for (string = "abcdefghijklmnopqrstuvwxyz_"
	 "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	 *string; string++)
	symbol_set[*string >> 5] |= 1 << (*string & 31);
}

void
finish_read(void)
{
    orem_root((oobject_t *)&input_vector);
    orem_root((oobject_t *)&macro_vector);
    orem_root((oobject_t *)&save_vector);
    orem_root((oobject_t *)&expand_vector);
    orem_root((oobject_t *)&alist_vector);
    orem_root((oobject_t *)&macro_table);
    orem_root((oobject_t *)&macro_state);
}

oobject_t
oread_object(void)
{
    oobject_t		object;

    if (input) {
	for (;;) {
	    if ((object = read_object()) != object_eof) {
		if (omacro_p(object))
		    return (((omacro_t *)object)->name);
		if (osymbol_p(object))
		    return (expand_symbol(object));
		return (object);
	    }
	    if (input->stream != std_input)
		oclose(input->stream);
	    assert(input_vector->offset);
	    if (--input_vector->offset) {
		input_vector->v.ptr[input_vector->offset] = null;
		input = input_vector->v.ptr[input_vector->offset - 1];
		input_note.name = input->stream->name;
		input_note.lineno = input->lineno;
		input_note.column = input->column;
	    }
	    else {
		input = null;
		break;
	    }
	}
    }
    if (macro_state->offset > 0)
	oread_error("unfinished conditional starting at %p:%d",
		    macro_name, macro_line);

    return (object_eof);
}

void
opush_input(ostream_t *stream)
{
    if (input_vector->offset >= input_vector->length)
	orenew_vector(input_vector, input_vector->length + 16);

    if (input) {
	input->lineno = input_note.lineno;
	input->column = input_note.column;
    }

    onew(input_vector->v.ptr + input_vector->offset, input);
    input = input_vector->v.ptr[input_vector->offset++];
    input->stream = stream;
    input->lineno = 1;

    input_note.name = stream->name;
    input_note.lineno = 1;
    input_note.column = 0;
}

static oword_t
scan_chr(void)
{
    GET_THREAD_SELF()
    oint32_t		ch;
    oword_t		bytes;

    bytes = 0;
    thread_self->r0.v.w = 0;
    thread_self->r0.t = t_word;

    for (ch = getc_quoted();
	 ch != eof && ch != '\'';
	 ch = getc_quoted()) {
	if (ch == '\n')
	    return (eof);
	if (bytes < sizeof(oword_t))
	    thread_self->r0.v.w = (thread_self->r0.v.w << 8) | (ch & 0xff);
	else {
	    if (bytes == sizeof(oword_t)) {
		mpz_set_ui(thr_zr, thread_self->r0.v.w);
		thread_self->r0.t = t_mpz;
	    }
	    mpz_mul_2exp(thr_zr, thr_zr, 8);
	    mpz_add_ui(thr_zr, thr_zr, ch & 0xff);
	}
	++bytes;
    }
    if (bytes == sizeof(oword_t) && mpz_fits_slong_p(thr_zr)) {
	thread_self->r0.v.w = mpz_get_si(thr_zr);
	thread_self->r0.t = t_word;
    }

    return (ch != eof ? bytes : eof);
}

static oword_t
scan_str(void)
{
    GET_THREAD_SELF()
    oint32_t		 ch;
    oword_t		 size;
    oword_t		 length;
    ouint8_t		*string;

    length = 0;
    string = thread_self->vec->v.u8;
    size = thread_self->vec->length;

    for (ch = getc_quoted();
	 ch != eof && ch != '"';
	 ch = getc_quoted()) {
	if (ch == '\n')
	    return (eof);
	if (length >= size) {
	    size += BUFSIZ;
	    size += BUFSIZ - (size % BUFSIZ);
	    if (size < length) {
		thread_self->xcpt = except_out_of_bounds;
		return (eof);
	    }
	    orenew_vector(thread_self->vec, size);
	    string = thread_self->vec->v.u8;
	}
	string[length++] = ch;
    }
    if (ch != eof) {
	thread_self->r0.t = t_string;

	return (length);
    }

    return (eof);
}

static oword_t
scan_num(oint32_t ch, oint32_t radix, obool_t uppr)
{
    GET_THREAD_SELF()
    otype_t		type;
    oword_t		offset;

    /* save offset as may need to rollback read state */
    offset = input->stream->offset;

    /* check for real number specification */
    thread_self->vec->offset = 0;
    if ((type = scan_num_real(radix, ch, uppr)) == t_void) {
	/* failed to read a number */
	thread_self->r0.t = t_void;
	input->stream->offset = offset;

	return (eof);
    }

    thread_self->r0.t = type;

    return (input->stream->offset - offset);
}

static otype_t
scan_num_real(oint32_t ch, oint32_t radix, obool_t uppr)
{
    GET_THREAD_SELF()
    ouint8_t		*ptr;
    oword_t		 offset;

    /* don't check bounds in up to 3 bytes */
    if (thread_self->vec->length - thread_self->vec->offset <= 3)
	orenew_vector(thread_self->vec, thread_self->vec->length + BUFSIZ);
    ptr = thread_self->vec->v.u8;
    offset = thread_self->vec->offset;

    if (ch == '-' || ch == '+') {
	ptr[thread_self->vec->offset++] = ch;
	/* if input ends in '-' or '+' */
	if ((ch = getc()) == eof) {
	    ungetc(ptr[thread_self->vec->offset - 1]);
	    return (t_void);
	}
    }

    /* if need to figure radix */
    if (radix == 0) {
	radix = 10;
	if (ch == '0') {
	    ch = getc();
	    switch (ch) {
		case 'b': case 'B':
		    radix = 2;
		    break;
		case 'x': case 'X':
		    radix = 16;
		    break;
		case '0'...'7':
		    radix = 8;
		    ptr[thread_self->vec->offset++] = ch;
		    break;
		default:
		    ptr[thread_self->vec->offset++] = '0';
		    ungetc(ch);
		    break;
	    }
	}
	else {
	    if (ch < '1' || ch > '9') {
		if (ch == '.')
		    ungetc(ch);
		else
		    return (t_void);
	    }
	    else
		ptr[thread_self->vec->offset++] = ch;
	}
    }
    else {
	if (ch == '.')
	    ungetc(ch);
	else
	    ptr[thread_self->vec->offset++] = ch;
    }

    return (scan_num_radix(offset, radix, uppr));
}

static otype_t
scan_num_radix(oword_t offset, oint32_t radix, obool_t uppr)
{
    GET_THREAD_SELF()
    oint32_t		 ch;
    oint32_t		 ech;
    obool_t		 done;
    otype_t		 type;
    oint32_t		 state;
    oword_t		 length;
    ouint8_t		*string;

#define has_dot			(1 << 0)
#define has_exponent		(1 << 1)
#define has_sign		(1 << 2)
#define has_modifier		(1 << 3)
    ech = 0;
    state = 0;
    done = false;
    type = t_void;
    string = thread_self->vec->v.u8;

    for (ch = getc(); ch != eof; ch = getc()) {
	switch (ch) {
	    case '.':
		done = state & (has_dot | has_exponent);
		state |= has_dot;
		break;
	    case 'e': case 'E':
		if (radix == 16)
		    break;
		ech = ch;
		ch = 'e';
		done = state & has_exponent;
		state |= has_exponent;
		break;
	    case '@':
		ech = ch;
		done = state & has_exponent;
		state |= has_exponent;
		break;
	    case '+': case '-':
		done = (state & has_sign) || !(state & has_exponent);
		if (!done) {
		    state |= has_sign;
		    if (string[thread_self->vec->offset - 1] != '@')
			done = radix > 10 ||
			       string[thread_self->vec->offset - 1] != 'e';
		}
		break;
	    case '0'...'1':				break;
	    case '2'...'7':	done = radix < 8;	break;
	    case '8'...'9':	done = radix < 10;	break;
	    case 'A'...'D':	case 'F':
	    case 'a'...'d':	case 'f':
				done = radix < 16;	break;
	    default:		done = true;		break;
	}

	if (done) {
	    switch (ch) {
		case 'l': case 'L':
		    state |= has_modifier;
		    type = t_mpr;
		    break;
		default:
		    ungetc(ch);
	    }
	    if ((state & has_exponent) &&
		string[thread_self->vec->offset - 1] == ech) {
		ungetc(ech);
		state &= ~has_exponent;
	    }
	    else if ((state & has_dot) &&
		     string[thread_self->vec->offset - 1] == '.') {
		/* allow trailing dot for floats */
		if ((ch = getc()) == '.') {
		    /* two sequential dots should be ellipsis in case range */
		    ungetc('.');
		    ungetc('.');
		    state &= ~has_dot;
		}
		else
		    ungetc(ch);
	    }
	    break;
	}

	if (thread_self->vec->offset + 1 >= thread_self->vec->length) {
	    length = thread_self->vec->length + BUFSIZ;
	    length += BUFSIZ - (length % BUFSIZ);
	    if (length < thread_self->vec->length) {
		//thread_self->except = except_out_of_bounds;
		return (t_void);
	    }
	    orenew_vector(thread_self->vec, length);
	    string = thread_self->vec->v.u8;
	}
	string[thread_self->vec->offset++] = ch;
    }

    string[thread_self->vec->offset] = '\0';
    string += offset;
    if (string[0] == '+')
	++string;

    if (type == t_void && (state & (has_dot | has_exponent)))
	type = uppr ? t_mpr : t_float;

    switch (type) {
	case t_mpr:
	    mpfr_set_str(thr_rr, (char *)string, radix, thr_rnd);
 	    break;
	case t_float:
	    mpfr_set_str(thr_rr, (char *)string, radix, thr_rnd);
	    thread_self->r0.v.d = mpfr_get_d(thr_rr, thr_rnd);
	    break;
	default:
	    if (thread_self->vec->offset - offset) {
		errno = 0;
		thread_self->r0.v.w = strtol((char *)string, null, radix);
		if (errno == ERANGE) {
		    mpz_set_str(thr_zr, (char *)string, radix);
		    type = t_mpz;
		}
		else
		    type = t_word;
	    }
	    break;
    }

    /* could also check if is scanning a rational */
    if (state & has_modifier)
	thread_self->vec->v.u8[thread_self->vec->offset++] = ch;

#undef has_dot
#undef has_exponent
#undef has_sign
#undef has_modifier
    return (type);
}

static ovector_t *
read_ident(oint32_t ch)
{
    oword_t		 size;
    oword_t		 length;
    ouint8_t		*string;
    ovector_t		*vector;
    oobject_t		*pointer;
    ouint8_t		 buffer[BUFSIZ];
    gc_enter();

    string = buffer;
    size = sizeof(buffer);
    length = 1;
    string[0] = ch;

    for (ch = getc(); ch != eof && symbol_char_p(ch); ch = getc()) {
	if (length >= size) {
	    size += BUFSIZ;
	    if (string == buffer) {
		gc_ref(pointer);
		onew_vector(pointer, t_uint8, size);
		vector = *pointer;
		string = vector->v.u8;
		memcpy(string, buffer, length);
	    }
	    else {
		orenew_vector(vector, size);
		string = vector->v.u8;
	    }
	}
	string[length++] = ch;
    }
    if (ch != eof)
	ungetc(ch);

    vector = oget_string(string, length);
    gc_leave();

    return (vector);
}

static oobject_t
read_object(void)
{
    oint32_t		 ch;
    omacro_t		*macro;
    oobject_t		 object;

    if (macro_offset < macro_vector->offset)
	object = macro_vector->v.ptr[macro_offset++];
    else {
	if ((ch = skip()) != eof) {
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	}
	switch (ch) {
	    case eof:
		object = object_eof;
		break;
	    case '#':
		object = macro_read();
		break;
	    case '"':
		object = read_string();
		break;
	    case '\'':
		object = read_character();
		break;
	    case '0'...'9':
		object = read_number(ch);
		break;
	    case 'a'...'z': case 'A'...'Z': case '_':
		object = read_symbol(ch);
		break;
	    case '.':
		/* allow leading dot for floats */
		ch = getc();
		if (ch >= '0' && ch <= '9') {
		    ungetc(ch);
		    object = read_number('.');
		    break;
		}
		ungetc(ch);
		ch = '.';
	    default:
		object = read_keyword(ch);
		break;
	}
    }
    if (osymbol_p(object) &&
	!((osymbol_t *)object)->keyword &&
	(macro = (omacro_t *)oget_hash(macro_table, object)))
	object = macro_expand(macro);

    return (object);
}

static oint32_t
getc_noeof(void)
{
    oint32_t	ch;

    if ((ch = getc()) == eof)
	oread_error("unexpected end of file");

    return (ch);
}

static oint32_t
getc(void)
{
    oint32_t	ch;

    if ((ch = ogetc(input->stream)) == '\n') {
	++input_note.lineno;
	input_note.column = 0;
    }
    else
	++input_note.column;

    return (ch);
}

static void
ungetc(oint32_t ch)
{
    if (ch == '\n') {
	--input_note.lineno;
	input_note.column = 0;
    }
    else
	--input_note.column;
    oungetc(input->stream, ch);
}

static oint32_t
skip(void)
{
    oint32_t		ch;

    for (ch = getc(); ; ch = getc()) {
	if (ch == '/')
	    ch = skip_comment();
	switch (ch) {
	    case '\n':
	    case ' ':	case '\t':
	    case '\r':	case '\v':	case '\f':
		break;
	    default:
		return (ch);
	}
    }
}

static oint32_t
skip_comment(void)
{
    oint32_t		ch;
    obool_t		line;
    oint32_t		nest;

    /* this function is called after reading a '/' */
    ch = getc();
again:
    switch (ch) {
	case '/':
	    line = true;
	    for (ch = getc(); ch != '\n' && ch != eof; ch = getc())
		;
	    break;
	case '*':
	    line = false;
	    for (nest = 1; nest > 0;) {
		while ((ch = getc_noeof()) != '*') {
		    if (ch == '/') {
			if (getc_noeof() == '*')
			    ++nest;
		    }
		}
		while ((ch = getc_noeof()) == '*')
		    ;
		if (ch == '/')
		    --nest;
	    }
	    ch = getc();
	    break;
	default:
	    /* not a comment */
	    ungetc(ch);
	    return ('/');
    }

    /* return ending '\n' */
    if (line && ch == '\n') {
	/* fast check for multiple comments */
	if ((nest = getc()) == '/') {
	    ch = getc();
	    if (ch == '/' || ch == '*')
		goto again;
	    ungetc(ch);
	    ungetc(nest);
	    return ('\n');
	}
	else
	    ungetc(nest);
    }

    return (ch);
}

static oint32_t
getc_quoted(void)
{
    oint32_t		i;
    oint32_t		ch;
    oint32_t		digit;
    oint32_t		value;
    oint32_t		quote;

    quote = 0;
    ch = getc();
    if (ch == '\\') {
	quote = 1;
	ch = getc();
	switch (ch) {
	    case 'a': ch = '\a'; break;
	    case 'b': ch = '\b'; break;
	    case 'f': ch = '\f'; break;
	    case 'n': ch = '\n'; break;
	    case 'r': ch = '\r'; break;
	    case 't': ch = '\t'; break;
	    case 'v': ch = '\v'; break;
	    case '"': case '\'': break;
	    case '0': case '1': case '2':
		value = getc();
		digit = char_value(value, 8);
		if (digit != eof) {
		    ch = digit;
		    for (i = 1; i < 2; i++) {
			value = getc();
			digit = char_value(value, 8);
			if (digit != eof) {
			    ch = (ch << 3) | digit;
			    if (ch > 255)
				break;
			}
			else {
			    ungetc(value);
			    break;
			}
		    }
		}
		else
		    ungetc(value);
		break;
	    case 'x':
		value = getc();
		digit = char_value(value, 16);
		if (digit != eof) {
		    ch = digit;
		    for (i = 0; i < 2; i++) {
			value = getc();
			digit = char_value(value, 16);
			if (digit != eof)
			    ch = (ch << 4) | digit;
			else {
			    ungetc(value);
			    break;
			}
		    }
		}
		else
		    ungetc(value);
		break;
	    case '\n':
		quote = 0;
		ch = getc_quoted();
		break;
	}
    }

    return (quote ? ch | 0x100 : ch);
}

static oobject_t
read_character(void)
{
    GET_THREAD_SELF()
    oint32_t		bytes;
    oobject_t		object;

    bytes = scan_chr();
    if (bytes == 0)
	oread_error("character constant too short");
    else if (bytes == eof)
	oread_error("expecting quote");

    if (thread_self->r0.t == t_word)
	onew_word(&object, thread_self->r0.v.w);
    else
	onew_mpz(&object, thr_zr);

    return (object);
}

static ovector_t *
read_string(void)
{
    GET_THREAD_SELF()
    oword_t		length;

    if ((length = scan_str()) < 0)
	oread_error("expecting double quote");

    return (oget_string(thread_self->vec->v.u8, length));
}

static oobject_t
read_number(oint32_t ch)
{
    GET_THREAD_SELF()
    oobject_t		 object;

    oread_lock(input->stream);
    scan_num(0, ch, false);
    oread_unlock(input->stream);

    ch = getc();
    /* check delimiter */
    if (symbol_char_p(ch))
	oread_error("number followed by '%c'", ch);
    ungetc(ch);

    switch (thread_self->r0.t) {
	case t_word:
	    onew_word(&object, thread_self->r0.v.w);
	    break;
	case t_float64:
	    onew_float(&object, thread_self->r0.v.d);
	    break;
	case t_mpz:
	    onew_mpz(&object, thr_zr);
	    break;
	case t_mpq:
	    onew_mpq(&object, thr_qr);
	    break;
	case t_mpr:
	    onew_mpr(&object, thr_rr);
	    break;
	case t_cdd:
	    onew_cdd(&object, thread_self->r0.v.dd);
	    break;
	case t_cqq:
	    onew_cqq(&object, thr_qq);
	    break;
	case t_mpc:
	    onew_mpc(&object, thr_cc);
	    break;
	default:
	    oread_error("expecting number");
    }

    return (object);
}

static oobject_t
read_symbol(oint32_t ch)
{
    osymbol_t		*symbol;
    oobject_t		*pointer;

    symbol = onew_identifier(read_ident(ch));
    if (unlikely(symbol == symbol_counter)) {
	gc_ref(pointer);
	onew_word(pointer, macro_counter);
	gc_dec();
	++macro_counter;
	return (*pointer);
    }
    else if (!macro_expand_level)
	return (expand_symbol(symbol));

    return (symbol);
}

static oobject_t
expand_symbol(osymbol_t *symbol)
{
    oast_t		*ast;
    oobject_t		*pointer;

    assert(macro_expand_level == 0);
    if (symbol->keyword) {
	gc_ref(pointer);
	switch (*(oword_t *)symbol->value) {
	    case tok_FILE:
		*pointer = input_note.name;
		break;
	    case tok_LINE:
		onew_word(pointer, input->lineno);
		break;
	    case tok_DATE:
		macro_date(pointer);
		break;
	    case tok_TIME:
		macro_time(pointer);
		break;
	    default:
		onew_ast(pointer, *(oword_t *)symbol->value,
			 input_note.name, input->lineno, input->column);
		break;
	}
	gc_dec();

	return (*pointer);
    }
    else if (symbol->type) {
	gc_ref(pointer);
	onew_ast(pointer, tok_type,
		 input_note.name, input->lineno, input->column);
	ast = *pointer;
	ast->l.value = symbol->tag;
	gc_dec();

	return (ast);
    }

    return (symbol);
}

static oobject_t
read_keyword(oint32_t ch)
{
    otoken_t		 token;
    oobject_t		 object;
    oobject_t		*pointer;

    switch (ch) {
	case '#':
	    if ((ch = getc()) == '#')	token = tok_concat;
	    else {
		ungetc(ch);		token = tok_stringify;
	    }
	    break;
	case '(':			token = tok_oparen;		break;
	case ')':			token = tok_cparen;		break;
	case '[':			token = tok_obrack;		break;
	case ']':			token = tok_cbrack;		break;
	case '{':			token = tok_obrace;		break;
	case '}':			token = tok_cbrace;		break;
	case ';':			token = tok_semicollon;		break;
	case ':':			token = tok_collon;		break;
	case ',':			token = tok_comma;		break;
	case '.':
	    if ((ch = getc()) != '.') {
		ungetc(ch);		token = tok_dot;
	    }
	    else if ((ch = getc()) != '.')
		oread_error("syntax error at '..'");
	    else			token = tok_ellipsis;
	    break;
	case '=':
	    if ((ch = getc()) == '=')	token = tok_eq;
	    else {
		ungetc(ch);		token = tok_set;
	    }
	    break;
	case '&':
	    if ((ch = getc()) == '&')		token = tok_andand;
	    else if (ch == '=')			token = tok_andset;
	    else {
		ungetc(ch);			token = tok_and;
	    }
	    break;
	case '|':
	    if ((ch = getc()) == '|')		token = tok_oror;
	    else if (ch == '=')			token = tok_orset;
	    else {
		ungetc(ch);			token = tok_or;
	    }
	    break;
	case '^':
	    if ((ch = getc()) == '=')		token = tok_xorset;
	    else {
		ungetc(ch);			token = tok_xor;
	    }
	    break;
	case '<':
	    if ((ch = getc()) == '=')		token = tok_le;
	    else if (ch == '<') {
		if ((ch = getc()) == '=')	token = tok_mul2set;
		else if (ch == '<') {
		    if ((ch = getc()) == '=')	token = tok_shlset;
		    else {
			ungetc(ch);		token = tok_shl;
		    }
		}
		else {
		    ungetc(ch);			token = tok_mul2;
		}
	    }
	    else {
		ungetc(ch);			token = tok_lt;
	    }
	    break;
	case '>':
	    if ((ch = getc()) == '=')		token = tok_ge;
	    else if (ch == '>') {
		if ((ch = getc()) == '=')	token = tok_div2set;
		else if (ch == '>') {
		    if ((ch = getc()) == '=')	token = tok_shrset;
		    else {
			ungetc(ch);		token = tok_shr;
		    }
		}
		else {
		    ungetc(ch);			token = tok_div2;
		}
	    }
	    else {
		ungetc(ch);			token = tok_gt;
	    }
	    break;
	case '+':
	    if ((ch = getc()) == '+')		token = tok_inc;
	    else if (ch == '=')			token = tok_addset;
	    else {
		ungetc(ch);			token = tok_add;
	    }
	    break;
	case '-':
	    if ((ch = getc()) == '-')		token = tok_dec;
	    else if (ch == '=')			token = tok_subset;
	    else {
		ungetc(ch);			token = tok_sub;
	    }
	    break;
	case '*':
	    if ((ch = getc()) == '=')		token = tok_mulset;
	    else {
		ungetc(ch);			token = tok_mul;
	    }
	    break;
	case '/':
	    if ((ch = getc()) == '=')		token = tok_divset;
	    else {
		ungetc(ch);			token = tok_div;
	    }
	    break;
	case '\\':
	    if ((ch = getc()) == '=')		token = tok_trunc2set;
	    else {
		ungetc(ch);			token = tok_trunc2;
	    }
	    break;
	case '%':
	    if ((ch = getc()) == '=')		token = tok_remset;
	    else {
		ungetc(ch);			token = tok_rem;
	    }
	    break;
	case '!':
	    if ((ch = getc()) == '=')		token = tok_ne;
	    else {
		ungetc(ch);			token = tok_not;
	    }
	    break;
	case '~':				token = tok_com;	break;
	case '?':				token = tok_question;	break;
	default:
	    oread_error("syntax error at '%c'", ch);
    }

    if (macro_expand_level)
	object = symbol_token_vector[token];
    else {
	gc_ref(pointer);
	onew_ast(pointer, token, input_note.name, input->lineno, input->column);
	gc_pop(object);
    }

    return (object);
}

static oint32_t
char_value(oint32_t ch, oint32_t base)
{
    oint32_t		value;

    if (ch >= '0' && ch <= '9')
	value = ch - '0';
    else if (ch >= 'a' && ch <= 'f')
	value = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
	value = ch - 'A' + 10;
    else
	value = eof;

    return (value >= 0 && value < base ? value : eof);
}

/* adapted from <gcc-base>/libcpp/macro.c:_cpp_builtin_macro_text(),
 * but instead of having a static global value, recalculate it everytime
 * the macro is expanded, so it can be used to measure compile time
 * (in seconds resolution) */
static void
macro_date(oobject_t *pointer)
{
    oformat_t		 format;
    ostream_t		*stream;
    struct tm		*macro_tm;
    time_t		 macro_time;
    static const char *const monthnames[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    errno = 0;
    macro_tm = null;
    macro_time = time(null);
    if (macro_time != (time_t)-1 || errno == 0)
	macro_tm = localtime(&macro_time);

    stream = (ostream_t *)thread_main->vec;
    if (macro_tm) {
	memset(&format, 0, sizeof(oformat_t));
	format.width = 2;
	format.radix = 10;
	memcpy(stream->ptr, monthnames[macro_tm->tm_mon], 3);
	stream->ptr[3] = ' ';
	stream->offset = stream->length = 4;
	oprint_wrd(stream, &format, macro_tm->tm_mday);
	stream->ptr[6] = ' ';
	stream->offset = stream->length = 7;
	oprint_wrd(stream, &format, macro_tm->tm_year + 1900);
	*pointer = oget_string(stream->ptr, stream->length);
    }
    else {
	oread_warn("could not determine date");
	*pointer = oget_string((ouint8_t *)"??? ?? ????", 11);
    }
}

static void
macro_time(oobject_t *pointer)
{
    oformat_t		 format;
    ostream_t		*stream;
    struct tm		*macro_tm;
    time_t		 macro_time;

    errno = 0;
    macro_tm = null;
    macro_time = time(null);
    if (macro_time != (time_t)-1 || errno == 0)
	macro_tm = localtime(&macro_time);

    stream = (ostream_t *)thread_main->vec;
    if (macro_tm) {
	memset(&format, 0, sizeof(oformat_t));
	format.zero = 1;
	format.width = 2;
	format.radix = 10;
	stream->offset = stream->length = 0;
	oprint_wrd(stream, &format, macro_tm->tm_hour);
	stream->ptr[2] = ':';
	stream->offset = stream->length = 3;
	oprint_wrd(stream, &format, macro_tm->tm_min);
	stream->ptr[5] = ':';
	stream->offset = stream->length = 6;
	oprint_wrd(stream, &format, macro_tm->tm_sec);
	*pointer = oget_string(stream->ptr, stream->length);
    }
    else {
	oread_warn("could not determine time");
	*pointer = oget_string((ouint8_t *)"??:??:??", 8);
    }
}

static oobject_t
macro_string(ovector_t *vector)
{
    GET_THREAD_SELF()
    obool_t		 first;
    oformat_t		 format;
    oobject_t		 object;
    oword_t		 offset;
    ostream_t		*stream;
    char		*string;

    first = true;
    memset(&format, 0, sizeof(oformat_t));
    format.read = 1;
    format.prec = 6;
    format.radix = 10;
    thread_self->vec->offset = thread_self->vec->length = 0;
    stream = (ostream_t *)thread_self->vec;

    for (offset = 0; offset < vector->offset; offset++) {
	object = vector->v.ptr[offset];
	if (!first)
	    oputc(stream, ' ');
	else
	    first = false;
	if (object == null)
	    owrite(stream, "null", 4);
	else {
	    switch (otype(object)) {
		case t_word:
		    oprint_wrd(stream, &format, *(oword_t *)object);
		    break;
		case t_float:
		    oprint_flt(stream, &format, *(ofloat_t *)object);
		    break;
		case t_mpz:
		    oprint_mpz(stream, &format, object);
		    break;
		case t_rat:
		    oprint_rat(stream, &format, object);
		    break;
		case t_mpq:
		    oprint_mpq(stream, &format, object);
		    break;
		case t_mpr:
		    oprint_mpr(stream, &format, object);
		    break;
		case t_cdd:
		    oprint_cdd(stream, &format, object);
		    break;
		case t_cqq:
		    oprint_cqq(stream, &format, object);
		    break;
		case t_mpc:
		    oprint_mpc(stream, &format, object);
		    break;
		case t_string:
		    oprint_str(stream, &format, object);
		    break;
		case t_symbol:
		    format.read = 0;
		    oprint_str(stream, &format, ((osymbol_t *)object)->name);
		    format.read = 1;
		    break;
		case t_ast:
		    string = otoken_to_charp(((oast_t *)object)->token);
		    owrite(stream, string, strlen(string));
		    break;
		default:
		    abort();
	    }
	}
    }

    return (oget_string(thread_self->vec->v.obj, thread_self->vec->offset));
}

static void
macro_insert_vector(ovector_t *vector)
{
    oword_t		 offset;

    offset = macro_vector->offset - macro_offset + vector->offset;
    if (macro_vector->length < offset)
	orenew_vector(macro_vector, (offset + 16) & -16);
    memmove(macro_vector->v.ptr + vector->offset,
	    macro_vector->v.ptr + macro_offset,
	    (offset - vector->offset) * sizeof(oobject_t));
    macro_vector->offset = offset;
    macro_offset = 0;
    for (offset = 0; offset < vector->offset; offset++)
	macro_get_object(macro_vector->v.ptr + offset, vector->v.ptr[offset]);
}

static void
macro_expand_object(ovector_t *expand, oobject_t object)
{
    if (expand->offset + 1 >= expand->length)
	orenew_vector(expand, (expand->offset + 1 + 4) & -4);
    expand->v.ptr[expand->offset++] = object;
}

static void
macro_expand_vector(ovector_t *expand, ovector_t *vector)
{
    oword_t		offset;

    if (vector) {
	assert(otype(vector) == t_vector);
	if (expand->offset + vector->offset >= expand->length)
	    orenew_vector(expand, (expand->offset + vector->offset + 4) & -4);
	for (offset = 0; offset < vector->offset; offset++)
	    expand->v.ptr[expand->offset++] = vector->v.ptr[offset];
    }
}

static void
macro_function(omacro_t *macro, ovector_t *expand, ovector_t *alist)
{
    obool_t		 eval;
    ovector_t		*elist;
    oentry_t		*entry;
    oword_t		 offset;
    oword_t		 length;
    osymbol_t		*symbol;
    osymbol_t		*concat;
    ovector_t		*vector;

    if (alist_vector->v.ptr[alist_vector->offset] == null)
	onew_vector(alist_vector->v.ptr + alist_vector->offset,
		    t_void,
		    ((macro->table->count + (!!macro->vararg)) + 4) & -4);
    elist = alist_vector->v.ptr[alist_vector->offset++];
    if (elist->length < alist->offset)
	orenew_vector(elist, (alist->offset + 4) & -4);
    elist->offset = alist->offset;

    for (offset = 0; offset < alist->offset; offset++) {
	if ((vector = alist->v.ptr[offset])) {
	    length = (vector->offset + 4) & -4;
	    if ((vector = elist->v.ptr[offset]) == null) {
		onew_vector(elist->v.ptr + offset, t_void, length);
		vector = elist->v.ptr[offset];
	    }
	    else if (vector->length < length)
		orenew_vector(vector, length);
	    macro_evaluate(vector, alist->v.ptr[offset]);
	}
    }

    for (offset = 0; offset < macro->vector->offset; offset++) {
	symbol = macro->vector->v.ptr[offset];
	if (osymbol_p(symbol)) {
	    if (symbol->keyword) {
		switch (*(oword_t *)symbol->value) {
		    case tok_stringify:
			entry = null;
			if (offset + 1 < macro->vector->offset) {
			    symbol = macro->vector->v.ptr[++offset];
			    if (osymbol_p(symbol))
				entry = oget_hash(macro->table, symbol);
			}
			if (entry) {
			    vector = alist->v.ptr[*(oword_t *)entry->value];
			    macro_expand_object(expand, macro_string(vector));
			}
			else
			    oread_warn("# not followed by macro parameter");
			continue;
		    case tok_VA_ARGS:
			if (!macro->vararg) {
			    oread_warn("__VA_ARGS__ on non vararg macro");
			    continue;
			}
			macro_expand_vector(expand,
					    alist->v.ptr[macro->table->count]);
			break;
			/* Check for special ",##arg" expansion with
			 * with empty arg, and do not print a warning
			 * on that condition */
		    case tok_comma:
			if (offset + 2 > macro->vector->offset)
			    goto comma_fail;
			concat = macro->vector->v.ptr[offset + 1];
			if (concat != symbol_concat)
			    goto comma_fail;
			concat = macro->vector->v.ptr[offset + 2];
			if (!osymbol_p(concat))
			    goto comma_fail;
			vector = null;
			if (concat->keyword &&
			    *(oword_t *)concat->value == tok_VA_ARGS)
			    vector = alist->v.ptr[macro->table->count];
			else if ((entry = oget_hash(macro->table, concat))) {
			    assert(*(oword_t *)entry->value >= 0 &&
				   *(oword_t *)entry->value < alist->offset);
			    vector = elist->v.ptr[*(oword_t *)entry->value];
			}
			else
			    goto comma_fail;
			if (vector == null || vector->offset == 0)
			    offset += 2;
			else {
			    /* Remove the ## as it for sure will not
			     * not generate valid token, and was
			     * actually already processed when
			     * checking for empty argument */
			    macro_expand_object(expand, symbol);
			    ++offset;
			}
			break;
		    default:
		    comma_fail:
			macro_expand_object(expand, symbol);
			break;
		}
	    }
	    else {
		if ((entry = oget_hash(macro->table, symbol))) {
		    eval = true;
		    /* Do not evaluate if prev argument is ## */
		    if (offset > 0) {
			concat = macro->vector->v.ptr[offset - 1];
			if (concat == symbol_concat)
			    eval = false;
		    }
		    /* Do not evaluate if next argument is ## */
		    if (eval && offset + 1 < macro->vector->offset) {
			concat = macro->vector->v.ptr[offset + 1];
			if (concat == symbol_concat)
			    eval = false;
		    }
		    vector = eval ? elist : alist;
		    assert(*(oword_t *)entry->value >= 0 &&
			   *(oword_t *)entry->value < vector->offset);
		    macro_expand_vector(expand,
					vector->v.ptr[*(oword_t *)entry->value]);
		}
		else
		    macro_expand_object(expand, symbol);
	    }
	}
	else
	    macro_expand_object(expand, symbol);
    }

    for (offset = 0; offset < alist->offset; offset++) {
	if ((vector = alist->v.ptr[offset])) {
	    memset(vector->v.ptr, 0, vector->offset * sizeof(oobject_t));
	    vector->offset = 0;
	    vector = elist->v.ptr[offset];
	    assert(vector);
	    memset(vector->v.ptr, 0, vector->offset * sizeof(oobject_t));
	    vector->offset = 0;
	}
	else
	    assert( elist->v.ptr[offset] == null);
    }
    alist->offset = elist->offset = 0;
    alist_vector->offset -= 2;
}

static void
macro_evaluate(ovector_t *final, ovector_t *expand)
{
    oword_t		 offset;
    osymbol_t		*symbol;
    ovector_t		*vector;

    /* Check if some macro will (or may) expand */
    for (offset = 0; offset < expand->offset; offset++) {
	symbol = expand->v.ptr[offset];
	if (osymbol_p(symbol) &&
	    !symbol->keyword && oget_hash(macro_table, symbol))
	    break;
    }
    if (offset == expand->offset) {
	if (final->offset < expand->offset)
	    orenew_vector(final, (expand->offset + 4) & -4);
	memcpy(final->v.ptr, expand->v.ptr,
	       expand->offset * sizeof(oobject_t));
	final->offset = expand->offset;
	return;
    }

    if (save_vector->offset >= save_vector->length)
	orenew_vector(save_vector, save_vector->offset + 4);
    save_vector->v.ptr[save_vector->offset++] = macro_vector;

    offset = macro_offset;
    macro_vector = expand;
    macro_offset = 0;

    /* Ensure the expand argument is not modified; it can be modified
     * in some macro expansions  */
    if (expand_vector->offset >= expand_vector->length)
	orenew_vector(expand_vector, expand_vector->offset + 4);
    if ((vector = expand_vector->v.ptr[expand_vector->offset]) == null) {
	onew_vector(expand_vector->v.ptr + expand_vector->offset,
		    t_void, (expand->offset + 4) & -4);
	vector = expand_vector->v.ptr[expand_vector->offset];
    }
    else if (expand->offset >= vector->length)
	orenew_vector(vector, (expand->offset + 4) & -4);
    ++expand_vector->offset;
    memcpy(vector->v.ptr, expand->v.ptr, expand->offset * sizeof(oobject_t));
    vector->offset = expand->offset;

    assert(final->offset == 0);
    while (macro_offset < macro_vector->offset) {
	symbol = read_object();
	if (final->offset >= final->length)
	    orenew_vector(final, final->offset + 4);
	final->v.ptr[final->offset++] = symbol;
    }

    /* Restore the expand argument */
    if (expand->offset > vector->offset)
	memset(expand->v.ptr + vector->offset, 0,
	       (expand->offset - vector->offset) * sizeof(oobject_t));
    memcpy(expand->v.ptr, vector->v.ptr, vector->offset * sizeof(oobject_t));
    expand->offset = vector->offset;
    memset(vector->v.ptr, 0, vector->offset * sizeof(oobject_t));
    vector->offset = 0;

    /* Restore toplevel expansion */
    macro_vector = save_vector->v.ptr[--save_vector->offset];
    macro_offset = offset;
}

static otoken_t
macro_vector_token(ovector_t *vector)
{
    otoken_t		token;

    token = tok_none;
    switch (vector->v.u8[0]) {
	case '.':
	    if (vector->length == 3 &&
		vector->v.u8[1] == '.' && vector->v.u8[2] == '.')
		token = tok_ellipsis;
	    break;
	case '=':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_eq;
	    break;
	case '&':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '&')
		    token = tok_andand;
		else if (vector->v.u8[1] == '=')
		    token = tok_andset;
	    }
	    break;
	case '|':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '|')
		    token = tok_oror;
		else if (vector->v.u8[1] == '=')
		    token = tok_orset;
	    }
	    break;
	case '^':
	    if (vector->length == 2 &&
		vector->v.u8[1] == '=')
		token = tok_xorset;
	    break;
	case '<':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '<')
		    token = tok_mul2;
		else if (vector->v.u8[1] == '=')
		    token = tok_le;
	    }
	    else if (vector->length == 3) {
		if (vector->v.u8[1] == '<') {
		    if (vector->v.u8[2] == '<')
			token = tok_shl;
		    else if (vector->v.u8[2] == '=')
			token = tok_mul2set;
		}
	    }
	    else if (vector->length == 4 && vector->v.u8[1] == '<' &&
		     vector->v.u8[2] == '<' && vector->v.u8[3] == '=')
		token = tok_shlset;
	    break;
	case '>':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '>')
		    token = tok_div2;
		else if (vector->v.u8[1] == '=')
		    token = tok_ge;
	    }
	    else if (vector->length == 3) {
		if (vector->v.u8[1] == '>') {
		    if (vector->v.u8[2] == '>')
			token = tok_shr;
		    else if (vector->v.u8[2] == '=')
			token = tok_div2set;
		}
	    }
	    else if (vector->length == 4 && vector->v.u8[1] == '>' &&
		     vector->v.u8[2] == '>' && vector->v.u8[3] == '=')
		token = tok_shrset;
	    break;
	case '+':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '+')
		    token = tok_inc;
		else if (vector->v.u8[1] == '=')
		    token = tok_addset;
	    }
	    break;
	case '-':
	    if (vector->length == 2) {
		if (vector->v.u8[1] == '-')
		    token = tok_dec;
		else if (vector->v.u8[1] == '=')
		    token = tok_subset;
	    }
	    break;
	case '*':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_mulset;
	    break;
	case '/':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_divset;
	    break;
	case '\\':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_trunc2set;
	    break;
	case '%':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_remset;
	    break;
	case '!':
	    if (vector->length == 2 && vector->v.u8[1] == '=')
		token = tok_ne;
	    break;
	default:
	    break;
    }

    return (token);
}

static oobject_t
macro_expand(omacro_t *macro)
{
    GET_THREAD_SELF()
    oword_t		 word;
    ovector_t		*alist;
    ovector_t		*final;
    oword_t		 level;
    otoken_t		 token;
    oobject_t		 object;
    ovector_t		*vector;
    ovector_t		*expand;
    oformat_t		 format;
    osymbol_t		*concat;
    oword_t		 offset;
    osymbol_t		*symbol;
    oobject_t		*pointer;

    if (++macro_expand_level > 65536)
	/* not really overflow but do not allow it */
	oread_error("macro expansion depth overflow");

    if (expand_vector->offset >= expand_vector->length)
	orenew_vector(expand_vector, expand_vector->length + 4);
    if (expand_vector->v.ptr[expand_vector->offset] == null)
	onew_vector(expand_vector->v.ptr + expand_vector->offset, t_void, 4);
    expand = expand_vector->v.ptr[expand_vector->offset++];
    alist = null;

    /* If a function macro */
    if (macro->table) {
	assert(!macro->unsafe);
	symbol = macro_object(false);
	if (symbol == symbol_oparen) {
	    /* Allocate argument name -> value translation place holders */
	    if (alist_vector->offset >= alist_vector->length)
		orenew_vector(alist_vector, alist_vector->length + 4);
	    if (alist_vector->v.ptr[alist_vector->offset] == null)
		onew_vector(alist_vector->v.ptr +
			    alist_vector->offset,
			    t_void, macro->table->count);
	    alist = alist_vector->v.ptr[alist_vector->offset++];
	    offset = macro->table->count + !!macro->vararg;
	    if (alist->length < offset)
		orenew_vector(alist, (offset + 4) & -4);

	    level = 1;
	    vector = null;
	    while ((symbol = macro_object(false))) {
		if (symbol == object_eof)
		    oread_error("unexpected end of file");
		if (osymbol_p(symbol) && symbol->keyword) {
		    switch (*(oword_t *)symbol->value) {
			case tok_oparen:
			    ++level;
			    break;
			case tok_cparen:
			    if (--level == 0) {
				if (++alist->offset < macro->table->count)
				    oread_error("too few arguments to '%p'",
						macro->name);
				goto macro_expand;
			    }
			    break;
			case tok_comma:
			    if (level == 1) {
				vector = null;
				if (++alist->offset >= macro->table->count) {
				    if (macro->vararg)
					goto macro_vararg;
				    oread_error("too many arguments to '%p'",
						macro->name);
				}
				continue;
			    }
			    break;
			default:
			    break;
		    }
		}
		else if (alist->offset >= macro->table->count && !macro->vararg)
		    oread_error("too many arguments to '%p'", macro->name);

		if (vector == null &&
		    (vector = alist->v.ptr[alist->offset]) == null) {
		    onew_vector(alist->v.ptr + alist->offset, t_void, 4);
		    vector = alist->v.ptr[alist->offset];
		}
		else if (vector->offset >= vector->length)
		    orenew_vector(vector, vector->length + 4);
		vector->v.ptr[vector->offset++] = symbol;
		/* Check for macro with only a __VA_ARGS__ argument */
		if (alist->offset >= macro->table->count) {
		    assert(macro->vararg &&
			   alist->offset == macro->table->count);
		    goto macro_vararg_next;
		}
	    }

	macro_vararg:
	    assert(alist->offset == macro->table->count);
	    if ((vector = alist->v.ptr[alist->offset]) == null) {
		onew_vector(alist->v.ptr + alist->offset, t_void, 4);
		vector = alist->v.ptr[alist->offset];
	    }

	macro_vararg_next:
	    while ((symbol = macro_object(false))) {
		if (symbol == object_eof)
		    oread_error("unexpected end of file");
		if (osymbol_p(symbol) && symbol->keyword) {
		    switch (*(oword_t *)symbol->value) {
			case tok_oparen:
			    ++level;
			    break;
			case tok_cparen:
			    if (--level == 0) {
				++alist->offset;
				goto macro_expand;
			    }
			    break;
			default:
			    break;
		    }
		}
		if (vector->offset >= vector->length)
		    orenew_vector(vector, vector->length + 4);
		vector->v.ptr[vector->offset++] = symbol;
	    }

	macro_expand:
	    macro_function(macro, expand, alist);
	}
	else
	    /* This is actually valid preprocessor, but confusing;
	     * since incorrect number of arguments already generate
	     * an error (besides valid preprocessor, that would not
	     * be expanded), make this also generate an error for the
	     * sake of avoiding the complexity of handling it, and
	     * allowing difficult to read, macro code to be written;
	     * the usage would be to pass a macro name as an argument
	     * to macro invoking it... */
	    oread_error("'%p' is as macro function", macro->name);
    }
    else {
	if (macro->expand) {
	    --macro_expand_level;
	    return (macro);
	}
	macro->expand = macro->unsafe;

	if (!macro->vector->offset) {
	    if (!macro_conditional)
		macro_expand_object(expand, read_object());
	}
	else
	    macro_expand_vector(expand, macro->vector);
    }

    /* Final expansion checking for concatenation */
    if (expand_vector->offset >= expand_vector->length)
	orenew_vector(expand_vector, expand_vector->length + 4);
    if (expand_vector->v.ptr[expand_vector->offset] == null)
	onew_vector(expand_vector->v.ptr + expand_vector->offset, t_void, 4);
    final = expand_vector->v.ptr[expand_vector->offset++];

    gc_ref(pointer);
    for (offset = 0; offset < expand->offset; offset++) {
	symbol = expand->v.ptr[offset];
	if (symbol == null)
	    continue;
	concat = null;
	if (osymbol_p(symbol) && symbol->keyword) {
	    switch (*(oword_t *)symbol->value) {
		case tok_VA_ARGS:
		    if (!macro->vararg)
			oread_warn("__VA_ARGS__ on non vararg macro");
		    /* Remove empty __VA_ARGS__ in macro expansion */
		    continue;
		case tok_stringify:
		    /* Remove leftover # in macro expansion */
		    oread_warn("# not followed by macro parameter");
		    continue;
		case tok_concat:
		    /* Remove misplaced ## in macro expansion */
		    oread_warn("## doesn't form a valid token");
		    continue;
		default:
		    concat = symbol;
		    break;
	    }
	}
	else if (osymbol_p(symbol) || omacro_p(symbol))
	    concat = symbol;

	if (concat && offset + 2 < expand->offset) {
	    concat = expand->v.ptr[offset + 1];
	    if (concat == symbol_concat) {
		concat = expand->v.ptr[offset + 2];
		if (concat == null)
		    goto concat_fail;
		vector = thread_self->vec;
		vector->offset = vector->length = 0;

		if (omacro_p(symbol))
		    symbol = ((omacro_t *)symbol)->name;
		else
		    owrite((ostream_t *)vector, symbol->name->v.ptr,
			   symbol->name->length);

		if (osymbol_p(concat) && concat->keyword) {
		    switch (*(oword_t *)concat->value) {
			case tok_VA_ARGS:
			case tok_stringify:
			case tok_concat:
			    /* Do not update offset so that warning is
			     * triggered above for misplaced tokens */
			    goto concat_fail;
			default:
			    owrite((ostream_t *)vector, concat->name->v.ptr,
				   concat->name->length);
			    break;
		    }
		}
		else if (osymbol_p(concat) || omacro_p(concat)) {
		    if (omacro_p(concat))
			concat = ((omacro_t *)concat)->name;
		    owrite((ostream_t *)vector, concat->name->v.ptr,
			   concat->name->length);
		}
		else if (otype(concat) == t_word) {
		    memset(&format, 0, sizeof(oformat_t));
		    format.radix = 10;
		    oprint_wrd((ostream_t *)vector, &format,
			       *(oword_t *)concat);
		}

		if (vector->offset >= vector->length)
		    orenew_vector(vector, vector->offset + 1);
		vector->v.u8[vector->offset] = '\0';
		if (symbol_char_p(vector->v.u8[0])) {
		    if (vector->v.u8[0] >= '0' && vector->v.u8[0] <= '9') {
			/* Concatenation generated an integer */
			errno = 0;
			word = strtol((char *)vector->v.ptr, null, 10);
			/* Do not concatenate due to overflow */
			if (errno == ERANGE)
			    continue;
			onew_word(pointer, word);
			symbol = *pointer;
		    }
		    else {
			/* Concatenation generated a symbol */
			for (word = 1; word < vector->offset; word++)
			    if (!symbol_char_p(vector->v.u8[word]))
				break;
			if (word != vector->offset)
			    /* Do not concatenate due to invalid name */
			    continue;
			vector = oget_string(vector->v.u8, vector->offset);
			symbol = onew_identifier(vector);
		    }
		}
		else {
		    /* Concatenation generated a "token" */
		    token = macro_vector_token(vector);
		    /* Did not generate a valid token  */
		    if (token == tok_none)
			continue;
		    symbol = symbol_token_vector[token];
		}

		/* Concatenation done */
		offset += 2;
	    }
	}

    concat_fail:
	macro_expand_object(final, symbol);
    }
    gc_dec();

    memset(expand->v.ptr, 0, expand->offset * sizeof(oobject_t));
    expand->offset = 0;
    if (macro->unsafe) {
	macro_evaluate(expand, final);
	macro_insert_vector(expand);
	memset(expand->v.ptr, 0, expand->offset * sizeof(oobject_t));
	expand->offset = 0;
    }
    else
	macro_insert_vector(final);

    expand_vector->offset -= 2;
    memset(final->v.ptr, 0, final->offset * sizeof(oobject_t));
    final->offset = 0;

    object = read_object();

    if (macro->unsafe)
	macro->expand = false;

    --macro_expand_level;

    return (object);
}

static oobject_t
macro_read(void)
{
    GET_THREAD_SELF()
    oobject_t		 p;
    oobject_t		 q;
    oint32_t		 ch;
    obool_t		 same;
    omacro_t		*prev;
    omacro_t		*macro;
    osymbol_t		*symbol;
    ostream_t		*stream;
    oword_t		 offset;

    symbol = macro_ident();
    assert(osymbol_p(symbol));
    for (offset = 0; offset < osize(macros); offset++) {
	if (symbol == macros[offset].symbol)
	    break;
    }
    if (offset == osize(macros))
	oread_error("bad preprocessor '%p'", symbol);

    switch (macros[offset].macro) {
	case mac_define:
	    symbol = macro_ident();
	    if (symbol->keyword || symbol == symbol_defined)
		oread_error("'%p' cannot name a macro", symbol);
	    if ((prev = (omacro_t *)oget_hash(macro_table, symbol)))
		gc_push(prev);
	    macro = macro_define(symbol);
	    if (prev) {
		same = ((prev->table && macro->table) ||
			(!prev->table && !macro->table));
		if (same && prev->table)
		    same = prev->table->count == macro->table->count;
		if (same)
		    same = prev->vector->offset == macro->vector->offset;
		if (same)
		    same = prev->vararg == macro->vararg;
		if (same) {
		    for (offset = 0; offset < macro->vector->offset; offset++) {
			p = prev->vector->v.ptr[offset];
			q = macro->vector->v.ptr[offset];
			if (p != q) {
			    if (otype(p) != otype(q)) {
				same = false;
				break;
			    }
			    if (otype(p) == t_word) {
				if (*(oword_t *)p != *(oword_t *)q) {
				    same = false;
				    break;
				}
			    }
			    if (otype(p) == t_float) {
				if (*(ofloat_t *)p != *(ofloat_t *)q) {
				    same = false;
				    break;
				}
			    }
			}
		    }
		}
		if (same && macro->table) {
		    for (offset = prev->table->size;
			 same && offset < prev->table->size; offset++) {
			p = prev->table->entries[offset];
			q = prev->table->entries[offset];
			if (!p ^ !q) {
			    same = false;
			    break;
			}
			while (p && q) {
			    if (((oentry_t *)p)->name != ((oentry_t *)q)->name) {
				same = false;
				break;
			    }
			}
			if (p || q) {
			    same = false;
			    break;
			}
		    }
		}
		if (!same)
		    oread_warn("macro '%p' redefined", symbol);
		gc_dec();
	    }
	    break;
	case mac_undef:
	    symbol = macro_ident();
	    macro_undef((omacro_t *)oget_hash(macro_table, symbol));
	    break;

	case mac_include:
	    /* TODO */
	    break;

	case mac_ifdef:
	    symbol = macro_ident();
	    macro_check();
	    macro_state_inc();
	    if (macro_state->offset == 1) {
		macro_name = input_note.name;
		macro_line = input->lineno;
	    }
	    if (oget_hash(macro_table, symbol))
		macro_set_true();
	    else
		macro_ignore();
	    break;
	case mac_ifndef:
	    symbol = macro_ident();
	    macro_check();
	    macro_state_inc();
	    if (macro_state->offset == 1) {
		macro_name = input_note.name;
		macro_line = input->lineno;
	    }
	    if (oget_hash(macro_table, symbol))
		macro_ignore();
	    else
		macro_set_true();
	    break;
	    break;
	case mac_if:
	    macro_state_inc();
	    macro_name = input_note.name;
	    macro_line = input->lineno;
	    if (macro_cond() == false)
		macro_ignore();
	    else
		macro_set_true();
	    break;
	case mac_elif:
	    if (!macro_state->offset || macro_get_else())
		oread_error("unexpected #elif");
	    else if (macro_state->offset == 1) {
		macro_name = input_note.name;
		macro_line = input->lineno;
	    }
	    macro_skip_line();
	    macro_ignore();
	    break;
	case mac_else:
	    if (!macro_state->offset || macro_get_else())
		oread_error("unexpected #else");
	    else if (macro_state->offset == 1) {
		macro_name = input_note.name;
		macro_line = input->lineno;
	    }
	    macro_set_else();
	    macro_check();
	    macro_ignore();
	    break;
	case mac_endif:
	    if (!macro_state->offset)
		oread_error("unexpected #endif");
	    macro_check();
	    macro_clr_bits();
	    if (--macro_state->offset > 0 && !macro_get_true())
		macro_ignore();
	    break;
	case mac_error:
	case mac_warning:
	    stream = (ostream_t *)thread_self->vec;
	    stream->offset = stream->length = 0;
	    ungetc(macro_skip());
	    for (ch = getc(); ch != eof && ch != '\n'; ch = getc())
		oputc(stream, ch);
	    /* backup line number */
	    ungetc(ch);
	    if (macros[offset].macro == mac_warning)
		oread_warn("%p", stream);
	    else
		oread_error("%p", stream);
	    break;
	default:
	    abort();
    }

    return (read_object());
}

static oobject_t
macro_object(obool_t define)
{
    oint32_t		ch;
    oobject_t		object;

    if (macro_offset < macro_vector->offset)
	object = macro_vector->v.ptr[macro_offset++];
    else {
	ch = define ? macro_skip() : skip();
	switch (ch) {
	    case '\n':
		/* Print proper line number on warnings and errors */
		ungetc('\n');
	    case eof:
		object = object_eof;
		break;
	    case '"':
		object = read_string();
		break;
	    case '\'':
		object = read_character();
		break;
	    case '0'...'9':
		object = read_number(ch);
		break;
	    case 'a'...'z': case 'A'...'Z': case '_':
		object = read_symbol(ch);
		break;
	    case '.':
		/* allow leading dot for floats */
		ch = getc();
		if (ch >= '0' && ch <= '9') {
		    ungetc(ch);
		    object = read_number('.');
		    break;
		}
		ungetc(ch);
		ch = '.';
	    default:
		object = read_keyword(ch);
		break;
	}
    }

    return (object);
}

static osymbol_t *
macro_ident(void)
{
    oint32_t		 ch;
    ovector_t		*vector;

    switch (ch = macro_skip()) {
	case eof:	oread_error("unexpected end of file");
	case 'a'...'z': case 'A'...'Z': case '_':
	    vector = read_ident(ch);
	    break;
	case '\n':	oread_error("expecting identifier");
	default:	oread_error("'%c' is not a symbol char", ch);
    }

    return (onew_identifier(vector));
}

static oint32_t
macro_skip(void)
{
    oint32_t		ch;

    for (ch = getc(); ; ch = getc()) {
	if (ch == '/')
	    ch = skip_comment();
	switch (ch) {
	    case '\n':
		return (ch);
	    case ' ':	case '\t':
	    case '\r':	case '\v':	case '\f':
		break;
	    case '\\':
		ch = getc();
		if (ch == eof)
		    oread_error("unexpected end of input");
		else if (ch != '\n') {
		    ungetc(ch);
		    return ('\\');
		}
		break;
	    default:
		return (ch);
	}
    }
}

static omacro_t *
macro_define(osymbol_t *symbol)
{
    osymbol_t		*name;
    oentry_t		*entry;
    omacro_t		*macro;
    oint32_t		 state;
    oobject_t		 object;
    oword_t		 offset;
    obool_t		 unsafe;
    oobject_t		*pointer;

    ++macro_expand_level;

    gc_ref(pointer);
    onew(pointer, macro);
    macro = *pointer;
    macro->name = symbol;
    oput_hash(macro_table, (oentry_t *)macro);
    gc_dec();

    state = getc();
    if (state == '(') {
	unsafe = false;
#define wait_sep	(1 << 0)
#define wait_sym	(1 << 1)
#define wait_end	(1 << 2)
	offset = 0;
	state = wait_sym | wait_end;
	onew_hash((oobject_t *)&macro->table, 4);
	for (name = macro_object(true); ; name = macro_object(true)) {
	    if (!osymbol_p(name))
		goto macro_fail;
	    if (name->keyword) {
		switch (*(oword_t *)name->value) {
		    case tok_cparen:
			if (!(state & wait_end))
			    goto macro_fail;
			goto macro_define;
		    case tok_comma:
			if (!(state & wait_sep))
			    goto macro_fail;
			state = wait_sym;
			break;
		    case tok_ellipsis:
			if (macro->vararg || !(state & wait_sym))
			    goto macro_fail;
			macro->vararg = true;
			state = wait_end;
			break;
		    default:
			goto macro_fail;
		}
	    }
	    else {
		if (!(state & wait_sym))
		    goto macro_fail;
		if (oget_hash(macro->table, name))
		    oread_error("macro argument '%p' redefined", name);
		gc_ref(pointer);
		onew_entry(pointer, name, null);
		entry = *pointer;
		oput_hash(macro->table, entry);
		gc_dec();
		onew_word(&entry->value, offset);
		++offset;
		state = wait_sep|wait_end;
	    }
	}
#undef wait_end
#undef wait_sym
#undef wait_sep
    }
    else {
	unsafe = true;
	ungetc(state);
    }

macro_define:
    onew_vector((oobject_t *)&macro->vector, t_void, 4);
    while ((object = macro_object(true)) != object_eof) {
	if (macro->vector->offset >= macro->vector->length)
	    orenew_vector(macro->vector, macro->vector->length * 2);
	macro->vector->v.ptr[macro->vector->offset++] = object;
	if (unsafe && osymbol_p(object)) {
	    macro->unsafe = true;
	    unsafe = false;
	}
    }

    --macro_expand_level;

    return (macro);
macro_fail:
    oread_error("macro argument syntax error");
}

static void
macro_undef(omacro_t *macro)
{
    oword_t		offset;

    if (macro && macro->vector) {
	for (offset = 0; offset < macro->vector->offset; offset++)
	    odel_object(macro->vector->v.ptr + offset);
	orem_hash(macro_table, (oentry_t *)macro);
    }
}

static void
macro_check(void)
{
    oint32_t		ch;

    if ((ch = macro_skip()))
	ungetc(ch);
    else if (ch != eof)
	oread_error("'%c' after preprocessor", ch);
}

static void
macro_ignore(void)
{
    oint32_t		 ch;
    oint32_t		 line;
    ovector_t		*name;
    osymbol_t		*ident;
    oint32_t		 quote;
    oword_t		 offset;

    for (ch = skip(); ch != eof; ch = skip()) {
	if (ch == '#') {
	    /* save value now, as macro_ident() may read a newline */
	    name = input_note.name;
	    line = input->lineno;
	    ident = macro_ident();
	    for (offset = 0; offset < osize(macros); offset++)
		if (ident == macros[offset].symbol)
		    break;
	    if (offset == osize(macros))
		oread_error("bad preprocessor '%p'", ident);
	    switch (macros[offset].macro) {
		case mac_ifdef:
		case mac_ifndef:
		case mac_if:
		    macro_state_inc();
		    macro_set_skip();
		    macro_skip_line();
		    macro_ignore();
		    break;
		case mac_elif:
		    if (!macro_state->offset || macro_get_else())
			oread_error("unexpected #elif");
		    else if (macro_state->offset == 1) {
			macro_name = name;
			macro_line = line;
		    }
		    if (!macro_get_true() && !macro_get_skip()) {
			if (macro_cond()) {
			    macro_set_true();
			    return;
			}
			/* dont skip line as newline was already read */
			continue;
		    }
		    break;
		case mac_else:
		    macro_check();
		    if (!macro_state->offset || macro_get_else())
			oread_error("unexpected #else");
		    else if (macro_state->offset == 1) {
			macro_name = name;
			macro_line = line;
		    }
		    macro_set_else();
		    if (!macro_get_true() && !macro_get_skip()) {
			macro_set_true();
			return;
		    }
		    break;
		case mac_endif:
		    macro_clr_bits();
		    macro_check();
		    --macro_state->offset;
		    return;
		default:
		    break;
	    }
	}
	else if (ch == '"' || ch == '\'') {
	    quote = ch;
	    while ((ch = getc_quoted()) != quote) {
		if (ch == eof || ch == '\n')
		    oread_error("expecting '\\%c'", quote);
	    }
	}
	/* skip to next line or eof */
	macro_skip_line();
    }
}

static otoken_t
macro_unary(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		 token;
    union {
	oobject_t	 object;
	osymbol_t	*symbol;
    } value;

    if (*offset >= expand->offset)
	return (tok_eof);

    value.object = expand->v.ptr[(*offset)++];
    if (osymbol_p(value.object)) {
	switch (*(oword_t *)value.symbol->value) {
	    case tok_add:
		token = macro_unary(offset, lvalue, expand);
		if (token != tok_number)
		    oread_error("macro value not an integer");
		break;
	    case tok_sub:
		token = macro_unary(offset, lvalue, expand);
		if (token != tok_number || *lvalue == MININT)
		    oread_error("macro value overflow");
		*lvalue = -*lvalue;
		break;
	    case tok_not:
		token = macro_unary(offset, lvalue, expand);
		if (token != tok_number)
		    oread_error("macro value not an integer");
		*lvalue = !*lvalue;
		break;
	    case tok_oparen:
		token = macro_logop(offset, lvalue, expand);
		if (*offset >= expand->length)
		    macro_cond_error();
		value.object = expand->v.ptr[(*offset)++];
		if (value.symbol != symbol_cparen)
		    macro_cond_error();
		macro_next_token = tok_none;
		break;
	    default:
		macro_cond_error();
		break;
	}
    }
    else {
	assert(otype(value.object) == t_word);
	*lvalue = *(oword_t *)value.object;
	token = tok_number;
    }

    return (token);
}

static otoken_t
macro_do_lookahead(oword_t *offset, ovector_t *expand)
{
    union {
	oobject_t	 object;
	osymbol_t	*symbol;
    } value;

    if (*offset >= expand->offset)
	return (tok_eof);
    value.object = expand->v.ptr[*offset];
    if (osymbol_p(value.object))
	return (*(oword_t *)value.symbol->value);
    assert(otype(value.object) == t_word);

    return (tok_number);
}

static otoken_t
macro_muldivrem(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		value;
    oword_t		rvalue;

    if ((token = macro_unary(offset, lvalue, expand)) != tok_number)
	return (token);

    for (;;) {
	macro_lookahead(token, offset, expand);
	if (token != tok_mul    && token != tok_div &&
	    token != tok_trunc2 && token != tok_rem)
	    return (tok_number);
	macro_consume(offset);
	if (macro_unary(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	if (token == tok_mul) {
	    value = *lvalue * rvalue;
	    if (rvalue && (value / rvalue) != *lvalue)
		oread_error("macro mul value overflow");
	    *lvalue = value;
	}
	else if (token == tok_div) {
	    if (*lvalue % rvalue)
		oread_error("macro inexact division");
	    *lvalue /= rvalue;
	}
	else if (token == tok_trunc2)
	    *lvalue /= rvalue;
	else
	    *lvalue %= rvalue;
    }
}

static otoken_t
macro_addsub(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		value;
    oword_t		rvalue;

    if ((token = macro_muldivrem(offset, lvalue, expand)) == tok_eof)
	return (tok_eof);
    if (token != tok_number)
	macro_cond_error();

    for (;;) {
	macro_lookahead(token, offset, expand);
	if (token != tok_add && token != tok_sub)
	    return (tok_number);
	macro_consume(offset);
	if (macro_muldivrem(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	if (token == tok_add) {
	    value = *lvalue + rvalue;
	    if ((value - rvalue) != *lvalue)
		oread_error("macro add value integer");
	    *lvalue = value;
	}
	else {
	    value = *lvalue - rvalue;
	    if ((value + rvalue) != *lvalue)
		oread_error("macro sub value overflow");
	    *lvalue = value;
	}
    }
}

static otoken_t
macro_shift(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		value;
    oword_t		rvalue;

    if ((token = macro_addsub(offset, lvalue, expand)) == tok_eof)
	return (tok_eof);
    if (token != tok_number)
	macro_cond_error();

    for (;;) {
	macro_lookahead(token, offset, expand);
	if (token != tok_mul2 && token != tok_div2 &&
	    token != tok_shl  && token != tok_shr)
	    return (tok_number);
	macro_consume(offset);
	if (macro_addsub(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	if (token == tok_mul2 || token == tok_shl) {
	    value = *lvalue << rvalue;
	    if ((value >> rvalue) != *lvalue)
		oread_error("macro shift value overflow");
	    *lvalue = value;
	}
	else if (token == tok_div2) {
	    value = *lvalue >> rvalue;
	    if ((value << rvalue) != *lvalue)
		oread_error("macro shift value underflow");
	    *lvalue = value;
	}
	else
	    *lvalue = *lvalue >> rvalue;
    }
}

static otoken_t
macro_bitop(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		rvalue;

    if ((token = macro_shift(offset, lvalue, expand)) == tok_eof)
	return (tok_eof);
    if (token != tok_number)
	macro_cond_error();

    for (;;) {
	macro_lookahead(token, offset, expand);
	if (token != tok_and && token != tok_or && token != tok_xor)
	    return (tok_number);
	macro_consume(offset);
	if (macro_shift(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	if (token == tok_and)
	    *lvalue = *lvalue & rvalue;
	else if (token == tok_or)
	    *lvalue = *lvalue | rvalue;
	else
	    *lvalue = *lvalue ^ rvalue;
    }
}

static otoken_t
macro_logcmp(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		rvalue;

    if ((token = macro_bitop(offset, lvalue, expand)) == tok_eof)
	return (tok_eof);
    if (token != tok_number)
	macro_cond_error();
    for (;;) {
	macro_lookahead(token, offset, expand);
	switch (token) {
	    case tok_lt:		case tok_le:
	    case tok_eq:		case tok_ge:
	    case tok_gt:		case tok_ne:
		break;
	    default:
		return (tok_number);
	}
	macro_consume(offset);
	if (macro_bitop(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	switch (token) {
	    case tok_lt:
		*lvalue = *lvalue <  rvalue;
		break;
	    case tok_le:
		*lvalue = *lvalue <= rvalue;
		break;
	    case tok_eq:
		*lvalue = *lvalue == rvalue;
		break;
	    case tok_ge:
		*lvalue = *lvalue >= rvalue;
		break;
	    case tok_gt:
		*lvalue = *lvalue >  rvalue;
		break;
	    default:
		*lvalue = *lvalue != rvalue;
		break;
	}
    }
}

static otoken_t
macro_logop(oword_t *offset, oword_t *lvalue, ovector_t *expand)
{
    otoken_t		token;
    oword_t		rvalue;

    if ((token = macro_logcmp(offset, lvalue, expand)) == tok_eof)
	return (tok_eof);
    if (token != tok_number)
	macro_cond_error();

    for (;;) {
	macro_lookahead(token, offset, expand);
	if (token != tok_andand && token != tok_oror)
	    return (tok_number);
	macro_consume(offset);
	if (macro_logcmp(offset, &rvalue, expand) != tok_number)
	    macro_cond_error();
	if (token == tok_andand)
	    *lvalue = *lvalue && rvalue;
	else
	    *lvalue = *lvalue || rvalue;
    }
}

static void
macro_cond_error(void)
{
    input->lineno = macro_cond_lineno;
    input->column = macro_cond_column;
    oread_error("macro condition syntax error");
}

static obool_t
macro_cond(void)
{
    oword_t		 result;
    oint32_t		 lineno;
    oint32_t		 column;
    oobject_t		 object;
    oword_t		 offset;
    ovector_t		*expand;

    /* Not recursive */
    assert(macro_expand_level == 0);
    ++macro_expand_level;

    macro_conditional = true;
    lineno = macro_cond_lineno;
    column = macro_cond_column;
    macro_cond_lineno = input->lineno;
    macro_cond_column = input->column;

    if (expand_vector->offset >= expand_vector->length)
	orenew_vector(expand_vector, expand_vector->length + 4);
    if (expand_vector->v.ptr[expand_vector->offset] == null)
	onew_vector(expand_vector->v.ptr + expand_vector->offset, t_void, 4);
    expand = expand_vector->v.ptr[expand_vector->offset++];

    object = macro_object(true);
    for (; object != object_eof; object = macro_object(true))
	macro_expand_object(expand, object);
    if (!expand->offset)
	oread_error("unexpected end of file");

    macro_defined(expand);
    if (expand->offset == 0)
	macro_cond_error();

    offset = 0;
    macro_next_token = tok_none;
    (void)macro_logop(&offset, &result, expand);

    if (offset < expand->offset)
	macro_cond_error();

    expand->offset = 0;
    --expand_vector->offset;
    memset(expand->v.ptr, 0, expand->length * sizeof(oobject_t));

    macro_cond_column = column;
    macro_cond_lineno = lineno;
    macro_conditional = false;

    --macro_expand_level;

    return (!!result);
}

static oword_t
macro_value(omacro_t *macro)
{
    omacro_t		*expand;
    oword_t		 result;
    oobject_t		 object;

    if (macro->expand || macro->vector->offset == 0)
	return (0);
    object = macro->vector->v.ptr[macro->vector->offset - 1];
    if (osymbol_p(object)) {
	if ((expand = (omacro_t *)oget_hash(macro_table, object))) {
	    macro->expand = true;
	    result = macro_value(expand);
	    macro->expand = false;
	    return (result);
	}
	return (0);
    }
    /* Only integer simple expressions supported */
    if (otype(object) != t_word)
	oread_error("macro condition is not an integer");

    return (*(oword_t *)object);
}

static void
macro_defined(ovector_t *expand)
{
    omacro_t		*macro;
    oword_t		 offset;
    osymbol_t		*symbol;

    for (offset = 0; offset < expand->offset; offset++) {
	symbol = expand->v.ptr[offset];
	if (!osymbol_p(symbol)) {
	    /* Only integer simple expressions supported */
	    if (otype(symbol) != t_word)
		oread_error("macro condition is not an integer");
	}
	else if ((macro = (omacro_t *)oget_hash(macro_table, symbol)))
	    onew_word(expand->v.ptr + offset, macro_value(macro));
	else if (symbol == symbol_defined) {
	    /* defined */
	    if (offset + 3 >= expand->offset)
		macro_cond_error();

	    /* defined ( */
	    if (expand->v.ptr[offset + 1] != symbol_oparen)
		macro_cond_error();

	    /* defined ( symbol */
	    symbol = expand->v.ptr[offset + 2];
	    if (!osymbol_p(symbol))
		macro_cond_error();

	    if ((macro = (omacro_t *)oget_hash(macro_table, symbol)))
		onew_word(expand->v.ptr + offset, 1);
	    else
		onew_word(expand->v.ptr + offset, 0);

	    /* defined ( symbol ) */
	    if (expand->v.ptr[offset + 3] != symbol_cparen)
		macro_cond_error();

	    /* defined(symbol) -> number */
	    memmove(expand->v.ptr + offset + 1,
		    expand->v.ptr + offset + 4,
		    (expand->offset - offset - 3) * sizeof(oobject_t));
	    /* ensure there are no duplicated oast_t references */
	    expand->offset -= 3;
	    memset(expand->v.ptr + expand->offset, 0, 3 * sizeof(oobject_t));
   	}
	else if (!symbol->keyword)
	    onew_word(expand->v.ptr + offset, 0);
    }
}

static void
macro_get_object(oobject_t *pointer, oobject_t object)
{
    if (object == null)
	*pointer = null;
    else {
	switch (otype(object)) {
	    case t_word:	case t_float:	case t_rat:
	    case t_mpz:		case t_mpq:	case t_mpr:
	    case t_cdd:		case t_cqq:	case t_mpc:
		ocopy(pointer, object);
		break;
	    default:
		*pointer = object;
		break;
	}
    }
}
