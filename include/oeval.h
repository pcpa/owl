/*
 * Copyright (C) 2012  Paulo Cesar Pereira de Andrade.
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

#ifndef _oeval_t
#define _oeval_t

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_eval(void);

extern void
finish_eval(void);

extern void
ofold(oast_t *ast);

extern void
ocoerce(oobject_t *pointer, otype_t type, oobject_t value);

extern void
ocopy(oobject_t *pointer, oobject_t value);

extern obool_t
ofalse_p(oobject_t object);

extern void
oeval_unary(oast_t *ast);

extern void
oeval_binary(oast_t *ast);

extern void
oeval_shift(oast_t *ast);

extern void
oeval_boolean(oast_t *ast);

extern oobject_t
oeval_integer_p(void);

extern oobject_t
oeval_rational_p(void);

extern oobject_t
oeval_float_p(void);

extern oobject_t
oeval_real_p(void);

extern oobject_t
oeval_complex_p(void);

extern oobject_t
oeval_number_p(void);

extern oobject_t
oeval_finite_p(void);

extern oobject_t
oeval_inf_p(void);

extern oobject_t
oeval_nan_p(void);

extern oobject_t
oeval_neg(void);

extern oobject_t
oeval_not(void);

extern oobject_t
oeval_com(void);

extern oobject_t
oeval_ne(void);

extern oobject_t
oeval_lt(void);

extern oobject_t
oeval_le(void);

extern oobject_t
oeval_eq(void);

extern oobject_t
oeval_ge(void);

extern oobject_t
oeval_gt(void);

extern oobject_t
oeval_and(void);

extern oobject_t
oeval_or(void);

extern oobject_t
oeval_xor(void);

extern oobject_t
oeval_mul2(oint32_t shift);

extern oobject_t
oeval_div2(oint32_t shift);

extern oobject_t
oeval_shl(oint32_t shift);

extern oobject_t
oeval_shr(oint32_t shift);

extern oobject_t
oeval_add(void);

extern oobject_t
oeval_sub(void);

extern oobject_t
oeval_mul(void);

extern oobject_t
oeval_div(void);

extern oobject_t
oeval_trunc2(void);

extern oobject_t
oeval_rem(void);

extern oobject_t
oeval_complex(void);

#endif /* _oeval_h */
