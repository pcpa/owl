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

#ifndef _oemit_h
#define _oemit_h

#include "otypes.h"

/*
 * Types
 */
struct ooperand {
    oint32_t		 t;
    oint32_t		 s;
    union {
	oword_t		 w;
	ofloat_t	 d;
	oobject_t	 o;
    } u;
    otag_t		*k;
};

struct ojump {
    ovector_t		*t;
    ovector_t		*f;
    otoken_t		 k;
};

/*
 * Prototypes
 */
extern void
init_emit(void);

extern void
finish_emit(void);

extern void
oemit(void);

/*
 * Externs
 */
extern oint8_t		*jit_code_exit;

#endif /* _oemit_h */
