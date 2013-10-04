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

typedef void (*onative_t)(oobject_t, oint32_t);

/*
 * Prototypes
 */
extern void
otype_ast(oast_t *ast);

#endif /* _otypes_h */
