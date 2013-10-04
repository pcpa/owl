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

#ifndef _oerror_h
#define _oerror_h

#include "odefs.h"

/*
 * Prototypes
 */
extern void
oerror(const char *format, ...) noreturn;

extern void
onote_error(onote_t *note, const char *format, ...) noreturn;

extern void
owarn(const char *format, ...);

extern void
onote_warn(onote_t *note, const char *format, ...);

extern void
oabort(oint8_t *code, const char *string);

extern char *
otoken_to_charp(otoken_t token);

#endif /* _oerror_h */
