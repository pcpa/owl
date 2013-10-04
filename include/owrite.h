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

#ifndef _owrite_h
#define _owrite_h

#include "otypes.h"

/*
 * Types
 */
struct oformat {
    oint32_t	 width;
    oint32_t	 radix;
    oint32_t	 prec;		/* float precision */
    oint32_t	 left : 1;	/* align left */
    oint32_t	 aspc : 1;	/* print a space before positive numbers */
    oint32_t	 sign : 1;	/* always show number sign */
    oint32_t	 zero : 1;	/* pad with zeros */
    oint32_t	 read : 1;	/* output in format suitable for read back */
    oint32_t	 aflt : 1;	/* formatting a float64 */
    oint32_t	 uppr : 1;	/* uppercase */
    oint32_t	 eflt : 1;	/* exponent float */
    oint32_t	 gflt : 1;	/* general float */
};

/*
 * Prototypes
 */
extern void
init_write(void);

extern void
finish_write(void);

extern oword_t
oprint_ptr(ostream_t *stream, oformat_t *format, oobject_t object);

extern oword_t
oprint_str(ostream_t *stream, oformat_t *format, ovector_t *vector);

extern oword_t
oprint_wrd(ostream_t *stream, oformat_t *format, oword_t word);

extern oword_t
oprint_flt(ostream_t *stream, oformat_t *format, ofloat_t floating);

extern oword_t
oprint_rat(ostream_t *stream, oformat_t *format, orat_t rat);

extern oword_t
oprint_mpz(ostream_t *stream, oformat_t *format, ompz_t integer);

extern oword_t
oprint_mpq(ostream_t *stream, oformat_t *format, ompq_t rational);

extern oword_t
oprint_mpr(ostream_t *stream, oformat_t *format, ompr_t floating);

extern oword_t
oprint_cdd(ostream_t *stream, oformat_t *format, ocdd_t *dd);

extern oword_t
oprint_cqq(ostream_t *stream, oformat_t *format, ocqq_t qq);

extern oword_t
oprint_mpc(ostream_t *stream, oformat_t *format, ompc_t c);

#endif /* _owrite_h */
