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

#include <gmp.h>

#ifndef _cqq_h
#define _cqq_h

#define cqq_realref(x)		((x)->re)
#define cqq_imagref(x)		((x)->im)

/*
 * Types
 */
typedef struct {
    mpq_t	re;
    mpq_t	im;
} __cqq_struct;
typedef __cqq_struct		cqq_t[1];
typedef __cqq_struct		*cqq_ptr;

/*
 * Prototypes
 */
extern void
cqq_init(cqq_ptr u);

extern void
cqq_clear(cqq_ptr u);

extern void
cqq_set(cqq_ptr u, cqq_ptr v);

extern void
cqq_set_si(cqq_ptr u, long re, long im);

extern void
cqq_set_ui(cqq_ptr u, unsigned long re, unsigned long im);

extern void
cqq_set_z(cqq_ptr u, mpz_t z);

extern void
cqq_set_q(cqq_ptr u, mpq_t q);

extern void
cqq_neg(cqq_ptr u, cqq_ptr v);

extern void
cqq_add(cqq_ptr u, cqq_ptr v, cqq_ptr w);

extern void
cqq_sub(cqq_ptr u, cqq_ptr v, cqq_ptr w);

extern void
cqq_mul(cqq_ptr u, cqq_ptr v, cqq_ptr w);

extern void
cqq_div(cqq_ptr u, cqq_ptr v, cqq_ptr w);

extern void
cqq_si_div(cqq_ptr u, long v, cqq_ptr w);

extern void
cqq_mul_2exp(cqq_ptr u, cqq_ptr v, int w);

extern void
cqq_div_2exp(cqq_ptr u, cqq_ptr v, int w);

#endif /* _cqq_h */
