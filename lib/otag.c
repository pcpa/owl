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
 * Prototypes
 */
static otag_t *
tag_opaque(otype_t type, otagtype_t tag_type);

static otag_t *
make_tag_vector(otag_t *base, oword_t length);

static otag_t *
tag_ast_data_record(otag_t *tag, oast_t *ast);

static otag_t *
tag_ast_data_vector(otag_t *tag, oast_t *ast);

static otag_t *
tag_vector_check(otag_t *base, oword_t length, oast_t *ast);

/*
 * Initialization
 */
otag_t			*word_tag;
otag_t			*float_tag;
otag_t			*auto_tag;
otag_t			*vector_tag;
otag_t			*string_tag;
otag_t			*varargs_tag;
otag_t			*hash_tag;

static ohash_t		*tag_table;
static ovector_t	*key_vector;
static ovector_t	*proto_vector;
static struct {
    char		*name;
    oint32_t		 size;
    otype_t		 type;
    otag_t		*tag;
    otag_t		*vtag;
} types[] = {
    { "void",		0,			t_void    },
#if __WORDSIZE == 32
    { "int32_t",	4,			t_int32   },
#else
    { "int64_t",	8,			t_int64   },
#endif
    { "float64_t",	8,			t_float64 },
    { "int8_t",		1,			t_int8    },
    { "uint8_t",	1,			t_uint8   },
    { "int16_t",	2,			t_int16   },
    { "uint16_t",	2,			t_uint16  },
#if __WORDSIZE == 64
    { "int32_t",	4,			t_int32   },
#endif
    { "uint32_t",	4,			t_uint32  },
#if __WORDSIZE == 32
    { "int64_t",	8,			t_int64	  },
#endif
    { "uint64_t",	8,			t_uint64  },
    { "float32_t",	4,			t_float32 },
    /* internal, used to fill type information */
    { "*rat_t*",	sizeof(oobject_t),	t_rat	  },
    { "*mpz_t*",	sizeof(oobject_t),	t_mpz	  },
    { "*mpq_t*",	sizeof(oobject_t),	t_mpq	  },
    { "*mpr_t*",	sizeof(oobject_t),	t_mpr	  },
    { "*cdd_t*",	sizeof(oobject_t),	t_cdd	  },
    { "*cqq_t*",	sizeof(oobject_t),	t_cqq	  },
    { "*mpc_t*",	sizeof(oobject_t),	t_mpc	  },
};

/*
 * Implementation
 */
void
init_tag(void)
{
    oint32_t		 offset;
    osymbol_t		*symbol;

    oadd_root((oobject_t *)&tag_table);
    onew_hash((oobject_t *)&tag_table, 128);

    /* force check by contents */
    oadd_root((oobject_t *)&key_vector);
    onew_vector((oobject_t *)&key_vector, t_uint8, sizeof(oword_t));

    /* temporary storage when generating prototype tags */
    oadd_root((oobject_t *)&proto_vector);
    onew_vector((oobject_t *)&proto_vector, t_void, 16);

    assert(type_vector->offset == t_void);
    for (offset = 0; offset < osize(types); offset++) {
	symbol = onew_identifier(oget_string((ouint8_t *)types[offset].name,
					     strlen(types[offset].name)));
	onew_basic(symbol, types[offset].type, types[offset].size);
	symbol->type = true;
    }
    /* t_root already in use */
    assert(type_vector->offset == t_root);
    ++type_vector->offset;

    string_tag = otag_vector(otag_object(type_vector->v.ptr[t_uint8]), 0);

    oadd_root((oobject_t *)&auto_tag);
    onew((oobject_t *)&auto_tag, tag);
    auto_tag->type = tag_auto;
    auto_tag->size = sizeof(oobject_t);

    symbol = onew_identifier(oget_string((ouint8_t *)"auto", 4));
    symbol->type = true;
    symbol->tag = auto_tag;

    for (offset = 0; offset < osize(types); offset++) {
	types[offset].tag = otag_object(type_vector->v.ptr[offset]);
	types[offset].vtag = otag_vector(types[offset].tag, 0);
    }
    vector_tag = otag_vector(auto_tag, 0);
    word_tag = types[t_word].tag;
    float_tag = types[t_float].tag;

    symbol = onew_identifier(oget_string((ouint8_t *)"string_t", 8));
    symbol->type = true;
    symbol->tag = types[t_uint8].vtag;

    oadd_root((oobject_t *)&varargs_tag);
    onew((oobject_t *)&varargs_tag, tag);
    varargs_tag->type = tag_varargs;
    varargs_tag->base = auto_tag;
    symbol = onew_identifier(oget_string((ouint8_t *)"...", 3));
    symbol->tag = varargs_tag;

    hash_tag = tag_opaque(t_hash, tag_hash);
    symbol = onew_identifier(oget_string((ouint8_t *)"hash_t", 6));
    symbol->type = true;
    symbol->tag = hash_tag;
}

void
finish_tag(void)
{
    orem_root((oobject_t *)&tag_table);
    orem_root((oobject_t *)&key_vector);
    orem_root((oobject_t *)&proto_vector);
    orem_root((oobject_t *)&auto_tag);
}

otag_t *
otag_object(oobject_t object)
{
    otag_t		*tag;
    otype_t		 type;
    orecord_t		*record;
    oobject_t		*pointer;

    if (object == null)
	return (null);
    switch (type = otype(object)) {
	case t_basic:		case t_record:
	    record = object;
	    if ((tag = (otag_t *)oget_hash(tag_table, record)) == null) {
		gc_ref(pointer);
		onew(pointer, tag);
		tag = *pointer;
		tag->name = record;
		tag->type = type == t_basic ? tag_basic : tag_class;
		if (record->type <= t_float32)
		    tag->size = record->length;
		else
		    tag->size = sizeof(oobject_t);
		oput_hash(tag_table, (oentry_t *)tag);
		gc_dec();
	    }
	    break;
	case t_tag:
	    tag = object;
	    break;
	case t_void ... t_mpc:
	    tag = types[type].tag;
	    break;
	default:
	    if (type & t_vector) {
		type &=  ~t_vector;
		if (type >= t_void && type < t_mpc)
		    tag = types[type].vtag;
		else
		    tag = vector_tag;
	    }
	    else
		tag = auto_tag;
	    break;
    }

    return (tag);

}

otag_t *
otag_vector(otag_t *base, oword_t length)
{
    otag_t		*tag;

    assert(length >= 0);

    tag = null;
    if (base->data == null)
	onew_hash((oobject_t *)&base->data, 8);
    else {
	*key_vector->v.w = length;
	assert(ohash_p(base->data));
	tag = (otag_t *)oget_hash(base->data, key_vector);
    }

    if (tag == null)
	tag = make_tag_vector(base, length);

    return (tag);
}

otag_t *
otag_proto(otag_t *base, oast_t *decl)
{
    oast_t		*ast;
    otag_t		*tag;
    oast_t		*list;
    oast_t		*type;
    oint32_t		 length;
    oint32_t		 offset;
    ovector_t		*vector;
    oobject_t		*pointer;

    /* remove void as single argument */
    if ((ast = decl->r.ast) && ast->token == tok_type) {
	tag = ast->l.value;
	if (tag->type == tag_basic && tag->size == 0) {
	    if (ast->next)
		oparse_error(ast, "'void' must be single argument");
	    odel_object(&decl->r.value);
	}
    }

    for (list = decl->r.ast, length = 1; list; list = list->next, length++)
	;

    /* avoid allocating an object for checking */
    if (length >= proto_vector->length)
	orenew_vector(proto_vector, (length + 15) & ~15);
    if ((vector = proto_vector->v.ptr[length]) == null) {
	onew_vector(proto_vector->v.ptr + length,
		    t_uint8, length * sizeof(oobject_t));
	vector = proto_vector->v.ptr[length];
    }

    vector->v.ptr[0] = base;
    for (list = decl->r.ast, offset = 1; list; list = list->next, offset++) {
	if (list->token == tok_ellipsis) {
	    if (list->next)
		oparse_error(list, "%A follows '...' in argument list");
	    tag = varargs_tag;
	}
	else if (list->token == tok_type)
	    tag = list->l.value;
	else {
	    if (list->token != tok_declexpr)
		oparse_error(list, "expecting declaration");
	    type = list->l.ast;
	    ast = list->r.ast;
	    assert(type->token == tok_type);
	    tag = otag_ast(type->l.value, &ast);
	}
	vector->v.ptr[offset] = tag;
    }

    if ((tag = (otag_t *)oget_hash(tag_table, vector)) == null) {
	gc_ref(pointer);
	onew(pointer, tag);
	tag = *pointer;
	onew_vector(&tag->name, t_uint8, vector->length);
	memcpy(((ovector_t *)tag->name)->v.ptr, vector->v.ptr, vector->length);
	tag->type = tag_function;
	tag->size = sizeof(oobject_t);
	tag->base = base;
	oput_hash(tag_table, (oentry_t *)tag);
	gc_dec();
    }

    return (tag);
}

otag_t *
otag_builtin(otag_t *base, ovector_t *vector, obool_t varargs)
{
    otag_t		*tag;
    ovector_t		*proto;
    oint32_t		 length;
    oint32_t		 offset;
    osymbol_t		*symbol;
    oobject_t		*pointer;

    length = vector->offset + 1 + !!varargs;
    if (length >= proto_vector->length)
	orenew_vector(proto_vector, (length + 15) & ~15);
    if ((proto = proto_vector->v.ptr[length]) == null) {
	onew_vector(proto_vector->v.ptr + length,
		    t_uint8, length * sizeof(oobject_t));
	proto = proto_vector->v.ptr[length];
    }

    proto->v.ptr[0] = base;
    for (offset = 0; offset < vector->offset; offset++) {
	symbol = vector->v.ptr[offset];
	proto->v.ptr[offset + 1] = symbol->tag;
    }
    if (varargs)
	proto->v.ptr[offset + 1] = varargs_tag;

    if ((tag = (otag_t *)oget_hash(tag_table, proto)) == null) {
	gc_ref(pointer);
	onew(pointer, tag);
	tag = *pointer;
	onew_vector(&tag->name, t_uint8, proto->length);
	memcpy(((ovector_t *)tag->name)->v.ptr, proto->v.ptr, proto->length);
	tag->type = tag_function;
	tag->size = sizeof(oobject_t);
	tag->base = base;
	oput_hash(tag_table, (oentry_t *)tag);
	gc_dec();
    }

    return (tag);
}

otag_t *
otag_ast(otag_t *tag, oast_t **list)
{
    oast_t		*ast;
    oword_t		 length;

    if ((ast = *list) == null)
	return (tag);
    for (;;) {
	switch (ast->token) {
	    case tok_vector:
		/* This is the best place to "patch" the token value,
		 * so that other code will not confuse a vector
		 * declaration with a vector access. */
		ast->token = tok_vecdcl;
	    case tok_vecdcl:
		if (ast->r.ast) {
		    if (ast->r.ast->token != tok_number ||
			otype(ast->r.ast->l.value) != t_word)
			oparse_error(ast->r.ast,
				     "vector length not an integer");
		    if ((length = *(oword_t *)ast->r.ast->l.value) <= 0)
			oparse_error(ast->r.ast,
				     "invalid vector length");
		    tag = tag_vector_check(tag, length, ast->r.ast);
		}
		else
		    tag = otag_vector(tag, 0);
		if ((ast = ast->l.value) == null) {
		    *list = null;
		    return (tag);
		}
		break;
	    case tok_call:
	    case tok_symbol:
		*list = ast;
		return (tag);
	    case tok_ellipsis:
		return (varargs_tag);
	    default:
		oparse_error(ast, "expecting expression");
	}
    }
}

otag_t *
otag_ast_data(otag_t *tag, oast_t *ast)
{
    oword_t		 length;
    ovector_t		*vector;

    if (ast->token == tok_data) {
	if (tag->type == tag_class)
	    return (tag_ast_data_record(tag, ast->l.ast));
	if (tag->type != tag_vector)
	    oparse_error(ast, "not a vector");
	return (tag_ast_data_vector(tag, ast->l.ast));
    }
    /* XXX Explicit null initializer uses tok_string */
    else if (ast->token == tok_string && ast->l.value) {
	if ((vector = ast->l.value))
	    length = vector->length;
	else
	    length = 0;
	if (tag->type == tag_auto)
	    return (tag_vector_check(types[t_uint8].tag, length, ast));
	if (tag->type != tag_vector)
	    oparse_error(ast, "not a vector");
	if (tag->base->type != tag_basic || tag->base->size != 1)
	    oparse_error(ast, "invalid initializer");
	if (tag->size && tag->size < length)
	    oparse_error(ast, "too many initializers");
	return (tag_vector_check(tag->base, length, ast));
    }

    return (tag);
}

otype_t
otag_to_type(otag_t *tag)
{
    otype_t		 eltype;
    orecord_t		*record;
    ovector_t		*vector;

    switch (tag->type) {
	case tag_auto:
	    return (t_undef);
	case tag_basic:		case tag_class:
	    record = (orecord_t *)tag->name;
	    return (record->type);
	case tag_vector:
	    eltype = otag_to_type(tag->base);
	    if (eltype & t_vector)
		return (t_vector);
	    return (t_vector | eltype);
	case tag_hash:
	    return (t_hashtable);
	default:
	    assert(tag->type == tag_function);
	    vector = tag->name;
	    return (t_function | otag_to_type(vector->v.ptr[0]));
    }
}

static otag_t *
tag_opaque(otype_t type, otagtype_t tag_type)
{
    otag_t		*tag;
    oobject_t		 object;
    oobject_t		*pointer;
    gc_enter();

    gc_ref(pointer);
    onew_word(pointer, type);
    object = *pointer;
    gc_ref(pointer);
    onew(pointer, tag);
    tag = *pointer;
    tag->name = object;
    tag->type = tag_type;
    tag->size = sizeof(oobject_t);
    oput_hash(tag_table, (oentry_t *)tag);
    gc_leave();

    return (tag);
}

static otag_t *
make_tag_vector(otag_t *base, oword_t length)
{
    otag_t		*tag;
    oword_t		 size;
    ovector_t		*vector;
    oobject_t		*pointer;

    /* Check for overflow */
    if (length == 0)
	size = 0;
    else if (base->type == tag_basic) {
	size = base->size * length;
	if (size / base->size != length)
	    return (null);
    }
    else {
	size = sizeof(oobject_t) * length;
	if (size / sizeof(oobject_t) != length)
	    return (null);
    }

    gc_ref(pointer);
    onew(pointer, tag);
    tag = *pointer;
    tag->name = base;
    tag->type = tag_vector;
    onew_vector(&tag->name, t_uint8, sizeof(oword_t));
    vector = tag->name;
    *vector->v.w = length;
    tag->size = length;
    tag->base = base;
    oput_hash(base->data, (oentry_t *)tag);
    gc_dec();

    return (tag);
}

static otag_t *
tag_ast_data_record(otag_t *tag, oast_t *ast)
{
    oast_t		*rast;
    oword_t		 index;
    oword_t		 offset;
    osymbol_t		*symbol;
    orecord_t		*record;
    ovector_t		*vector;

    assert(tag->type == tag_class);
    record = tag->name;
    vector = record->vector;
    for (offset = 0; ast; offset++) {
	if (ast->token == tok_init) {
	    rast = ast->l.ast;
	    if (rast->token != tok_symbol)
		oparse_error(rast, "not a symbol");
	    symbol = rast->l.value;
	    symbol = oget_symbol(record, symbol->name);
	    if (symbol == null)
		oparse_error(rast, "no '%p' field in '%p'",
			     rast->l.value, record->name);
	    for (index = 0; index < vector->offset; index++) {
		if (vector->v.ptr[index] == symbol)
		    break;
	    }
	    assert(index < vector->offset);
	    /* FIXME silently allow overwriting data? */
	    offset = index;
	    (void)otag_ast_data(symbol->tag, ast->r.value);
	}
	else {
	    if (offset >= vector->offset)
		oparse_error(ast, "too many initializers");
	    symbol = vector->v.ptr[offset];
	    if (!symbol->field)
		continue;
	    (void)otag_ast_data(symbol->tag, ast);
	}
	ast = ast->next;
    }

    return (tag);
}

static otag_t *
tag_ast_data_vector(otag_t *tag, oast_t *ast)
{
    oast_t		*rast;
    otag_t		*btag;
    otag_t		*rtag;
    oword_t		 length;
    oword_t		 offset;

    btag = tag->base;
    for (offset = length = 0; ast; ast = ast->next, offset++) {
	if (ast->token == tok_init) {
	    rast = ast->l.ast;
	    if (rast->token != tok_number || otype(rast->l.value) != t_word)
		oparse_error(rast, "not an integer");
	    offset = *(oword_t *)rast->l.value;
	    if (offset < 0)
		oparse_error(rast, "negative offset");
	    if (tag->size && tag->size <= offset)
		oparse_error(rast, "offset out of bounds");
	    rtag = otag_ast_data(tag->base, ast->r.value);
	}
	else {
	    rtag = otag_ast_data(tag->base, ast);
	    if (tag->size && tag->size <= offset)
		oparse_error(ast, "too many initializers");
	}
	if (rtag->type == btag->type && rtag->size > btag->size)
	    btag = rtag;
	if (length <= offset)
	    length = offset + 1;
    }
    if (!tag->size)
	tag = tag_vector_check(btag, length, ast);

    return (tag);
}

static otag_t *
tag_vector_check(otag_t *base, oword_t length, oast_t *ast)
{
    otag_t		*tag;

    if ((tag = otag_vector(base, length)) == null)
	oparse_error(ast, "offset too large");

    return (tag);
}
