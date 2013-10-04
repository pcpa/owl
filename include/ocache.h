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

#ifndef _ocache_h
#define _ocache_h

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_cache(void);

extern void
finish_cache(void);

extern void
odel_object(oobject_t *pointer);

extern oast_t *
oget_ast(oobject_t *pointer);

extern void
oget_word(oobject_t *pointer);

extern void
oget_float(oobject_t *pointer);

extern void
oget_rat(oobject_t *pointer);

extern void
oget_mpz(oobject_t *pointer);

extern void
oget_mpq(oobject_t *pointer);

extern void
oget_mpr(oobject_t *pointer);

extern void
oget_cdd(oobject_t *pointer);

extern void
oget_cqq(oobject_t *pointer);

extern void
oget_mpc(oobject_t *pointer);

#endif /* _ocache_h */
