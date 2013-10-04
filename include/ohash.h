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

#ifndef _ohash_h
#define _ohash_h

#include "otypes.h"

#define ohash_p(object)		(otype(object) == t_hash)

#define onew_entry(pointer, name, value)				\
    onew_entry_base(pointer, name, value, t_entry, sizeof(oentry_t))

/*
 * Types
 */
struct oentry {
    oentry_t		 *next;
    oobject_t		  value;
    oobject_t		  name;
    oword_t		  key;
};

struct ohash {
    /* ovector_t */
    oentry_t		**entries;
    oword_t		  size;			/* size of entries vector */
    oword_t		  count;		/* number of entries */
};

/*
 * Prototypes
 */
extern void
init_hash(void);

extern void
finish_hash(void);

extern void
onew_hash(oobject_t *pointer, oword_t size);

extern void
onew_entry_base(oobject_t *pointer, oobject_t name, oobject_t value,
		otype_t type, oword_t length);

extern void
oput_hash(ohash_t *hash, oentry_t *entry);

extern oentry_t *
oget_hash(ohash_t *hash, oobject_t name);

extern oentry_t *
oget_hash_string(ohash_t *hash, ouint8_t *name, oint32_t length);

extern void
orem_hash(ohash_t *hash, oentry_t *entry);

extern void
oclr_hash(ohash_t *hash);

#endif /* _ohash_t */
