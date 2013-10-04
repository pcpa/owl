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

#ifndef _orat_h
#define _orat_h

#include "otypes.h"

#define rat_num(rat)		(rat).num
#define rat_den(rat)		(rat).den
#define rat_get_si(rat)							\
    (rat_num(rat) / rat_den(rat))
#define rat_get_d(rat)							\
    ((ofloat_t)rat_num(rat) / (ofloat_t)rat_den(rat))
#define rat_sgn(rat)		(rat_num(rat) < 0 ? -1 :		\
				 rat_num(rat) > 0 ? 1 : 0)

#define orat_num(rat)		(rat)->num
#define orat_den(rat)		(rat)->den
#define orat_get_si(rat)						\
    (orat_num(rat) / orat_den(rat))
#define orat_get_d(rat)							\
    ((ofloat_t)orat_num(rat) / (ofloat_t)orat_den(rat))
#define orat_sgn(rat)		(orat_num(rat) < 0 ? -1 :		\
				 orat_num(rat) > 0 ?  1 : 0)

/*
 * Types
 */
struct rat {
    oword_t		num;
    oword_t		den;
};

/*
 * Prototypes
 */
extern obool_t
orat_set_si(orat_t u, oword_t n, oword_t d);

extern int
orat_cmp(orat_t u, orat_t v);

extern int
orat_cmp_si(orat_t u, oword_t v);

extern obool_t
orat_add(orat_t u, orat_t v, orat_t w);

#define orat_si_add(u, v, w)	orat_add_si(u, w, v)
extern obool_t
orat_add_si(orat_t u, orat_t v, oword_t w);

extern obool_t
orat_sub(orat_t u, orat_t v, orat_t w);

extern obool_t
orat_sub_si(orat_t u, orat_t v, oword_t w);

extern obool_t
orat_si_sub(orat_t u, oword_t v, orat_t w);

extern obool_t
orat_mul(orat_t u, orat_t v, orat_t w);

#define orat_si_mul(u, v, w)	orat_mul_si(u, w, v)
extern obool_t
orat_mul_si(orat_t u, orat_t v, oword_t w);

extern obool_t
orat_div(orat_t u, orat_t v, orat_t w);

extern obool_t
orat_div_si(orat_t u, orat_t v, oword_t w);

extern obool_t
orat_si_div(orat_t u, oword_t v, orat_t w);

extern obool_t
orat_canonicalize(orat_t rat);

#endif /* _orat_h */
