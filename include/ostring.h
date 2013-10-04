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

#ifndef _ostring_h
#define _ostring_h

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_string(void);

extern void
finish_string(void);

extern ovector_t *
oget_string(ouint8_t *name, oint32_t length);

extern ovector_t *
enew_string(ovector_t *string);

#endif /* _ostring_h */
