/*
 * Copyright (C) 2014  Paulo Cesar Pereira de Andrade.
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

#ifndef _ogl_h
#define _ogl_h

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_gl(void);

extern void
finish_gl(void);

/*
 * Externs
 */
extern orecord_t	*gl_record;

#endif /* _ogl_h */
