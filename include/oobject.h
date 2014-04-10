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

#ifndef _oobject_h
#define _oobject_h

#include "otypes.h"

#define OBJECT_TYPE_OFFSET			-4

#define otype(object)	(((oint32_t *)(object))[-1])
#define gc_enter()	oint32_t	_tgci = gc_offset
#define gc_push(object)							\
    do {								\
	assert(gc_offset < osize(gc_vector));				\
	gc_vector[gc_offset++] = object;				\
    } while (0)
#define gc_ref(pointer)							\
    do {								\
	assert(gc_offset < osize(gc_vector));				\
	pointer = &gc_vector[gc_offset];				\
	gc_vector[gc_offset] = null;					\
	++gc_offset;							\
    } while (0)
#define gc_inc()							\
    do {								\
	assert(gc_offset < osize(gc_vector));				\
	++gc_offset;							\
    } while (0)
#define gc_dec()							\
    do {								\
	assert(gc_offset > 0);						\
	--gc_offset;							\
    } while (0)
#define gc_pop(object)							\
    do {								\
	assert(gc_offset > 0);						\
	object = gc_vector[--gc_offset];				\
    } while (0)
#define gc_top(object)							\
    do {								\
	assert(gc_offset > 0);						\
	object = gc_vector[gc_offset - 1];				\
    } while (0)
#define gc_set(object)							\
    do {								\
	assert(gc_offset > 0);						\
	gc_vector[gc_offset - 1] = object;				\
    } while (0)
#define gc_leave()	gc_offset = _tgci
#define onew_vector(pointer, type, length)				\
    onew_vector_base(pointer,						\
		     type, t_vector | type,				\
		     length, sizeof(ovector_t))

/*
 * Types
 */
enum otype {
    t_void,

    t_word,
    t_float,
#define t_float64		t_float

    t_int8,
    t_uint8,
    t_int16,
    t_uint16,
#if __WORDSIZE == 64
    t_int32,
#else
#  define t_half		t_int16
#  define t_int32		t_word
#endif
    t_uint32,
#if __WORDSIZE == 32
    t_int64,
#else
#  define t_half		t_int32
#  define t_int64		t_word
#endif
    t_uint64,
    t_float32,
#define t_single		t_float32

    t_rat,
    t_mpz,
    t_mpq,
    t_mpr,
    t_cdd,
    t_cqq,
    t_mpc,

#if SDL
    t_sdl,			/* sdl namespace */
    t_point,
    t_rect,
    t_color,
    t_window,
    t_renderer,
    t_surface,
    t_texture,
    t_font,
    t_glyph_metrics,
    t_timer,
    t_music,
    t_event,
#endif

    t_root,		/* root module */

    t_thread		= 0x10000000,
    t_mutex,
    t_hashtable,
    t_hashentry,
    t_rtti,

    t_stream,
    t_input,

    t_hash,
    t_entry,

    t_symbol,

    t_macro,

    t_ast,
    t_tag,
    t_case,
    t_label,

    t_operand,
    t_jump,

    t_basic,
    t_record,
    t_namespace,
    t_prototype,
    t_builtin,

    t_null,		/* static null */
    t_bool,		/* zero or one */
    t_integer,		/* bool, word or mpz */
    t_exact,		/* integer or mpq */
    t_inexact,		/* float or mpr */
    t_real,		/* exact or inexact */
    t_number,		/* real or complex */
    t_undef,

    t_vector		= 0x20000000,
    t_vector_int8	= t_vector | t_int8,
#define t_string	t_vector_uint8
    t_vector_uint8	= t_vector | t_uint8,
    t_vector_int16	= t_vector | t_int16,
    t_vector_uint16	= t_vector | t_uint16,
    t_vector_int32	= t_vector | t_int32,
    t_vector_uint32	= t_vector | t_uint32,
    t_vector_int64	= t_vector | t_int64,
    t_vector_uint64	= t_vector | t_uint64,
    t_vector_float32	= t_vector | t_float32,
    t_vector_float64	= t_vector | t_float64,

    t_function		= 0x40000000,

    /* bitmasks */
    t_spill		= 0x40000000,
    t_register		= 0x80000000,
};

struct ovector {
    union {
	oword_t		*w;
	oint8_t		*i8;
	ouint8_t	*u8;
	oint16_t	*i16;
	ouint16_t	*u16;
	oint32_t	*i32;
	ouint32_t	*u32;
	oint64_t	*i64;
	ouint64_t	*u64;
	ofloat32_t	*f32;
	ofloat64_t	*f64;
	oobject_t	*ptr;
	oobject_t	 obj;
    } v;
    oword_t		 length;
    oword_t		 offset;
    oword_t		 memory;	/* real memory in bytes length */
};

struct ortti {
    oword_t		 gcsize;	/* number of gc offsets */
    oword_t		*gcinfo;	/* gc pointer offsets */
    oword_t		 mdsize;	/* number of methods */
    oobject_t		*mdinfo;	/* method pointers */
    oint32_t		 super;		/* superclass type */
    oint32_t		 frame;		/* arguments stack size */
    oint32_t		 stack;		/* locals and temporaries stack size */
    oint32_t		 varargs;	/* varargs stack offset */
    oobject_t		 address;	/* function address */
};

/*
 * Prototypes
 */
extern void
init_object(void);

extern void
finish_object(void);

extern void
oadd_root(oobject_t *object);

extern void
orem_root(oobject_t *object);

extern void
onew_object(oobject_t *pointer, otype_t type, oword_t length);

extern void
onew_thread(oobject_t *pointer);

extern void
onew_word(oobject_t *pointer, oword_t w);

extern void
onew_float(oobject_t *pointer, ofloat_t d);

extern void
onew_float_w(oobject_t *pointer, oword_t w);

extern void
onew_rat(oobject_t *pointer, orat_t R);

extern void
onew_rat_w(oobject_t *pointer, oword_t w);

extern void
onew_mpz(oobject_t *pointer, ompz_t Z);

extern void
onew_mpz_w(oobject_t *pointer, oword_t w);

extern void
onew_mpq(oobject_t *pointer, ompq_t Q);

extern void
onew_mpq_w(oobject_t *pointer, oword_t w);

extern void
onew_mpr(oobject_t *pointer, ompr_t R);

extern void
onew_mpr_w(oobject_t *pointer, oword_t w);

extern void
onew_cdd(oobject_t *pointer, ocdd_t dd);

extern void
onew_cdd_w(oobject_t *pointer, oword_t w);

extern void
onew_cqq(oobject_t *pointer, ocqq_t QQ);

extern void
onew_cqq_w(oobject_t *pointer, oword_t w);

extern void
onew_mpc(oobject_t *pointer, ompc_t CC);

extern void
onew_mpc_w(oobject_t *pointer, oword_t w);

extern void
onew_vector_base(oobject_t *pointer,
		 otype_t type, otype_t base_type,
		 oword_t length, oword_t base_length);

extern void
orenew_vector(ovector_t *vector, oword_t length);

/*
 * Externs
 */
extern othread_t	*thread_main;
extern ovector_t	*rtti_vector;

/* simple way to protect several parser temporaries */
extern oint32_t		 gc_offset;
extern oobject_t	 gc_vector[16];

extern oint8_t		*gd;
extern ovector_t	*cv;

extern pthread_mutex_t	 othread_mutex;

#if GC_DEBUG
/* if set, call gc every at memory allocation */
extern oint32_t		 gc_debug;
#endif

#if CACHE_DEBUG
/*   Check if a referenced object is moved to recycle cache and
 * also for "leaks" missing objects that could be cached, instead
 * of getting unreferenced and collected by gc. */
extern void
gc_cache_debug(oobject_t object);

extern oint32_t		 cache_debug;
#endif

#endif /* _oobject_h */
