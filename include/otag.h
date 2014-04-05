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

#ifndef _otag_h
#define _otag_h

#include "otypes.h"

#define otag_p(object)		(otype(object) == t_tag)

/*
 * Types
 */
enum otagtype {
    tag_auto,
    tag_basic,
    tag_class,
    tag_namespace,
    tag_vector,
    tag_function,
    tag_varargs,
    tag_hash,
};

struct otag {
    otag_t		*next;
    ohash_t		*data;
    oobject_t		 name;
    oword_t		 key;
    otagtype_t		 type;
    otag_t		*base;
    oword_t		 size;
};

/*
 * Prototypes
 */
extern void
init_tag(void);

extern void
finish_tag(void);

extern otag_t *
otag_object(oobject_t object);

extern otag_t *
otag_vector(otag_t *base, oword_t length);

extern otag_t *
otag_proto(otag_t *base, oast_t *decl);

extern otag_t *
otag_builtin(otag_t *base, ovector_t *vector, obool_t varargs);

extern otag_t *
otag_ast(otag_t *tag, oast_t **list);

extern otag_t *
otag_ast_data(otag_t *tag, oast_t *ast);

extern otype_t
otag_to_type(otag_t *tag);

/*
 * Externs
 */
extern otag_t		*word_tag;
extern otag_t		*float_tag;
extern otag_t		*auto_tag;
extern otag_t		*vector_tag;
extern otag_t		*string_tag;
extern otag_t		*varargs_tag;
extern otag_t		*hash_tag;

#endif /* _otag_h */
