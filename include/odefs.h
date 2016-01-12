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

#ifndef _odefs_h
#define _odefs_h

#define noreturn		__attribute__ ((noreturn))
#define printf_format(f, v)	__attribute__ ((format (printf, f, v)))
#define maybe_unused		__attribute__ ((unused))
#define unlikely(exprn)		__builtin_expect(!!(exprn), 0)
#define likely(exprn)		__builtin_expect(!!(exprn), 1)
#if (__GNUC__ >= 4)
#  define PUBLIC		__attribute__ ((visibility("default")))
#  define HIDDEN		__attribute__ ((visibility("hidden")))
#else
#  define PUBLIC		/**/
#  define HIDDEN		/**/
#endif

#define real(dd)		__real__ dd
#define imag(dd)		__imag__ dd

#define onew(pointer, type)						\
    onew_object((oobject_t *)pointer, t_##type, sizeof(o##type##_t))

#define osize(vector)		(sizeof(vector) / sizeof((vector)[0]))

#define null			((oobject_t)0)
#define eof			-1
#define false			0
#define true			1

#ifndef BUFSIZ
#define BUFSIZ			8192
#endif

/*
 * Stream defines
 */
#define S_read			(1 << 0)
#define	S_write			(1 << 1)
#define S_io			(S_read | S_write)
#define S_append		(1 << 2)
#define S_nonblock		(1 << 3)
#define S_buffered		(1 << 4)
#define S_unbuffered		(1 << 5)

#define SUSPEND_SIGNAL		SIGUSR1

#endif /* _odefs_h */
