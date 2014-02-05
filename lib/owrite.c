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

#include <stdio.h>
#include "owl.h"

/*
 * Types
 */
typedef struct nat_print {
    ovector_t		*vector;
    oobject_t		 list[1];
} nat_print_t;

typedef struct nat_printf {
    ostream_t		*stream;
    ovector_t		*vector;
    oobject_t		 list[1];
} nat_printf_t;

/*
 * Prototypes
 */
static obool_t
print_check(ostream_t *stream, oformat_t *format);

static oword_t
print_pad(ostream_t *stream, obool_t left, oword_t bytes, oword_t count);

static void
native_print(oobject_t list, oint32_t size);

static void
native_printf(oobject_t list, oint32_t size);

static void
native_print_impl(ostream_t *stream,
		  ovector_t *vector, oobject_t *list, oword_t size);

/*
 * Implementation
 */
void
init_write(void)
{
    obuiltin_t		*builtin;

    builtin = onew_builtin("print", native_print, t_word, true);
    onew_argument(builtin, t_void);		/* format */
    oend_builtin(builtin);

    builtin = onew_builtin("printf", native_printf, t_word, true);
    onew_argument(builtin, t_void);		/* stream */
    onew_argument(builtin, t_void);		/* format */
    oend_builtin(builtin);
}

void
finish_write(void)
{
}

oword_t
oprint_ptr(ostream_t *stream, oformat_t *format, oobject_t object)
{
    GET_THREAD_SELF()
    otype_t		type;
    oword_t		word;
    oword_t		bytes;
    oformat_t		local;

    if (!print_check(stream, format)) {
	bytes = eof;
	goto done;
    }

    memcpy(&local, format, sizeof(oformat_t));
    type = t_word;
    local.zero = 1;
    local.read = local.aspc = local.sign = 0;
    local.radix = 16;
    local.width = sizeof(oword_t) >> 2;
    word = (oword_t)object;
    if (word < 0) {
	/* thread_self->z0 is used by oprint_mpz (at base 256) */
	mpz_set_ui(thr_zr, word);
	type = t_mpz;
    }
    bytes = owrite(stream, "#<", 2);
    if (type == t_word)
	bytes += oprint_wrd(stream, &local, word);
    else
	bytes += oprint_mpz(stream, &local, thr_zr);
    oputc(stream, '>');
    ++bytes;
    if (ostream_p(stream)) {
	stream->s_w_mode = !!stream->offset;
	if (!stream->s_buffered)
	    oflush(stream);
    }

done:
    return (bytes);
}

oword_t
oprint_str(ostream_t *stream, oformat_t *format, ovector_t *vector)
{
    ouint8_t		*ptr;
    oword_t		 size;
    oword_t		 bytes;
    oword_t		 count;
    ouint8_t		*f, *t, *l;

    if (!print_check(stream, format))
	return (eof);

    if (vector == null)
	size = count = 4;
    else {
	size = count = vector->length;
	if (format->read) {
	    for (t = vector->v.u8, l = t + size; t < l; t++) {
		switch (*t) {
		    case '\a':		case '\b':	case '\t':
		    case '\n':		case '\v':	case '\f':
		    case '\r':		case '"':	case '\\':
			++count;
			break;
		    case ' ':		case '!':	case '#' ... '[':
		    case ']' ... '~':
			break;
		    default:
			count += 3;
			break;
		}
	    }
	    count += 2;
	}
	else if (format->width && format->width < vector->length)
	    size = count = format->width;
    }

    bytes = count > format->width ? count : format->width;
    ocheck_buffer(stream, bytes);

    ptr = stream->ptr + stream->offset;
    if (!format->left)
	ptr += bytes - count;

    if (vector == null)
	memcpy(ptr, "null", 4);
    else {
	if (format->read) {
	    *ptr++ = '"';
	    for (f = t = vector->v.u8, l = f + size; t < l; t++) {
		switch (*t) {
		    case '\a':		case '\b':	case '\t':
		    case '\n':		case '\v':	case '\f':
		    case '\r':		case '"':	case '\\':
			memcpy(ptr, f, t - f);
			ptr += t - f;
			*ptr++ = '\\';
			switch (*t) {
			    case '\a':	*ptr++ = 'a';	break;
			    case '\b':	*ptr++ = 'b';	break;
			    case '\t':	*ptr++ = 't';	break;
			    case '\n':	*ptr++ = 'n';	break;
			    case '\v':	*ptr++ = 'v';	break;
			    case '\f':	*ptr++ = 'f';	break;
			    case '\r':	*ptr++ = 'r';	break;
			    default:	*ptr++ = *t;	break;
			}
			f = t + 1;
			break;
		    case ' ':		case '!':	case '#' ... '[':
		    case ']' ... '~':
			break;
		    default:
			memcpy(ptr, f, t - f);
			ptr += t - f;
			*ptr++ = '\\';
			*ptr++ = '0' + ((*t >> 6) & 7);
			*ptr++ = '0' + ((*t >> 3) & 7);
			*ptr++ = '0' + (*t & 7);
			f = t + 1;
			break;
		}
	    }
	    memcpy(ptr, f, t - f);
	    ptr[t - f] = '"';
	}
	else
	    memcpy(ptr, vector->v.u8, count);
    }

    return (print_pad(stream, format->left, bytes, count));
}

oword_t
oprint_wrd(ostream_t *stream, oformat_t *format, oword_t word)
{
    oint32_t		 chr;
    ouint8_t		*ptr;
    ouint8_t		*base;
    ouword_t		 mask;
    oword_t		 bytes;
    oint32_t		 shift;
    oword_t		 count;
    ouword_t		 value;
    oint32_t		 zeros;
    char		*digits;

    if (!print_check(stream, format))
	return (eof);

    /* if printing a character */
    if (format->radix == 256) {
	shift = (sizeof(oword_t) << 3) - 8;
	mask = 0xffL << shift;
	for (count = sizeof(oword_t); mask; mask >>= 8, count--, shift -= 8) {
	    if (word & mask)
		break;
	}	    

	if (format->read) {
	    ouword_t	rm;
	    oint32_t	rs;

	    for (rm = mask, rs = shift; rm; rm >>= 8, rs -= 8) {
		chr = (word & rm) >> rs;
		if (chr == '\'' || chr == '\\')
		    ++count;
	    }
	    count += 2;
	}

	bytes = count > format->width ? count : format->width;
	ocheck_buffer(stream, bytes);
	ptr = stream->ptr + stream->offset;

	if (!format->left)
	    ptr += bytes - count;

	if (format->read) {
	    *ptr++ = '\'';
	    for (; mask; mask >>= 8, shift -= 8) {
		chr = (word & mask) >> shift;
		if (chr == '\'' || chr == '\\')
		    *ptr++ = '\\';
		*ptr++ = chr;
	    }
	    *ptr = '\'';
	}
	else {
	    for (; mask; mask >>= 8, shift -= 8)
		*ptr++ = (word & mask) >> shift;
	}

	return (print_pad(stream, format->left, bytes, count));
    }

    if ((value = word < 0 ? -word : word)) {
	count = sizeof(oword_t) << 3;
	mask = 0x1L << (count - 1);
	for (; mask; count--, mask >>= 1) {
	    if (value & mask)
		break;
	}
    }
    else
	count = 1;
    switch (format->radix) {
	case 2:
	    mask = shift = 1;
	    if (format->read)
		count += 2;
	    break;
	case 8:
	    mask = 07;
	    shift = 3;
	    count = count * 0.3333333333333334 + 1;
	    if (format->read)
		++count;
	    break;
	case 16:
	    mask = 0xf;
	    shift = 4;
	    count = count * 0.25 + 1;
	    if (format->read)
		count += 2;
	    break;
	default:
	    mask = shift = 0;
	    count = count * 0.3010299956639811 + 1;
	    break;
    }
    if (word < 0 || format->sign || format->aspc)
	++count;

    bytes = count > format->width ? count : format->width;
    ocheck_buffer(stream, bytes);
    if (format->left) {
	base = stream->ptr + stream->offset;
	ptr = base + count;
    }
    else {
	ptr = stream->ptr + stream->offset + bytes;
	base = ptr - count;
    }

    if (value == 0)
	*--ptr = '0';
    /* if power of 2 radix */
    else if (shift) {
	if (format->uppr)
	    digits = "0123456789ABCDEF";
	else
	    digits = "0123456789abcdef";
	for (; value; value >>= shift)
	    *--ptr = digits[value & mask];
    }
    else {
	for (; value; value /= 10)
	    *--ptr = (value % 10) + '0';
    }

    if (format->read) {
	switch (format->radix) {
	    case 2:	*--ptr = format->uppr ? 'B' : 'b';
		goto zero;
	    case 16:	*--ptr = format->uppr ? 'X' : 'x';
	    case 8:
	    zero:	*--ptr = '0';
	    default:	break;
	}
    }
    else if (format->zero) {
	base = stream->ptr + stream->offset;
	zeros = ptr - base;
	ptr = base;
	if (word < 0 || format->sign || format->aspc) {
	    --zeros;
	    ++ptr;
	}
	if (zeros && format->width && bytes > format->width) {
	    /* may have allocated one more byte then required */
	    --zeros;
	    --bytes;
	    memmove(base, base + 1, bytes);
	}
	if (zeros > 0)
	    memset(ptr, '0', zeros);
	/* no padding */
	count = bytes;
    }

    if (word < 0)		*--ptr = '-';
    else if (format->sign)	*--ptr = '+';
    else if (format->aspc)	*--ptr = ' ';

    if (ptr != base) {
	--count;
	/* if left aligned and result is one digit to the right */
	if (format->left || bytes > format->width)
	    memmove(base, ptr, count);
	if (bytes - 1 > format->width)
	    --bytes;
    }

    return (print_pad(stream, format->left, bytes, count));
}

oword_t
oprint_flt(ostream_t *stream, oformat_t *format, ofloat_t floating)
{
    GET_THREAD_SELF()
    obool_t		aflt;
    oword_t		bytes;

    aflt = format->aflt;
    mpfr_set_d(thr_rr, floating, thr_rnd);

    format->aflt = 1;
    bytes = oprint_mpr(stream, format, thr_rr);
    format->aflt = aflt;

    return (bytes);
}

oword_t
oprint_rat(ostream_t *stream, oformat_t *format, orat_t rat)
{
    GET_THREAD_SELF()
    mpq_set_si(thr_qr, orat_num(rat), orat_den(rat));

    return (oprint_mpq(stream, format, thr_qr));
}

oword_t
oprint_mpz(ostream_t *stream, oformat_t *format, ompz_t integer)
{
    GET_THREAD_SELF()
    ouint8_t		*ptr;
    ouint8_t		*base;
    oword_t		 size;
    oword_t		 count;
    oword_t		 bytes;
    oword_t		 zeros;

    if (!print_check(stream, format))
	return (eof);

    if (format->radix == 256) {
	oint32_t	c;
	oint32_t	s;
	oint32_t	S;
	ouword_t	i;
	ouword_t	m;
	ouword_t	M;

	count = mpz_sizeinbase(integer, 16) / 2;
	/* simplify code below if value fits in an unsigned 64 bits integer */
	if (count <= (sizeof(oword_t) >> 3))
	    return (oprint_wrd(stream, format, mpz_get_ui(integer)));

	assert(thr_zs != integer);
	S = (sizeof(oword_t) << 3) - 8;
	M = 0xffL << S;
	size = count - (count & 7);

	/* update mask and shift if count is not a multiple of 8 */
	if (size != count) {
	    mpz_fdiv_q_2exp(thr_zs, integer, size << 3);
	    i = mpz_get_ui(thr_zs);
	    for (; M; M >>= 8, S -= 8) {
		if (i & M)
		    break;
	    }
	}
	else
	    i = 0;

	/* check for characters that must be quoted */
	if (format->read) {
	    if (S != (sizeof(oword_t) << 3) - 8) {
		for (m = M, s = S; m; m >>= 8, s -= 8) {
		    c = (i & m) >> s;
		    if (c == '\'' || c == '\\')
			++count;
		}
	    }
	    for (bytes = size - 8; bytes >= 0; bytes -= 8) {
		if (bytes) {
		    mpz_fdiv_q_2exp(thr_zs, integer, bytes << 3);
		    i = mpz_get_ui(thr_zs);
		}
		else
		    i = mpz_get_ui(integer);
		s = (sizeof(oword_t) << 3) - 8;
		m = 0xffL << s;
		for (; m; m >>= 8, s -= 8) {
		    c = (i & m) >> s;
		    if (c == '\'' || c == '\\')
			++count;
		}
	    }
	    count += 2;
	}
	bytes = count > format->width ? count : format->width;
	ocheck_buffer(stream, bytes);
	ptr = stream->ptr + stream->offset;
	if (!format->left)
	    ptr += bytes - count;
	if (format->read)	*ptr++ = '\'';

	/* if not a multiple of 8 */
	if (S != (sizeof(oword_t) << 3) - 8) {
	    mpz_fdiv_q_2exp(thr_zs, integer, size << 3);
	    i = mpz_get_ui(thr_zs);
	    for (; M; M >>= 8, S -= 8) {
		c = (i & M) >> S;
		if (format->read && (c == '\'' || c == '\\'))	*ptr++ = '\\';
		*ptr++ = c;
	    }
	}

	/* print value */
	for (size -= 8; size >= 0; size -= 8) {
	    if (size) {
		mpz_fdiv_q_2exp(thr_zs, integer, size << 3);
		i = mpz_get_ui(thr_zs);
	    }
	    else
		i = mpz_get_ui(integer);
	    s = (sizeof(oword_t) << 3) - 8;
	    m = 0xffL << s;
	    for (; m; m >>= 8, s -= 8) {
		c = (i & m) >> s;
		if (format->read && (c == '\'' || c == '\\'))	*ptr++ = '\\';
		*ptr++ = c;
	    }
	}
	if (format->read)	*ptr++ = '\'';

	return (print_pad(stream, format->left, bytes, count));
    }

    count = size = mpz_sizeinbase(integer, format->radix);
    if (mpz_sgn(integer) < 0) {
	++count;
	++size;
    }
    else if (format->sign || format->aspc)
	++count;

    if (format->read) {
	switch (format->radix) {
	    case 2: case 16:	++count;
	    case 8:		++count;
	    default:		break;
	}
    }

    bytes = count > format->width ? count : format->width;
    /* plus one for ending '\0' */
    ocheck_buffer(stream, bytes + 1);
    base = ptr = stream->ptr + stream->offset;
    if (!format->left)
	ptr += bytes - count;

    if (mpz_sgn(integer) >= 0 && (format->sign || format->aspc))
	++ptr;

    if (format->read) {
	switch (format->radix) {
	    case 2: case 16:	++ptr;
	    case 8:		++ptr;
	    default:		break;
	}
    }

    mpz_get_str((char *)ptr, format->uppr ?
		-format->radix : format->radix, integer);

    if (format->radix == 10) {
	zeros = strlen((char *)ptr);
	/* if right aligned and result is one digit to the left */
	if (zeros != size) {
	    --count;
	    if (!format->left && format->width == bytes) {
		memmove(ptr + 1, ptr, zeros);
		++ptr;
	    }
	    if (bytes - 1 > format->width)
		--bytes;
	}
    }

    if (format->read) {
	if (mpz_sgn(integer) < 0)
	    ++ptr;
	switch (format->radix) {
	    case 2:		*--ptr = format->uppr ? 'B' : 'b';
		goto zero;
	    case 16:		*--ptr = format->uppr ? 'X' : 'x';
	    case 8:
	    zero:		*--ptr = '0';
	    default:		break;
	}
    }
    else if (format->zero) {
	/* skip sign already in string */
	if (mpz_sgn(integer) < 0)
	    ++ptr;
	zeros = ptr - base;
	ptr = base;
	/* check if need one extra byte for sign or space */
	if (mpz_sgn(integer) < 0 || format->sign || format->aspc) {
	    --zeros;
	    ++ptr;
	}
	if (zeros)
	    memset(ptr, '0', zeros);
	/* no padding */
	count = bytes;
    }

    if (mpz_sgn(integer) < 0) {
	if (format->read || format->zero)
	    *--ptr = '-';
    }
    else if (format->sign)	*--ptr = '+';
    else if (format->aspc)	*--ptr = ' ';

    return (print_pad(stream, format->left, bytes, count));
}

oword_t
oprint_mpq(ostream_t *stream, oformat_t *format, ompq_t rational)
{
    ouint8_t		*ptr;
    obool_t		 neg;
    ouint8_t		*base;
    oword_t		 size;
    oword_t		 bytes;
    oword_t		 count;
    oword_t		 nsize;
    oword_t		 zeros;

    /* check it only once */
    if (!mpz_cmp_ui(mpq_denref(rational), 1))
	return (oprint_mpz(stream, format, mpq_numref(rational)));

    if (!print_check(stream, format))
	return (eof);

    if (format->radix == 256)
	format->radix = 10;

    count = nsize = mpz_sizeinbase(mpq_numref(rational), format->radix);
    count += mpz_sizeinbase(mpq_denref(rational), format->radix) + 1;

    size = count;
    if ((neg = mpq_sgn(rational) < 0)) {
	++size;
	++count;
    }
    else if (format->sign || format->aspc)
	++count;

    if (format->read) {
	switch (format->radix) {
	    case 2: case 16:	count += 2;
	    case 8:		count += 2;
	    default:		break;
	}
    }

    bytes = count > format->width ? count : format->width;
    /* plus one for ending '\0' */
    ocheck_buffer(stream, bytes + 1);
    base = ptr = stream->ptr + stream->offset;
    if (!format->left)
	ptr += bytes - count;

    if (!neg && (format->sign || format->aspc))
	++ptr;

    if (format->read && format->radix != 10) {
	/* remember current value */
	base = ptr;

	/* since the reader doesn't truly read rationals,
	 * add base prefix on numerator and denominator */
	switch (format->radix) {
	    case 2: case 16:	++ptr;
	    case 8:		++ptr;
	}
	mpz_get_str((char *)ptr, format->uppr ? -format->radix : format->radix,
		    mpq_numref(rational));
	if (neg)		++ptr;
	switch (format->radix) {
	    case 2:
		ptr[-1] = format->uppr ? 'B' : 'b';
		ptr[-2] = '0';
		break;
	    case 16:
		ptr[-1] = format->uppr ? 'X' : 'x';
		ptr[-2] = '0';
		break;
	    case 8:
		ptr[-1] = '0';
	}

	ptr += nsize;
	*ptr++ = '/';
	switch (format->radix) {
	    case 2:
		*ptr++ = '0';
		*ptr++ = format->uppr ? 'B' : 'b';
		break;
	    case 16:
		*ptr++ = '0';
		*ptr++ = format->uppr ? 'X' : 'x';
		break;
	    case 8:
		*ptr++ = '0';
	}
	mpz_get_str((char *)ptr,
		    format->uppr ? -format->radix : format->radix,
		    mpq_denref(rational));
	/* restore values */
	ptr = base;
	base = stream->ptr + stream->offset;
    }
    else
	mpq_get_str((char *)ptr, format->uppr ?
		    -format->radix : format->radix, rational);

    if (format->radix == 10) {
	zeros = strlen((char *)ptr);
	if (zeros != size) {
	    /* nsize no longer used */
	    nsize = size - zeros;
	    count -= nsize;
	    /* if right aligned and result is one digit to the left */
	    if (!format->left && format->width == bytes) {
		memmove(ptr + 1, ptr, count);
		++ptr;
	    }
	    if (bytes - nsize > format->width)
		bytes -= nsize;
	}
    }

    if (format->read) {
	/* if base is 10, this is a noop */
	if (neg)
	    ++ptr;
    }
    else if (format->zero) {
	/* skip sign already in string */
	if (neg)
	    ++ptr;
	zeros = ptr - base;
	ptr = base;
	if (neg || format->sign || format->aspc) {
	    ++ptr;
	    --zeros;
	}
	if (zeros)
	    memset(ptr, '0', zeros);
	/* no padding */
	count = bytes;
    }

    if (neg) {
	if (format->read || format->zero)
	    *--ptr = '-';
    }
    else if (format->sign)	*--ptr = '+';
    else if (format->aspc)	*--ptr = ' ';

    return (print_pad(stream, format->left, bytes, count));
}

/*
 * Logic:
 * o "width" is not the number of digits bofore the dot, but a hint
 *   about alignment, that is used if the output string is smaller.
 * o "prec" is not the number of digits after the dot, but the number
 *   of significant digits to use.
 * o by default, all significant digits are printed, what may be
 *   confusing, as floats are not base 10, but the prec field should
 *   help.
 */
oword_t
oprint_mpr(ostream_t *stream, oformat_t *format, ompr_t floating)
{
    char		*dst;
    char		*src;
    obool_t		 neg;
    char		*base;
    mp_exp_t		 fexp;
    oword_t		 bytes;
    oword_t		 count;
    oword_t		 zeros;
    oword_t		 digits;
    mp_exp_t		 aexp;		/* absolute value of fexp */
    oword_t		 mask;
    oint32_t		 shift;
    obool_t		 emode;		/* exponent mode flag */
    oword_t		 ecount;	/* number of bytes for exponent */
    char		 buffer[8192];

    if (!print_check(stream, format))
	return (eof);

    if (format->radix == 256)
	format->radix = 10;

    neg = mpfr_signbit(floating) && !mpfr_nan_p(floating);

    if (!mpfr_number_p(floating)) {
	count = 3;
	src = buffer;
	if (neg) {
	    ++count;
	    *src++ = '-';
	}
	else if (format->sign) {
	    ++count;
	    *src++ = '+';
	}
	else if (format->aspc) {
	    ++count;
	    *src++ = ' ';
	}
	if (mpfr_inf_p(floating))
	    strcpy(src, format->uppr ? "INF" : "inf");
	else
	    strcpy(src, format->uppr ? "NAN" : "nan");
	if (format->width > count)
	    bytes = format->width;
	else
	    bytes = count;
	ocheck_buffer(stream, bytes);
	base = dst = (char *)stream->ptr + stream->offset;
	if (!format->left)
	    dst += bytes - count;
	memcpy(dst, buffer, bytes);

	return (print_pad(stream, format->left, bytes, count));
    }

    /* calculate maximum number of digits required to represent number */
    count = mpfr_get_prec(floating);
    switch (format->radix) {
	case 2:							break;
	case 8:  count = count * 0.3333333333333334 + 1;	break;
	case 16: count = count * 0.25 + 1;			break;
	default: count = count * 0.3010299956639811 + 1;	break;
    }

    /* if a precision is specified */
    if (format->prec && format->prec < count)
	count = format->prec;

    /* get number representation, plus one for sign and one for ending '\0' */
    if (count + 2 > sizeof(buffer))
	onew_object((oobject_t *)&src, t_uint8, count + 2);/* left for gc */
    else
	src = buffer;

    mpfr_get_str(src, &fexp, format->radix,
		 format->prec, floating, thr_rnd);
    bytes = strlen(src);
    /* follow the previous logic using mpf_t to format numbers:
     * mpfr_get_str pads with zeros, remove them */
    for (; bytes > 1 && src[bytes - 1] == '0'; --bytes)
	;
    if (format->uppr && format->radix > 10) {
	/* mpfr_get_str doesn't uppcase result, and unlike mpf_get_str,
	 * passing a negative radix argument causes an error */
	for (count = 0; count < bytes; count++)
	    if (src[count] >= 'a' && src[count] <= 'f')
		src[count] -= 'a' - 'A';
    }
    else
	count = bytes;

    /* check if using exponential representation */
    if (format->eflt)
	emode = true;
    else if (format->gflt) {
	if (format->prec > 2)
	    emode = fexp < -(format->prec - 2) || fexp > format->prec;
	else
	    emode = fexp < -4 || fexp > 6;
    }
    else
	/* already too large, but don't print that many zeros */
	emode = fexp < -65536 || fexp > 65536;

    /* skip the leading sign */
    if (neg) {
	++src;
	--count;
    }

    /* if zero or negative exponent, allocate space for zero also */
    if (fexp <= 0)
	bytes += -fexp + 1;
    /* this also handles (fexp==count) == 0, i.e. the number is zero */
    if (fexp >= count)
	bytes += fexp - count + 1;
    /* one extra byte for the dot */
    ++bytes;

    /* calculate number of bytes for exponent */
    if (emode) {
	if (mpfr_zero_p(floating))
	    aexp = 0;
	else {
	    if (fexp > 0)
		aexp = fexp - 1;
	    else
		aexp = -fexp + 1;
	}
	if (aexp) {
	    ecount = (sizeof(oword_t) << 3) - 1;
	    mask = 1L << (ecount - 1);
	    for (; mask; ecount--, mask >>= 1)
		if (aexp & mask) break;
	}
	else
	    ecount = 1;
	switch (format->radix) {
	    case 2:
		mask = 1;   shift = 1;
		break;
	    case 8:
		mask = 07;  shift = 3;
		ecount = ecount * 0.3333333333333334 + 1;
		break;
	    case 16: mask = 0xf; shift = 4;
		ecount = ecount * 0.25 + 1;
		break;
	    default: mask = 0;   shift = 0;
		ecount = ecount * 0.3010299956639811 + 1;
		break;
	}
	/* one extra for exponent char and possibly another for exponent sign
	 * note that it is overestimating for fexp == 0 if count == 0,
	 * if count is not 0, need the extra byte as exponent will be -1 */
	if (fexp <= 0 || format->sign || format->aspc)
	    ecount += 2;
	else
	    ++ecount;
	bytes += ecount;
    }
    /* compiler warnings when -O2 compiling */
    else
	aexp = mask = shift = ecount = 0;

    if (!neg && (format->sign || format->aspc))
	++bytes;
    if (format->read) {
	switch (format->radix) {
	    case 2: case 16:	++bytes;
	    case 8:		++bytes;
	    default:		break;
	}
	/* if need to specify it is a mpfr_t */
	if (!format->aflt)	++bytes;
    }

    /* ensure stream buffer has enough bytes for number representation */
    ocheck_buffer(stream, bytes);

    /* remember start of number representation */
    base = dst = (char *)stream->ptr + stream->offset;

    if (neg || format->sign || format->aspc)
	++dst;
    if (format->read) {
	switch (format->radix) {
	    case 2:
		*dst++ = '0';
		*dst++ = format->uppr ? 'B' : 'b';
		break;
	    case 16:
		*dst++ = '0';
		*dst++ = format->uppr ? 'X' : 'x';
		break;
	    case 8:
		*dst++ = '0';
		break;
	}
    }

    /* in exponent mode, only use one digit bofore the dot */
    if (emode) {
	/* if value is zero */
	if (count == 0)
	    *dst++ = '0';
	else {
	    *dst++ = *src++;
	    if (fexp > 0)
		++fexp;
	    else
		--fexp;
	    --count;
	}
    }
    /* put all integral significant digits bofore the dot */
    else {
	/* number of digits bofore the dot */
	digits = fexp <= 0 ? 1 : fexp;
	if (fexp <= 0)
	    *dst++ = '0';
	else if (digits > count) {
	    memcpy(dst, src, count);
	    dst += count;
	    src += count;
	    digits -= count;
	    memset(dst, '0', digits);
	    dst += digits;
	    count = 0;
	}
	else {
	    memcpy(dst, src, digits);
	    dst += digits;
	    src += digits;
	    count -= digits;
	}
    }

    /* Always print a dot */
    *dst++ = '.';

    /* check if need to fill padding zeros after dot */
    if (!emode && fexp < 0) {
	memset(dst, '0', -fexp);
	dst += -fexp;
    }

    /* check if need to copy any significant digits after dot */
    if (count) {
	memcpy(dst, src, count);
	dst += count;
    }
    else {
	*dst++ = '0';
	++count;
    }

    if (emode && (zeros = format->prec - count - 1) > 0) {
	memset(dst, '0', zeros);
	dst += zeros;
    }

    /* print exponent */
    if (emode) {
	*dst++ = format->radix == 10 ? format->uppr ? 'E' : 'e' : '@';
	/* save pointer in case of needing to move integer representation */
	src = dst;
	dst += ecount;
	if (aexp == 0)
	    *--dst = '0';
	else if (shift) {
	    char	*repr;

	    repr = format->uppr ? "0123456789ABCDEF" : "0123456789abcdef";
	    for (; aexp; aexp >>= shift)
		*--dst = repr[aexp & mask];
	}
	else {
	    for (; aexp; aexp /= 10)
		*--dst = (aexp % 10) + '0';
	}
	if (fexp < 0)
	    *--dst = '-';
	else if (format->sign || format->aspc)
	    *--dst = '+';
	if (dst != src) {
	    ecount -= dst - src;
	    memmove(src, dst, ecount);
	}
	dst = src + ecount;
    }

    if (format->read && !format->aflt)
	*dst++ = format->uppr ? 'L' : 'l';

    /* check if need to adjust format width */
    count = dst - base;
    bytes = format->width > count ? format->width : count;
    dst = base;
    if (neg || format->sign || format->aspc)
	++dst;
    if (bytes > count && !format->left) {
	zeros = bytes - count;
	memmove(dst + zeros, dst, count);
	if (format->zero) {
	    memset(dst, '0', zeros);
	    /* no padding */
	    count = bytes;
	}
	else
	    dst += zeros;
    }

    if (neg)			*--dst = '-';
    else if (format->sign)	*--dst = '+';
    else if (format->aspc)	*--dst = ' ';

    return (print_pad(stream, format->left, bytes, count));
}

#define real_prt		1
#define imag_prt		2
oword_t
oprint_cdd(ostream_t *stream, oformat_t *format, ocdd_t *dd)
{
    GET_THREAD_SELF()
    ouint8_t		*ptr;
    oword_t		 bytes;
    oformat_t		 local;
    oint32_t		 state;
    ostream_t		*buffer;

    if (!print_check(stream, format))
	return (eof);

    buffer = (ostream_t *)thread_self->vec;
    buffer->offset = 0;
    memcpy(&local, format, sizeof(oformat_t));
    if (local.radix == 256)
	local.radix = 10;
    local.width = 0;
    local.aspc = 0;

    state = 0;
    if (!finite(real(*dd)) || real(*dd) != 0)
	state |= real_prt;
    if (format->read || !finite(imag(*dd)) || fabs(imag(*dd)) != 1.0)
	state |= imag_prt;
    if (state & real_prt)
	oputc(buffer, '(');
    if (state & real_prt) {
	oprint_flt(buffer, &local, real(*dd));
	local.sign = 1;
    }
    if (state & imag_prt) {
	oprint_flt(buffer, &local, imag(*dd));
	owrite(buffer, "*I", 2);
    }
    else if (imag(*dd) > 0) {
	if (!state & real_prt)
	    oputc(buffer, 'I');
	else
	    owrite(buffer, "+I", 2);
    }
    else
	owrite(buffer, "-I", 2);
    if (state & real_prt)
	oputc(buffer, ')');

    bytes = buffer->offset > format->width ? buffer->offset : format->width;
    ocheck_buffer(stream, bytes);
    ptr = stream->ptr + stream->offset;
    memcpy(ptr, buffer->ptr, buffer->offset);

    return (print_pad(stream, format->left, bytes, buffer->offset));
}

oword_t
oprint_cqq(ostream_t *stream, oformat_t *format, ocqq_t qq)
{
    GET_THREAD_SELF()
    ouint8_t		*ptr;
    oword_t		 bytes;
    oword_t		 state;
    oformat_t		 local;
    ostream_t		*buffer;

    if (!print_check(stream, format))
	return (eof);

    buffer = (ostream_t *)thread_self->vec;
    buffer->offset = 0;
    memcpy(&local, format, sizeof(oformat_t));
    if (local.radix == 256)
	local.radix = 10;
    local.width = 0;
    local.aspc = 0;

    state = 0;
    if (mpq_sgn(cqq_realref(qq)))
	state |= real_prt;
    if (mpz_cmp_ui(mpq_denref(cqq_imagref(qq)), 1) ||
	mpz_cmpabs_ui(mpq_numref(cqq_imagref(qq)), 1))
	state |= imag_prt;
    if (state & real_prt)
	oputc(buffer, '(');
    if (state & real_prt) {
	oprint_mpq(buffer, &local, cqq_realref(qq));
	local.sign = 1;
    }
    if (state & imag_prt) {
	oprint_mpq(buffer, &local, cqq_imagref(qq));
	owrite(buffer, "*I", 2);
    }
    else if (mpq_sgn(cqq_imagref(qq)) > 0) {
	if (!state & real_prt)
	    oputc(buffer, 'I');
	else
	    owrite(buffer, "+I", 2);
    }
    else
	owrite(buffer, "-I", 2);
    if (state & real_prt)
	oputc(buffer, ')');

    bytes = buffer->offset > format->width ? buffer->offset : format->width;
    ocheck_buffer(stream, bytes);
    ptr = stream->ptr + stream->offset;
    memcpy(ptr, buffer->ptr, buffer->offset);

    return (print_pad(stream, format->left, bytes, buffer->offset));
}

oword_t
oprint_mpc(ostream_t *stream, oformat_t *format, ompc_t c)
{
    GET_THREAD_SELF()
    ouint8_t		*ptr;
    oint32_t		 sign;
    oword_t		 bytes;
    oformat_t		 local;
    oint32_t		 state;
    ostream_t		*buffer;

    if (!print_check(stream, format))
	return (eof);

    buffer = (ostream_t *)thread_self->vec;
    buffer->offset = 0;
    memcpy(&local, format, sizeof(oformat_t));
    if (local.radix == 256)
	local.radix = 10;
    local.width = 0;
    local.aspc = 0;

    state = 0;
    if (!mpfr_number_p(mpc_realref(c)) || !mpfr_zero_p(mpc_realref(c)))
	state |= real_prt;
    if (format->read ||
	!mpfr_number_p(mpc_imagref(c)) ||
	!(sign = mpfr_sgn(mpc_imagref(c))) ||
	(sign > 0 && mpfr_cmp_ui(mpc_imagref(c), 1)) ||
	(sign < 0 && mpfr_cmp_si(mpc_imagref(c), -1)))
	state |= imag_prt;
    if (state & real_prt)
	oputc(buffer, '(');
    if (state & real_prt) {
	oprint_mpr(buffer, &local, mpc_realref(c));
	local.sign = 1;
    }
    if (state & imag_prt) {
	oprint_mpr(buffer, &local, mpc_imagref(c));
	owrite(buffer, "*I", 2);
    }
    else if (mpfr_sgn(mpc_imagref(c)) > 0) {
	if (!state & real_prt)
	    oputc(buffer, 'I');
	else
	    owrite(buffer, "+I", 2);
    }
    else
	owrite(buffer, "-I", 2);
    if (state & real_prt)
	oputc(buffer, ')');

    bytes = buffer->offset > format->width ? buffer->offset : format->width;
    ocheck_buffer(stream, bytes);
    ptr = stream->ptr + stream->offset;
    memcpy(ptr, buffer->ptr, buffer->offset);

    return (print_pad(stream, format->left, bytes, buffer->offset));
}

static obool_t
print_check(ostream_t *stream, oformat_t *format)
{
    if (ostream_p(stream) && !stream->s_write)
	return (false);

    switch (format->radix) {
	case 2: case 8: case 10: case 16: case 256:
	    break;
	default:
	    format->radix = 10;
	    break;
    }
    if (format->width < 0 || format->width > 0xffff)
	format->width = 0;
    if (format->prec < 0 || format->prec > 0xffff)
	format->prec = 0;
    if (format->read || format->left)
	format->zero = 0;
    if (format->sign)
	format->aspc = 0;
    if (format->eflt)
	format->gflt = 0;

    return (true);
}

static oword_t
print_pad(ostream_t *stream, obool_t left, oword_t bytes, oword_t count)
{
    ouint8_t	*ptr;
    oword_t	 length;

    if (bytes > count) {
	length = bytes - count;
	ptr = stream->ptr + stream->offset;
	if (left)	ptr += count;
	switch (length) {
	    default: memset(ptr, ' ', length);     break;
	    case 8:	*ptr++ = ' ';	case 7:	*ptr++ = ' ';
	    case 6:	*ptr++ = ' ';	case 5:	*ptr++ = ' ';
	    case 4:	*ptr++ = ' ';	case 3:	*ptr++ = ' ';
	    case 2:	*ptr++ = ' ';	case 1:	*ptr++ = ' ';
	    break;
	}
    }

    stream->offset += bytes;
    if (ostream_p(stream)) {
	if (stream->offset > stream->size)
	    stream->size = stream->offset;
	stream->s_w_mode = !!stream->offset;
	if (!stream->s_buffered)
	    oflush(stream);
    }

    return (bytes);
}

static  void
native_print(oobject_t list, oint32_t size)
{
    nat_print_t		*alist;

    alist = (nat_print_t *)list;
    native_print_impl(std_output, alist->vector, alist->list, size);
}

static  void
native_printf(oobject_t list, oint32_t size)
{
    nat_printf_t	*alist;

    alist = (nat_printf_t *)list;
    native_print_impl(alist->stream, alist->vector, alist->list, size);
}

static  void
native_print_impl(ostream_t *stream,
		  ovector_t *vector, oobject_t *list, oword_t size)
{
    GET_THREAD_SELF()
    oword_t		 i;
    oobject_t		 arg;
    oint32_t		 aidx;
    obool_t		 argp;		/* positional argument */
    obool_t		 prec;
    oword_t		 bytes;
    oformat_t		 format;
    obool_t		 streamp;
    ouint8_t		*f, *t, *l;

    if (stream == null)
	othrow(except_invalid_argument);
    if ((streamp = otype(stream) == t_stream) == false) {
	if (otype(stream) != t_string)
	    othrow(except_invalid_argument);
	stream->offset = 0;
    }
    else if (!stream->s_write)
	othrow(except_invalid_argument);
    if (vector == null || otype(vector) != t_string)
	othrow(except_invalid_argument);

#define next_argument()							\
    do {								\
	if (argp)							\
	    argp = false;						\
	else {								\
	    if (aidx >= size) {						\
		f = t + 1 > l ? l : t + 1;				\
		goto done;						\
	    }								\
	    arg = list[aidx++];						\
	}								\
    } while (0)
#define position_argument(n)						\
    do {								\
	if (n < 0 || n >= size) {					\
	    f = t + 1 > l ? l : t + 1;					\
	    goto done;							\
	}								\
	aidx = n;							\
	arg = list[aidx];						\
	argp = true;							\
    } while (0)

    if (streamp)
	omutex_lock(&stream->mutex);

    aidx = 0;
    argp = false;
    f = t = vector->v.u8;
    l = f + vector->length;

    bytes = 0;
    memset(&format, 0, sizeof(oformat_t));
    while (t < l) {
	if (*t != '%') {
	    ++t;
	    continue;
	}

	bytes += owrite(stream, f, t - f);
	prec = false;
	format.left = format.aspc = format.sign = format.zero = format.read =
	    format.aflt = format.uppr = format.eflt = format.gflt = 0;
	format.prec = format.width = 0;
	format.radix = 10;

    again:
	if (++t >= l)
	    break;
    check:
	switch (*t) {
	    case '@':				format.eflt = 1;
		goto again;
	    case '&':				format.gflt = 1;
		goto again;
	    case '#':				format.read = 1;
		goto again;
	    case ' ':				format.aspc = 1;
		goto again;
	    case '+':				format.sign = 1;
		goto again;
	    case '-':				format.left = 1;
		goto again;
	    case '.':				prec = 1;
		goto again;
	    case '0':
		if (t + 1 < l && t[1] == '$') {
		    ++t;
		    position_argument(0);
		}
		else
		    format.zero = 1;
		goto again;
	    case '1'...'9':
		i = 0;
		do {
		    if (i >= 0x70000000 / 10)	goto overflow;
		    i = i * 10 + (*t - '0');
		    if (++t >= l)		goto format_object;
		} while (*t >= '0' && *t <= '9');
		if (*t == '$') {
		    position_argument(i);
		    ++t;
		}
		else if (prec)			format.prec = i;
		else				format.width = i;
		goto check;
	    case '%':
		oputc(stream, '%');		++bytes;
		break;
	    case '*':
		next_argument();
		if (arg && otype(arg) == t_word) {
		    i = *(oword_t *)arg;
		    if (i < 0 || i != (oint32_t)i)
			goto overflow;
		    if (prec)			format.prec = i;
		    else			format.width = i;
		}
		goto again;
	    case 'B':				format.uppr = 1;
	    case 'b':				format.radix = 2;
		goto format_object;
	    case 'O':				format.uppr = 1;
	    case 'o':				format.radix = 8;
		goto format_object;
	    case 'D':				format.uppr = 1;
	    case 'd':
		goto format_object;
	    case 'X':				format.uppr = 1;
	    case 'x':				format.radix = 16;
		goto format_object;
	    case 'C':
	    case 'c':				format.radix = 256;
		goto format_object;
	    case 'E':				format.uppr = 1;
	    case 'e':				format.eflt = 1;
		goto format_object;
	    case 'F':				format.uppr = 1;
	    case 'f':
		goto format_object;
	    case 'G':				format.uppr = 1;
	    case 'g':				format.gflt = 1;
		goto format_object;

	    case 'A': case 'S':			format.uppr = 1;
	    case 'a': case 's':
	    format_object:
		next_argument();
		if (arg == null)
		    bytes += owrite(stream, "null", 4);
		else {
		    switch (otype(arg)) {
			case t_word:
			    bytes += oprint_wrd(stream,
						&format, *(oword_t*)arg);
			    break;
			case t_float:
			    bytes += oprint_flt(stream,
						&format, *(ofloat_t *)arg);
			    break;
			case t_mpz:
			    bytes += oprint_mpz(stream,
						&format, arg);
			    break;
			case t_rat:
			    bytes += oprint_rat(stream,
						&format, arg);
			    break;
			case t_mpq:
			    bytes += oprint_mpq(stream,
						&format, arg);
			    break;
			case t_mpr:
			    bytes += oprint_mpr(stream,
						&format, arg);
			    break;
			case t_cdd:
			    bytes += oprint_cdd(stream,
						&format, arg);
			    break;
			case t_cqq:
			    bytes += oprint_cqq(stream,
						&format, arg);
			    break;
			case t_mpc:
			    bytes += oprint_mpc(stream,
						&format, arg);
			    break;
			case t_string:
			    bytes += oprint_str(stream,
						&format, arg);
			    break;
			default:
			    bytes += oprint_ptr(stream,
						&format, arg);
			    break;
		    }
		}
		break;
	    default:
	    overflow:
		if (streamp) {
		    oflush(stream);
		    omutex_unlock(&stream->mutex);
		}
		othrow(except_invalid_argument);
	}
	++t;
	f = t;
    }

done:
    bytes += owrite(stream, f, l - f);
    if (streamp) {
	oflush(stream);
	omutex_unlock(&stream->mutex);
    }
    else
	stream->length = bytes;
    thread_self->r0.t = t_word;
    thread_self->r0.v.w = bytes;
}
