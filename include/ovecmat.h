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

#ifndef _ovector_h
#define _ovector_h

#include "otypes.h"

/*
 * Prototypes
 */
extern void
init_vecmat(void);

extern void
finish_vecmat(void);

/*
 * Externs
 */
extern orecord_t	*v2f_record;
extern orecord_t	*v3f_record;
extern orecord_t	*v4f_record;
extern orecord_t	*v2d_record;
extern orecord_t	*v3d_record;
extern orecord_t	*v4d_record;
extern orecord_t	*m2f_record;
extern orecord_t	*m3f_record;
extern orecord_t	*m4f_record;
extern orecord_t	*m2d_record;
extern orecord_t	*m3d_record;
extern orecord_t	*m4d_record;

#endif /* _ovector_h */
