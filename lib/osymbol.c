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

#define make_symbol(pointer, name)					\
    onew_entry_base(pointer, name, null, t_symbol, sizeof(osymbol_t))

/*
 * Prototypes
 */
static osymbol_t *
new_symbol(orecord_t *record, ovector_t *name, otag_t *tag, obool_t add);

static orecord_t *
new_record(osymbol_t *name, otype_t type);

/*
 * Initialization
 */
ovector_t		*type_vector;
orecord_t		*root_record;
static ohash_t		*language_table;

/*
 * Implementation
 */
void
init_symbol(void)
{
    oadd_root((oobject_t *)&type_vector);
    onew_vector((oobject_t *)&type_vector, t_void, 128);

    onew_object(type_vector->v.ptr + t_root,
		t_namespace, sizeof(orecord_t));
    root_record = type_vector->v.ptr[t_root];
    root_record->type = t_root;
    onew_hash((oobject_t *)&root_record->fields, 8);
    onew_vector((oobject_t *)&root_record->vector, t_void, 4);
    onew_hash((oobject_t *)&root_record->methods, 8);

    oadd_root((oobject_t *)&language_table);
    onew_hash((oobject_t *)&language_table, 256);
}

void
finish_symbol(void)
{
    orem_root((oobject_t *)&type_vector);
    orem_root((oobject_t *)&language_table);
}

osymbol_t *
oget_identifier(ovector_t *name)
{
    return ((osymbol_t *)oget_hash(language_table, name));
}

osymbol_t *
onew_identifier(ovector_t *name)
{
    osymbol_t		*symbol;
    oobject_t		*pointer;

    if ((symbol = (osymbol_t *)oget_hash(language_table, name)) == null) {
	gc_ref(pointer);
	make_symbol(pointer, name);
	symbol = *pointer;
	symbol->record = (orecord_t *)language_table;
	oput_hash(language_table, (oentry_t *)symbol);
	gc_dec();
    }

    return (symbol);
}

osymbol_t *
oget_symbol(orecord_t *record, ovector_t *name)
{
    if (record->fields)
	return ((osymbol_t *)oget_hash(record->fields, name));
    return (null);
}

osymbol_t *
oget_bound_symbol(ovector_t *name)
{
    osymbol_t		*symbol;
    orecord_t		*record;

    for (record = current_record; record; record = record->parent) {
	if ((symbol = oget_symbol(record, name)))
	    return (symbol);
    }

    return (null);
}

osymbol_t *
onew_symbol(orecord_t *record, ovector_t *name, otag_t *tag)
{
    return (new_symbol(record, name, tag, false));
}

oword_t
onew_exception(orecord_t *record)
{
    oword_t		 offset;

    assert(record == root_record || otype(record) == t_prototype);
    assert(record->function);

    if (otype(record) == t_prototype) {
#if __WORDSIZE == 32
	record->offset = record->offset & ~3;
	record->length = (record->length + 3) & ~3;
#else
	record->offset = record->offset & ~7;
	record->length = (record->length + 7) & ~7;
#endif
	record->offset -= sizeof(oexception_t);
	offset = record->offset;
	record->length += sizeof(oexception_t);
#if __WORDSIZE == 32
	assert(record->offset < 0);
#else
	assert((oint32_t)record->offset == record->offset);
#endif
    }
    else {
	assert(otype(record) == t_namespace);
#if __WORDSIZE == 32
	record->offset = (record->offset + 3) & ~3;
	assert(record->offset >= 0);
#else
	record->offset = (record->offset + 7) & ~7;
	assert((oint32_t)record->offset == record->offset);
#endif
	record->length = record->offset + sizeof(oexception_t);
	offset = record->offset;
	record->offset = record->length;
    }

    return (offset);
}

extern oint32_t
onew_slot(orecord_t *record)
{
    oword_t		offset;

    if (otype(record) == t_prototype) {
	record->offset = record->offset & ~7;
	record->length = (record->length + 7) & ~7;
	record->offset -= 16;
	record->length += 16;
#if __WORDISZE == 64
	assert((oint32_t)record->offset == record->offset);
#else
	assert(record->offset < 0);
#endif
	offset = record->offset;
    }
    else {
	assert(otype(record) == t_namespace);
	record->offset = (record->offset + 7) & ~7;
	record->length = record->offset + 16;
	offset = record->offset;
	record->offset = record->length;
#if __WORDISZE == 64
	assert((oint32_t)record->offset == record->offset);
#else
	assert(record->offset >= 0);
#endif
    }

    if (record->gcinfo == null)
	onew_vector((oobject_t *)&record->gcinfo, t_word, 4);
    else if ((record->gcinfo->offset & 3) == 0)
	orenew_vector(record->gcinfo, record->gcinfo->offset + 4);
    record->gcinfo->v.w[record->gcinfo->offset++] = offset + 8;

    return (offset);
}

obasic_t *
onew_basic(osymbol_t *name, otype_t type, oint32_t length)
{
    otag_t		*tag;
    obasic_t		*basic;

    onew(type_vector->v.ptr + type, basic);
    basic = type_vector->v.ptr[type];
    basic->name = name;
    basic->type = type;
    basic->length = length;
    type_vector->offset = type + 1;

    tag = otag_object(basic);

    name->value = basic;
    name->tag = tag;
    name->type = true;

    return (basic);
}

orecord_t *
onew_record(osymbol_t *name)
{
    return (new_record(name, t_record));
}

orecord_t *
onew_namespace(osymbol_t *name)
{
    return (new_record(name, t_namespace));
}

orecord_t *
onew_prototype(void)
{
    return (new_record(null, t_prototype));
}

void
oadd_record(orecord_t *record, orecord_t *super)
{
    oword_t		 offset;
    osymbol_t		*symbol;

    assert(otype(record) == t_record && otype(super) == t_record);
    record->super = super;
    for (offset = 0; offset < super->vector->offset; offset++) {
	symbol = super->vector->v.ptr[offset];
	new_symbol(record, symbol->name, symbol->tag, true);
    }
    record->nmethod = super->nmethod;
}

void
oend_record(orecord_t *record)
{
    ortti_t		*rtti;

    assert(rtti_vector->v.ptr[record->type] == null);
#if __WORDSIZE == 32
    if (otype(record) == t_prototype)
	record->offset &= ~3;
    record->length = (record->length + 3) & ~3;
#else
    if (otype(record) == t_prototype)
	record->offset &= ~7;
    record->length = (record->length + 7) & ~7;
#endif

    onew_object(rtti_vector->v.ptr + record->type, t_rtti, sizeof(ortti_t));
    rtti = rtti_vector->v.ptr[record->type];
    if (record->super)
	rtti->super = record->super->type;
    if (record->gcinfo) {
	rtti->gcsize = record->gcinfo->offset;
	onew_object((oobject_t *)&rtti->gcinfo, t_word,
		    rtti->gcsize * sizeof(oword_t));
	memcpy(rtti->gcinfo, record->gcinfo->v.w,
	       rtti->gcsize * sizeof(oword_t));
    }

    if (otype(record) == t_record) {
	rtti->mdsize = record->nmethod;
	onew_object((oobject_t *)&rtti->mdinfo, t_void,
		    rtti->mdsize * sizeof(oobject_t));
    }

    /* For runtime type bounds checking */
    if (rtti_vector->offset < record->type)
	rtti_vector->offset = record->type;
}

static osymbol_t *
new_symbol(orecord_t *record, ovector_t *name, otag_t *tag, obool_t add)
{
    obool_t		 gc;
    obool_t		 down;
    otype_t		 type;
    oword_t		 length;
    osymbol_t		*method;
    osymbol_t		*symbol;
    oobject_t		*pointer;

    /* caller must check with file/line/column context for error messages */
    if (add)
	method = oget_symbol(record->super, name);
    else
	method = oget_symbol(record, name);

    gc_ref(pointer);
    make_symbol(pointer, name);
    symbol = *pointer;
    symbol->tag = tag;
    symbol->record = record;
    oput_hash(record->fields, (oentry_t *)symbol);
    gc_dec();

    if (record->vector->offset >= record->vector->length)
	orenew_vector(record->vector, record->vector->length + 4);
    record->vector->v.ptr[record->vector->offset++] = symbol;

    gc = true;
    length = sizeof(oobject_t);
    if (tag) {
	switch (tag->type) {
	    case tag_basic:
		length = ((obasic_t *)tag->name)->length;
		gc = false;
		break;
	    case tag_auto:	case tag_class:
	    case tag_vector:	case tag_hash:
		break;
	    default:
		assert(tag->type == tag_function);
		/* Must have been called from oadd_record */
		if (method) {
		    assert(otype(record) == t_record);
		    symbol->value = method->value;
		    symbol->offset = method->offset;
		    symbol->function = symbol->method = true;
		}
		return (symbol);
	}
    }
    else {
	/* FIXME assert creating a builtin */
	assert(record == root_record);
	return (symbol);
    }

    type = otype(record);
    assert(type == t_record || type == t_namespace || type == t_prototype);
    if (type == t_prototype) {
	assert(record->function);
	if (ofunction_p(record->function))
	    down = record->function->local;
	else
	    down = false;
	symbol->field = false;
	symbol->argument = !down;
    }
    else {
	down = false;
	symbol->field = type == t_record;
	symbol->global = type == t_namespace;
	/* Simplify check of if return value of oget_bound_symbol */
	if (symbol->field)
	    symbol->bound = true;
    }

    switch (length) {
	case 0:
	    break;
	case 1:
	    if (down)	--record->offset;
	    ++record->length;
	    break;
	case 2:
	    if (down)	record->offset = (record->offset - 2) & ~1;
	    else	record->offset = (record->offset + 1) & ~1;
	    record->length = (record->length + 1) & ~1;
	    break;
	case 3:		case 4:
	    if (down)	record->offset = (record->offset - 4) & ~3;
	    else	record->offset = (record->offset + 3) & ~3;
	    record->length = (record->length + 3) & ~3;
	    break;
	default:
	    if (down)	record->offset = (record->offset - 8) & ~7;
	    else	record->offset = (record->offset + 7) & ~7;
	    record->length = (record->length + 7) & ~7;
	    break;
    }
    symbol->offset = record->offset;
    if (!down)
	record->offset += length;
    record->length += length;
    if (gc) {
	if (record->gcinfo == null)
	    onew_vector((oobject_t *)&record->gcinfo, t_word, 4);
	else if ((record->gcinfo->offset & 3) == 0)
	    orenew_vector(record->gcinfo, record->gcinfo->offset + 4);
	record->gcinfo->v.w[record->gcinfo->offset++] = symbol->offset;
    }

    return (symbol);
}

static orecord_t *
new_record(osymbol_t *name, otype_t type)
{
    otag_t		*tag;
    orecord_t		*record;

    if (type_vector->offset >= type_vector->length) {
	orenew_vector(type_vector, type_vector->length + 128);
	orenew_vector(rtti_vector, type_vector->length + 128);
    }
    onew_object(type_vector->v.ptr + type_vector->offset,
		type, sizeof(orecord_t));
    record = type_vector->v.ptr[type_vector->offset];
    record->name = name;
    record->type = type_vector->offset;
    ++type_vector->offset;

    onew_hash((oobject_t *)&record->fields, 8);
    record->parent = current_record;

    onew_vector((oobject_t *)&record->vector, t_void, 4);

    if (type != t_prototype) {
	/* Method number zero is reserved for constructor */
	record->nmethod = 1;
	onew_hash((oobject_t *)&record->methods, 8);
    }

    if (name) {
	tag = otag_object(record);
	name->value = record;
	name->tag = tag;
	name->type = true;
    }

    return (record);
}
