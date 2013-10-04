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
static void
collect_ast(oast_t *ast);

static obool_t
get_number(oobject_t *pointer, otype_t type);

static void
collect_number(oobject_t object, otype_t type);

/*
 * Initialization
 */
static oast_t		*cache_ast;
static ovector_t	*cache_number;

/*
 * Implementation
 */
void
init_cache(void)
{
    oadd_root((oobject_t *)&cache_ast);
    oadd_root((oobject_t *)&cache_number);
    onew_vector((oobject_t *)&cache_number, t_void, t_mpc + 1);
}

void
finish_cache(void)
{
    orem_root((oobject_t *)&cache_ast);
    orem_root((oobject_t *)&cache_number);
}

void
odel_object(oobject_t *pointer)
{
    otype_t		type;
    oobject_t		object;

    if ((object = *pointer)) {
	switch (type = otype(object)) {
	    case t_ast:
		collect_ast(object);
		break;
	    case t_word:	case t_float:		case t_rat:
	    case t_mpz:		case t_mpq:		case t_mpr:
	    case t_cdd:		case t_cqq:		case t_mpc:
		collect_number(object, type);
		break;
	    default:
		/* add support for extra fast recycling objects here */
		object = null;
		break;
	}
	*pointer = null;
#if CACHE_DEBUG
	gc_cache_debug(object);
#endif
    }
}

oast_t *
oget_ast(oobject_t *pointer)
{
    oast_t		*next;

    if (cache_ast) {
	*pointer = cache_ast;
	next = cache_ast->next;
	cache_ast->next = null;
	cache_ast = next;
    }
    else
	onew_object(pointer, t_ast, sizeof(oast_t));
#if CACHE_DEBUG
    gc_cache_debug(null);
#endif

    return ((oast_t *)*pointer);
}

void
oget_word(oobject_t *pointer)
{
    if (!get_number(pointer, t_word))
	onew_word(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_float(oobject_t *pointer)
{
    if (!get_number(pointer, t_float))
	onew_float_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_rat(oobject_t *pointer)
{
    if (!get_number(pointer, t_rat))
	onew_rat_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_mpz(oobject_t *pointer)
{
    if (!get_number(pointer, t_mpz))
	onew_mpz_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_mpq(oobject_t *pointer)
{
    if (!get_number(pointer, t_mpq))
	onew_mpq_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_mpr(oobject_t *pointer)
{
    if (!get_number(pointer, t_mpr))
	onew_mpr_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_cdd(oobject_t *pointer)
{
    if (!get_number(pointer, t_cdd))
	onew_cdd_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_cqq(oobject_t *pointer)
{
    if (!get_number(pointer, t_cqq))
	onew_cqq_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

void
oget_mpc(oobject_t *pointer)
{
    if (!get_number(pointer, t_mpc))
	onew_mpc_w(pointer, 0);
#if CACHE_DEBUG
	gc_cache_debug(null);
#endif
}

static void
collect_ast(oast_t *ast)
{
    odel_object(&ast->l.value);
    odel_object(&ast->r.value);
    odel_object(&ast->t.value);
    odel_object(&ast->c.value);
    odel_object((oobject_t *)&ast->next);
    ast->offset = 0;
    ast->next = cache_ast;
    cache_ast = ast;
}

static obool_t
get_number(oobject_t *pointer, otype_t type)
{
    ovector_t		*vector;

    if ((vector = cache_number->v.ptr[type]) && vector->offset) {
	*pointer = vector->v.ptr[--vector->offset];
#if CACHE_DEBUG
	vector->v.ptr[vector->offset] = null;
#endif

	return (true);
    }

    return (false);
}

static void
collect_number(oobject_t object, otype_t type)
{
    ovector_t		*vector;

    if ((vector = cache_number->v.ptr[type]) == null) {
	onew_vector(cache_number->v.ptr + type, t_void, 8);
	vector = cache_number->v.ptr[type];
    }
    else if (vector->offset >= vector->length)
	orenew_vector(vector, vector->length + 8);
    vector->v.ptr[vector->offset++] = object;
}
