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

#endif /* _oeval_h */
