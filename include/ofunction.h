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

#define define_builtin0(TYPE, NAME)					\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin1(TYPE, NAME, A0)					\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin2(TYPE, NAME, A0, A1)				\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin3(TYPE, NAME, A0, A1, A2)				\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin4(TYPE, NAME, A0, A1, A2, A3)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin5(TYPE, NAME, A0, A1, A2, A3, A4)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin6(TYPE, NAME, A0, A1, A2, A3, A4, A5)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin7(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin8(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6, A7)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin9(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6, A7, A8)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	onew_argument(builtin, A8);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin10(TYPE,NAME,A0,A1,A2,A3,A4,A5,A6,A7,A8,A9)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	onew_argument(builtin, A8);					\
	onew_argument(builtin, A9);					\
	oend_builtin(builtin);						\
    } while (0)

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

    oint32_t		 framesize;	/* stack + called functions arguments */

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

extern ofunction_t *
oget_constructor(orecord_t *record);

/*
 * Externs
 */
extern ofunction_t	*current_function;

#endif /* _ofunction_h */
