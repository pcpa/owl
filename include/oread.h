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

#ifndef _oread_h
#define _oread_h

#include "otypes.h"

#define omacro_p(object)		(otype(object) == t_macro)
#define oread_warn(...)			onote_warn(&input_note, __VA_ARGS__)
#define oread_error(...)		onote_error(&input_note, __VA_ARGS__)

/*
 * Types
 */
struct oinput {
    ostream_t		*stream;
    oint32_t		 lineno;
    oint32_t		 column;
};

struct omacro {
    omacro_t		*next;
    ovector_t		*vector;
    osymbol_t		*name;
    oword_t		 key;

    ohash_t		*table;			/* macro function if non null */
    obool_t		 expand		: 1;	/* expanding */
    obool_t		 vararg		: 1;	/* multiple arguments */
    obool_t		 unsafe		: 1;	/* may lead to recursion */
};

/*
 * Prototypes
 */
extern void
init_read(void);

extern void
init_read_builtin(void);

extern void
finish_read(void);

extern oobject_t
oread_object(void);

extern void
opush_input(ostream_t *stream);

extern void
opush_string(char *string);

/*
 * Externs
 */
extern oinput_t		*input;
extern onote_t		 input_note;
extern oobject_t	 object_eof;

#endif /* _oread_h */
