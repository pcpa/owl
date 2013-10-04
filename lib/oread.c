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

/*
 * Initialization
 */
oinput_t		*input;
onote_t			 input_note;
oobject_t		 object_eof;

static oint32_t		 spaces_set[256 / 4];
static oint32_t		 string_set[256 / 4];
static oint32_t		 symbol_set[256 / 4];
static ovector_t	*input_vector;

/*
 * Implementation
 */
void
init_read(void)
{
    char		*string;

    oadd_root((oobject_t *)&input_vector);
    onew_vector((oobject_t *)&input_vector, t_stream, 16);

    object_eof = onew_identifier(oget_string((ouint8_t *)"*eof*", 5));

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
}

oobject_t
oread_object(void)
{
    oobject_t	object;

    if (input) {
	for (;;) {
	    if ((object = read_object()) != object_eof)
		return (object);
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
		//thread_self->except = except_out_of_bounds;
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
    oint32_t		ch;
    oobject_t		object;

    switch ((ch = skip())) {
	case eof:
	    object = object_eof;
	    break;
	case '"':
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    object = read_string();
	    break;
	case '\'':
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    object = read_character();
	    break;
	case '0'...'9':
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    object = read_number(ch);
	    break;
	case 'a'...'z': case 'A'...'Z': case '_':
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    object = read_symbol(ch);
	    break;
	case '.':
	    /* allow leading dot for floats */
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    ch = getc();
	    if (ch >= '0' && ch <= '9') {
		ungetc(ch);
		object = read_number('.');
		break;
	    }
	    ungetc(ch);
	    ch = '.';
	default:
	    input->lineno = input_note.lineno;
	    input->column = input_note.column;
	    object = read_keyword(ch);
	    break;
    }

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
    oast_t		*ast;
    osymbol_t		*symbol;
    oobject_t		*pointer;

    symbol = onew_identifier(read_ident(ch));
    if (symbol->keyword) {
	gc_enter();

	gc_ref(pointer);
	onew_ast(pointer, *(oword_t *)symbol->value,
		 input_note.name, input->lineno, input->column);
	gc_leave();

	return (*pointer);
    }
    else if (symbol->type) {
	gc_enter();

	gc_ref(pointer);
	onew_ast(pointer, tok_type,
		 input_note.name, input->lineno, input->column);
	ast = *pointer;
	ast->l.value = symbol->tag;
	gc_leave();

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

    gc_ref(pointer);
    onew_ast(pointer, token, input_note.name, input->lineno, input->column);
    gc_pop(object);

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
