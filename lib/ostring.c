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

#include "owl.h"

/*
 * Initialization
 */
static ohash_t		*string_table;

/*
 * Implementation
 */
void
init_string(void)
{
    oadd_root((oobject_t *)&string_table);
    onew_hash((oobject_t *)&string_table, 256);
}

void
finish_string(void)
{
    orem_root((oobject_t *)&string_table);
}

ovector_t *
oget_string(ouint8_t *name, oint32_t length)
{
    oentry_t		*entry;
    ovector_t		*string;
    oobject_t		*pointer;

    if ((entry = oget_hash_string(string_table,
				  (ouint8_t *)name, length)) == null) {
	gc_enter();
	gc_ref(pointer);
	onew_vector(pointer, t_uint8, length);
	string = *pointer;
	memcpy(string->v.u8, name, length);
	gc_ref(pointer);
	onew_entry(pointer, string, string);
	oput_hash(string_table, *pointer);
	gc_leave();
    }
    else
	string = entry->value;

    return (string);
}

ovector_t *
onew_string(ovector_t *string)
{
    oentry_t		*entry;
    oobject_t		*pointer;

    assert(otype(string) == t_string);
    if ((entry = oget_hash(string_table, string)) == null) {
	gc_enter();
	gc_push(string);
	gc_ref(pointer);
	onew_entry(pointer, string, string);
	oput_hash(string_table, *pointer);
	gc_leave();
    }
    else
	string = entry->value;

    return (string);
}
