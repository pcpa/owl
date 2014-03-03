/*
 * Copyright (C) 2014  Paulo Cesar Pereira de Andrade.
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

#ifndef _ohashtable_h
#define _ohashtable_h

#include "otypes.h"
#include "orat.h"

#define ohashtable_p(object)		(otype(object) == t_hashtable)

/*
 * Types
 */
struct ohashentry {
    ohashentry_t	 *next;
    oint32_t		  nt;
    oint32_t		  vt;
    union {
	oword_t		  w;
	ofloat_t	  d;
	rat_t		  x;
	oobject_t	  o;
	ovector_t	 *v;
	ompz_t		  z;
	ompq_t		  q;
	ompr_t		  r;
	ocdd_t		  dd;
	union {
	    oint64_t	  re;
	    oint64_t	  im;
	} ii;
	ocqq_t		  qq;
	ompc_t		  cc;
    } nv;
    union {
	oword_t		  w;
	ofloat_t	  d;
	rat_t		  x;
	oobject_t	  o;
	ovector_t	 *v;
	ompz_t		  z;
	ompq_t		  q;
	ompr_t		  r;
	ocdd_t		  dd;
	ocqq_t		  qq;
	ompc_t		  cc;
    } vv;
    oword_t		  key;
};

struct ohashtable {
    /* ovector_t */
    ohashentry_t	**entries;
    oword_t		  size;			/* size of entries vector */
    oword_t		  count;		/* number of entries */
};

/*
 * Prototypes
 */
extern void
init_hashtable(void);

extern void
finish_hashtable(void);

extern void
okey_hashentry(ohashentry_t *entry);

extern void
oput_hashentry(ohashtable_t *hash, ohashentry_t *entry);

extern ohashentry_t *
oget_hashentry(ohashtable_t *hash, ohashentry_t *entry);

extern void
orem_hashentry(ohashtable_t *hash, ohashentry_t *entry);

#endif /* _ohashtable_t */
