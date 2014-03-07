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

#ifndef _osymbol_h
#define _osymbol_h

#include "otypes.h"

#define osymbol_p(object)	(otype(object) == t_symbol)
#define orecord_p(object)	(otype(object) == t_record)

/*
 * Types
 */
struct osymbol {
    osymbol_t		*next;
    oobject_t		 value;
    ovector_t		*name;
    oword_t		 key;

    oword_t		 offset;
    otag_t		*tag;
    orecord_t		*record;
    osymbol_t		*parent;

    ouint32_t		 type		: 1;
    ouint32_t		 keyword	: 1;
    ouint32_t		 special	: 1;
    ouint32_t		 argument	: 1;
    ouint32_t		 field		: 1;
    ouint32_t		 global		: 1;
    ouint32_t		 bound		: 1;
    ouint32_t		 except		: 1;
    ouint32_t		 ctor		: 1;
    ouint32_t		 method		: 1;
    ouint32_t		 builtin	: 1;
    ouint32_t		 function	: 1;
};

struct obasic {
    osymbol_t		*name;
    oword_t		 type;
    oword_t		 length;
};

struct orecord {
    osymbol_t		*name;
    oword_t		 type;
    oword_t		 length;

    oword_t		 offset;
    ohash_t		*fields;
    ovector_t		*vector;

    ovector_t		*gcinfo;
    orecord_t		*parent;

    /* class specific fields */
    orecord_t		*super;
    ohash_t		*methods;
    oword_t		 nmethod;

    /* frame specific fields */
    ofunction_t		*function;
};

/*
 * Prototypes
 */
extern void
init_symbol(void);

extern void
finish_symbol(void);

extern osymbol_t *
oget_identifier(ovector_t *name);

extern osymbol_t *
onew_identifier(ovector_t *name);

extern osymbol_t *
oget_symbol(orecord_t *record, ovector_t *name);

extern osymbol_t *
oget_bound_symbol(ovector_t *name);

extern osymbol_t *
onew_symbol(orecord_t *record, ovector_t *name, otag_t *tag);

extern oword_t
onew_exception(orecord_t *record);

extern oint32_t
onew_slot(orecord_t *record);

extern obasic_t *
onew_basic(osymbol_t *name, otype_t type, oint32_t length);

extern orecord_t *
onew_record(osymbol_t *name);

extern orecord_t *
onew_namespace(osymbol_t *name);

extern orecord_t *
onew_prototype(void);

extern void
oadd_record(orecord_t *record, orecord_t *super);

extern void
oend_record(orecord_t *record);

extern obool_t
osubclass_p(orecord_t *record, orecord_t *super);

/*
 * Externs
 */
extern ovector_t	*type_vector;
extern orecord_t	*root_record;

#endif /* _osymbol_h */
