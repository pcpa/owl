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

#ifndef _ofunction_h
#define _ofunction_h

#include "otypes.h"

#define obuiltin_p(object)	(otype(object) == t_builtin)
#define ofunction_p(object)	(otype(object) == t_function)

/*
 * Types
 */
struct obuiltin {
    obuiltin_t		*next;
    orecord_t		*record;
    osymbol_t		*name;
    oword_t		 key;
    otag_t		*tag;
    onative_t		 native;
    oint32_t		 frame;
    oint32_t		 varargs;
};

struct ofunction {
    ofunction_t		*next;
    orecord_t		*record;
    osymbol_t		*name;
    oword_t		 key;
    otag_t		*tag;
    /* Generic address pointer:
     *	o jit_node_t suitable to jit_address before jit_emit
     *	o actuall function code start address after jit_emit */
    oobject_t		 address;
    oint32_t		 frame;		/* arguments size */
    /* Offset of the first varargs argument if non zero. */
    oint32_t		 varargs;

    oast_t		*ast;
    ohash_t		*labels;

    jit_node_t		*patch;		/* suitable for jit_patch */

    oint32_t		 stack;		/* locals and temporaries size */

    /* parsing local variables */
    ouint32_t		 local		: 1;
    /* parsing temp variables */
    ouint32_t		 temps		: 1;
};

/*
 * Prototypes
 */
extern void
init_function(void);

extern void
finish_function(void);

extern ofunction_t *
oget_function(orecord_t *record, ovector_t *name);

extern ofunction_t *
onew_function(orecord_t *record, ovector_t *name, otag_t *tag);

extern obuiltin_t *
onew_builtin(char *name, onative_t native, otype_t type, obool_t varargs);

extern void
onew_argument(obuiltin_t *builtin, otype_t type);

extern void
oend_builtin(obuiltin_t *builtin);

extern void
ofunction_start_args(ofunction_t *function);

extern void
ofunction_start_locs(ofunction_t *function);

/*
 * Externs
 */
extern ofunction_t	*current_function;

#endif /* _ofunction_h */
