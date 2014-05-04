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
#include <stdio.h>

/*
 * Initialization
 */
ofunction_t		*current_function;
static struct {
    char		*name;
    otag_t		*tag;
    osymbol_t		*symbol;
} types[] = {
    { "auto"		},
#if __WORDSIZE == 32
    { "int32_t",	},
#else
    { "int64_t",	},
#endif
    { "float64_t",	},
    { "int8_t",		},
    { "uint8_t",	},
    { "int16_t",	},
    { "uint16_t",	},
#if __WORDSIZE == 64
    { "int32_t",	},
#endif
    { "uint32_t",	},
#if __WORDSIZE == 32
    { "int64_t",	},
#endif
    { "uint64_t",	},
    { "float32_t",	},
};

/*
 * Implementation
 */
void
init_function(void)
{
    oword_t		 offset;
    osymbol_t		*symbol;
    oobject_t		*pointer;
    ofunction_t		*function;

    gc_ref(pointer);
    onew_object(pointer, t_function, sizeof(ofunction_t));
    function = *pointer;

    function->record = root_record;

    function->record = root_record;
    root_record->function = function;
    gc_dec();
    onew_hash((oobject_t *)&function->labels, 8);

    current_function = function;

    /* setup ast node information for root function */
    onew_ast((oobject_t *)&function->ast, tok_function, null, 0, 0);
    onew_ast(&function->ast->c.value, tok_code, null, 0, 0);
    function->ast->t.value = function;

    for (offset = 0; offset < osize(types); offset++) {
	symbol = oget_identifier(oget_string((ouint8_t *)types[offset].name,
					     strlen(types[offset].name)));
	types[offset].symbol = symbol;
	types[offset].tag = symbol->tag;
    }
}

void
finish_function(void)
{
}

ofunction_t *
oget_function(orecord_t *record, ovector_t *name)
{
    osymbol_t		*symbol;

    if ((symbol = oget_symbol(record, name)))
	return ((ofunction_t *)oget_hash(record->methods, symbol));
    return (null);
}

ofunction_t *
onew_function(orecord_t *record, ovector_t *name, otag_t *tag)
{
#if DEBUG
    otype_t		 type;
#endif
    oword_t		 length;
    ovector_t		*vector;
    oobject_t		*pointer;
    ofunction_t		*function;

    assert(tag->type == tag_function);

#if DEBUG
    /* Functions (and namespaces) can only be declared at toplevel.
     * If type == t_record this should be a class method. */
    type = otype(record);
    assert(type == t_record || type == t_namespace);
#endif

    gc_ref(pointer);
    onew_object(pointer, t_function, sizeof(ofunction_t));
    function = *pointer;

    /* Prototype or class derivation may have created symbol */
    if ((function->name = oget_symbol(record, name)))
	assert(function->name->tag == tag);
    else
	function->name = onew_symbol(record, name, tag);
    function->record = onew_prototype();

    function->record->parent = record;
    function->record->function = function;

    function->tag = tag;
    oput_hash(record->methods, (oentry_t *)function);
    gc_dec();
    onew_hash((oobject_t *)&function->labels, 8);

    function->name->value = function;
    function->name->method = otype(record) != t_namespace;
    function->name->ctor = function->name->name == symbol_new->name;
    /* If an actual new method */
    if (!function->name->offset && !function->name->ctor)
	function->name->offset = record->nmethod++;
    function->name->function = true;

    /* Prototype tags are stored as strings for comparison by contents */
    vector = tag->name;
    length = vector->length / sizeof(oword_t);
    if (length && vector->v.ptr[length - 1] == varargs_tag)
	function->varargs = THIS_OFFSET;

    return (function);
}

obuiltin_t *
onew_builtin(char *name, onative_t native, otype_t type, obool_t varargs)
{
    ovector_t		*vector;
    oobject_t		*pointer;
    obuiltin_t		*builtin;

    vector = oget_string((ouint8_t *)name, strlen(name));
    assert(otype(current_record) == t_namespace);
    if (oget_symbol(current_record, vector))
	oerror("builtin '%p' already defined", vector);

    gc_ref(pointer);
    onew_object(pointer, t_builtin, sizeof(obuiltin_t));
    builtin = *pointer;

    builtin->name = onew_symbol(current_record, vector, null);
    builtin->record = onew_prototype();
    builtin->record->offset = builtin->record->length = THIS_OFFSET;
    if (varargs)
	builtin->varargs = THIS_OFFSET;
    builtin->record->parent = current_record;
    builtin->record->function = (ofunction_t *)builtin;
    builtin->native = native;
    oput_hash(current_record->methods, (oentry_t *)builtin);
    gc_dec();
    builtin->name->value = builtin;
    builtin->name->builtin = true;

    switch (type) {
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	case t_int64:		case t_uint64:
	case t_float32:		case t_float64:
	    break;
	default:
	    type = t_void;
	    break;
    }
    builtin->name->tag = types[type].tag;

    return (builtin);
}

void
onew_argument(obuiltin_t *builtin, otype_t type)
{
    oword_t		 length;
    orecord_t		*record;
    ovector_t		*vector;
    char		 string[32];

    record = builtin->record;
    length = sprintf(string, "$%d", (oint32_t)record->offset);
    vector = oget_string((ouint8_t *)string, length);
    switch (type) {
	case t_int8:		case t_uint8:
	case t_int16:		case t_uint16:
	case t_int32:		case t_uint32:
	case t_int64:		case t_uint64:
	case t_float32:		case t_float64:
	    break;
	default:
	    type = t_void;
	    break;
    }
    (void)onew_symbol(record, vector, types[type].tag);
}

void
oend_builtin(obuiltin_t *builtin)
{
    otag_t		*tag;
    ortti_t		*rtti;
    orecord_t		*record;

    record = builtin->record;
    oend_record(record);
    builtin->frame = record->length;
    if (builtin->varargs)
	builtin->varargs = record->length;
    tag = otag_builtin(builtin->name->tag, record->vector, !!builtin->varargs);
    if (builtin->varargs) {
	rtti = rtti_vector->v.ptr[record->type];
	rtti->frame = builtin->frame;
	rtti->varargs = builtin->varargs;
	rtti->address = builtin->native;
    }
    builtin->name->tag = tag;
}

void
ofunction_start_args(ofunction_t *function)
{
    oword_t		offset;

    assert(function->record->length == 0);
    offset = THIS_OFFSET;
    /* this pointer */
    if (function->record->parent &&
	otype(function->record->parent) != t_namespace)
	offset += sizeof(oobject_t);
    function->record->offset = function->record->length = offset;
}

void
ofunction_start_locs(ofunction_t *function)
{
    assert(!function->local);
#if __WORDSIZE == 32
    function->record->length = (function->record->length + 3) & -4;
    if (function->varargs)
	function->varargs = (function->record->offset + 3) & -4;
#else
    function->record->length = (function->record->length + 7) & -8;
    if (function->varargs)
	function->varargs = (function->record->offset + 7) & -8;
#endif
    function->frame = function->record->length;
    function->record->offset = 0;
    function->local = 1;
}

ofunction_t *
oget_constructor(orecord_t *record)
{
    osymbol_t		*symbol;

    symbol = oget_symbol(record, symbol_new->name);

    return ((ofunction_t *)oget_hash(record->methods, symbol));
}
