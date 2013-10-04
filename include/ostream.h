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

#ifndef _ostream_h
#define _ostream_h

#include "odefs.h"
#include "otypes.h"

#define ostream_p(object)	(otype(object) == t_stream)

/*
 * Types
 */
struct ostream {
    ouint8_t		*ptr;
    oword_t		 length;
    oword_t		 offset;
    oword_t		 size;
    oint32_t		 fileno;

    oint32_t		 s_read		: 1;
    oint32_t		 s_write	: 1;
    oint32_t		 s_append	: 1;
    oint32_t		 s_nonblock	: 1;
    oint32_t		 s_buffered	: 1;
    oint32_t		 s_regular	: 1;
    oint32_t		 s_r_mode	: 1;
    oint32_t		 s_w_mode	: 1;
    /* don't wrap read buffer, must not be set when calling eseek or ewrite */
    oint32_t		 s_r_lock	: 1;

    pthread_mutex_t	mutex;
    ovector_t		*name;
};

/*
 * Prototypes
 */
extern void
init_stream(void);

extern void
finish_stream(void);

extern void
ofdopen(oobject_t *pointer, int fileno, oint32_t mode);

extern void
ofopen(oobject_t *pointer, ovector_t *path, oint32_t mode);

extern off_t
oseek(ostream_t *stream, off_t position, oint32_t whence);

extern ssize_t
oread(ostream_t *stream, void *data, size_t count);

extern ssize_t
owrite(ostream_t *stream, void *data, size_t count);

extern obool_t
oflush(ostream_t *stream);

extern oint32_t
ogetc(ostream_t *stream);

extern void
oungetc(ostream_t *stream, oint32_t ch);

extern oint32_t
oputc(ostream_t *stream, oint32_t ch);

extern oint32_t
oclose(ostream_t *stream);

extern void
ocheck_buffer(ostream_t *stream, oword_t bytes);

/* don't wrap read buffer while lock is held */
extern void
oread_lock(ostream_t *stream);

/* allow wrap of read buffer again */
extern void
oread_unlock(ostream_t *stream);

/*
 * Externs
 */
extern ostream_t	*std_input, *std_output, *std_error;

#endif /* _ostream_h */
