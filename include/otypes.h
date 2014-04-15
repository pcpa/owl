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

#ifndef _otypes_h
#define _otypes_h

#include <lightning.h>

typedef jit_int8_t		oint8_t;
typedef jit_uint8_t		ouint8_t;
typedef jit_int16_t		oint16_t;
typedef jit_uint16_t		ouint16_t;
typedef jit_int32_t		oint32_t;
typedef jit_uint32_t		ouint32_t;
typedef jit_int64_t		oint64_t;
typedef jit_uint64_t		ouint64_t;
typedef jit_float32_t		ofloat32_t;
typedef jit_float64_t		ofloat64_t;
typedef void*			oobject_t;

#if __WORDSIZE == 32
#  define HALFSHIFT		15
#  define SGN_MASK		0x7fffffff
#  define MININT		0x80000000
#  define ohalf_t		oint16_t
#  define ouhalf_t		ouint16_t
#  define oword_t		oint32_t
#  define ouword_t		ouint32_t
#else
#  define HALFSHIFT		31
#  define SGN_MASK		0x7fffffffffffffff
#  define MININT		0x8000000000000000
#  define ohalf_t		oint32_t
#  define ouhalf_t		ouint32_t
#  define oword_t		oint64_t
#  define ouword_t		ouint64_t
#endif

typedef oint32_t		obool_t;

#define ofloat_t		ofloat64_t
#define osingle_t		ofloat32_t

typedef struct rat		rat_t;
typedef struct rat*		orat_t;
typedef __mpz_struct*		ompz_t;
typedef __mpq_struct*		ompq_t;
typedef __mpfr_struct*		ompr_t;
typedef __mpf_struct*		ompf_t;

typedef complex double		ocdd_t;
typedef __cqq_struct*		ocqq_t;
typedef __mpc_struct*		ompc_t;

#if SDL
/* sdl */
typedef struct opoint		opoint_t;
typedef struct orect		orect_t;
typedef struct ocolor		ocolor_t;
typedef struct owindow		owindow_t;
typedef struct osurface		osurface_t;
typedef struct orenderer	orenderer_t;
typedef struct otexture		otexture_t;
typedef struct ofont		ofont_t;
typedef struct oglyph		oglyph_t;
typedef struct otimer		otimer_t;
typedef struct ochunk		ochunk_t;
typedef struct omusic		omusic_t;
typedef struct oaudio		oaudio_t;
typedef struct oevent		oevent_t;
/* sdl.gl */
typedef struct ocontext		ocontext_t;
/* glu */
typedef struct onurbs		onurbs_t;
typedef struct oquadric		oquadric_t;
typedef struct otesselator	otesselator_t;
#endif

/* Virtual machine hash table */
typedef struct ohashtable	ohashtable_t;
typedef struct ohashentry	ohashentry_t;

typedef struct ortti		ortti_t;
typedef enum otype		otype_t;
typedef enum otoken		otoken_t;
typedef enum otagtype		otagtype_t;
typedef struct ovector		ovector_t;
typedef struct ostream		ostream_t;
typedef struct oinput		oinput_t;
typedef struct othread		othread_t;
typedef struct oentry		oentry_t;
typedef struct ohash		ohash_t;
typedef struct osymbol		osymbol_t;
typedef struct oformat		oformat_t;
typedef struct onote		onote_t;
typedef struct omacro		omacro_t;

typedef struct oast		oast_t;
typedef struct otag		otag_t;
typedef struct obasic		obasic_t;
typedef struct orecord		orecord_t;
typedef struct obuiltin		obuiltin_t;
typedef struct ofunction	ofunction_t;
typedef struct olabel		olabel_t;
typedef struct ocase		ocase_t;
typedef struct oregister	oregister_t;
typedef struct ooperand		ooperand_t;
typedef struct ojump		ojump_t;
typedef enum oexcept		oexcept_t;
typedef struct oexception	oexception_t;
typedef struct oframe		oframe_t;

typedef void (*onative_t)(oobject_t, oint32_t);

#endif /* _otypes_h */
