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

#include "owl.h"
#include "obuiltin.h"

#define sqr(s)			((s) * (s))
#define sqrf(s)			((float)(s) * (float)(s))
#define radians(deg)		((deg) * (M_PI / 180.0))
#define radiansf(deg)		((deg) * ((float)M_PI / 180.0f))
#define degrees(rad)		((rad) * (180.0 / M_PI))
#define degreesf(rad)		((rad) * (180.0f / (float)M_PI))
#ifndef min
#  define min(a, b)		((a) < (b) ? (a) : (b))
#endif
#ifndef max
#  define max(a, b)		((a) > (b) ? (a) : (b))
#endif
#define clamp(a, minV, maxV)	min(maxV, max(minV, a))

/*
 * Other than trivial operations, a good share of the code here is direct
 * translation from glm (http://glm.g-truc.net)
 */

/* NOTE vNT.mmul(v0, m0, v1) treats v0 as a transposed square matrix,
 * then, transposes back the result to a vector. */

/*
 * Prototypes
 */
static void native_v2f_set(oobject_t list, oint32_t ac);
static void v2f_fill(ofloat32_t v0[2], ofloat32_t s0);
static void native_v2f_fill(oobject_t list, oint32_t ac);
static void native_v2f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v2f_length(ofloat32_t v0[2]);
static void native_v2f_length(oobject_t list, oint32_t ac);
static ofloat32_t v2f_distance(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_distance(oobject_t list, oint32_t ac);
static ofloat32_t v2f_dot(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_dot(oobject_t list, oint32_t ac);
static void v2f_normalize(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_normalize(oobject_t list, oint32_t ac);
static void native_v2f_eq(oobject_t list, oint32_t ac);
static void native_v2f_ne(oobject_t list, oint32_t ac);
static void v2f_adds(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_adds(oobject_t list, oint32_t ac);
static void v2f_subs(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_subs(oobject_t list, oint32_t ac);
static void v2f_muls(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_muls(oobject_t list, oint32_t ac);
static void v2f_divs(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_divs(oobject_t list, oint32_t ac);
static void v2f_mods(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_mods(oobject_t list, oint32_t ac);
static void v2f_mulm(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[4]);
static void native_v2f_mulm(oobject_t list, oint32_t ac);
static void v2f_mmul(ofloat32_t v0[2], ofloat32_t v1[4], ofloat32_t v2[2]);
static void native_v2f_mmul(oobject_t list, oint32_t ac);
static void v2f_clamp(ofloat32_t v0[2], ofloat32_t v1[2],
		      ofloat32_t minVal, ofloat32_t maxVal);
static void native_v2f_clamp(oobject_t list, oint32_t ac);
static void v2f_abs(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_abs(oobject_t list, oint32_t ac);
static void v2f_floor(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_floor(oobject_t list, oint32_t ac);
static void v2f_trunc(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_trunc(oobject_t list, oint32_t ac);
static void v2f_round(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_round(oobject_t list, oint32_t ac);
static void v2f_ceil(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_ceil(oobject_t list, oint32_t ac);
static void native_v2f_neg(oobject_t list, oint32_t ac);
static void v2f_add(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_add(oobject_t list, oint32_t ac);
static void v2f_sub(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_sub(oobject_t list, oint32_t ac);
static void v2f_mul(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_mul(oobject_t list, oint32_t ac);
static void v2f_div(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_div(oobject_t list, oint32_t ac);
static void v2f_mod(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_mod(oobject_t list, oint32_t ac);
static void v2f_rotate(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_rotate(oobject_t list, oint32_t ac);
static void v2f_closestPointOnLine(ofloat32_t v0[2], ofloat32_t v1[2],
				   ofloat32_t v2[2], ofloat32_t v3[2]);
static void native_v2f_closestPointOnLine(oobject_t list, oint32_t ac);

static void native_v3f_set(oobject_t list, oint32_t ac);
static void v3f_fill(ofloat32_t v0[3], ofloat32_t s0);
static void native_v3f_fill(oobject_t list, oint32_t ac);
static void native_v3f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v3f_length(ofloat32_t v0[3]);
static void native_v3f_length(oobject_t list, oint32_t ac);
static ofloat32_t v3f_distance(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_distance(oobject_t list, oint32_t ac);
static ofloat32_t v3f_dot(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_dot(oobject_t list, oint32_t ac);
static void v3f_normalize(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_normalize(oobject_t list, oint32_t ac);
static void native_v3f_eq(oobject_t list, oint32_t ac);
static void native_v3f_ne(oobject_t list, oint32_t ac);
static void v3f_adds(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_adds(oobject_t list, oint32_t ac);
static void v3f_subs(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_subs(oobject_t list, oint32_t ac);
static void v3f_muls(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_muls(oobject_t list, oint32_t ac);
static void v3f_divs(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_divs(oobject_t list, oint32_t ac);
static void v3f_mods(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_mods(oobject_t list, oint32_t ac);
static void v3f_mulm(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[9]);
static void native_v3f_mulm(oobject_t list, oint32_t ac);
static void v3f_mmul(ofloat32_t v0[3], ofloat32_t v1[9], ofloat32_t v2[3]);
static void native_v3f_mmul(oobject_t list, oint32_t ac);
static void v3f_clamp(ofloat32_t v0[3], ofloat32_t v1[3],
		      ofloat32_t minVal, ofloat32_t maxVal);
static void native_v3f_clamp(oobject_t list, oint32_t ac);
static void v3f_cross(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[0]);
static void native_v3f_cross(oobject_t list, oint32_t ac);
static void v3f_abs(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_abs(oobject_t list, oint32_t ac);
static void v3f_floor(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_floor(oobject_t list, oint32_t ac);
static void v3f_trunc(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_trunc(oobject_t list, oint32_t ac);
static void v3f_round(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_round(oobject_t list, oint32_t ac);
static void v3f_ceil(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_ceil(oobject_t list, oint32_t ac);
static void native_v3f_neg(oobject_t list, oint32_t ac);
static void v3f_add(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_add(oobject_t list, oint32_t ac);
static void v3f_sub(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_sub(oobject_t list, oint32_t ac);
static void v3f_mul(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_mul(oobject_t list, oint32_t ac);
static void v3f_div(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_div(oobject_t list, oint32_t ac);
static void v3f_mod(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_mod(oobject_t list, oint32_t ac);
static void v3f_project(ofloat32_t v0[3], ofloat32_t obj[3],
			ofloat32_t model[16], ofloat32_t proj[16],
			oint32_t viewport[4]);
static void native_v3f_project(oobject_t list, oint32_t ac);
static void v3f_unProject(ofloat32_t v0[3], ofloat32_t win[3],
			  ofloat32_t model[16], ofloat32_t proj[16],
			  oint32_t viewport[4]);
static void native_v3f_unProject(oobject_t list, oint32_t ac);
static void v3f_m4f_mul_v3f(ofloat32_t v0[3],
			    ofloat32_t v1[16], ofloat32_t v2[3]);
static void v3f_rotate(ofloat32_t v0[3], ofloat32_t v1[3],
		       ofloat32_t s0, ofloat32_t v2[3]);
static void native_v3f_rotate(oobject_t list, oint32_t ac);
static void v3f_rotateX(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_rotateX(oobject_t list, oint32_t ac);
static void v3f_rotateY(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_rotateY(oobject_t list, oint32_t ac);
static void v3f_rotateZ(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_rotateZ(oobject_t list, oint32_t ac);
static void v3f_closestPointOnLine(ofloat32_t v0[3], ofloat32_t v1[3],
				   ofloat32_t v2[3], ofloat32_t v3[3]);
static void native_v3f_closestPointOnLine(oobject_t list, oint32_t ac);

static void native_v4f_set(oobject_t list, oint32_t ac);
static void v4f_fill(ofloat32_t v0[4], ofloat32_t s0);
static void native_v4f_fill(oobject_t list, oint32_t ac);
static void native_v4f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v4f_length(ofloat32_t v0[4]);
static void native_v4f_length(oobject_t list, oint32_t ac);
static ofloat32_t v4f_distance(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_distance(oobject_t list, oint32_t ac);
static ofloat32_t v4f_dot(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_dot(oobject_t list, oint32_t ac);
static void v4f_normalize(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_normalize(oobject_t list, oint32_t ac);
static void native_v4f_eq(oobject_t list, oint32_t ac);
static void native_v4f_ne(oobject_t list, oint32_t ac);
static void v4f_adds(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_adds(oobject_t list, oint32_t ac);
static void v4f_subs(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_subs(oobject_t list, oint32_t ac);
static void v4f_muls(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_muls(oobject_t list, oint32_t ac);
static void v4f_divs(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_divs(oobject_t list, oint32_t ac);
static void v4f_mods(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_mods(oobject_t list, oint32_t ac);
static void v4f_mulm(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[16]);
static void native_v4f_mulm(oobject_t list, oint32_t ac);
static void v4f_mmul(ofloat32_t v0[4], ofloat32_t v1[16], ofloat32_t v2[4]);
static void native_v4f_mmul(oobject_t list, oint32_t ac);
static void v4f_clamp(ofloat32_t v0[4], ofloat32_t v1[4],
		      ofloat32_t minVal, ofloat32_t maxVal);
static void native_v4f_clamp(oobject_t list, oint32_t ac);
static void v4f_abs(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_abs(oobject_t list, oint32_t ac);
static void v4f_floor(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_floor(oobject_t list, oint32_t ac);
static void v4f_trunc(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_trunc(oobject_t list, oint32_t ac);
static void v4f_round(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_round(oobject_t list, oint32_t ac);
static void v4f_ceil(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_ceil(oobject_t list, oint32_t ac);
static void native_v4f_neg(oobject_t list, oint32_t ac);
static void v4f_add(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_add(oobject_t list, oint32_t ac);
static void v4f_sub(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_sub(oobject_t list, oint32_t ac);
static void v4f_mul(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_mul(oobject_t list, oint32_t ac);
static void v4f_div(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_div(oobject_t list, oint32_t ac);
static void v4f_mod(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_mod(oobject_t list, oint32_t ac);
static void v4f_m4f_mul_v4f(ofloat32_t v0[4],
			    ofloat32_t v1[16], ofloat32_t v2[4]);
static void v4f_rotate(ofloat32_t v0[4], ofloat32_t v1[4],
		       ofloat32_t s0, ofloat32_t v2[3]);
static void native_v4f_rotate(oobject_t list, oint32_t ac);
static void native_v4f_rotate(oobject_t list, oint32_t ac);
static void v4f_rotateX(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0);
static void native_v4f_rotateX(oobject_t list, oint32_t ac);
static void v4f_rotateY(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0);
static void native_v4f_rotateY(oobject_t list, oint32_t ac);
static void v4f_rotateZ(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0);
static void native_v4f_rotateZ(oobject_t list, oint32_t ac);

static void native_v2d_set(oobject_t list, oint32_t ac);
static void v2d_fill(ofloat64_t v0[2], ofloat64_t s0);
static void native_v2d_fill(oobject_t list, oint32_t ac);
static void native_v2d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v2d_length(ofloat64_t v0[2]);
static void native_v2d_length(oobject_t list, oint32_t ac);
static ofloat64_t v2d_distance(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_distance(oobject_t list, oint32_t ac);
static ofloat64_t v2d_dot(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_dot(oobject_t list, oint32_t ac);
static void v2d_normalize(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_normalize(oobject_t list, oint32_t ac);
static void native_v2d_eq(oobject_t list, oint32_t ac);
static void native_v2d_ne(oobject_t list, oint32_t ac);
static void v2d_adds(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_adds(oobject_t list, oint32_t ac);
static void v2d_subs(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_subs(oobject_t list, oint32_t ac);
static void v2d_muls(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_muls(oobject_t list, oint32_t ac);
static void v2d_divs(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_divs(oobject_t list, oint32_t ac);
static void v2d_mods(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_mods(oobject_t list, oint32_t ac);
static void v2d_mulm(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[4]);
static void native_v2d_mulm(oobject_t list, oint32_t ac);
static void v2d_mmul(ofloat64_t v0[2], ofloat64_t v1[4], ofloat64_t v2[2]);
static void native_v2d_mmul(oobject_t list, oint32_t ac);
static void v2d_clamp(ofloat64_t v0[2], ofloat64_t v1[2],
		      ofloat64_t minVal, ofloat64_t maxVal);
static void native_v2d_clamp(oobject_t list, oint32_t ac);
static void v2d_abs(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_abs(oobject_t list, oint32_t ac);
static void v2d_floor(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_floor(oobject_t list, oint32_t ac);
static void v2d_trunc(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_trunc(oobject_t list, oint32_t ac);
static void v2d_round(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_round(oobject_t list, oint32_t ac);
static void v2d_ceil(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_ceil(oobject_t list, oint32_t ac);
static void native_v2d_neg(oobject_t list, oint32_t ac);
static void v2d_add(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_add(oobject_t list, oint32_t ac);
static void v2d_sub(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_sub(oobject_t list, oint32_t ac);
static void v2d_mul(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_mul(oobject_t list, oint32_t ac);
static void v2d_div(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_div(oobject_t list, oint32_t ac);
static void v2d_mod(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_mod(oobject_t list, oint32_t ac);
static void v2d_rotate(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_rotate(oobject_t list, oint32_t ac);
static void v2d_closestPointOnLine(ofloat64_t v0[2], ofloat64_t v1[2],
				   ofloat64_t v2[2], ofloat64_t v3[2]);
static void native_v2d_closestPointOnLine(oobject_t list, oint32_t ac);

static void native_v3d_set(oobject_t list, oint32_t ac);
static void v3d_fill(ofloat64_t v0[3], ofloat64_t s0);
static void native_v3d_fill(oobject_t list, oint32_t ac);
static void native_v3d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v3d_length(ofloat64_t v0[3]);
static void native_v3d_length(oobject_t list, oint32_t ac);
static ofloat64_t v3d_distance(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_distance(oobject_t list, oint32_t ac);
static ofloat64_t v3d_dot(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_dot(oobject_t list, oint32_t ac);
static void v3d_normalize(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_normalize(oobject_t list, oint32_t ac);
static void native_v3d_eq(oobject_t list, oint32_t ac);
static void native_v3d_ne(oobject_t list, oint32_t ac);
static void v3d_adds(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_adds(oobject_t list, oint32_t ac);
static void v3d_subs(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_subs(oobject_t list, oint32_t ac);
static void v3d_muls(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_muls(oobject_t list, oint32_t ac);
static void v3d_divs(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_divs(oobject_t list, oint32_t ac);
static void v3d_mods(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_mods(oobject_t list, oint32_t ac);
static void v3d_mulm(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[9]);
static void native_v3d_mulm(oobject_t list, oint32_t ac);
static void v3d_mmul(ofloat64_t v0[3], ofloat64_t v1[9], ofloat64_t v2[3]);
static void native_v3d_mmul(oobject_t list, oint32_t ac);
static void v3d_clamp(ofloat64_t v0[3], ofloat64_t v1[3],
		      ofloat64_t minVal, ofloat64_t maxVal);
static void native_v3d_clamp(oobject_t list, oint32_t ac);
static void v3d_cross(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[0]);
static void native_v3d_cross(oobject_t list, oint32_t ac);
static void v3d_abs(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_abs(oobject_t list, oint32_t ac);
static void v3d_floor(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_floor(oobject_t list, oint32_t ac);
static void v3d_trunc(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_trunc(oobject_t list, oint32_t ac);
static void v3d_round(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_round(oobject_t list, oint32_t ac);
static void v3d_ceil(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_ceil(oobject_t list, oint32_t ac);
static void native_v3d_neg(oobject_t list, oint32_t ac);
static void v3d_add(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_add(oobject_t list, oint32_t ac);
static void v3d_sub(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_sub(oobject_t list, oint32_t ac);
static void v3d_mul(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_mul(oobject_t list, oint32_t ac);
static void v3d_div(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_div(oobject_t list, oint32_t ac);
static void v3d_mod(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_mod(oobject_t list, oint32_t ac);
static void v3d_project(ofloat64_t v0[3], ofloat64_t obj[3],
			ofloat64_t model[16], ofloat64_t proj[16],
			oint32_t viewport[4]);
static void native_v3d_project(oobject_t list, oint32_t ac);
static void v3d_unProject(ofloat64_t v0[3], ofloat64_t win[3],
			  ofloat64_t model[16], ofloat64_t proj[16],
			  oint32_t viewport[4]);
static void native_v3d_unProject(oobject_t list, oint32_t ac);
static void v3d_m4d_mul_v3d(ofloat64_t v0[3],
			    ofloat64_t v1[16], ofloat64_t v2[3]);
static void v3d_rotate(ofloat64_t v0[3], ofloat64_t v1[3],
		       ofloat64_t s0, ofloat64_t v2[3]);
static void native_v3d_rotate(oobject_t list, oint32_t ac);
static void v3d_rotateX(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_rotateX(oobject_t list, oint32_t ac);
static void v3d_rotateY(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_rotateY(oobject_t list, oint32_t ac);
static void v3d_rotateZ(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_rotateZ(oobject_t list, oint32_t ac);
static void v3d_closestPointOnLine(ofloat64_t v0[3], ofloat64_t v1[3],
				   ofloat64_t v2[3], ofloat64_t v3[3]);
static void native_v3d_closestPointOnLine(oobject_t list, oint32_t ac);

static void native_v4d_set(oobject_t list, oint32_t ac);
static void v4d_fill(ofloat64_t v0[4], ofloat64_t s0);
static void native_v4d_fill(oobject_t list, oint32_t ac);
static void native_v4d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v4d_length(ofloat64_t v0[4]);
static void native_v4d_length(oobject_t list, oint32_t ac);
static ofloat64_t v4d_distance(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_distance(oobject_t list, oint32_t ac);
static ofloat64_t v4d_dot(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_dot(oobject_t list, oint32_t ac);
static void v4d_normalize(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_normalize(oobject_t list, oint32_t ac);
static void native_v4d_eq(oobject_t list, oint32_t ac);
static void native_v4d_ne(oobject_t list, oint32_t ac);
static void v4d_adds(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_adds(oobject_t list, oint32_t ac);
static void v4d_subs(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_subs(oobject_t list, oint32_t ac);
static void v4d_muls(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_muls(oobject_t list, oint32_t ac);
static void v4d_divs(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_divs(oobject_t list, oint32_t ac);
static void v4d_mods(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_mods(oobject_t list, oint32_t ac);
static void v4d_mulm(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[16]);
static void native_v4d_mulm(oobject_t list, oint32_t ac);
static void v4d_mmul(ofloat64_t v0[4], ofloat64_t v1[16], ofloat64_t v2[4]);
static void native_v4d_mmul(oobject_t list, oint32_t ac);
static void v4d_clamp(ofloat64_t v0[4], ofloat64_t v1[4],
		      ofloat64_t minVal, ofloat64_t maxVal);
static void native_v4d_clamp(oobject_t list, oint32_t ac);
static void v4d_abs(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_abs(oobject_t list, oint32_t ac);
static void v4d_floor(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_floor(oobject_t list, oint32_t ac);
static void v4d_trunc(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_trunc(oobject_t list, oint32_t ac);
static void v4d_round(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_round(oobject_t list, oint32_t ac);
static void v4d_ceil(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_ceil(oobject_t list, oint32_t ac);
static void native_v4d_neg(oobject_t list, oint32_t ac);
static void v4d_add(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_add(oobject_t list, oint32_t ac);
static void v4d_sub(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_sub(oobject_t list, oint32_t ac);
static void v4d_mul(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_mul(oobject_t list, oint32_t ac);
static void v4d_div(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_div(oobject_t list, oint32_t ac);
static void v4d_mod(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_mod(oobject_t list, oint32_t ac);
static void v4d_m4d_mul_v4d(ofloat64_t v0[4],
			    ofloat64_t v1[16], ofloat64_t v2[4]);
static void v4d_rotate(ofloat64_t v0[4], ofloat64_t v1[4],
		       ofloat64_t s0, ofloat64_t v2[3]);
static void native_v4d_rotate(oobject_t list, oint32_t ac);
static void v4d_rotateX(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_rotateX(oobject_t list, oint32_t ac);
static void v4d_rotateY(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_rotateY(oobject_t list, oint32_t ac);
static void v4d_rotateZ(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_rotateZ(oobject_t list, oint32_t ac);

static void native_m2f_set(oobject_t list, oint32_t ac);
static void m2f_fill(ofloat32_t v0[4], ofloat32_t s0);
static void native_m2f_fill(oobject_t list, oint32_t ac);
static void m2f_identity(ofloat32_t v0[4]);
static void native_m2f_identity(oobject_t list, oint32_t ac);
static void native_m2f_copy(oobject_t list, oint32_t ac);
static ofloat32_t m2f_det_inverse(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_m2f_det(oobject_t list, oint32_t ac);
static void native_m2f_transpose(oobject_t list, oint32_t ac);
static void native_m2f_adds(oobject_t list, oint32_t ac);
static void native_m2f_subs(oobject_t list, oint32_t ac);
static void native_m2f_muls(oobject_t list, oint32_t ac);
static void native_m2f_divs(oobject_t list, oint32_t ac);
static void native_m2f_inverse(oobject_t list, oint32_t ac);
static void native_m2f_eq(oobject_t list, oint32_t ac);
static void native_m2f_ne(oobject_t list, oint32_t ac);
static void native_m2f_neg(oobject_t list, oint32_t ac);
static void m2f_add(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_m2f_add(oobject_t list, oint32_t ac);
static void m2f_sub(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_m2f_sub(oobject_t list, oint32_t ac);
static void m2f_mul(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_m2f_mul(oobject_t list, oint32_t ac);
static void m2f_div(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_m2f_div(oobject_t list, oint32_t ac);

static void native_m3f_set(oobject_t list, oint32_t ac);
static void m3f_fill(ofloat32_t v0[9], ofloat32_t s0);
static void native_m3f_fill(oobject_t list, oint32_t ac);
static void m3f_identity(ofloat32_t v0[9]);
static void native_m3f_identity(oobject_t list, oint32_t ac);
static void native_m3f_copy(oobject_t list, oint32_t ac);
static ofloat32_t m3f_det_inverse(ofloat32_t v0[9], ofloat32_t v1[9]);
static void native_m3f_det(oobject_t list, oint32_t ac);
static void native_m3f_transpose(oobject_t list, oint32_t ac);
static void native_m3f_adds(oobject_t list, oint32_t ac);
static void native_m3f_subs(oobject_t list, oint32_t ac);
static void native_m3f_muls(oobject_t list, oint32_t ac);
static void native_m3f_divs(oobject_t list, oint32_t ac);
static void native_m3f_inverse(oobject_t list, oint32_t ac);
static void native_m3f_eq(oobject_t list, oint32_t ac);
static void native_m3f_ne(oobject_t list, oint32_t ac);
static void native_m3f_neg(oobject_t list, oint32_t ac);
static void m3f_add(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9]);
static void native_m3f_add(oobject_t list, oint32_t ac);
static void m3f_sub(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9]);
static void native_m3f_sub(oobject_t list, oint32_t ac);
static void m3f_mul(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9]);
static void native_m3f_mul(oobject_t list, oint32_t ac);
static void m3f_div(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9]);
static void native_m3f_div(oobject_t list, oint32_t ac);

static void native_m4f_set(oobject_t list, oint32_t ac);
static void m4f_fill(ofloat32_t v0[16], ofloat32_t s0);
static void native_m4f_fill(oobject_t list, oint32_t ac);
static void m4f_identity(ofloat32_t v0[16]);
static void native_m4f_identity(oobject_t list, oint32_t ac);
static void native_m4f_copy(oobject_t list, oint32_t ac);
static ofloat32_t m4f_det_inverse(ofloat32_t v0[16], ofloat32_t v1[16]);
static void native_m4f_det(oobject_t list, oint32_t ac);
static void native_m4f_transpose(oobject_t list, oint32_t ac);
static void native_m4f_adds(oobject_t list, oint32_t ac);
static void native_m4f_subs(oobject_t list, oint32_t ac);
static void native_m4f_muls(oobject_t list, oint32_t ac);
static void native_m4f_divs(oobject_t list, oint32_t ac);
static void native_m4f_inverse(oobject_t list, oint32_t ac);
static void native_m4f_eq(oobject_t list, oint32_t ac);
static void native_m4f_ne(oobject_t list, oint32_t ac);
static void native_m4f_neg(oobject_t list, oint32_t ac);
static void m4f_add(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16]);
static void native_m4f_add(oobject_t list, oint32_t ac);
static void m4f_sub(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16]);
static void native_m4f_sub(oobject_t list, oint32_t ac);
static void m4f_mul(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16]);
static void native_m4f_mul(oobject_t list, oint32_t ac);
static void m4f_div(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16]);
static void native_m4f_div(oobject_t list, oint32_t ac);
static void m4f_translate(ofloat32_t v0[16],
			  ofloat32_t v1[16], ofloat32_t v2[3]);
static void native_m4f_translate(oobject_t list, oint32_t ac);
static void m4f_rotate(ofloat32_t v0[16], ofloat32_t v1[16],
		       ofloat32_t angle, ofloat32_t v2[3]);
static void native_m4f_rotate(oobject_t list, oint32_t ac);
static void m4f_scale(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[3]);
static void native_m4f_scale(oobject_t list, oint32_t ac);
static void m4f_ortho(ofloat32_t v0[16], ofloat32_t left,
		      ofloat32_t right, ofloat32_t bottom, ofloat32_t top,
		      ofloat32_t zNear, ofloat32_t zFar);
static void native_m4f_ortho(oobject_t list, oint32_t ac);
static void m4f_frustum(ofloat32_t v0[16], ofloat32_t left,
			ofloat32_t right, ofloat32_t bottom,
			ofloat32_t top, ofloat32_t nearVal, ofloat32_t farVal);
static void native_m4f_frustum(oobject_t list, oint32_t ac);
static void m4f_perspective(ofloat32_t v0[16], ofloat32_t fovy,
			    ofloat32_t aspect, ofloat32_t zNear,
			    ofloat32_t zFar);
static void native_m4f_perspective(oobject_t list, oint32_t ac);
static void m4f_pickMatrix(ofloat32_t v0[16], ofloat32_t center[2],
			   ofloat32_t delta[2], oint32_t viewport[4]);
static void native_m4f_pickMatrix(oobject_t list, oint32_t ac);
static void m4f_lookAt(ofloat32_t v0[16], ofloat32_t eye[3],
		       ofloat32_t center[3], ofloat32_t up[3]);
static void native_m4f_lookAt(oobject_t list, oint32_t ac);

static void native_m2d_set(oobject_t list, oint32_t ac);
static void m2d_fill(ofloat64_t v0[4], ofloat64_t s0);
static void native_m2d_fill(oobject_t list, oint32_t ac);
static void m2d_identity(ofloat64_t v0[4]);
static void native_m2d_identity(oobject_t list, oint32_t ac);
static void native_m2d_copy(oobject_t list, oint32_t ac);
static ofloat64_t m2d_det_inverse(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_m2d_det(oobject_t list, oint32_t ac);
static void native_m2d_transpose(oobject_t list, oint32_t ac);
static void native_m2d_adds(oobject_t list, oint32_t ac);
static void native_m2d_subs(oobject_t list, oint32_t ac);
static void native_m2d_muls(oobject_t list, oint32_t ac);
static void native_m2d_divs(oobject_t list, oint32_t ac);
static void native_m2d_inverse(oobject_t list, oint32_t ac);
static void native_m2d_eq(oobject_t list, oint32_t ac);
static void native_m2d_ne(oobject_t list, oint32_t ac);
static void native_m2d_neg(oobject_t list, oint32_t ac);
static void m2d_add(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_m2d_add(oobject_t list, oint32_t ac);
static void m2d_sub(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_m2d_sub(oobject_t list, oint32_t ac);
static void m2d_mul(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_m2d_mul(oobject_t list, oint32_t ac);
static void m2d_div(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_m2d_div(oobject_t list, oint32_t ac);

static void native_m3d_set(oobject_t list, oint32_t ac);
static void m3d_fill(ofloat64_t v0[9], ofloat64_t s0);
static void native_m3d_fill(oobject_t list, oint32_t ac);
static void m3d_identity(ofloat64_t v0[9]);
static void native_m3d_identity(oobject_t list, oint32_t ac);
static void native_m3d_copy(oobject_t list, oint32_t ac);
static ofloat64_t m3d_det_inverse(ofloat64_t v0[9], ofloat64_t v1[9]);
static void native_m3d_det(oobject_t list, oint32_t ac);
static void native_m3d_transpose(oobject_t list, oint32_t ac);
static void native_m3d_adds(oobject_t list, oint32_t ac);
static void native_m3d_subs(oobject_t list, oint32_t ac);
static void native_m3d_muls(oobject_t list, oint32_t ac);
static void native_m3d_divs(oobject_t list, oint32_t ac);
static void native_m3d_inverse(oobject_t list, oint32_t ac);
static void native_m3d_eq(oobject_t list, oint32_t ac);
static void native_m3d_ne(oobject_t list, oint32_t ac);
static void native_m3d_neg(oobject_t list, oint32_t ac);
static void m3d_add(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9]);
static void native_m3d_add(oobject_t list, oint32_t ac);
static void m3d_sub(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9]);
static void native_m3d_sub(oobject_t list, oint32_t ac);
static void m3d_mul(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9]);
static void native_m3d_mul(oobject_t list, oint32_t ac);
static void m3d_div(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9]);
static void native_m3d_div(oobject_t list, oint32_t ac);

static void native_m4d_set(oobject_t list, oint32_t ac);
static void m4d_fill(ofloat64_t v0[16], ofloat64_t s0);
static void native_m4d_fill(oobject_t list, oint32_t ac);
static void m4d_identity(ofloat64_t v0[16]);
static void native_m4d_identity(oobject_t list, oint32_t ac);
static void native_m4d_copy(oobject_t list, oint32_t ac);
static ofloat64_t m4d_det_inverse(ofloat64_t v0[16], ofloat64_t v1[16]);
static void native_m4d_det(oobject_t list, oint32_t ac);
static void native_m4d_transpose(oobject_t list, oint32_t ac);
static void native_m4d_adds(oobject_t list, oint32_t ac);
static void native_m4d_subs(oobject_t list, oint32_t ac);
static void native_m4d_muls(oobject_t list, oint32_t ac);
static void native_m4d_divs(oobject_t list, oint32_t ac);
static void native_m4d_inverse(oobject_t list, oint32_t ac);
static void native_m4d_eq(oobject_t list, oint32_t ac);
static void native_m4d_ne(oobject_t list, oint32_t ac);
static void native_m4d_neg(oobject_t list, oint32_t ac);
static void m4d_add(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16]);
static void native_m4d_add(oobject_t list, oint32_t ac);
static void m4d_sub(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16]);
static void native_m4d_sub(oobject_t list, oint32_t ac);
static void m4d_mul(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16]);
static void native_m4d_mul(oobject_t list, oint32_t ac);
static void m4d_div(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16]);
static void native_m4d_div(oobject_t list, oint32_t ac);
static void m4d_translate(ofloat64_t v0[16],
			  ofloat64_t v1[16], ofloat64_t v2[3]);
static void native_m4d_translate(oobject_t list, oint32_t ac);
static void m4d_rotate(ofloat64_t v0[16], ofloat64_t v1[16],
		       ofloat64_t angle, ofloat64_t v2[3]);
static void native_m4d_rotate(oobject_t list, oint32_t ac);
static void m4d_scale(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[3]);
static void native_m4d_scale(oobject_t list, oint32_t ac);
static void m4d_ortho(ofloat64_t v0[16], ofloat64_t left,
		      ofloat64_t right, ofloat64_t bottom, ofloat64_t top,
		      ofloat64_t zNear, ofloat64_t zFar);
static void native_m4d_ortho(oobject_t list, oint32_t ac);
static void m4d_frustum(ofloat64_t v0[16], ofloat64_t left,
			ofloat64_t right, ofloat64_t bottom,
			ofloat64_t top, ofloat64_t nearVal, ofloat64_t farVal);
static void native_m4d_frustum(oobject_t list, oint32_t ac);
static void m4d_perspective(ofloat64_t v0[16], ofloat64_t fovy,
			    ofloat64_t aspect, ofloat64_t zNear,
			    ofloat64_t zFar);
static void native_m4d_perspective(oobject_t list, oint32_t ac);
static void m4d_pickMatrix(ofloat64_t v0[16], ofloat64_t center[2],
			   ofloat64_t delta[2], oint32_t viewport[4]);
static void native_m4d_pickMatrix(oobject_t list, oint32_t ac);
static void m4d_lookAt(ofloat64_t v0[16], ofloat64_t eye[3],
		       ofloat64_t center[3], ofloat64_t up[3]);
static void native_m4d_lookAt(oobject_t list, oint32_t ac);

/*
 * Initialization
 */
orecord_t			*v2f_record;
orecord_t			*v3f_record;
orecord_t			*v4f_record;
orecord_t			*v2d_record;
orecord_t			*v3d_record;
orecord_t			*v4d_record;
orecord_t			*m2f_record;
orecord_t			*m3f_record;
orecord_t			*m4f_record;
orecord_t			*m2d_record;
orecord_t			*m3d_record;
orecord_t			*m4d_record;
static ofloat32_t		 identityf[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};
static ofloat64_t		 identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

/*
 * Implementation
 */
void
init_vecmat(void)
{
    orecord_t			*record;
    obuiltin_t			*builtin;

    record = current_record;

#define t_vi				(t_vector|t_int32)
#define t_f				t_float32
#define t_vf				(t_vector|t_float32)
#define t_d				t_float64
#define t_vd				(t_vector|t_float64)

    current_record = v2f_record;
    define_nsbuiltin3(t_vf, v2f_, set, t_vf, t_f, t_f);
    define_nsbuiltin2(t_vf, v2f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v2f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v2f_, length, t_vf);
    define_nsbuiltin2(t_d, v2f_, distance, t_vf, t_vf);
    define_nsbuiltin2(t_d, v2f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, normalize, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v2f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v2f_, ne, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v2f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v2f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v2f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v2f_, mods, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v2f_, mulm, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, mmul, t_vf, t_vf, t_vf);
    define_nsbuiltin4(t_vf, v2f_, clamp, t_vf, t_vf, t_f, t_f);
    define_nsbuiltin2(t_vf, v2f_, abs, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, floor, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, trunc, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, round, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, ceil, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, div, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, mod, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, rotate, t_vf, t_vf, t_f);
    define_nsbuiltin4(t_vf, v2f_, closestPointOnLine, t_vf, t_vf, t_vf, t_vf);

    current_record = v3f_record;
    define_nsbuiltin4(t_vf, v3f_, set, t_vf, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, v3f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v3f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v3f_, length, t_vf);
    define_nsbuiltin2(t_d, v3f_, distance, t_vf, t_vf);
    define_nsbuiltin2(t_d, v3f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, normalize, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v3f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v3f_, ne, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, mods, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, mulm, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, mmul, t_vf, t_vf, t_vf);
    define_nsbuiltin4(t_vf, v3f_, clamp, t_vf, t_vf, t_f, t_f);
    define_nsbuiltin3(t_vf, v3f_, cross, t_vf, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, abs, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, floor, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, trunc, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, round, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, ceil, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, div, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, mod, t_vf, t_vf, t_vf);
    define_nsbuiltin5(t_vf, v3f_, project, t_vf, t_vf, t_vf, t_vf, t_vi);
    define_nsbuiltin5(t_vf, v3f_, unProject, t_vf, t_vf, t_vf, t_vf, t_vi);
    define_nsbuiltin4(t_vf, v3f_, rotate, t_vf, t_vf, t_f, t_vf);
    define_nsbuiltin3(t_vf, v3f_, rotateX, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, rotateY, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, rotateZ, t_vf, t_vf, t_f);
    define_nsbuiltin4(t_vf, v3f_, closestPointOnLine, t_vf, t_vf, t_vf, t_vf);

    current_record = v4f_record;
    define_nsbuiltin5(t_vf, v4f_, set, t_vf, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, v4f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v4f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v4f_, length, t_vf);
    define_nsbuiltin2(t_d, v4f_, distance, t_vf, t_vf);
    define_nsbuiltin2(t_d, v4f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, normalize, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v4f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, v4f_, ne, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, mods, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, mulm, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, mmul, t_vf, t_vf, t_vf);
    define_nsbuiltin4(t_vf, v4f_, clamp, t_vf, t_vf, t_f, t_f);
    define_nsbuiltin2(t_vf, v4f_, abs, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, floor, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, trunc, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, round, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, ceil, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, div, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, mod, t_vf, t_vf, t_vf);
    define_nsbuiltin4(t_vf, v4f_, rotate, t_vf, t_vf, t_f, t_vf);
    define_nsbuiltin3(t_vf, v4f_, rotateX, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, rotateY, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v4f_, rotateZ, t_vf, t_vf, t_f);

    current_record = v2d_record;
    define_nsbuiltin3(t_vd, v2d_, set, t_vd, t_d, t_d);
    define_nsbuiltin2(t_vd, v2d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v2d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, v2d_, length, t_vd);
    define_nsbuiltin2(t_d, v2d_, distance, t_vd, t_vd);
    define_nsbuiltin2(t_d, v2d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, normalize, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v2d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v2d_, ne, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v2d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v2d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v2d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v2d_, mods, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v2d_, mulm, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, mmul, t_vd, t_vd, t_vd);
    define_nsbuiltin4(t_vd, v2d_, clamp, t_vd, t_vd, t_d, t_d);
    define_nsbuiltin2(t_vd, v2d_, abs, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, floor, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, trunc, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, round, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, ceil, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, div, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, mod, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, rotate, t_vd, t_vd, t_d);
    define_nsbuiltin4(t_vd, v2d_, closestPointOnLine, t_vd, t_vd, t_vd, t_vd);

    current_record = v3d_record;
    define_nsbuiltin4(t_vd, v3d_, set, t_vd, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, v3d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v3d_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v3d_, length, t_vd);
    define_nsbuiltin2(t_d, v3d_, distance, t_vd, t_vd);
    define_nsbuiltin2(t_d, v3d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, normalize, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v3d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v3d_, ne, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, mods, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, mulm, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, mmul, t_vd, t_vd, t_vd);
    define_nsbuiltin4(t_vd, v3d_, clamp, t_vd, t_vd, t_d, t_d);
    define_nsbuiltin3(t_vd, v3d_, cross, t_vd, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, abs, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, floor, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, trunc, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, round, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, ceil, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, div, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, mod, t_vd, t_vd, t_vd);
    define_nsbuiltin5(t_vd, v3d_, project, t_vd, t_vd, t_vd, t_vd, t_vi);
    define_nsbuiltin5(t_vd, v3d_, unProject, t_vd, t_vd, t_vd, t_vd, t_vi);
    define_nsbuiltin4(t_vd, v3d_, rotate, t_vd, t_vd, t_d, t_vd);
    define_nsbuiltin3(t_vd, v3d_, rotateX, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, rotateY, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, rotateZ, t_vd, t_vd, t_d);
    define_nsbuiltin4(t_vd, v3d_, closestPointOnLine, t_vd, t_vd, t_vd, t_vd);

    current_record = v4d_record;
    define_nsbuiltin5(t_vd, v4d_, set, t_vd, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, v4d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v4d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, v4d_, length, t_vd);
    define_nsbuiltin2(t_d, v4d_, distance, t_vd, t_vd);
    define_nsbuiltin2(t_d, v4d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, normalize, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v4d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, v4d_, ne, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, mods, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, mulm, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, mmul, t_vd, t_vd, t_vd);
    define_nsbuiltin4(t_vd, v4d_, clamp, t_vd, t_vd, t_d, t_d);
    define_nsbuiltin2(t_vd, v4d_, abs, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, floor, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, trunc, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, round, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, ceil, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, div, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, mod, t_vd, t_vd, t_vd);
    define_nsbuiltin4(t_vd, v4d_, rotate, t_vd, t_vd, t_d, t_vd);
    define_nsbuiltin3(t_vd, v4d_, rotateX, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, rotateY, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v4d_, rotateZ, t_vd, t_vd, t_d);

    current_record = m2f_record;
    define_nsbuiltin5(t_vf, m2f_, set,
		      t_vf, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, m2f_, fill, t_vf, t_f);
    define_nsbuiltin1(t_vf, m2f_, identity, t_vf);
    define_nsbuiltin2(t_vf, m2f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, m2f_, det, t_vf);
    define_nsbuiltin2(t_vf, m2f_, transpose, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m2f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m2f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m2f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m2f_, inverse, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m2f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m2f_, ne, t_vf, t_vf);
    define_nsbuiltin2(t_vf, m2f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, div, t_vf, t_vf, t_vf);

    current_record = m3f_record;
    define_nsbuiltin10(t_vf, m3f_, set,
		       t_vf, t_f, t_f, t_f, t_f, t_f, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, m3f_, fill, t_vf, t_f);
    define_nsbuiltin1(t_vf, m3f_, identity, t_vf);
    define_nsbuiltin2(t_vf, m3f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, m3f_, det, t_vf);
    define_nsbuiltin2(t_vf, m3f_, transpose, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m3f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m3f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m3f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m3f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m3f_, inverse, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m3f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m3f_, ne, t_vf, t_vf);
    define_nsbuiltin2(t_vf, m3f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m3f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m3f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m3f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m3f_, div, t_vf, t_vf, t_vf);

    current_record = m4f_record;
    define_nsbuiltin17(t_vf, m4f_, set,
		       t_vf, t_f, t_f, t_f, t_f, t_f, t_f, t_f,
		       t_f,  t_f, t_f, t_f, t_f, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, m4f_, fill, t_vf, t_f);
    define_nsbuiltin1(t_vf, m4f_, identity, t_vf);
    define_nsbuiltin2(t_vf, m4f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, m4f_, det, t_vf);
    define_nsbuiltin2(t_vf, m4f_, transpose, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, adds, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m4f_, subs, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m4f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, m4f_, divs, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m4f_, inverse, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m4f_, eq, t_vf, t_vf);
    define_nsbuiltin2(t_uint8, m4f_, ne, t_vf, t_vf);
    define_nsbuiltin2(t_vf, m4f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, div, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m4f_, translate, t_vf, t_vf, t_vf);
    define_nsbuiltin4(t_vf, m4f_, rotate, t_vf, t_vf, t_f, t_vf);
    define_nsbuiltin3(t_vf, m4f_, scale, t_vf, t_vf, t_vf);
    define_nsbuiltin7(t_vf, m4f_, ortho, t_vf, t_f, t_f, t_f, t_f, t_f, t_f);
    define_nsbuiltin7(t_vf, m4f_, frustum, t_vf, t_f, t_f, t_f, t_f, t_f, t_f);
    define_nsbuiltin5(t_vf, m4f_, perspective, t_vf, t_f, t_f, t_f, t_f);
    define_nsbuiltin4(t_vf, m4f_, pickMatrix, t_vf, t_vf, t_vf, t_vi);
    define_nsbuiltin4(t_vf, m4f_, lookAt, t_vf, t_vf, t_vf, t_vf);

    current_record = m2d_record;
    define_nsbuiltin5(t_vd, m2d_, set,
		      t_vd, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, m2d_, fill, t_vd, t_d);
    define_nsbuiltin1(t_vd, m2d_, identity, t_vd);
    define_nsbuiltin2(t_vd, m2d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, m2d_, det, t_vd);
    define_nsbuiltin2(t_vd, m2d_, transpose, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m2d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m2d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m2d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m2d_, inverse, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m2d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m2d_, ne, t_vd, t_vd);
    define_nsbuiltin2(t_vd, m2d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, div, t_vd, t_vd, t_vd);

    current_record = m3d_record;
    define_nsbuiltin10(t_vd, m3d_, set,
		       t_vd, t_d, t_d, t_d, t_d, t_d, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, m3d_, fill, t_vd, t_d);
    define_nsbuiltin1(t_vd, m3d_, identity, t_vd);
    define_nsbuiltin2(t_vd, m3d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, m3d_, det, t_vd);
    define_nsbuiltin2(t_vd, m3d_, transpose, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m3d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m3d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m3d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m3d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m3d_, inverse, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m3d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m3d_, ne, t_vd, t_vd);
    define_nsbuiltin2(t_vd, m3d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m3d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m3d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m3d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m3d_, div, t_vd, t_vd, t_vd);

    current_record = m4d_record;
    define_nsbuiltin17(t_vd, m4d_, set,
		       t_vd, t_d, t_d, t_d, t_d, t_d, t_d, t_d,
		       t_d,  t_d, t_d, t_d, t_d, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, m4d_, fill, t_vd, t_d);
    define_nsbuiltin1(t_vd, m4d_, identity, t_vd);
    define_nsbuiltin2(t_vd, m4d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, m4d_, det, t_vd);
    define_nsbuiltin2(t_vd, m4d_, transpose, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, adds, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m4d_, subs, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m4d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, m4d_, divs, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m4d_, inverse, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m4d_, eq, t_vd, t_vd);
    define_nsbuiltin2(t_uint8, m4d_, ne, t_vd, t_vd);
    define_nsbuiltin2(t_vd, m4d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, div, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m4d_, translate, t_vd, t_vd, t_vd);
    define_nsbuiltin4(t_vd, m4d_, rotate, t_vd, t_vd, t_d, t_vd);
    define_nsbuiltin3(t_vd, m4d_, scale, t_vd, t_vd, t_vd);
    define_nsbuiltin7(t_vd, m4d_, ortho, t_vd, t_d, t_d, t_d, t_d, t_d, t_d);
    define_nsbuiltin7(t_vd, m4d_, frustum, t_vd, t_d, t_d, t_d, t_d, t_d, t_d);
    define_nsbuiltin5(t_vd, m4d_, perspective, t_vd, t_d, t_d, t_d, t_d);
    define_nsbuiltin4(t_vd, m4d_, pickMatrix, t_vd, t_vd, t_vd, t_vi);
    define_nsbuiltin4(t_vd, m4d_, lookAt, t_vd, t_vd, t_vd, t_vd);

    current_record = record;
#undef t_vi
#undef t_vf
#undef t_f
#undef t_vd
#undef t_d
}

void
finish_vecmat(void)
{
}

#define CHECK_VXX(A, T, L)						\
    do {								\
	CHECK_NULL(A);							\
	CHECK_TYPE(A, t_vector|T);					\
	CHECK_LENGTH(A, L);						\
    } while (0)
#define CHECK_VI4(A)			CHECK_VXX(A, t_int32, 4)
#define CHECK_VF2(A)			CHECK_VXX(A, t_float32, 2)
#define CHECK_VF3(A)			CHECK_VXX(A, t_float32, 3)
#define CHECK_VF4(A)			CHECK_VXX(A, t_float32, 4)
#define CHECK_VD2(A)			CHECK_VXX(A, t_float64, 2)
#define CHECK_VD3(A)			CHECK_VXX(A, t_float64, 3)
#define CHECK_VD4(A)			CHECK_VXX(A, t_float64, 4)
#define CHECK_MF2(A)			CHECK_VXX(A, t_float32, 4)
#define CHECK_MF3(A)			CHECK_VXX(A, t_float32, 9)
#define CHECK_MF4(A)			CHECK_VXX(A, t_float32, 16)
#define CHECK_MD2(A)			CHECK_VXX(A, t_float64, 4)
#define CHECK_MD3(A)			CHECK_VXX(A, t_float64, 9)
#define CHECK_MD4(A)			CHECK_VXX(A, t_float64, 16)
#define A2(M, R, C)			M[(R) * 2 + (C)]
#define A3(M, R, C)			M[(R) * 3 + (C)]
#define A4(M, R, C)			M[(R) * 4 + (C)]

static void
native_v2f_set(oobject_t list, oint32_t ac)
/* float32_t v2f.set(float32_t v0[2], float32_t s0, float32_t s1)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_t		*alist;

    alist = (nat_vec_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    v0 = alist->a0->v.f32;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_fill(ofloat32_t v0[2], ofloat32_t s0)
{
    v0[0] = v0[1] = s0;
}

static void
native_v2f_fill(oobject_t list, oint32_t ac)
/* float32_t v2f.fill(float32_t v0[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    v2f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v2f_copy(oobject_t list, oint32_t ac)
/* float32_t v2f.copy(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF2(alist->a1);
	v1 = alist->a1->v.f32;
	v0[0] = v1[0];
	v0[1] = v1[1];
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
v2f_length(ofloat32_t v0[2])
{
    return (sqrtf(sqrf(v0[0]) + sqrf(v0[1])));
}

static void
native_v2f_length(oobject_t list, oint32_t ac)
/* float64_t v2f.length(float32_t v0[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF2(alist->a0);
    r0->v.d = v2f_length(alist->a0->v.f32);
}

static ofloat32_t
v2f_distance(ofloat32_t v0[2], ofloat32_t v1[2])
{
    ofloat32_t		v[2];
    v2f_sub(v, v0, v1);
    return (v2f_length(v));
}

static void
native_v2f_distance(oobject_t list, oint32_t ac)
/* float64_t v2f.distance(float32_t v0[2], float32_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    r0->v.d = v2f_distance(alist->a0->v.f32, alist->a1->v.f32);
}

static ofloat32_t
v2f_dot(ofloat32_t v0[2], ofloat32_t v1[2])
{
    return (v0[0] * v1[0] + v0[1] * v1[1]);
}

static void
native_v2f_dot(oobject_t list, oint32_t ac)
/* float64_t v2f.dot(float32_t v0[2], float32_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    r0->v.d = v2f_dot(alist->a0->v.f32, alist->a1->v.f32);
}

static void
v2f_normalize(ofloat32_t v0[2], ofloat32_t v1[2])
{
    ofloat32_t		s0;

    if (!v1[0] && !v1[1]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	}
    }
    else {
	s0 = 1.0f / v2f_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
    }
}

static void
native_v2f_normalize(oobject_t list, oint32_t ac)
/* float32_t v2f.normalize(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_normalize(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v2f_eq(oobject_t list, oint32_t ac)
/* uint8_t v2f.eq(float32_t v0[2], float32_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF2(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1];
    }
    else
	r0->v.w = true;
}

static void
native_v2f_ne(oobject_t list, oint32_t ac)
/* uint8_t v2f.ne(float32_t v0[2], float32_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF2(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1];
    }
    else
	r0->v.w = false;
}

static void
v2f_adds(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
}

static void
native_v2f_adds(oobject_t list, oint32_t ac)
/* float32_t v2f.adds(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_adds(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_subs(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
}

static void
native_v2f_subs(oobject_t list, oint32_t ac)
/* float32_t v2f.subs(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_subs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_muls(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
}

static void
native_v2f_muls(oobject_t list, oint32_t ac)
/* float32_t v2f.muls(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_muls(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_divs(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
}

static void
native_v2f_divs(oobject_t list, oint32_t ac)
/* float32_t v2f.divs(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_divs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_mods(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    v0[0] = fmodf(v1[0], s0);
    v0[1] = fmodf(v1[1], s0);
}

static void
native_v2f_mods(oobject_t list, oint32_t ac)
/* float32_t v2f.mods(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_mods(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_mulm(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[4])
{
    ofloat32_t		a0, a1;

    a0 = v1[0];		a1 = v1[1];
    v0[0] = a0 * A2(v2, 0, 0) + a1 * A2(v2, 1, 0);
    v0[1] = a0 * A2(v2, 0, 1) + a1 * A2(v2, 1, 1);
}

static void
native_v2f_mulm(oobject_t list, oint32_t ac)
/* float32_t v2f.mulm(float32_t v0[2], float32_t v1[2],
		      float32_t v2[4])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_MF2(alist->a2);
    v2f_mulm(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_mmul(ofloat32_t v0[2], ofloat32_t v1[4], ofloat32_t v2[2])
{
    ofloat32_t		a0, a1;

    a0 = v2[0];		a1 = v2[1];
    v0[0] = A2(v1, 0, 0) * a0 + A2(v1, 0, 1) * a1;
    v0[1] = A2(v1, 1, 0) * a0 + A2(v1, 1, 1) * a1;
}

static void
native_v2f_mmul(oobject_t list, oint32_t ac)
/* float32_t v2f.mmul(float32_t v0[2], float32_t v1[4],
		      float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_MF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_mmul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_clamp(ofloat32_t v0[2], ofloat32_t v1[2],
	  ofloat32_t minVal, ofloat32_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
}

static void
native_v2f_clamp(oobject_t list, oint32_t ac)
/* float32_t v2f.clamp(float32_t v0[2], float32_t v1[2],
		       float32_t minVal, float32_t maxVal)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_f32_t	*alist;

    alist = (nat_vec_vec_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_clamp(alist->a0->v.f32, alist->a1->v.f32, alist->a2, alist->a3);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_abs(ofloat32_t v0[2], ofloat32_t v1[2])
{
    v0[0] = fabsf(v1[0]);
    v0[1] = fabsf(v1[1]);
}

static void
native_v2f_abs(oobject_t list, oint32_t ac)
/* float32_t v2f.abs(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_abs(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_floor(ofloat32_t v0[2], ofloat32_t v1[2])
{
    v0[0] = floorf(v1[0]);
    v0[1] = floorf(v1[1]);
}

static void
native_v2f_floor(oobject_t list, oint32_t ac)
/* float32_t v2f.floor(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_floor(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_trunc(ofloat32_t v0[2], ofloat32_t v1[2])
{
    v0[0] = truncf(v1[0]);
    v0[1] = truncf(v1[1]);
}

static void
native_v2f_trunc(oobject_t list, oint32_t ac)
/* float32_t v2f.trunc(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_trunc(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_round(ofloat32_t v0[2], ofloat32_t v1[2])
{
    v0[0] = roundf(v1[0]);
    v0[1] = roundf(v1[1]);
}

static void
native_v2f_round(oobject_t list, oint32_t ac)
/* float32_t v2f.round(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_round(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2f_ceil(ofloat32_t v0[2], ofloat32_t v1[2])
{
    v0[0] = ceilf(v1[0]);
    v0[1] = ceilf(v1[1]);
}

static void
native_v2f_ceil(oobject_t list, oint32_t ac)
/* float32_t v2f.ceil(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_ceil(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v2f_neg(oobject_t list, oint32_t ac)
/* float32_t v2f.neg(float32_t v0[2], float32_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF2(alist->a1);
    v1 = alist->a1->v.f32;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_add(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
}

static void
native_v2f_add(oobject_t list, oint32_t ac)
/* float32_t v2f.add(float32_t v0[2], float32_t v1[2], float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_add(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_sub(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
}

static void
native_v2f_sub(oobject_t list, oint32_t ac)
/* float32_t v2f.sub(float32_t v0[2], float32_t v1[2], float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_sub(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_mul(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
}

static void
native_v2f_mul(oobject_t list, oint32_t ac)
/* float32_t v2f.mul(float32_t v0[2], float32_t v1[2], float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_mul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_div(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
}

static void
native_v2f_div(oobject_t list, oint32_t ac)
/* float32_t v2f.div(float32_t v0[2], float32_t v1[2], float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_div(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_mod(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2])
{
    v0[0] = fmodf(v1[0], v2[0]);
    v0[1] = fmodf(v1[1], v2[1]);
}

static void
native_v2f_mod(oobject_t list, oint32_t ac)
/* float32_t v2f.mod(float32_t v0[2], float32_t v1[2], float32_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    v2f_mod(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_rotate(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0)
{
    float	_cos = cosf(s0);
    float	_sin = sinf(s0);
    float	x = v1[0] * _cos - v1[1] * _sin;
    float	y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
}

static void
native_v2f_rotate(oobject_t list, oint32_t ac)
/* float32_t v2f.rotate(float32_t v0[2], float32_t v1[2], float32_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    v2f_rotate(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v2f_closestPointOnLine(ofloat32_t v0[2], ofloat32_t v1[2],
		       ofloat32_t v2[2], ofloat32_t v3[2])
{
    float	line_length;
    float	vector[2];
    float	line_direction[2];
    float	distance;
    line_length = v2f_distance(v2, v3);
    v2f_sub(vector, v1, v2);
    v2f_sub(line_direction, v3, v2);
    v2f_divs(line_direction, line_direction, line_length);
    distance = v2f_dot(vector, line_direction);
    if (distance <= 0) {
	v0[0] = v2[0];
	v0[1] = v2[1];
    }
    else if (distance >= line_length) {
	v0[0] = v3[0];
	v0[1] = v3[1];
    }
    else {
	v2f_muls(line_direction, line_direction, distance);
	v2f_add(v0, v2, line_direction);
    }
}

static void
native_v2f_closestPointOnLine(oobject_t list, oint32_t ac)
/* float32_t v2f.closestPointOnLine(float32_t res[2], float32_t point[2],
				    float32_t a[2], float32_t b[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF2(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    CHECK_VF2(alist->a3);
    v2f_closestPointOnLine(alist->a0->v.f32, alist->a1->v.f32,
			   alist->a2->v.f32, alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v3f_set(oobject_t list, oint32_t ac)
/* float32_t v3f.set(float32_t v0[3], float32_t s0,
		     float32_t s1, float32_t s2)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    v0 = alist->a0->v.f32;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_fill(ofloat32_t v0[3], ofloat32_t s0)
{
    v0[0] = v0[1] = v0[2] = s0;
}

static void
native_v3f_fill(oobject_t list, oint32_t ac)
/* float32_t v3f.fill(float32_t v0[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    v3f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v3f_copy(oobject_t list, oint32_t ac)
/* float32_t v3f.copy(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF3(alist->a1);
	v1 = alist->a1->v.f32;
	v0[0] = v1[0];
	v0[1] = v1[1];
	v0[2] = v1[2];
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
v3f_length(ofloat32_t v0[3])
{
    return (sqrtf(sqrf(v0[0]) + sqrf(v0[1]) + sqrf(v0[2])));
}

static void
native_v3f_length(oobject_t list, oint32_t ac)
/* float64_t v3f.length(float32_t v0[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF3(alist->a0);
    r0->v.d = v3f_length(alist->a0->v.f32);
}

static ofloat32_t
v3f_distance(ofloat32_t v0[3], ofloat32_t v1[3])
{
    ofloat32_t		v[3];
    v3f_sub(v, v0, v1);
    return (v3f_length(v));
}

static void
native_v3f_distance(oobject_t list, oint32_t ac)
/* float64_t v3f.distance(float32_t v0[3], float32_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    r0->v.d = v3f_distance(alist->a0->v.f32, alist->a1->v.f32);
}

static ofloat32_t
v3f_dot(ofloat32_t v0[3], ofloat32_t v1[3])
{
    return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]);
}

static void
native_v3f_dot(oobject_t list, oint32_t ac)
/* float64_t v3f.dot(float32_t v0[3], float32_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    r0->v.d = v3f_dot(alist->a0->v.f32, alist->a1->v.f32);
}

static void
v3f_normalize(ofloat32_t v0[3], ofloat32_t v1[3])
{
    ofloat32_t		s0;

    if (!v1[0] && !v1[1] && !v1[2]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	    v0[2] = v1[2];
	}
    }
    else {
	s0 = 1.0f / v3f_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
	v0[2] = v1[2] * s0;
    }
}

static void
native_v3f_normalize(oobject_t list, oint32_t ac)
/* float32_t v3f.normalize(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_normalize(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v3f_eq(oobject_t list, oint32_t ac)
/* uint8_t v3f.eq(float32_t v0[3], float32_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF3(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] && v0[2] != v1[2];
    }
    else
	r0->v.w = true;
}

static void
native_v3f_ne(oobject_t list, oint32_t ac)
/* uint8_t v3f.ne(float32_t v0[3], float32_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF3(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] || v0[2] != v1[2];
    }
    else
	r0->v.w = false;
}

static void
v3f_adds(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
    v0[2] = v1[2] + s0;
}

static void
native_v3f_adds(oobject_t list, oint32_t ac)
/* float32_t v3f.adds(float32_t v0[3], float32_t v1[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_adds(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_subs(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
    v0[2] = v1[2] - s0;
}

static void
native_v3f_subs(oobject_t list, oint32_t ac)
/* float32_t v3f.subs(float32_t v0[3], float32_t v1[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_subs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_muls(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
    v0[2] = v1[2] * s0;
}

static void
native_v3f_muls(oobject_t list, oint32_t ac)
/* float32_t v3f.muls(float32_t v0[3], float32_t v1[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_muls(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_divs(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
    v0[2] = v1[2] / s0;
}

static void
native_v3f_divs(oobject_t list, oint32_t ac)
/* float32_t v3f.divs(float32_t v0[3], float32_t v1[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_divs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_mods(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    v0[0] = fmodf(v1[0], s0);
    v0[1] = fmodf(v1[1], s0);
    v0[2] = fmodf(v1[2], s0);
}

static void
native_v3f_mods(oobject_t list, oint32_t ac)
/* float32_t v3f.mods(float32_t v0[3], float32_t v1[3], float32_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_mods(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_mulm(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[9])
{
    ofloat32_t		a0, a1, a2;

    a0 = v1[0];		a1 = v1[1];	a2 = v1[2];
    v0[0] = a0 * A3(v2, 0, 0) + a1 * A3(v2, 1, 0) + a2 * A3(v2, 2, 0);
    v0[1] = a0 * A3(v2, 0, 1) + a1 * A3(v2, 1, 1) + a2 * A3(v2, 2, 1);
    v0[2] = a0 * A3(v2, 0, 2) + a1 * A3(v2, 1, 2) + a2 * A3(v2, 2, 2);
}

static void
native_v3f_mulm(oobject_t list, oint32_t ac)
/* float32_t v3f.mulm(float32_t v0[3], float32_t v1[3],
		      float32_t v2[9])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_MF3(alist->a2);
    v3f_mulm(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_mmul(ofloat32_t v0[3], ofloat32_t v1[9], ofloat32_t v2[3])
{
    ofloat32_t		a0, a1, a2;

    a0 = v2[0];		a1 = v2[1];	a2 = v2[2];
    v0[0] = A3(v1, 0, 0) * a0 + A3(v1, 0, 1) * a1 + A3(v1, 0, 2) * a2;
    v0[1] = A3(v1, 1, 0) * a0 + A3(v1, 1, 1) * a1 + A3(v1, 1, 2) * a2;
    v0[2] = A3(v1, 2, 0) * a0 + A3(v1, 2, 1) * a1 + A3(v1, 2, 2) * a2;
}

static void
native_v3f_mmul(oobject_t list, oint32_t ac)
/* float32_t v3f.mmul(float32_t v0[3], float32_t v1[9],
		      float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_MF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_mmul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_clamp(ofloat32_t v0[3], ofloat32_t v1[3],
	  ofloat32_t minVal, ofloat32_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
    v0[2] = clamp(minVal, maxVal, v1[2]);
}

static void
native_v3f_clamp(oobject_t list, oint32_t ac)
/* float32_t v3f.clamp(float32_t v0[3], float32_t v1[3],
		       float32_t minVal, float32_t maxVal)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_f32_t	*alist;

    alist = (nat_vec_vec_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_clamp(alist->a0->v.f32, alist->a1->v.f32, alist->a2, alist->a3);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_cross(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    ofloat32_t		*vn, v[9];

    if (likely(v0 != v1 && v0 != v2))
	vn = v0;
    else
	vn = v;
    vn[0] = v1[1] * v2[2] - v1[2] * v2[1];
    vn[1] = v1[2] * v2[0] - v1[0] * v2[2];
    vn[2] = v1[0] * v2[1] - v1[1] * v2[0];
    if (unlikely(vn != v0)) {
	v0[0] = vn[0];
	v0[1] = vn[1];
	v0[2] = vn[2];
    }
}

static void
native_v3f_cross(oobject_t list, oint32_t ac)
/* float32_t v3f.cross(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_cross(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_abs(ofloat32_t v0[3], ofloat32_t v1[3])
{
    v0[0] = fabsf(v1[0]);
    v0[1] = fabsf(v1[1]);
    v0[2] = fabsf(v1[2]);
}

static void
native_v3f_abs(oobject_t list, oint32_t ac)
/* float32_t v3f.abs(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_abs(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_floor(ofloat32_t v0[3], ofloat32_t v1[3])
{
    v0[0] = floorf(v1[0]);
    v0[1] = floorf(v1[1]);
    v0[2] = floorf(v1[2]);
}

static void
native_v3f_floor(oobject_t list, oint32_t ac)
/* float32_t v3f.floor(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_floor(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_trunc(ofloat32_t v0[3], ofloat32_t v1[3])
{
    v0[0] = truncf(v1[0]);
    v0[1] = truncf(v1[1]);
    v0[2] = truncf(v1[2]);
}

static void
native_v3f_trunc(oobject_t list, oint32_t ac)
/* float32_t v3f.trunc(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_trunc(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_round(ofloat32_t v0[3], ofloat32_t v1[3])
{
    v0[0] = roundf(v1[0]);
    v0[1] = roundf(v1[1]);
    v0[2] = roundf(v1[2]);
}

static void
native_v3f_round(oobject_t list, oint32_t ac)
/* float32_t v3f.round(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_round(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3f_ceil(ofloat32_t v0[3], ofloat32_t v1[3])
{
    v0[0] = ceilf(v1[0]);
    v0[1] = ceilf(v1[1]);
    v0[2] = ceilf(v1[2]);
}

static void
native_v3f_ceil(oobject_t list, oint32_t ac)
/* float32_t v3f.ceil(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_ceil(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v3f_neg(oobject_t list, oint32_t ac)
/* float32_t v3f.neg(float32_t v0[3], float32_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF3(alist->a1);
    v1 = alist->a1->v.f32;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    v0[2] = -v1[2];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_add(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];
}

static void
native_v3f_add(oobject_t list, oint32_t ac)
/* float32_t v3f.add(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_add(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_sub(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];
}

static void
native_v3f_sub(oobject_t list, oint32_t ac)
/* float32_t v3f.sub(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_sub(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_mul(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
    v0[2] = v1[2] * v2[2];
}

static void
native_v3f_mul(oobject_t list, oint32_t ac)
/* float32_t v3f.mul(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_mul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_div(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
    v0[2] = v1[2] / v2[2];
}

static void
v3f_mod(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3])
{
    v0[0] = fmodf(v1[0], v2[0]);
    v0[1] = fmodf(v1[1], v2[1]);
    v0[2] = fmodf(v1[2], v2[2]);
}

static void
native_v3f_mod(oobject_t list, oint32_t ac)
/* float32_t v3f.mod(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_mod(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v3f_div(oobject_t list, oint32_t ac)
/* float32_t v3f.div(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    v3f_div(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_project(ofloat32_t v0[3], ofloat32_t obj[3],
	    ofloat32_t model[16], ofloat32_t proj[16], oint32_t viewport[4])
{
    ofloat32_t		tmp[4];

    tmp[0] = obj[0];	tmp[1] = obj[1];
    tmp[2] = obj[2];	tmp[3] = 1.0f;
    v4f_mulm(tmp, tmp, model);
    v4f_mulm(tmp, tmp, proj);
    v3f_divs(tmp, tmp, tmp[3]);
    v3f_muls(tmp, tmp, 0.5f);
    v3f_adds(tmp, tmp, 0.5f);
    v0[0] = tmp[0] * viewport[2] + viewport[0];
    v0[1] = tmp[1] * viewport[3] + viewport[1];
    v0[2] = tmp[2];
}

static void
native_v3f_project(oobject_t list, oint32_t ac)
/* float32_t v3f.project(float32_t v0[3], float32_t obj[3],
			 float32_t model[16], float32_t proj[16],
			 int32_t viewport[4])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_MF4(alist->a2);
    CHECK_MF4(alist->a3);
    CHECK_VI4(alist->a4);
    v3f_project(alist->a0->v.f32, alist->a1->v.f32,
		alist->a2->v.f32, alist->a3->v.f32, alist->a4->v.i32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_unProject(ofloat32_t v0[3], ofloat32_t win[3],
	      ofloat32_t model[16], ofloat32_t proj[16],
	      oint32_t viewport[4])
{
    ofloat32_t		inv[16];
    ofloat32_t		tmp[4];

    m4f_mul(inv, proj, model);
    (void)m4f_det_inverse(inv, inv);

    tmp[0] = win[0];	tmp[1] = win[1];
    tmp[2] = win[2];	tmp[3] = 1.0f;

    tmp[0] = (tmp[0] - viewport[0]) / viewport[2];
    tmp[1] = (tmp[1] - viewport[1]) / viewport[3];
    v4f_muls(tmp, tmp, 2.0f);
    v4f_subs(tmp, tmp, 1.0f);

    v4f_mulm(tmp, tmp, inv);

    v3f_divs(v0, tmp, tmp[3]);
}

static void
native_v3f_unProject(oobject_t list, oint32_t ac)
/* float32_t v3f.unProject(float32_t v0[3], float32_t win[3],
			   float32_t model[16], float32_t proj[16],
			   float32_t viewport[4])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_MF4(alist->a2);
    CHECK_MF4(alist->a3);
    CHECK_VI4(alist->a4);
    v3f_unProject(alist->a0->v.f32, alist->a1->v.f32,
		  alist->a2->v.f32, alist->a3->v.f32, alist->a4->v.i32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

/*
 * v3 = m4x4 * v3
 */
static void
v3f_m4f_mul_v3f(ofloat32_t v0[3], ofloat32_t v1[16], ofloat32_t v2[3])
{
    v0[0] = A4(v1,0,0) * v2[0] + A4(v1,1,0) * v2[1] + A4(v1,2,0) * v2[2];
    v0[1] = A4(v1,0,1) * v2[0] + A4(v1,1,1) * v2[1] + A4(v1,2,1) * v2[2];
    v0[2] = A4(v1,0,2) * v2[0] + A4(v1,1,2) * v2[1] + A4(v1,2,2) * v2[2];
}

static void
v3f_rotate(ofloat32_t v0[3], ofloat32_t v1[3],
	   ofloat32_t s0, ofloat32_t v2[3])
{
    ofloat32_t		m[16];
    m4f_rotate(m, identityf, s0, v2);
    v3f_m4f_mul_v3f(v0, m, v1);
}

static void
native_v3f_rotate(oobject_t list, oint32_t ac)
/* float32_t v3f.rotate(float32_t v0[3], float32_t v1[3],
			   float32_t angle, float32_t normal[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_vec_t	*alist;

    alist = (nat_vec_vec_f32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a3);
    v3f_rotate(alist->a0->v.f32, alist->a1->v.f32,
	       alist->a2, alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_rotateX(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			y = v1[1] * _cos - v1[2] * _sin;
    float			z = v1[1] * _sin + v1[2] * _cos;
    if (v0 != v1)
	v0[0] = v1[0];
    v0[1] = y;
    v0[2] = z;
}

static void
native_v3f_rotateX(oobject_t list, oint32_t ac)
/* float32_t v3f.rotateX(float32_t v0[3], float32_t v1[3],
			 float32_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_rotateX(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_rotateY(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			x =  v1[0] * _cos + v1[2] * _sin;
    float			z = -v1[0] * _sin + v1[2] * _cos;
    v0[0] = x;
    if (v0 != v1)
	v0[1] = v1[1];
    v0[2] = z;
}

static void
native_v3f_rotateY(oobject_t list, oint32_t ac)
/* float32_t v3f.rotateY(float32_t v0[3], float32_t v1[3],
			 float32_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_rotateY(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_rotateZ(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			x = v1[0] * _cos - v1[1] * _sin;
    float			y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
    if (v0 != v1)
	v0[2] = v1[2];
}

static void
native_v3f_rotateZ(oobject_t list, oint32_t ac)
/* float32_t v3f.rotateZ(float32_t v0[3], float32_t v1[3],
			 float32_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v3f_rotateZ(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v3f_closestPointOnLine(ofloat32_t v0[3], ofloat32_t v1[3],
		       ofloat32_t v2[3], ofloat32_t v3[3])
{
    float	line_length = v3f_distance(v2, v3);
    float	vector[3];
    float	line_direction[3];
    float	distance;
    v3f_sub(vector, v1, v2);
    v3f_sub(line_direction, v3, v2);
    v3f_divs(line_direction, line_direction, line_length);
    distance = v3f_dot(vector, line_direction);
    if (distance <= 0) {
	v0[0] = v2[0];
	v0[1] = v2[1];
	v0[2] = v2[2];
    }
    else if (distance >= line_length) {
	v0[0] = v3[0];
	v0[1] = v3[1];
	v0[2] = v3[2];
    }
    else {
	v3f_muls(line_direction, line_direction, distance);
	v3f_add(v0, v2, line_direction);
    }
}

static void
native_v3f_closestPointOnLine(oobject_t list, oint32_t ac)
/* float32_t v3f.closestPointOnLine(float32_t res[3], float32_t point[3],
				    float32_t a[3], float32_t b[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    CHECK_VF3(alist->a3);
    v3f_closestPointOnLine(alist->a0->v.f32, alist->a1->v.f32,
			   alist->a2->v.f32, alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v4f_set(oobject_t list, oint32_t ac)
/* float32_t v4f.set(float32_t v0[4], float32_t s0,
		     float32_t s1, float32_t s2, float32_t s3)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    v0 = alist->a0->v.f32;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_fill(ofloat32_t v0[4], ofloat32_t s0)
{
    v0[0] = v0[1] = v0[2] = v0[3] = s0;
}

static void
native_v4f_fill(oobject_t list, oint32_t ac)
/* float32_t v4f.fill(float32_t v0[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    v4f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v4f_copy(oobject_t list, oint32_t ac)
/* float32_t v4f.copy(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF4(alist->a1);
	v1 = alist->a1->v.f32;
	v0[0] = v1[0];
	v0[1] = v1[1];
	v0[2] = v1[2];
	v0[3] = v1[3];
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
v4f_length(ofloat32_t v0[4])
{
    return (sqrtf(sqrf(v0[0]) + sqrf(v0[1]) + sqrf(v0[2]) + sqrf(v0[3])));
}

static void
native_v4f_length(oobject_t list, oint32_t ac)
/* float64_t v4f.length(float32_t v0[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF4(alist->a0);
    r0->v.d = v4f_length(alist->a0->v.f32);
}

static ofloat32_t
v4f_distance(ofloat32_t v0[4], ofloat32_t v1[4])
{
    ofloat32_t		v[4];
    v4f_sub(v, v0, v1);
    return (v4f_length(v));
}

static void
native_v4f_distance(oobject_t list, oint32_t ac)
/* float64_t v4f.distance(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    r0->v.d = v4f_distance(alist->a0->v.f32, alist->a1->v.f32);
}

static ofloat32_t
v4f_dot(ofloat32_t v0[4], ofloat32_t v1[4])
{
    return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3]);
}

static void
native_v4f_dot(oobject_t list, oint32_t ac)
/* float64_t v4f.dot(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    r0->v.d = v4f_dot(alist->a0->v.f32, alist->a1->v.f32);
}

static void
v4f_normalize(ofloat32_t v0[4], ofloat32_t v1[4])
{
    ofloat32_t		s0;

    if (!v1[0] && !v1[1] && !v1[2] && !v1[3]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	    v0[2] = v1[2];
	    v0[3] = v1[3];
	}
    }
    else {
	s0 = 1.0f / v4f_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
	v0[2] = v1[2] * s0;
	v0[3] = v1[3] * s0;
    }
}

static void
native_v4f_normalize(oobject_t list, oint32_t ac)
/* float32_t v4f.normalize(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_normalize(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v4f_eq(oobject_t list, oint32_t ac)
/* uint8_t v4f.eq(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF4(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] != v1[2] && v0[3] != v1[3];
    }
    else
	r0->v.w = true;
}

static void
native_v4f_ne(oobject_t list, oint32_t ac)
/* uint8_t v4f.ne(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_VF4(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3];
    }
    else
	r0->v.w = false;
}

static void
v4f_adds(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
    v0[2] = v1[2] + s0;
    v0[3] = v1[3] + s0;
}

static void
native_v4f_adds(oobject_t list, oint32_t ac)
/* float32_t v4f.adds(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_adds(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_subs(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
    v0[2] = v1[2] - s0;
    v0[3] = v1[3] - s0;
}

static void
native_v4f_subs(oobject_t list, oint32_t ac)
/* float32_t v4f.subs(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_subs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_muls(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
    v0[2] = v1[2] * s0;
    v0[3] = v1[3] * s0;
}

static void
native_v4f_muls(oobject_t list, oint32_t ac)
/* float32_t v4f.muls(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_muls(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_divs(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
    v0[2] = v1[2] / s0;
    v0[3] = v1[3] / s0;
}

static void
native_v4f_divs(oobject_t list, oint32_t ac)
/* float32_t v4f.divs(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_divs(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_mods(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    v0[0] = fmodf(v1[0], s0);
    v0[1] = fmodf(v1[1], s0);
    v0[2] = fmodf(v1[2], s0);
    v0[3] = fmodf(v1[3], s0);
}

static void
native_v4f_mods(oobject_t list, oint32_t ac)
/* float32_t v4f.mods(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_mods(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_mulm(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[16])
{
    ofloat32_t		a0, a1, a2, a3;

    a0 = v1[0];		a1 = v1[1];
    a2 = v1[2];		a3 = v1[3];
    v0[0] = (a0 * A4(v2, 0, 0) + a1 * A4(v2, 1, 0) +
	     a2 * A4(v2, 2, 0) + a3 * A4(v2, 3, 0));
    v0[1] = (a0 * A4(v2, 0, 1) + a1 * A4(v2, 1, 1) +
	     a2 * A4(v2, 2, 1) + a3 * A4(v2, 3, 1));
    v0[2] = (a0 * A4(v2, 0, 2) + a1 * A4(v2, 1, 2) +
	     a2 * A4(v2, 2, 2) + a3 * A4(v2, 3, 2));
    v0[3] = (a0 * A4(v2, 0, 3) + a1 * A4(v2, 1, 3) +
	     a2 * A4(v2, 2, 3) + a3 * A4(v2, 3, 3));
}

static void
native_v4f_mulm(oobject_t list, oint32_t ac)
/* float32_t v4f.mulm(float32_t v0[4], float32_t v1[4],
		      float32_t v2[16])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_MF4(alist->a2);
    v4f_mulm(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}


static void
v4f_mmul(ofloat32_t v0[4], ofloat32_t v1[16], ofloat32_t v2[4])
{
    ofloat32_t		a0, a1, a2, a3;

    a0 = v2[0];		a1 = v2[1];
    a2 = v2[2];		a3 = v2[3];
    v0[0] = (A4(v1, 0, 0) * a0 + A4(v1, 0, 1) * a1 +
	     A4(v1, 0, 2) * a2 + A4(v1, 0, 3) * a3);
    v0[1] = (A4(v1, 1, 0) * a0 + A4(v1, 1, 1) * a1 +
	     A4(v1, 1, 2) * a2 + A4(v1, 1, 3) * a3);
    v0[2] = (A4(v1, 2, 0) * a0 + A4(v1, 2, 1) * a1 +
	     A4(v1, 2, 2) * a2 + A4(v1, 2, 3) * a3);
    v0[3] = (A4(v1, 3, 0) * a0 + A4(v1, 3, 1) * a1 +
	     A4(v1, 3, 2) * a2 + A4(v1, 3, 3) * a3);
}

static void
native_v4f_mmul(oobject_t list, oint32_t ac)
/* float32_t v4f.mmul(float32_t v0[4], float32_t v1[16],
		      float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_mmul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_clamp(ofloat32_t v0[4], ofloat32_t v1[4],
	  ofloat32_t minVal, ofloat32_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
    v0[2] = clamp(minVal, maxVal, v1[2]);
    v0[3] = clamp(minVal, maxVal, v1[3]);
}

static void
native_v4f_clamp(oobject_t list, oint32_t ac)
/* float32_t v4f.clamp(float32_t v0[4], float32_t v1[4],
		       float32_t minVal, float32_t maxVal)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_f32_t	*alist;

    alist = (nat_vec_vec_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_clamp(alist->a0->v.f32, alist->a1->v.f32, alist->a2, alist->a3);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_abs(ofloat32_t v0[4], ofloat32_t v1[4])
{
    v0[0] = fabsf(v1[0]);
    v0[1] = fabsf(v1[1]);
    v0[2] = fabsf(v1[2]);
    v0[3] = fabsf(v1[3]);
}

static void
native_v4f_abs(oobject_t list, oint32_t ac)
/* float32_t v4f.abs(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_abs(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_floor(ofloat32_t v0[4], ofloat32_t v1[4])
{
    v0[0] = floorf(v1[0]);
    v0[1] = floorf(v1[1]);
    v0[2] = floorf(v1[2]);
    v0[3] = floorf(v1[3]);
}

static void
native_v4f_floor(oobject_t list, oint32_t ac)
/* float32_t v4f.floor(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_floor(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_trunc(ofloat32_t v0[4], ofloat32_t v1[4])
{
    v0[0] = truncf(v1[0]);
    v0[1] = truncf(v1[1]);
    v0[2] = truncf(v1[2]);
    v0[3] = truncf(v1[3]);
}

static void
native_v4f_trunc(oobject_t list, oint32_t ac)
/* float32_t v4f.trunc(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_trunc(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_round(ofloat32_t v0[4], ofloat32_t v1[4])
{
    v0[0] = roundf(v1[0]);
    v0[1] = roundf(v1[1]);
    v0[2] = roundf(v1[2]);
    v0[3] = roundf(v1[3]);
}

static void
native_v4f_round(oobject_t list, oint32_t ac)
/* float32_t v4f.round(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_round(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4f_ceil(ofloat32_t v0[4], ofloat32_t v1[4])
{
    v0[0] = ceilf(v1[0]);
    v0[1] = ceilf(v1[1]);
    v0[2] = ceilf(v1[2]);
    v0[3] = ceilf(v1[3]);
}

static void
native_v4f_ceil(oobject_t list, oint32_t ac)
/* float32_t v4f.ceil(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_ceil(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v4f_neg(oobject_t list, oint32_t ac)
/* float32_t v4f.neg(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF4(alist->a1);
    v1 = alist->a1->v.f32;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    v0[2] = -v1[2];
    v0[3] = -v1[3];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_add(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];
    v0[3] = v1[3] + v2[3];
}

static void
native_v4f_add(oobject_t list, oint32_t ac)
/* float32_t v4f.add(float32_t v0[4], float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_add(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_sub(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];
    v0[3] = v1[3] - v2[3];
}

static void
native_v4f_sub(oobject_t list, oint32_t ac)
/* float32_t v4f.sub(float32_t v0[4], float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_sub(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_mul(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
    v0[2] = v1[2] * v2[2];
    v0[3] = v1[3] * v2[3];
}

static void
native_v4f_mul(oobject_t list, oint32_t ac)
/* float32_t v4f.mul(float32_t v0[4], float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_mul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_div(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
    v0[2] = v1[2] / v2[2];
    v0[3] = v1[3] / v2[3];
}

static void
native_v4f_div(oobject_t list, oint32_t ac)
/* float32_t v4f.div(float32_t v0[4], float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_div(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_mod(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = fmodf(v1[0], v2[0]);
    v0[1] = fmodf(v1[1], v2[1]);
    v0[2] = fmodf(v1[2], v2[2]);
    v0[3] = fmodf(v1[3], v2[3]);
}

static void
native_v4f_mod(oobject_t list, oint32_t ac)
/* float32_t v4f.mod(float32_t v0[4], float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF4(alist->a2);
    v4f_mod(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

/*
 * v4 = m4x4 * v4
 */
static void
v4f_m4f_mul_v4f(ofloat32_t v0[4], ofloat32_t v1[16], ofloat32_t v2[3])
{
    v0[0] = A4(v1,0,0) * v2[0] + A4(v1,1,0) * v2[1] + A4(v1,2,0) * v2[2] + A4(v1,3,0) * v2[3];
    v0[1] = A4(v1,0,1) * v2[0] + A4(v1,1,1) * v2[1] + A4(v1,2,1) * v2[2] + A4(v1,3,1) * v2[3];
    v0[2] = A4(v1,0,2) * v2[0] + A4(v1,1,2) * v2[1] + A4(v1,2,2) * v2[2] + A4(v1,3,2) * v2[3];
    v0[3] = A4(v1,0,3) * v2[0] + A4(v1,1,3) * v2[1] + A4(v1,2,3) * v2[2] + A4(v1,3,3) * v2[3];
}

static void
v4f_rotate(ofloat32_t v0[4], ofloat32_t v1[4],
	   ofloat32_t s0, ofloat32_t v2[3])
{
    ofloat32_t		m[16];
    m4f_rotate(m, identityf, s0, v2);
    v4f_m4f_mul_v4f(v0, m, v1);
}

static void
native_v4f_rotate(oobject_t list, oint32_t ac)
/* float32_t v4f.rotate(float32_t v0[4], float32_t v1[4],
			   float32_t angle, float32_t normal[3])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_vec_t	*alist;

    alist = (nat_vec_vec_f32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    CHECK_VF3(alist->a3);
    v4f_rotate(alist->a0->v.f32, alist->a1->v.f32,
	       alist->a2, alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_rotateX(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			y = v1[1] * _cos - v1[2] * _sin;
    float			z = v1[1] * _sin + v1[2] * _cos;
    if (v0 != v1) {
	v0[0] = v1[0];
	v0[3] = v1[3];
    }
    v0[1] = y;
    v0[2] = z;
}

static void
native_v4f_rotateX(oobject_t list, oint32_t ac)
/* float32_t v4f.rotateX(float32_t v0[4], float32_t v1[4],
			 float32_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_rotateX(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_rotateY(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			x =  v1[0] * _cos + v1[2] * _sin;
    float			z = -v1[0] * _sin + v1[2] * _cos;
    v0[0] = x;
    if (v0 != v1) {
	v0[1] = v1[1];
	v0[3] = v1[3];
    }
    v0[2] = z;
}

static void
native_v4f_rotateY(oobject_t list, oint32_t ac)
/* float32_t v4f.rotateY(float32_t v0[4], float32_t v1[4],
			 float32_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_rotateY(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4f_rotateZ(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t s0)
{
    float			_cos = cosf(s0);
    float			_sin = sinf(s0);
    float			x = v1[0] * _cos - v1[1] * _sin;
    float			y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
    if (v0 != v1) {
	v0[2] = v1[2];
	v0[3] = v1[3];
    }
}

static void
native_v4f_rotateZ(oobject_t list, oint32_t ac)
/* float32_t v4f.rotateZ(float32_t v0[4], float32_t v1[4],
			 float32_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF4(alist->a0);
    CHECK_VF4(alist->a1);
    v4f_rotateZ(alist->a0->v.f32, alist->a1->v.f32, alist->a2);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_v2d_set(oobject_t list, oint32_t ac)
/* float64_t v2d.set(float64_t v0[2], float64_t s0, float64_t s1)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_t		*alist;

    alist = (nat_vec_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    v0 = alist->a0->v.f64;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_fill(ofloat64_t v0[2], ofloat64_t s0)
{
    v0[0] = v0[1] = s0;
}

static void
native_v2d_fill(oobject_t list, oint32_t ac)
/* float64_t v2d.fill(float64_t v0[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    v2d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v2d_copy(oobject_t list, oint32_t ac)
/* float64_t v2d.copy(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD2(alist->a1);
	v1 = alist->a1->v.f64;
	v0[0] = v1[0];
	v0[1] = v1[1];
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
v2d_length(ofloat64_t v0[2])
{
    return (sqrt(sqr(v0[0]) + sqr(v0[1])));
}

static void
native_v2d_length(oobject_t list, oint32_t ac)
/* float64_t v2d.length(float64_t v0[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD2(alist->a0);
    r0->v.d = v2d_length(alist->a0->v.f64);
}

static ofloat64_t
v2d_distance(ofloat64_t v0[2], ofloat64_t v1[2])
{
    ofloat64_t		v[2];
    v2d_sub(v, v0, v1);
    return (v2d_length(v));
}

static void
native_v2d_distance(oobject_t list, oint32_t ac)
/* float64_t v2d.distance(float64_t v0[2], float64_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    r0->v.d = v2d_distance(alist->a0->v.f64, alist->a1->v.f64);
}

static ofloat64_t
v2d_dot(ofloat64_t v0[2], ofloat64_t v1[2])
{
    return (v0[0] * v1[0] + v0[1] * v1[1]);
}

static void
native_v2d_dot(oobject_t list, oint32_t ac)
/* float64_t v2d.dot(float64_t v0[2], float64_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    r0->v.d = v2d_dot(alist->a0->v.f64, alist->a1->v.f64);
}

static void
v2d_normalize(ofloat64_t v0[2], ofloat64_t v1[2])
{
    ofloat64_t		s0;

    if (!v1[0] && !v1[1]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	}
    }
    else {
	s0 = 1.0 / v2d_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
    }
}

static void
native_v2d_normalize(oobject_t list, oint32_t ac)
/* float64_t v2d.normalize(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_normalize(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v2d_eq(oobject_t list, oint32_t ac)
/* uint8_t v2d.eq(float64_t v0[2], float64_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD2(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1];
    }
    else
	r0->v.w = true;
}

static void
native_v2d_ne(oobject_t list, oint32_t ac)
/* uint8_t v2d.ne(float64_t v0[2], float64_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD2(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1];
    }
    else
	r0->v.w = false;
}

static void
v2d_adds(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
}

static void
native_v2d_adds(oobject_t list, oint32_t ac)
/* float64_t v2d.adds(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_adds(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_subs(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
}

static void
native_v2d_subs(oobject_t list, oint32_t ac)
/* float64_t v2d.subs(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_subs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}


static void
v2d_muls(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
}

static void
native_v2d_muls(oobject_t list, oint32_t ac)
/* float64_t v2d.muls(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_muls(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_divs(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
}

static void
native_v2d_divs(oobject_t list, oint32_t ac)
/* float64_t v2d.divs(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_divs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_mods(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    v0[0] = fmod(v1[0], s0);
    v0[1] = fmod(v1[1], s0);
}

static void
native_v2d_mods(oobject_t list, oint32_t ac)
/* float64_t v2d.mods(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_mods(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_mulm(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[4])
{
    ofloat64_t		a0, a1;

    a0 = v1[0];		a1 = v1[1];
    v0[0] = a0 * A2(v2, 0, 0) + a1 * A2(v2, 1, 0);
    v0[1] = a0 * A2(v2, 0, 1) + a1 * A2(v2, 1, 1);
}

static void
native_v2d_mulm(oobject_t list, oint32_t ac)
/* float64_t v2d.mulm(float64_t v0[2], float64_t v1[2],
		      float64_t v2[4])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_MD2(alist->a2);
    v2d_mulm(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_mmul(ofloat64_t v0[2], ofloat64_t v1[4], ofloat64_t v2[2])
{
    ofloat64_t		a0, a1;

    a0 = v2[0];		a1 = v2[1];
    v0[0] = A2(v1, 0, 0) * a0 + A2(v1, 0, 1) * a1;
    v0[1] = A2(v1, 1, 0) * a0 + A2(v1, 1, 1) * a1;
}

static void
native_v2d_mmul(oobject_t list, oint32_t ac)
/* float64_t v2d.mmul(float64_t v0[2], float64_t v1[4],
		      float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_MD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_mmul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_clamp(ofloat64_t v0[2], ofloat64_t v1[2],
	  ofloat64_t minVal, ofloat64_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
}

static void
native_v2d_clamp(oobject_t list, oint32_t ac)
/* float64_t v2d.clamp(float64_t v0[2], float64_t v1[2],
		       float64_t minVal, float64_t maxVal)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_f64_t	*alist;

    alist = (nat_vec_vec_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_clamp(alist->a0->v.f64, alist->a1->v.f64, alist->a2, alist->a3);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_abs(ofloat64_t v0[2], ofloat64_t v1[2])
{
    v0[0] = fabs(v1[0]);
    v0[1] = fabs(v1[1]);
}

static void
native_v2d_abs(oobject_t list, oint32_t ac)
/* float64_t v2d.abs(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_abs(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_floor(ofloat64_t v0[2], ofloat64_t v1[2])
{
    v0[0] = floor(v1[0]);
    v0[1] = floor(v1[1]);
}

static void
native_v2d_floor(oobject_t list, oint32_t ac)
/* float64_t v2d.floor(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_floor(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_trunc(ofloat64_t v0[2], ofloat64_t v1[2])
{
    v0[0] = trunc(v1[0]);
    v0[1] = trunc(v1[1]);
}

static void
native_v2d_trunc(oobject_t list, oint32_t ac)
/* float64_t v2d.trunc(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_trunc(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_round(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = round(v1[0]);
    v0[1] = round(v1[1]);
}

static void
native_v2d_round(oobject_t list, oint32_t ac)
/* float64_t v2d.round(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_round(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_ceil(ofloat64_t v0[2], ofloat64_t v1[2])
{
    v0[0] = ceil(v1[0]);
    v0[1] = ceil(v1[1]);
}

static void
native_v2d_ceil(oobject_t list, oint32_t ac)
/* float64_t v2d.ceil(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_ceil(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v2d_neg(oobject_t list, oint32_t ac)
/* float64_t v2d.neg(float64_t v0[2], float64_t v1[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD2(alist->a1);
    v1 = alist->a1->v.f64;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_add(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
}

static void
native_v2d_add(oobject_t list, oint32_t ac)
/* float64_t v2d.add(float64_t v0[2], float64_t v1[2], float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_add(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_sub(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
}

static void
native_v2d_sub(oobject_t list, oint32_t ac)
/* float64_t v2d.sub(float64_t v0[2], float64_t v1[2], float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_sub(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_mul(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
}

static void
native_v2d_mul(oobject_t list, oint32_t ac)
/* float64_t v2d.mul(float64_t v0[2], float64_t v1[2], float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_mul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_div(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
}

static void
native_v2d_div(oobject_t list, oint32_t ac)
/* float64_t v2d.div(float64_t v0[2], float64_t v1[2], float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_div(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_mod(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2])
{
    v0[0] = fmod(v1[0], v2[0]);
    v0[1] = fmod(v1[1], v2[1]);
}

static void
native_v2d_mod(oobject_t list, oint32_t ac)
/* float64_t v2d.mod(float64_t v0[2], float64_t v1[2], float64_t v2[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    v2d_mod(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_rotate(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0)
{
    double	_cos = cos(s0);
    double	_sin = sin(s0);
    double	x = v1[0] * _cos - v1[1] * _sin;
    double	y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
}

static void
native_v2d_rotate(oobject_t list, oint32_t ac)
/* float64_t v2d.rotate(float64_t v0[2], float64_t v1[2], float64_t s0)[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    v2d_rotate(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v2d_closestPointOnLine(ofloat64_t v0[2], ofloat64_t v1[2],
		       ofloat64_t v2[2], ofloat64_t v3[2])
{
    double	line_length;
    double	vector[2];
    double	line_direction[2];
    double	distance;
    line_length = v2d_distance(v2, v3);
    v2d_sub(vector, v1, v2);
    v2d_sub(line_direction, v3, v2);
    v2d_divs(line_direction, line_direction, line_length);
    distance = v2d_dot(vector, line_direction);
    if (distance <= 0) {
	v0[0] = v2[0];
	v0[1] = v2[1];
    }
    else if (distance >= line_length) {
	v0[0] = v3[0];
	v0[1] = v3[1];
    }
    else {
	v2d_muls(line_direction, line_direction, distance);
	v2d_add(v0, v2, line_direction);
    }
}

static void
native_v2d_closestPointOnLine(oobject_t list, oint32_t ac)
/* float64_t v2d.closestPointOnLine(float64_t res[2], float64_t point[2],
				    float64_t a[2], float64_t b[2])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD2(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    CHECK_VD2(alist->a3);
    v2d_closestPointOnLine(alist->a0->v.f64, alist->a1->v.f64,
			   alist->a2->v.f64, alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v3d_set(oobject_t list, oint32_t ac)
/* float64_t v3d.set(float64_t v0[3], float64_t s0,
		     float64_t s1, float64_t s2)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_f64_t	*alist;

    alist = (nat_vec_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    v0 = alist->a0->v.f64;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_fill(ofloat64_t v0[3], ofloat64_t s0)
{
    v0[0] = v0[1] = v0[2] = s0;
}

static void
native_v3d_fill(oobject_t list, oint32_t ac)
/* float64_t v3d.fill(float64_t v0[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    v3d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v3d_copy(oobject_t list, oint32_t ac)
/* float64_t v3d.copy(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD3(alist->a1);
	v1 = alist->a1->v.f64;
	v0[0] = v1[0];
	v0[1] = v1[1];
	v0[2] = v1[2];
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
v3d_length(ofloat64_t v0[3])
{
    return (sqrt(sqr(v0[0]) + sqr(v0[1]) + sqr(v0[2])));
}

static void
native_v3d_length(oobject_t list, oint32_t ac)
/* float64_t v3d.length(float64_t v0[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD3(alist->a0);
    r0->v.d = v3d_length(alist->a0->v.f64);
}

static ofloat64_t
v3d_distance(ofloat64_t v0[3], ofloat64_t v1[3])
{
    ofloat64_t		v[3];
    v3d_sub(v, v0, v1);
    return (v3d_length(v));
}

static void
native_v3d_distance(oobject_t list, oint32_t ac)
/* float64_t v3d.distance(float64_t v0[3], float64_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    r0->v.d = v3d_distance(alist->a0->v.f64, alist->a1->v.f64);
}

static ofloat64_t
v3d_dot(ofloat64_t v0[3], ofloat64_t v1[3])
{
    return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2]);
}

static void
native_v3d_dot(oobject_t list, oint32_t ac)
/* float64_t v3d.dot(float64_t v0[3], float64_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    r0->v.d = v3d_dot(alist->a0->v.f64, alist->a1->v.f64);
}

static void
v3d_normalize(ofloat64_t v0[3], ofloat64_t v1[3])
{
    ofloat64_t		s0;

    if (!v1[0] && !v1[1] && !v1[2]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	    v0[2] = v1[2];
	}
    }
    else {
	s0 = 1.0 / v3d_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
	v0[2] = v1[2] * s0;
    }
}

static void
native_v3d_normalize(oobject_t list, oint32_t ac)
/* float64_t v3d.normalize(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_normalize(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v3d_eq(oobject_t list, oint32_t ac)
/* uint8_t v3d.eq(float64_t v0[3], float64_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD3(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] && v0[2] != v1[2];
    }
    else
	r0->v.w = true;
}

static void
native_v3d_ne(oobject_t list, oint32_t ac)
/* uint8_t v3d.ne(float64_t v0[3], float64_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD3(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] || v0[2] != v1[2];
    }
    else
	r0->v.w = false;
}

static void
v3d_adds(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
    v0[2] = v1[2] + s0;
}

static void
native_v3d_adds(oobject_t list, oint32_t ac)
/* float64_t v3d.adds(float64_t v0[3], float64_t v1[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_adds(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_subs(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
    v0[2] = v1[2] - s0;
}

static void
native_v3d_subs(oobject_t list, oint32_t ac)
/* float64_t v3d.subs(float64_t v0[3], float64_t v1[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_subs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_muls(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
    v0[2] = v1[2] * s0;
}

static void
native_v3d_muls(oobject_t list, oint32_t ac)
/* float64_t v3d.muls(float64_t v0[3], float64_t v1[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_muls(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_divs(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
    v0[2] = v1[2] / s0;
}

static void
native_v3d_divs(oobject_t list, oint32_t ac)
/* float64_t v3d.divs(float64_t v0[3], float64_t v1[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_divs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_mods(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    v0[0] = fmod(v1[0], s0);
    v0[1] = fmod(v1[1], s0);
    v0[2] = fmod(v1[2], s0);
}

static void
native_v3d_mods(oobject_t list, oint32_t ac)
/* float64_t v3d.mods(float64_t v0[3], float64_t v1[3], float64_t s0)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_mods(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_cross(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    ofloat64_t		*vn, v[9];

    if (likely(v0 != v1 && v0 != v2))
	vn = v0;
    else
	vn = v;
    vn[0] = v1[1] * v2[2] - v1[2] * v2[1];
    vn[1] = v1[2] * v2[0] - v1[0] * v2[2];
    vn[2] = v1[0] * v2[1] - v1[1] * v2[0];
    if (unlikely(vn != v0)) {
	v0[0] = vn[0];
	v0[1] = vn[1];
	v0[2] = vn[2];
    }
}

static void
native_v3d_cross(oobject_t list, oint32_t ac)
/* float64_t v3d.cross(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_cross(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_abs(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = fabs(v1[0]);
    v0[1] = fabs(v1[1]);
    v0[2] = fabs(v1[2]);
}

static void
native_v3d_abs(oobject_t list, oint32_t ac)
/* float64_t v3d.abs(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_abs(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_floor(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = floor(v1[0]);
    v0[1] = floor(v1[1]);
    v0[2] = floor(v1[2]);
}

static void
native_v3d_floor(oobject_t list, oint32_t ac)
/* float64_t v3d.floor(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_floor(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_trunc(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = trunc(v1[0]);
    v0[1] = trunc(v1[1]);
    v0[2] = trunc(v1[2]);
}

static void
native_v3d_trunc(oobject_t list, oint32_t ac)
/* float64_t v3d.trunc(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_trunc(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_round(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = round(v1[0]);
    v0[1] = round(v1[1]);
    v0[2] = round(v1[2]);
}

static void
native_v3d_round(oobject_t list, oint32_t ac)
/* float64_t v3d.round(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_round(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_ceil(ofloat64_t v0[3], ofloat64_t v1[3])
{
    v0[0] = ceil(v1[0]);
    v0[1] = ceil(v1[1]);
    v0[2] = ceil(v1[2]);
}

static void
native_v3d_ceil(oobject_t list, oint32_t ac)
/* float64_t v3d.ceil(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_ceil(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v3d_neg(oobject_t list, oint32_t ac)
/* float64_t v3d.neg(float64_t v0[3], float64_t v1[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD3(alist->a1);
    v1 = alist->a1->v.f64;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    v0[2] = -v1[2];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_add(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];
}

static void
native_v3d_add(oobject_t list, oint32_t ac)
/* float64_t v3d.add(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_add(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_sub(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];
}

static void
native_v3d_sub(oobject_t list, oint32_t ac)
/* float64_t v3d.sub(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_sub(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_mul(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
    v0[2] = v1[2] * v2[2];
}

static void
native_v3d_mul(oobject_t list, oint32_t ac)
/* float64_t v3d.mul(float64_t v0[3], float64_t v1[3], float64_t v2[3])[2]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_mul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_div(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
    v0[2] = v1[2] / v2[2];
}

static void
native_v3d_div(oobject_t list, oint32_t ac)
/* float64_t v3d.div(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_div(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_mod(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3])
{
    v0[0] = fmod(v1[0], v2[0]);
    v0[1] = fmod(v1[1], v2[1]);
    v0[2] = fmod(v1[2], v2[2]);
}

static void
native_v3d_mod(oobject_t list, oint32_t ac)
/* float64_t v3d.mod(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_mod(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_mulm(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[9])
{
    ofloat64_t		a0, a1, a2;

    a0 = v1[0];		a1 = v1[1];	a2 = v1[2];
    v0[0] = a0 * A3(v2, 0, 0) + a1 * A3(v2, 1, 0) + a2 * A3(v2, 2, 0);
    v0[1] = a0 * A3(v2, 0, 1) + a1 * A3(v2, 1, 1) + a2 * A3(v2, 2, 1);
    v0[2] = a0 * A3(v2, 0, 2) + a1 * A3(v2, 1, 2) + a2 * A3(v2, 2, 2);
}

static void
native_v3d_mulm(oobject_t list, oint32_t ac)
/* float64_t v3d.mulm(float64_t v0[3], float64_t v1[3],
		      float64_t v2[9])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_MD3(alist->a2);
    v3d_mulm(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_mmul(ofloat64_t v0[3], ofloat64_t v1[9], ofloat64_t v2[3])
{
    ofloat64_t		a0, a1, a2;

    a0 = v2[0];		a1 = v2[1];	a2 = v2[2];
    v0[0] = A3(v1, 0, 0) * a0 + A3(v1, 0, 1) * a1 + A3(v1, 0, 2) * a2;
    v0[1] = A3(v1, 1, 0) * a0 + A3(v1, 1, 1) * a1 + A3(v1, 1, 2) * a2;
    v0[2] = A3(v1, 2, 0) * a0 + A3(v1, 2, 1) * a1 + A3(v1, 2, 2) * a2;
}

static void
native_v3d_mmul(oobject_t list, oint32_t ac)
/* float64_t v3d.mmul(float64_t v0[3], float64_t v1[9],
		      float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_MD3(alist->a1);
    CHECK_VD3(alist->a2);
    v3d_mmul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_clamp(ofloat64_t v0[3], ofloat64_t v1[3],
	  ofloat64_t minVal, ofloat64_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
    v0[2] = clamp(minVal, maxVal, v1[2]);
}

static void
native_v3d_clamp(oobject_t list, oint32_t ac)
/* float64_t v3d.clamp(float64_t v0[3], float64_t v1[3],
		       float64_t minVal, float64_t maxVal)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_f64_t	*alist;

    alist = (nat_vec_vec_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_clamp(alist->a0->v.f64, alist->a1->v.f64, alist->a2, alist->a3);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_project(ofloat64_t v0[3], ofloat64_t obj[3],
	    ofloat64_t model[16], ofloat64_t proj[16],
	    oint32_t viewport[4])
{
    ofloat64_t		tmp[4];

    tmp[0] = obj[0];	tmp[1] = obj[1];
    tmp[2] = obj[2];	tmp[3] = 1.0;
    v4d_mulm(tmp, tmp, model);
    v4d_mulm(tmp, tmp, proj);
    v3d_divs(tmp, tmp, tmp[3]);
    v3d_muls(tmp, tmp, 0.5);
    v3d_adds(tmp, tmp, 0.5);
    v0[0] = tmp[0] * viewport[2] + viewport[0];
    v0[1] = tmp[1] * viewport[3] + viewport[1];
    v0[2] = tmp[2];
}

static void
native_v3d_project(oobject_t list, oint32_t ac)
/* float64_t v3f.project(float64_t v0[3], float64_t obj[3],
			 float64_t model[16], float64_t proj[16],
			 int32_t viewport[4])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_MD4(alist->a2);
    CHECK_MD4(alist->a3);
    CHECK_VI4(alist->a4);
    v3d_project(alist->a0->v.f64, alist->a1->v.f64,
		alist->a2->v.f64, alist->a3->v.f64, alist->a4->v.i32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_unProject(ofloat64_t v0[3], ofloat64_t win[3],
	      ofloat64_t model[16], ofloat64_t proj[16],
	      oint32_t viewport[4])
{
    ofloat64_t		inv[16];
    ofloat64_t		tmp[4];

    m4d_mul(inv, proj, model);
    (void)m4d_det_inverse(inv, inv);

    tmp[0] = win[0];	tmp[1] = win[1];
    tmp[2] = win[2];	tmp[3] = 1.0;

    tmp[0] = (tmp[0] - viewport[0]) / viewport[2];
    tmp[1] = (tmp[1] - viewport[1]) / viewport[3];
    v4d_muls(tmp, tmp, 2.0);
    v4d_subs(tmp, tmp, 1.0);

    v4d_mulm(tmp, tmp, inv);

    v3d_divs(v0, tmp, tmp[3]);
}

static void
native_v3d_unProject(oobject_t list, oint32_t ac)
/* float64_t v3d.unProject(float64_t v0[3], float64_t win[3],
			   float64_t model[16], float64_t proj[16],
			   float64_t viewport[4])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_MD4(alist->a2);
    CHECK_MD4(alist->a3);
    CHECK_VI4(alist->a4);
    v3d_unProject(alist->a0->v.f64, alist->a1->v.f64,
		  alist->a2->v.f64, alist->a3->v.f64, alist->a4->v.i32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

/*
 * v3 = m4x4 * v3
 */
static void
v3d_m4d_mul_v3d(ofloat64_t v0[3], ofloat64_t v1[16], ofloat64_t v2[3])
{
    v0[0] = A4(v1,0,0) * v2[0] + A4(v1,1,0) * v2[1] + A4(v1,2,0) * v2[2];
    v0[1] = A4(v1,0,1) * v2[0] + A4(v1,1,1) * v2[1] + A4(v1,2,1) * v2[2];
    v0[2] = A4(v1,0,2) * v2[0] + A4(v1,1,2) * v2[1] + A4(v1,2,2) * v2[2];
}

static void
v3d_rotate(ofloat64_t v0[3], ofloat64_t v1[3],
	   ofloat64_t s0, ofloat64_t v2[3])
{
    ofloat64_t		m[16];
    m4d_rotate(m, identity, s0, v2);
    v3d_m4d_mul_v3d(v0, m, v1);
}

static void
native_v3d_rotate(oobject_t list, oint32_t ac)
/* float64_t v3d.rotate(float64_t v0[3], float64_t v1[3],
			   float64_t angle, float64_t normal[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_vec_t	*alist;

    alist = (nat_vec_vec_f64_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a3);
    v3d_rotate(alist->a0->v.f64, alist->a1->v.f64,
	       alist->a2, alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_rotateX(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			y = v1[1] * _cos - v1[2] * _sin;
    double			z = v1[1] * _sin + v1[2] * _cos;
    if (v0 != v1)
	v0[0] = v1[0];
    v0[1] = y;
    v0[2] = z;
}

static void
native_v3d_rotateX(oobject_t list, oint32_t ac)
/* float64_t v3d.rotateX(float64_t v0[3], float64_t v1[3],
			 float64_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_rotateX(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_rotateY(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			x =  v1[0] * _cos + v1[2] * _sin;
    double			z = -v1[0] * _sin + v1[2] * _cos;
    v0[0] = x;
    if (v0 != v1)
	v0[1] = v1[1];
    v0[2] = z;
}

static void
native_v3d_rotateY(oobject_t list, oint32_t ac)
/* float64_t v3d.rotateY(float64_t v0[3], float64_t v1[3],
			 float64_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_rotateY(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_rotateZ(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			x = v1[0] * _cos - v1[1] * _sin;
    double			y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
    if (v0 != v1)
	v0[2] = v1[2];
}

static void
native_v3d_rotateZ(oobject_t list, oint32_t ac)
/* float64_t v3d.rotateZ(float64_t v0[3], float64_t v1[3],
			 float64_t angle)[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v3d_rotateZ(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v3d_closestPointOnLine(ofloat64_t v0[3], ofloat64_t v1[3],
		       ofloat64_t v2[3], ofloat64_t v3[3])
{
    double	line_length;
    double	vector[3];
    double	line_direction[3];
    double	distance;
    line_length = v3d_distance(v2, v3);
    v3d_sub(vector, v1, v2);
    v3d_sub(line_direction, v3, v2);
    v3d_divs(line_direction, line_direction, line_length);
    distance = v3d_dot(vector, line_direction);
    if (distance <= 0) {
	v0[0] = v2[0];
	v0[1] = v2[1];
	v0[2] = v2[2];
    }
    else if (distance >= line_length) {
	v0[0] = v3[0];
	v0[1] = v3[1];
	v0[2] = v3[2];
    }
    else {
	v3d_muls(line_direction, line_direction, distance);
	v3d_add(v0, v2, line_direction);
    }
}

static void
native_v3d_closestPointOnLine(oobject_t list, oint32_t ac)
/* float64_t v3d.closestPointOnLine(float64_t res[3], float64_t point[3],
				    float64_t a[3], float64_t b[2])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    CHECK_VD3(alist->a3);
    v3d_closestPointOnLine(alist->a0->v.f64, alist->a1->v.f64,
			   alist->a2->v.f64, alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v4d_set(oobject_t list, oint32_t ac)
/* float64_t v4d.set(float64_t v0[3], float64_t s0,
		     float64_t s1, float64_t s2, float64_t s3)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_f64_f64_t	*alist;

    alist = (nat_vec_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    v0 = alist->a0->v.f64;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_fill(ofloat64_t v0[4], ofloat64_t s0)
{
    v0[0] = v0[1] = v0[2] = v0[3] = s0;
}

static void
native_v4d_fill(oobject_t list, oint32_t ac)
/* float64_t v4d.fill(float64_t v0[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    v4d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v4d_copy(oobject_t list, oint32_t ac)
/* float64_t v4d.copy(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD4(alist->a1);
	v1 = alist->a1->v.f64;
	v0[0] = v1[0];
	v0[1] = v1[1];
	v0[2] = v1[2];
	v0[3] = v1[3];
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
v4d_length(ofloat64_t v0[4])
{
    return (sqrt(sqr(v0[0]) + sqr(v0[1]) + sqr(v0[2]) + sqr(v0[3])));
}

static void
native_v4d_length(oobject_t list, oint32_t ac)
/* float64_t v4d.length(float64_t v0[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD4(alist->a0);
    r0->v.d = v4d_length(alist->a0->v.f64);
}

static ofloat64_t
v4d_distance(ofloat64_t v0[4], ofloat64_t v1[4])
{
    ofloat64_t		v[4];
    v4d_sub(v, v0, v1);
    return (v4d_length(v));
}

static void
native_v4d_distance(oobject_t list, oint32_t ac)
/* float64_t v4d.distance(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    r0->v.d = v4d_distance(alist->a0->v.f64, alist->a1->v.f64);
}

static ofloat64_t
v4d_dot(ofloat64_t v0[4], ofloat64_t v1[4])
{
    return (v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3]);
}

static void
native_v4d_dot(oobject_t list, oint32_t ac)
/* float64_t v4d.dot(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    r0->v.d = v4d_dot(alist->a0->v.f64, alist->a1->v.f64);
}

static void
v4d_normalize(ofloat64_t v0[4], ofloat64_t v1[4])
{
    ofloat64_t		s0;

    if (!v1[0] && !v1[1] && !v1[2] && !v1[3]) {
	if (likely(v0 != v1)) {
	    v0[0] = v1[0];
	    v0[1] = v1[1];
	    v0[2] = v1[2];
	    v0[3] = v1[3];
	}
    }
    else {
	s0 = 1.0 / v4d_length(v1);
	v0[0] = v1[0] * s0;
	v0[1] = v1[1] * s0;
	v0[2] = v1[2] * s0;
	v0[3] = v1[3] * s0;
    }
}

static void
native_v4d_normalize(oobject_t list, oint32_t ac)
/* float64_t v4d.normalize(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_normalize(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v4d_eq(oobject_t list, oint32_t ac)
/* uint8_t v4d.eq(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD4(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] != v1[2] && v0[3] != v1[3];
    }
    else
	r0->v.w = true;
}

static void
native_v4d_ne(oobject_t list, oint32_t ac)
/* uint8_t v4d.ne(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_VD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_VD4(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3];
    }
    else
	r0->v.w = false;
}

static void
v4d_adds(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    v0[0] = v1[0] + s0;
    v0[1] = v1[1] + s0;
    v0[2] = v1[2] + s0;
    v0[3] = v1[3] + s0;
}

static void
native_v4d_adds(oobject_t list, oint32_t ac)
/* float64_t v4d.adds(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_adds(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_subs(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    v0[0] = v1[0] - s0;
    v0[1] = v1[1] - s0;
    v0[2] = v1[2] - s0;
    v0[3] = v1[3] - s0;
}

static void
native_v4d_subs(oobject_t list, oint32_t ac)
/* float64_t v4d.subs(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_subs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_muls(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    v0[0] = v1[0] * s0;
    v0[1] = v1[1] * s0;
    v0[2] = v1[2] * s0;
    v0[3] = v1[3] * s0;
}

static void
native_v4d_muls(oobject_t list, oint32_t ac)
/* float64_t v4d.muls(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_muls(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_divs(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    v0[0] = v1[0] / s0;
    v0[1] = v1[1] / s0;
    v0[2] = v1[2] / s0;
    v0[3] = v1[3] / s0;
}

static void
native_v4d_divs(oobject_t list, oint32_t ac)
/* float64_t v4d.divs(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_divs(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_mods(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    v0[0] = fmod(v1[0], s0);
    v0[1] = fmod(v1[1], s0);
    v0[2] = fmod(v1[2], s0);
    v0[3] = fmod(v1[3], s0);
}

static void
native_v4d_mods(oobject_t list, oint32_t ac)
/* float64_t v4d.mods(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_mods(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_clamp(ofloat64_t v0[4], ofloat64_t v1[4],
	  ofloat64_t minVal, ofloat64_t maxVal)
{
    v0[0] = clamp(minVal, maxVal, v1[0]);
    v0[1] = clamp(minVal, maxVal, v1[1]);
    v0[2] = clamp(minVal, maxVal, v1[2]);
    v0[3] = clamp(minVal, maxVal, v1[3]);
}

static void
native_v4d_clamp(oobject_t list, oint32_t ac)
/* float64_t v4d.clamp(float64_t v0[4], float64_t v1[4],
		       float64_t minVal, float64_t maxVal)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_f64_t	*alist;

    alist = (nat_vec_vec_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_clamp(alist->a0->v.f64, alist->a1->v.f64, alist->a2, alist->a3);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_abs(ofloat64_t v0[4], ofloat64_t v1[4])
{
    v0[0] = fabs(v1[0]);
    v0[1] = fabs(v1[1]);
    v0[2] = fabs(v1[2]);
    v0[3] = fabs(v1[3]);
}

static void
native_v4d_abs(oobject_t list, oint32_t ac)
/* float64_t v4d.abs(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_abs(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_floor(ofloat64_t v0[4], ofloat64_t v1[4])
{
    v0[0] = floor(v1[0]);
    v0[1] = floor(v1[1]);
    v0[2] = floor(v1[2]);
    v0[3] = floor(v1[3]);
}

static void
native_v4d_floor(oobject_t list, oint32_t ac)
/* float64_t v4d.floor(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_floor(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_trunc(ofloat64_t v0[4], ofloat64_t v1[4])
{
    v0[0] = trunc(v1[0]);
    v0[1] = trunc(v1[1]);
    v0[2] = trunc(v1[2]);
    v0[3] = trunc(v1[3]);
}

static void
native_v4d_trunc(oobject_t list, oint32_t ac)
/* float64_t v4d.trunc(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_trunc(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_round(ofloat64_t v0[4], ofloat64_t v1[4])
{
    v0[0] = round(v1[0]);
    v0[1] = round(v1[1]);
    v0[2] = round(v1[2]);
    v0[3] = round(v1[3]);
}

static void
native_v4d_round(oobject_t list, oint32_t ac)
/* float64_t v4d.round(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_round(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_ceil(ofloat64_t v0[4], ofloat64_t v1[4])
{
    v0[0] = ceil(v1[0]);
    v0[1] = ceil(v1[1]);
    v0[2] = ceil(v1[2]);
    v0[3] = ceil(v1[3]);
}

static void
native_v4d_ceil(oobject_t list, oint32_t ac)
/* float64_t v4d.ceil(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_ceil(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_v4d_neg(oobject_t list, oint32_t ac)
/* float64_t v4d.neg(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD4(alist->a1);
    v1 = alist->a1->v.f64;
    v0[0] = -v1[0];
    v0[1] = -v1[1];
    v0[2] = -v1[2];
    v0[3] = -v1[3];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_add(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] + v2[0];
    v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];
    v0[3] = v1[3] + v2[3];
}

static void
native_v4d_add(oobject_t list, oint32_t ac)
/* float64_t v4d.add(float64_t v0[4], float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_add(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_sub(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] - v2[0];
    v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];
    v0[3] = v1[3] - v2[3];
}

static void
native_v4d_sub(oobject_t list, oint32_t ac)
/* float64_t v4d.sub(float64_t v0[4], float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_sub(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_mul(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] * v2[0];
    v0[1] = v1[1] * v2[1];
    v0[2] = v1[2] * v2[2];
    v0[3] = v1[3] * v2[3];
}

static void
native_v4d_mul(oobject_t list, oint32_t ac)
/* float64_t v4d.mul(float64_t v0[4], float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_mul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_div(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] / v2[0];
    v0[1] = v1[1] / v2[1];
    v0[2] = v1[2] / v2[2];
    v0[3] = v1[3] / v2[3];
}

static void
native_v4d_div(oobject_t list, oint32_t ac)
/* float64_t v4d.div(float64_t v0[4], float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_div(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_mod(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = fmod(v1[0], v2[0]);
    v0[1] = fmod(v1[1], v2[1]);
    v0[2] = fmod(v1[2], v2[2]);
    v0[3] = fmod(v1[3], v2[3]);
}

static void
native_v4d_mod(oobject_t list, oint32_t ac)
/* float64_t v4d.mod(float64_t v0[4], float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_mod(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

/*
 * v4 = m4x4 * v4
 */
static void
v4d_m4d_mul_v4d(ofloat64_t v0[4], ofloat64_t v1[16], ofloat64_t v2[3])
{
    v0[0] = A4(v1,0,0) * v2[0] + A4(v1,1,0) * v2[1] + A4(v1,2,0) * v2[2] + A4(v1,3,0) * v2[3];
    v0[1] = A4(v1,0,1) * v2[0] + A4(v1,1,1) * v2[1] + A4(v1,2,1) * v2[2] + A4(v1,3,1) * v2[3];
    v0[2] = A4(v1,0,2) * v2[0] + A4(v1,1,2) * v2[1] + A4(v1,2,2) * v2[2] + A4(v1,3,2) * v2[3];
    v0[3] = A4(v1,0,3) * v2[0] + A4(v1,1,3) * v2[1] + A4(v1,2,3) * v2[2] + A4(v1,3,3) * v2[3];
}

static void
v4d_rotate(ofloat64_t v0[4], ofloat64_t v1[4],
	   ofloat64_t s0, ofloat64_t v2[3])
{
    ofloat64_t		m[16];
    m4d_rotate(m, identity, s0, v2);
    v4d_m4d_mul_v4d(v0, m, v1);
}

static void
native_v4d_rotate(oobject_t list, oint32_t ac)
/* float64_t v4d.rotate(float64_t v0[4], float64_t v1[4],
			   float64_t angle, float64_t normal[3])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_vec_t	*alist;

    alist = (nat_vec_vec_f64_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_VD3(alist->a3);
    v4d_rotate(alist->a0->v.f64, alist->a1->v.f64,
	       alist->a2, alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_rotateX(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			y = v1[1] * _cos - v1[2] * _sin;
    double			z = v1[1] * _sin + v1[2] * _cos;
    if (v0 != v1) {
	v0[0] = v1[0];
	v0[2] = v1[2];
    }
    v0[1] = y;
    v0[2] = z;
}

static void
native_v4d_rotateX(oobject_t list, oint32_t ac)
/* float64_t v4d.rotateX(float64_t v0[4], float64_t v1[4],
			 float64_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_rotateX(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_rotateY(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			x =  v1[0] * _cos + v1[2] * _sin;
    double			z = -v1[0] * _sin + v1[2] * _cos;
    v0[0] = x;
    if (v0 != v1) {
	v0[1] = v1[1];
	v0[3] = v1[3];
    }
    v0[2] = z;
}

static void
native_v4d_rotateY(oobject_t list, oint32_t ac)
/* float64_t v4d.rotateY(float64_t v0[4], float64_t v1[4],
			 float64_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_rotateY(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_rotateZ(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0)
{
    double			_cos = cos(s0);
    double			_sin = sin(s0);
    double			x = v1[0] * _cos - v1[1] * _sin;
    double			y = v1[0] * _sin + v1[1] * _cos;
    v0[0] = x;
    v0[1] = y;
    if (v0 != v1) {
	v0[2] = v1[2];
	v0[3] = v1[3];
    }
}

static void
native_v4d_rotateZ(oobject_t list, oint32_t ac)
/* float64_t v4d.rotateZ(float64_t v0[4], float64_t v1[4],
			 float64_t angle)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    v4d_rotateZ(alist->a0->v.f64, alist->a1->v.f64, alist->a2);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2f_set(oobject_t list, oint32_t ac)
/* float32_t m2f.set(float32_t v0[4], float32_t s0,
		     float32_t s1, float32_t s2, float32_t s3)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_fill(ofloat32_t v0[4], ofloat32_t s0)
{
    v0[0] = v0[1] = v0[2] = v0[3] = s0;
}

static void
native_m2f_fill(oobject_t list, oint32_t ac)
/* float32_t m2f.fill(float32_t v0[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    m2f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_identity(ofloat32_t v0[4])
{
    A2(v0, 0, 1) = A2(v0, 1, 0) = 0.0;
    A2(v0, 0, 0) = A2(v0, 1, 1) = 1.0;
}

static void
native_m2f_identity(oobject_t list, oint32_t ac)
/* float32_t m2f.identity(float32_t v0[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    m2f_identity(alist->a0->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_transpose(oobject_t list, oint32_t ac)
/* float32_t m2f.transpose(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    if (likely(v0 != v1)) {
	A2(v0, 0, 0) = A2(v1, 0, 0);
	A2(v0, 0, 1) = A2(v1, 1, 0);
	A2(v0, 1, 0) = A2(v1, 0, 1);
	A2(v0, 1, 1) = A2(v1, 1, 1);
    }
    else {
	v2 = A3(v1, 0, 1);
	A2(v0, 0, 1) = A2(v1, 1, 0);
	A2(v0, 1, 0) = v2;
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_copy(oobject_t list, oint32_t ac)
/* float32_t m2f.copy(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MF2(alist->a1);
	memcpy(alist->a0->v.f32, alist->a1->v.f32, sizeof(float) * 4);
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
m2f_det_inverse(ofloat32_t v0[4], ofloat32_t v1[4])
{
    ofloat32_t		s00 = A2(v1, 0, 0);
    ofloat32_t		s01 = A2(v1, 0, 1);
    ofloat32_t		s10 = A2(v1, 1, 0);
    ofloat32_t		s11 = A2(v1, 1, 1);
    ofloat32_t		det;

    det = s00 * s11 - s10 * s01;

    if (v0) {
	v0[0] =  s11 / det;
	v0[1] = -s01 / det;
	v0[2] = -s10 / det;
	v0[3] =  s00 / det;
    }

    return (det);
}

static void
native_m2f_det(oobject_t list, oint32_t ac)
/* float64_t m2f.det(float32_t v0[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MF2(alist->a0);
    r0->v.d = m2f_det_inverse(null, alist->a0->v.f32);
}

static void
native_m2f_adds(oobject_t list, oint32_t ac)
/* float32_t m2f.adds(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] + v2;		v0[1] = v1[1] + v2;
    v0[2] = v1[2] + v2;		v0[3] = v1[3] + v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_subs(oobject_t list, oint32_t ac)
/* float32_t m2f.subs(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] - v2;		v0[1] = v1[1] - v2;
    v0[2] = v1[2] - v2;		v0[3] = v1[3] - v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_muls(oobject_t list, oint32_t ac)
/* float32_t m2f.muls(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] * v2;		v0[1] = v1[1] * v2;
    v0[2] = v1[2] * v2;		v0[3] = v1[3] * v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_divs(oobject_t list, oint32_t ac)
/* float32_t m2f.divs(float32_t v0[4], float32_t v1[4], float32_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] / v2;		v0[1] = v1[1] / v2;
    v0[2] = v1[2] / v2;		v0[3] = v1[3] / v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
v4d_mulm(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[16])
{
    ofloat64_t		a0, a1, a2, a3;

    a0 = v1[0];		a1 = v1[1];
    a2 = v1[2];		a3 = v1[3];
    v0[0] = (a0 * A4(v2, 0, 0) + a1 * A4(v2, 1, 0) +
	     a2 * A4(v2, 2, 0) + a3 * A4(v2, 3, 0));
    v0[1] = (a0 * A4(v2, 0, 1) + a1 * A4(v2, 1, 1) +
	     a2 * A4(v2, 2, 1) + a3 * A4(v2, 3, 1));
    v0[2] = (a0 * A4(v2, 0, 2) + a1 * A4(v2, 1, 2) +
	     a2 * A4(v2, 2, 2) + a3 * A4(v2, 3, 2));
    v0[3] = (a0 * A4(v2, 0, 3) + a1 * A4(v2, 1, 3) +
	     a2 * A4(v2, 2, 3) + a3 * A4(v2, 3, 3));
}

static void
native_v4d_mulm(oobject_t list, oint32_t ac)
/* float64_t v4d.mulm(float64_t v0[4], float64_t v1[4],
		      float64_t v2[16])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_VD4(alist->a1);
    CHECK_MD4(alist->a2);
    v4d_mulm(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
v4d_mmul(ofloat64_t v0[4], ofloat64_t v1[16], ofloat64_t v2[4])
{
    ofloat64_t		a0, a1, a2, a3;

    a0 = v2[0];		a1 = v2[1];
    a2 = v2[2];		a3 = v2[3];
    v0[0] = (A4(v1, 0, 0) * a0 + A4(v1, 0, 1) * a1 +
	     A4(v1, 0, 2) * a2 + A4(v1, 0, 3) * a3);
    v0[1] = (A4(v1, 1, 0) * a0 + A4(v1, 1, 1) * a1 +
	     A4(v1, 1, 2) * a2 + A4(v1, 1, 3) * a3);
    v0[2] = (A4(v1, 2, 0) * a0 + A4(v1, 2, 1) * a1 +
	     A4(v1, 2, 2) * a2 + A4(v1, 2, 3) * a3);
    v0[3] = (A4(v1, 3, 0) * a0 + A4(v1, 3, 1) * a1 +
	     A4(v1, 3, 2) * a2 + A4(v1, 3, 3) * a3);
}

static void
native_v4d_mmul(oobject_t list, oint32_t ac)
/* float64_t v4d.mmul(float64_t v0[4], float64_t v1[16],
		      float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_VD4(alist->a2);
    v4d_mmul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2f_inverse(oobject_t list, oint32_t ac)
/* float32_t m2f.inverse(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    CHECK_MF2(alist->a1);
    (void)m2f_det_inverse(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2f_eq(oobject_t list, oint32_t ac)
/* uint8_t m2f.eq(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF2(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] == v1[2] && v0[3] == v1[3];
    }
    else
	r0->v.w = true;
}

static void
native_m2f_ne(oobject_t list, oint32_t ac)
/* uint8_t m2f.ne(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF2(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3];
    }
    else
	r0->v.w = false;
}

static void
native_m2f_neg(oobject_t list, oint32_t ac)
/* float32_t m2f.neg(float32_t v0[4], float32_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF2(alist->a1);
    v1 = alist->a1->v.f32;
    v0[0] = -v1[0];	v0[1] = -v1[1];
    v0[2] = -v1[2];	v0[3] = -v1[3];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_add(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] + v2[0];	v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];	v0[3] = v1[3] + v2[3];
}

static void
native_m2f_add(oobject_t list, oint32_t ac)
/* float32_t m2f.add(float32_t v0[4],
		     float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    CHECK_MF2(alist->a1);
    CHECK_MF2(alist->a2);
    m2f_add(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_sub(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    v0[0] = v1[0] - v2[0];	v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];	v0[3] = v1[3] - v2[3];
}

static void
native_m2f_sub(oobject_t list, oint32_t ac)
/* float32_t m2f.sub(float32_t v0[4],
		     float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    CHECK_MF2(alist->a1);
    CHECK_MF2(alist->a2);
    m2f_sub(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_mul(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    float		a00, a01;
    float		a10, a11;
    float		b00, b01;
    float		b10, b11;

    a00 = A2(v1, 0, 0);		a01 = A2(v1, 0, 1);
    a10 = A2(v1, 1, 0);		a11 = A2(v1, 1, 1);
    b00 = A2(v2, 0, 0);		b01 = A2(v2, 0, 1);
    b10 = A2(v2, 1, 0);		b11 = A2(v2, 1, 1);

    A2(v0, 0, 0) = a00 * b00 + a10 * b01;
    A2(v0, 0, 1) = a01 * b00 + a11 * b01;
    A2(v0, 1, 0) = a00 * b10 + a10 * b11;
    A2(v0, 1, 1) = a01 * b10 + a11 * b11;
}

static void
native_m2f_mul(oobject_t list, oint32_t ac)
/* float32_t m2f.mul(float32_t v0[4],
		     float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    CHECK_MF2(alist->a1);
    CHECK_MF2(alist->a2);
    m2f_mul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m2f_div(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4])
{
    ofloat32_t			 inv[4];

    (void)m2f_det_inverse(inv, v2);
    m2f_mul(v0, v1, inv);
}

static void
native_m2f_div(oobject_t list, oint32_t ac)
/* float32_t m2f.div(float32_t v0[4],
		     float32_t v1[4], float32_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF2(alist->a0);
    CHECK_MF2(alist->a1);
    CHECK_MF2(alist->a2);
    m2f_div(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_set(oobject_t list, oint32_t ac)
/* float32_t m3f.set(float32_t v0[9],
		     float32_t s0, float32_t s1, float32_t s2,
		     float32_t s3, float32_t s4, float32_t s5,
		     float32_t s6, float32_t s7, float32_t s8)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_f32_f32_f32_f32_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_f32_f32_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    v0[4] = alist->a5;
    v0[5] = alist->a6;
    v0[6] = alist->a7;
    v0[7] = alist->a8;
    v0[8] = alist->a9;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_fill(ofloat32_t v0[9], ofloat32_t s0)
{
    v0[0] = v0[1] = v0[2] =
    v0[3] = v0[4] = v0[5] =
    v0[6] = v0[7] = v0[8] = s0;
}

static void
native_m3f_fill(oobject_t list, oint32_t ac)
/* float32_t m3f.fill(float32_t v0[9], float32_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    m3f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_identity(ofloat32_t v0[9])
{
    A3(v0, 0, 1) = A3(v0, 0, 2) = A3(v0, 1, 0) =
    A3(v0, 1, 2) = A3(v0, 2, 0) = A3(v0, 2, 1) = 0.0;
    A3(v0, 0, 0) = A3(v0, 1, 1) = A3(v0, 2, 2) = 1.0;
}

static void
native_m3f_identity(oobject_t list, oint32_t ac)
/* float32_t m3f.identity(float32_t v0[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    m3f_identity(alist->a0->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_transpose(oobject_t list, oint32_t ac)
/* float32_t m3f.transpose(float32_t v0[9], float32_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    if (likely(v0 != v1)) {
	A3(v0, 0, 0) = A3(v1, 0, 0);
	A3(v0, 0, 1) = A3(v1, 1, 0);
	A3(v0, 0, 2) = A3(v1, 2, 0);
	A3(v0, 1, 0) = A3(v1, 0, 1);
	A3(v0, 1, 1) = A3(v1, 1, 1);
	A3(v0, 1, 2) = A3(v1, 2, 1);
	A3(v0, 2, 0) = A3(v1, 0, 2);
	A3(v0, 2, 1) = A3(v1, 1, 2);
	A3(v0, 2, 2) = A3(v1, 2, 2);
    }
    else {
	v2 = A3(v1, 0, 1);
	A3(v0, 0, 1) = A3(v1, 1, 0);
	A3(v0, 1, 0) = v2;
	v2 = A3(v1, 0, 2);
	A3(v0, 0, 2) = A3(v1, 2, 0);
	A3(v0, 2, 0) = v2;
	v2 = A3(v1, 1, 2);
	A3(v0, 1, 2) = A3(v1, 2, 1);
	A3(v0, 2, 1) = v2;
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_copy(oobject_t list, oint32_t ac)
/* float32_t m3f.copy(float32_t v0[9], float32_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MF3(alist->a1);
	memcpy(alist->a0->v.f32, alist->a1->v.f32, sizeof(float) * 9);
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
m3f_det_inverse(ofloat32_t v0[9], ofloat32_t v1[9])
{
    ofloat32_t		s00 = A3(v1, 0, 0);
    ofloat32_t		s01 = A3(v1, 0, 1);
    ofloat32_t		s02 = A3(v1, 0, 2);
    ofloat32_t		s10 = A3(v1, 1, 0);
    ofloat32_t		s11 = A3(v1, 1, 1);
    ofloat32_t		s12 = A3(v1, 1, 2);
    ofloat32_t		s20 = A3(v1, 2, 0);
    ofloat32_t		s21 = A3(v1, 2, 1);
    ofloat32_t		s22 = A3(v1, 2, 2);
    ofloat32_t		inv[9];
    ofloat32_t		det;

    if (v0) {
	inv[0] = s11 * s22 - s12 * s21;
	inv[1] = s21 * s02 - s22 * s01;
	inv[2] = s01 * s12 - s02 * s11;
	inv[3] = s20 * s12 - s10 * s22;
	inv[4] = s00 * s22 - s20 * s02;
	inv[5] = s10 * s02 - s00 * s12;
	inv[6] = s21 * s10 - s11 * s20;
	inv[7] = s01 * s20 - s21 * s00;
	inv[8] = s11 * s00 - s01 * s10;
    }

    det = s00 * (s11 * s22 - s12 * s21) -
	  s01 * (s10 * s22 - s12 * s20) +
	  s02 * (s10 * s21 - s11 * s20);

    if (v0) {
	v0[0] = inv[0] / det;
	v0[1] = inv[1] / det;
	v0[2] = inv[2] / det;
	v0[3] = inv[3] / det;
	v0[4] = inv[4] / det;
	v0[5] = inv[5] / det;
	v0[6] = inv[6] / det;
	v0[7] = inv[7] / det;
	v0[8] = inv[8] / det;
    }

    return (det);
}

static void
native_m3f_det(oobject_t list, oint32_t ac)
/* float64_t m3f.det(float32_t v0[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MF3(alist->a0);
    r0->v.d = m3f_det_inverse(null, alist->a0->v.f32);
}

static void
native_m3f_adds(oobject_t list, oint32_t ac)
/* float32_t m3f.adds(float32_t v0[9], float32_t v1[9], float32_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] + v2;		v0[1] = v1[1] + v2;
    v0[2] = v1[2] + v2;		v0[3] = v1[3] + v2;
    v0[4] = v1[4] + v2;		v0[5] = v1[5] + v2;
    v0[6] = v1[6] + v2;		v0[7] = v1[7] + v2;
    v0[8] = v1[8] + v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_subs(oobject_t list, oint32_t ac)
/* float32_t m3f.subs(float32_t v0[9], float32_t v1[9], float32_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] - v2;		v0[1] = v1[1] - v2;
    v0[2] = v1[2] - v2;		v0[3] = v1[3] - v2;
    v0[4] = v1[4] - v2;		v0[5] = v1[5] - v2;
    v0[6] = v1[6] - v2;		v0[7] = v1[7] - v2;
    v0[8] = v1[8] - v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_muls(oobject_t list, oint32_t ac)
/* float32_t m3f.muls(float32_t v0[9], float32_t v1[9], float32_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] * v2;		v0[1] = v1[1] * v2;
    v0[2] = v1[2] * v2;		v0[3] = v1[3] * v2;
    v0[4] = v1[4] * v2;		v0[5] = v1[5] * v2;
    v0[6] = v1[6] * v2;		v0[7] = v1[7] * v2;
    v0[8] = v1[8] * v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_divs(oobject_t list, oint32_t ac)
/* float32_t m3f.divs(float32_t v0[9], float32_t v1[9], float32_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[0] = v1[0] / v2;		v0[1] = v1[1] / v2;
    v0[2] = v1[2] / v2;		v0[3] = v1[3] / v2;
    v0[4] = v1[4] / v2;		v0[5] = v1[5] / v2;
    v0[6] = v1[6] / v2;		v0[7] = v1[7] / v2;
    v0[8] = v1[8] / v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_inverse(oobject_t list, oint32_t ac)
/* float32_t m3f.inverse(float32_t v0[9], float32_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    CHECK_MF3(alist->a1);
    (void)m3f_det_inverse(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m3f_eq(oobject_t list, oint32_t ac)
/* uint8_t m3f.eq(float32_t v0[9], float32_t v1[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF3(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] == v1[2] && v0[3] == v1[3] &&
		  v0[4] == v1[4] && v0[5] == v1[5] &&
		  v0[6] == v1[6] && v0[7] == v1[7] &&
		  v0[8] == v1[8];
    }
    else
	r0->v.w = true;
}

static void
native_m3f_ne(oobject_t list, oint32_t ac)
/* uint8_t m3f.ne(float32_t v0[9], float32_t v1[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF3(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3] ||
		  v0[4] != v1[4] || v0[5] != v1[5] ||
		  v0[6] != v1[6] || v0[7] != v1[7] ||
		  v0[8] != v1[8];
    }
    else
	r0->v.w = false;
}

static void
native_m3f_neg(oobject_t list, oint32_t ac)
/* float32_t m3f.neg(float32_t v0[9], float32_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF3(alist->a1);
    v1 = alist->a1->v.f32;
    v0[0] = -v1[0];	v0[1] = -v1[1];		v0[2] = -v1[2];
    v0[3] = -v1[3];	v0[4] = -v1[4];		v0[5] = -v1[5];
    v0[6] = -v1[6];	v0[7] = -v1[7];		v0[8] = -v1[8];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_add(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9])
{
    v0[0] = v1[0] + v2[0];	v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];	v0[3] = v1[3] + v2[3];
    v0[4] = v1[4] + v2[4];	v0[5] = v1[5] + v2[5];
    v0[6] = v1[6] + v2[6];	v0[7] = v1[7] + v2[7];
    v0[8] = v1[8] + v2[8];
}

static void
native_m3f_add(oobject_t list, oint32_t ac)
/* float32_t m3f.add(float32_t v0[9],
		     float32_t v1[9], float32_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    CHECK_MF3(alist->a1);
    CHECK_MF3(alist->a2);
    m3f_add(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_sub(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9])
{
    v0[0] = v1[0] - v2[0];	v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];	v0[3] = v1[3] - v2[3];
    v0[4] = v1[4] - v2[4];	v0[5] = v1[5] - v2[5];
    v0[6] = v1[6] - v2[6];	v0[7] = v1[7] - v2[7];
    v0[8] = v1[8] - v2[8];
}

static void
native_m3f_sub(oobject_t list, oint32_t ac)
/* float32_t m3f.sub(float32_t v0[9],
		     float32_t v1[9], float32_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    CHECK_MF3(alist->a1);
    CHECK_MF3(alist->a2);
    m3f_sub(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_mul(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9])
{
    float		a00, a01, a02;
    float		a10, a11, a12;
    float		a20, a21, a22;
    float		b00, b01, b02;
    float		b10, b11, b12;
    float		b20, b21, b22;

    a00 = A3(v1, 0, 0);		a01 = A3(v1, 0, 1);	a02 = A3(v1, 0, 2);
    a10 = A3(v1, 1, 0);		a11 = A3(v1, 1, 1);	a12 = A3(v1, 1, 2);
    a20 = A3(v1, 2, 0);		a21 = A3(v1, 2, 1);	a22 = A3(v1, 2, 2);
    b00 = A3(v2, 0, 0);		b01 = A3(v2, 0, 1);	b02 = A3(v2, 0, 2);
    b10 = A3(v2, 1, 0);		b11 = A3(v2, 1, 1);	b12 = A3(v2, 1, 2);
    b20 = A3(v2, 2, 0);		b21 = A3(v2, 2, 1);	b22 = A3(v2, 2, 2);

    A3(v0, 0, 0) = a00 * b00 + a10 * b01 + a20 * b02;
    A3(v0, 0, 1) = a01 * b00 + a11 * b01 + a21 * b02;
    A3(v0, 0, 2) = a02 * b00 + a12 * b01 + a22 * b02;
    A3(v0, 1, 0) = a00 * b10 + a10 * b11 + a20 * b12;
    A3(v0, 1, 1) = a01 * b10 + a11 * b11 + a21 * b12;
    A3(v0, 1, 2) = a02 * b10 + a12 * b11 + a22 * b12;
    A3(v0, 2, 0) = a00 * b20 + a10 * b21 + a20 * b22;
    A3(v0, 2, 1) = a01 * b20 + a11 * b21 + a21 * b22;
    A3(v0, 2, 2) = a02 * b20 + a12 * b21 + a22 * b22;
}

static void
native_m3f_mul(oobject_t list, oint32_t ac)
/* float32_t m3f.mul(float32_t v0[9],
		     float32_t v1[9], float32_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    CHECK_MF3(alist->a1);
    CHECK_MF3(alist->a2);
    m3f_mul(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m3f_div(ofloat32_t v0[9], ofloat32_t v1[9], ofloat32_t v2[9])
{
    ofloat32_t			 inv[9];

    (void)m3f_det_inverse(inv, v2);
    m3f_mul(v0, v1, inv);
}

static void
native_m3f_div(oobject_t list, oint32_t ac)
/* float32_t m3f.div(float32_t v0[9],
		     float32_t v1[9], float32_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF3(alist->a0);
    CHECK_MF3(alist->a1);
    CHECK_MF3(alist->a2);
    m3f_div(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
   r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_set(oobject_t list, oint32_t ac)
/* float32_t m4f.set(float32_t v0[16],
		     float32_t  s0, float32_t  s1, float32_t  s2, float32_t  s3,
		     float32_t  s4, float32_t  s5, float32_t  s6, float32_t  s7,
		     float32_t  s8, float32_t  s9, float32_t s10, float32_t s11,
		     float32_t s12, float32_t s13, float32_t s14, float32_t s15)
		    [16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    float			*v0;
    nat_vec_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    v0[ 0] = alist->a1;
    v0[ 1] = alist->a2;
    v0[ 2] = alist->a3;
    v0[ 3] = alist->a4;
    v0[ 4] = alist->a5;
    v0[ 5] = alist->a6;
    v0[ 6] = alist->a7;
    v0[ 7] = alist->a8;
    v0[ 8] = alist->a9;
    v0[ 9] = alist->a10;
    v0[10] = alist->a11;
    v0[11] = alist->a12;
    v0[12] = alist->a13;
    v0[13] = alist->a14;
    v0[14] = alist->a15;
    v0[15] = alist->a16;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_fill(ofloat32_t v0[16], ofloat32_t s0)
{
    v0[ 0] = v0[ 1] = v0[ 2] = v0[ 3] =
    v0[ 4] = v0[ 5] = v0[ 6] = v0[ 7] =
    v0[ 8] = v0[ 9] = v0[10] = v0[11] =
    v0[12] = v0[13] = v0[14] = v0[15] =	s0;
}

static void
native_m4f_fill(oobject_t list, oint32_t ac)
/* float32_t m4f.fill(float32_t v0[16], float32_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_t		*alist;

    alist = (nat_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    m4f_fill(alist->a0->v.f32, alist->a1);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_identity(ofloat32_t v0[16])
{
    v0[ 1] = v0[ 2] = v0[ 3] = v0[ 4] =
    v0[ 6] = v0[ 7] = v0[ 8] = v0[ 9] =
    v0[11] = v0[12] = v0[13] = v0[14] = 0.0;
    v0[ 0] = v0[ 5] = v0[10] = v0[15] = 1.0;
}

static void
native_m4f_identity(oobject_t list, oint32_t ac)
/* float32_t m4d.identity(float32_t v0[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    m4f_identity(alist->a0->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_transpose(oobject_t list, oint32_t ac)
/* float32_t m4f.transpose(float32_t v0[16], float32_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    if (likely(v0 != v1)) {
	A4(v0, 0, 0) = A4(v1, 0, 0);
	A4(v0, 0, 1) = A4(v1, 1, 0);
	A4(v0, 0, 2) = A4(v1, 2, 0);
	A4(v0, 0, 3) = A4(v1, 3, 0);
	A4(v0, 1, 0) = A4(v1, 0, 1);
	A4(v0, 1, 1) = A4(v1, 1, 1);
	A4(v0, 1, 2) = A4(v1, 2, 1);
	A4(v0, 1, 3) = A4(v1, 3, 1);
	A4(v0, 2, 0) = A4(v1, 0, 2);
	A4(v0, 2, 1) = A4(v1, 1, 2);
	A4(v0, 2, 2) = A4(v1, 2, 2);
	A4(v0, 2, 3) = A4(v1, 3, 2);
	A4(v0, 3, 0) = A4(v1, 0, 3);
	A4(v0, 3, 1) = A4(v1, 1, 3);
	A4(v0, 3, 2) = A4(v1, 2, 3);
	A4(v0, 3, 3) = A4(v1, 3, 3);
    }
    else {
	v2 = A4(v1, 0, 1);
	A4(v0, 0, 1) = A4(v1, 1, 0);
	A4(v0, 1, 0) = v2;
	v2 = A4(v1, 0, 2);
	A4(v0, 0, 2) = A4(v1, 2, 0);
	A4(v0, 2, 0) = v2;
	v2 = A4(v1, 0, 3);
	A4(v0, 0, 3) = A4(v1, 3, 0);
	A4(v0, 3, 0) = v2;
	v2 = A4(v1, 1, 2);
	A4(v0, 1, 2) = A4(v1, 2, 1);
	A4(v0, 2, 1) = v2;
	v2 = A4(v1, 1, 3);
	A4(v0, 1, 3) = A4(v1, 3, 1);
	A4(v0, 3, 1) = v2;
	v2 = A4(v1, 2, 3);
	A4(v0, 2, 3) = A4(v1, 3, 2);
	A4(v0, 3, 2) = v2;
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_copy(oobject_t list, oint32_t ac)
/* float32_t m4f.copy(float32_t v0[16], float32_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MF4(alist->a1);
	memcpy(alist->a0->v.f32, alist->a1->v.f32, sizeof(float) * 16);
    }
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static ofloat32_t
m4f_det_inverse(ofloat32_t v0[16], ofloat32_t v1[16])
{
    ofloat32_t sf00 = A4(v1, 2, 2) * A4(v1, 3, 3) - A4(v1, 2, 3) * A4(v1, 3, 2);
    ofloat32_t sf01 = A4(v1, 1, 2) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 2);
    ofloat32_t sf02 = A4(v1, 1, 2) * A4(v1, 2, 3) - A4(v1, 1, 3) * A4(v1, 2, 2);
    ofloat32_t sf03 = A4(v1, 0, 2) * A4(v1, 3, 3) - A4(v1, 0, 3) * A4(v1, 3, 2);
    ofloat32_t sf04 = A4(v1, 0, 2) * A4(v1, 2, 3) - A4(v1, 0, 3) * A4(v1, 2, 2);
    ofloat32_t sf05 = A4(v1, 0, 2) * A4(v1, 1, 3) - A4(v1, 0, 3) * A4(v1, 1, 2);
    ofloat32_t sf06 = A4(v1, 2, 1) * A4(v1, 3, 3) - A4(v1, 2, 3) * A4(v1, 3, 1);
    ofloat32_t sf07 = A4(v1, 1, 1) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 1);
    ofloat32_t sf08 = A4(v1, 1, 1) * A4(v1, 2, 3) - A4(v1, 1, 3) * A4(v1, 2, 1);
    ofloat32_t sf09 = A4(v1, 0, 1) * A4(v1, 3, 3) - A4(v1, 0, 3) * A4(v1, 3, 1);
    ofloat32_t sf10 = A4(v1, 0, 1) * A4(v1, 2, 3) - A4(v1, 0, 3) * A4(v1, 2, 1);
    ofloat32_t sf11 = A4(v1, 1, 1) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 1);
    ofloat32_t sf12 = A4(v1, 0, 1) * A4(v1, 1, 3) - A4(v1, 0, 3) * A4(v1, 1, 1);
    ofloat32_t sf13 = A4(v1, 2, 1) * A4(v1, 3, 2) - A4(v1, 2, 2) * A4(v1, 3, 1);
    ofloat32_t sf14 = A4(v1, 1, 1) * A4(v1, 3, 2) - A4(v1, 1, 2) * A4(v1, 3, 1);
    ofloat32_t sf15 = A4(v1, 1, 1) * A4(v1, 2, 2) - A4(v1, 1, 2) * A4(v1, 2, 1);
    ofloat32_t sf16 = A4(v1, 0, 1) * A4(v1, 3, 2) - A4(v1, 0, 2) * A4(v1, 3, 1);
    ofloat32_t sf17 = A4(v1, 0, 1) * A4(v1, 2, 2) - A4(v1, 0, 2) * A4(v1, 2, 1);
    ofloat32_t sf18 = A4(v1, 0, 1) * A4(v1, 1, 2) - A4(v1, 0, 2) * A4(v1, 1, 1);
    ofloat32_t		inv[16];
    ofloat32_t		det;

    inv[ 0] = + A4(v1, 1, 1) * sf00 - A4(v1, 2, 1) * sf01 + A4(v1, 3, 1) * sf02;
    inv[ 1] = - A4(v1, 0, 1) * sf00 + A4(v1, 2, 1) * sf03 - A4(v1, 3, 1) * sf04;
    inv[ 2] = + A4(v1, 0, 1) * sf01 - A4(v1, 1, 1) * sf03 + A4(v1, 3, 1) * sf05;
    inv[ 3] = - A4(v1, 0, 1) * sf02 + A4(v1, 1, 1) * sf04 - A4(v1, 2, 1) * sf05;
    inv[ 4] = - A4(v1, 1, 0) * sf00 + A4(v1, 2, 0) * sf01 - A4(v1, 3, 0) * sf02;
    inv[ 5] = + A4(v1, 0, 0) * sf00 - A4(v1, 2, 0) * sf03 + A4(v1, 3, 0) * sf04;
    inv[ 6] = - A4(v1, 0, 0) * sf01 + A4(v1, 1, 0) * sf03 - A4(v1, 3, 0) * sf05;
    inv[ 7] = + A4(v1, 0, 0) * sf02 - A4(v1, 1, 0) * sf04 + A4(v1, 2, 0) * sf05;
    inv[ 8] = + A4(v1, 1, 0) * sf06 - A4(v1, 2, 0) * sf07 + A4(v1, 3, 0) * sf08;
    inv[ 9] = - A4(v1, 0, 0) * sf06 + A4(v1, 2, 0) * sf09 - A4(v1, 3, 0) * sf10;
    inv[10] = + A4(v1, 0, 0) * sf11 - A4(v1, 1, 0) * sf09 + A4(v1, 3, 0) * sf12;
    inv[11] = - A4(v1, 0, 0) * sf08 + A4(v1, 1, 0) * sf10 - A4(v1, 2, 0) * sf12;
    inv[12] = - A4(v1, 1, 0) * sf13 + A4(v1, 2, 0) * sf14 - A4(v1, 3, 0) * sf15;
    inv[13] = + A4(v1, 0, 0) * sf13 - A4(v1, 2, 0) * sf16 + A4(v1, 3, 0) * sf17;
    inv[14] = - A4(v1, 0, 0) * sf14 + A4(v1, 1, 0) * sf16 - A4(v1, 3, 0) * sf18;
    inv[15] = + A4(v1, 0, 0) * sf15 - A4(v1, 1, 0) * sf17 + A4(v1, 2, 0) * sf18;
    det = A4(v1, 0, 0) * A4(inv, 0, 0) +
	  A4(v1, 1, 0) * A4(inv, 0, 1) +
	  A4(v1, 2, 0) * A4(inv, 0, 2) +
	  A4(v1, 3, 0) * A4(inv, 0, 3);

    if (v0) {
	v0[ 0] = inv[ 0] / det;
	v0[ 1] = inv[ 1] / det;
	v0[ 2] = inv[ 2] / det;
	v0[ 3] = inv[ 3] / det;
	v0[ 4] = inv[ 4] / det;
	v0[ 5] = inv[ 5] / det;
	v0[ 6] = inv[ 6] / det;
	v0[ 7] = inv[ 7] / det;
	v0[ 8] = inv[ 8] / det;
	v0[ 9] = inv[ 9] / det;
	v0[10] = inv[10] / det;
	v0[11] = inv[11] / det;
	v0[12] = inv[12] / det;
	v0[13] = inv[13] / det;
	v0[14] = inv[14] / det;
	v0[15] = inv[15] / det;
    }

    return (det);
}

static void
native_m4f_det(oobject_t list, oint32_t ac)
/* float64_t m4f.det(float32_t v0[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MF4(alist->a0);
    r0->v.d = m4f_det_inverse(null, alist->a0->v.f32);
}

static void
native_m4f_adds(oobject_t list, oint32_t ac)
/* float32_t m4f.adds(float32_t v0[16], float32_t v1[16], float32_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] + v2;	v0[ 1] = v1[ 1] + v2;
    v0[ 2] = v1[ 2] + v2;	v0[ 3] = v1[ 3] + v2;
    v0[ 4] = v1[ 4] + v2;	v0[ 5] = v1[ 5] + v2;
    v0[ 6] = v1[ 6] + v2;	v0[ 7] = v1[ 7] + v2;
    v0[ 8] = v1[ 8] + v2;	v0[ 9] = v1[ 9] + v2;
    v0[10] = v1[10] + v2;	v0[11] = v1[11] + v2;
    v0[12] = v1[12] + v2;	v0[13] = v1[13] + v2;
    v0[14] = v1[14] + v2;	v0[15] = v1[15] + v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_subs(oobject_t list, oint32_t ac)
/* float32_t m4f.subs(float32_t v0[16], float32_t v1[16], float32_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] - v2;	v0[ 1] = v1[ 1] - v2;
    v0[ 2] = v1[ 2] - v2;	v0[ 3] = v1[ 3] - v2;
    v0[ 4] = v1[ 4] - v2;	v0[ 5] = v1[ 5] - v2;
    v0[ 6] = v1[ 6] - v2;	v0[ 7] = v1[ 7] - v2;
    v0[ 8] = v1[ 8] - v2;	v0[ 9] = v1[ 9] - v2;
    v0[10] = v1[10] - v2;	v0[11] = v1[11] - v2;
    v0[12] = v1[12] - v2;	v0[13] = v1[13] - v2;
    v0[14] = v1[14] - v2;	v0[15] = v1[15] - v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_muls(oobject_t list, oint32_t ac)
/* float32_t m4f.muls(float32_t v0[16], float32_t v1[16], float32_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] * v2;	v0[ 1] = v1[ 1] * v2;
    v0[ 2] = v1[ 2] * v2;	v0[ 3] = v1[ 3] * v2;
    v0[ 4] = v1[ 4] * v2;	v0[ 5] = v1[ 5] * v2;
    v0[ 6] = v1[ 6] * v2;	v0[ 7] = v1[ 7] * v2;
    v0[ 8] = v1[ 8] * v2;	v0[ 9] = v1[ 9] * v2;
    v0[10] = v1[10] * v2;	v0[11] = v1[11] * v2;
    v0[12] = v1[12] * v2;	v0[13] = v1[13] * v2;
    v0[14] = v1[14] * v2;	v0[15] = v1[15] * v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_divs(oobject_t list, oint32_t ac)
/* float32_t m4f.divs(float32_t v0[16], float32_t v1[16], float32_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_t		*alist;
    float			*v0, *v1, v2;

    alist = (nat_vec_vec_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] / v2;	v0[ 1] = v1[ 1] / v2;
    v0[ 2] = v1[ 2] / v2;	v0[ 3] = v1[ 3] / v2;
    v0[ 4] = v1[ 4] / v2;	v0[ 5] = v1[ 5] / v2;
    v0[ 6] = v1[ 6] / v2;	v0[ 7] = v1[ 7] / v2;
    v0[ 8] = v1[ 8] / v2;	v0[ 9] = v1[ 9] / v2;
    v0[10] = v1[10] / v2;	v0[11] = v1[11] / v2;
    v0[12] = v1[12] / v2;	v0[13] = v1[13] / v2;
    v0[14] = v1[14] / v2;	v0[15] = v1[15] / v2;
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_inverse(oobject_t list, oint32_t ac)
/* float32_t m4f.inverse(float32_t v0[16], float32_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    (void)m4f_det_inverse(alist->a0->v.f32, alist->a1->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m4f_eq(oobject_t list, oint32_t ac)
/* uint8_t m4f.eq(float32_t v0[16], float32_t v1[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF4(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[ 0] == v1[ 0] && v0[ 1] == v1[ 1] &&
		  v0[ 2] == v1[ 2] && v0[ 3] == v1[ 3] &&
		  v0[ 4] == v1[ 4] && v0[ 5] == v1[ 5] &&
		  v0[ 6] == v1[ 6] && v0[ 7] == v1[ 7] &&
		  v0[ 8] == v1[ 8] && v0[ 9] == v1[ 9] &&
		  v0[10] == v1[10] && v0[11] == v1[11] &&
		  v0[12] == v1[12] && v0[13] == v1[13] &&
		  v0[14] == v1[14] && v0[15] == v1[15];
    }
    else
	r0->v.w = true;
}

static void
native_m4f_ne(oobject_t list, oint32_t ac)
/* uint8_t m4f.ne(float32_t v0[16], float32_t v1[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat32_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MF4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f32;
	CHECK_MF4(alist->a1);
	v1 = alist->a1->v.f32;
	r0->v.w = v0[ 0] != v1[ 0] || v0[ 1] != v1[ 1] ||
		  v0[ 2] != v1[ 2] || v0[ 3] != v1[ 3] ||
		  v0[ 4] != v1[ 4] || v0[ 5] != v1[ 5] ||
		  v0[ 6] != v1[ 6] || v0[ 7] != v1[ 7] ||
		  v0[ 8] != v1[ 8] || v0[ 9] != v1[ 9] ||
		  v0[10] != v1[10] || v0[11] != v1[11] ||
		  v0[12] != v1[12] || v0[13] != v1[13] ||
		  v0[14] != v1[14] || v0[15] != v1[15];
    }
    else
	r0->v.w = false;
}

static void
native_m4f_neg(oobject_t list, oint32_t ac)
/* float32_t m4f.neg(float32_t v0[16], float32_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_MF4(alist->a1);
    v1 = alist->a1->v.f32;
    v0[ 0] = -v1[ 0]; v0[ 1] = -v1[ 1]; v0[ 2] = -v1[ 2]; v0[ 3] = -v1[ 3];
    v0[ 4] = -v1[ 4]; v0[ 5] = -v1[ 5]; v0[ 6] = -v1[ 6]; v0[ 7] = -v1[ 7];
    v0[ 8] = -v1[ 8]; v0[ 9] = -v1[ 9]; v0[10] = -v1[10]; v0[11] = -v1[11];
    v0[12] = -v1[12]; v0[13] = -v1[13]; v0[14] = -v1[14]; v0[15] = -v1[15];
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_add(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16])
{
    v0[ 0] = v1[ 0] + v2[ 0];	v0[ 1] = v1[ 1] + v2[ 1];
    v0[ 2] = v1[ 2] + v2[ 2];	v0[ 3] = v1[ 3] + v2[ 3];
    v0[ 4] = v1[ 4] + v2[ 4];	v0[ 5] = v1[ 5] + v2[ 5];
    v0[ 6] = v1[ 6] + v2[ 6];	v0[ 7] = v1[ 7] + v2[ 7];
    v0[ 8] = v1[ 8] + v2[ 8];	v0[ 9] = v1[ 9] + v2[ 9];
    v0[10] = v1[10] + v2[10];	v0[11] = v1[11] + v2[11];
    v0[12] = v1[12] + v2[12];	v0[13] = v1[13] + v2[13];
    v0[14] = v1[14] + v2[14];	v0[15] = v1[15] + v2[15];
}

static void
native_m4f_add(oobject_t list, oint32_t ac)
/* float32_t m4f.add(float32_t v0[16],
		     float32_t v1[16], float32_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_MF4(alist->a2);
    m4f_add(alist->a0->v.f32,  alist->a1->v.f32,  alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_sub(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16])
{
    v0[ 0] = v1[ 0] - v2[ 0];	v0[ 1] = v1[ 1] - v2[ 1];
    v0[ 2] = v1[ 2] - v2[ 2];	v0[ 3] = v1[ 3] - v2[ 3];
    v0[ 4] = v1[ 4] - v2[ 4];	v0[ 5] = v1[ 5] - v2[ 5];
    v0[ 6] = v1[ 6] - v2[ 6];	v0[ 7] = v1[ 7] - v2[ 7];
    v0[ 8] = v1[ 8] - v2[ 8];	v0[ 9] = v1[ 9] - v2[ 9];
    v0[10] = v1[10] - v2[10];	v0[11] = v1[11] - v2[11];
    v0[12] = v1[12] - v2[12];	v0[13] = v1[13] - v2[13];
    v0[14] = v1[14] - v2[14];	v0[15] = v1[15] - v2[15];
}

static void
native_m4f_sub(oobject_t list, oint32_t ac)
/* float32_t m4f.sub(float32_t v0[16],
		     float32_t v1[16], float32_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_MF4(alist->a2);
    m4f_sub(alist->a0->v.f32,  alist->a1->v.f32,  alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_mul(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16])
{
    float		a00, a01, a02, a03;
    float		a10, a11, a12, a13;
    float		a20, a21, a22, a23;
    float		a30, a31, a32, a33;
    float		b00, b01, b02, b03;
    float		b10, b11, b12, b13;
    float		b20, b21, b22, b23;
    float		b30, b31, b32, b33;

    a00 = A4(v1, 0, 0);	a01 = A4(v1, 0, 1);
    a02 = A4(v1, 0, 2);	a03 = A4(v1, 0, 3);
    a10 = A4(v1, 1, 0);	a11 = A4(v1, 1, 1);
    a12 = A4(v1, 1, 2);	a13 = A4(v1, 1, 3);
    a20 = A4(v1, 2, 0);	a21 = A4(v1, 2, 1);
    a22 = A4(v1, 2, 2);	a23 = A4(v1, 2, 3);
    a30 = A4(v1, 3, 0);	a31 = A4(v1, 3, 1);
    a32 = A4(v1, 3, 2);	a33 = A4(v1, 3, 3);
    b00 = A4(v2, 0, 0);	b01 = A4(v2, 0, 1);
    b02 = A4(v2, 0, 2);	b03 = A4(v2, 0, 3);
    b10 = A4(v2, 1, 0);	b11 = A4(v2, 1, 1);
    b12 = A4(v2, 1, 2);	b13 = A4(v2, 1, 3);
    b20 = A4(v2, 2, 0);	b21 = A4(v2, 2, 1);
    b22 = A4(v2, 2, 2);	b23 = A4(v2, 2, 3);
    b30 = A4(v2, 3, 0);	b31 = A4(v2, 3, 1);
    b32 = A4(v2, 3, 2);	b33 = A4(v2, 3, 3);
    A4(v0, 0, 0) = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    A4(v0, 0, 1) = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    A4(v0, 0, 2) = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    A4(v0, 0, 3) = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    A4(v0, 1, 0) = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    A4(v0, 1, 1) = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    A4(v0, 1, 2) = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    A4(v0, 1, 3) = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    A4(v0, 2, 0) = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    A4(v0, 2, 1) = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    A4(v0, 2, 2) = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    A4(v0, 2, 3) = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    A4(v0, 3, 0) = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    A4(v0, 3, 1) = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    A4(v0, 3, 2) = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    A4(v0, 3, 3) = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

static void
native_m4f_mul(oobject_t list, oint32_t ac)
/* float32_t m4f.mul(float32_t v0[16],
		     float32_t v1[16], float32_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_MF4(alist->a2);
    m4f_mul(alist->a0->v.f32,  alist->a1->v.f32,  alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_div(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[16])
{
    ofloat32_t			 inv[16];

    (void)m4f_det_inverse(inv, v2);
    m4f_mul(v0, v1, inv);
}

static void
native_m4f_div(oobject_t list, oint32_t ac)
/* float32_t m4f.div(float32_t v0[16],
		     float32_t v1[16], float32_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_MF4(alist->a2);
    m4f_div(alist->a0->v.f32,  alist->a1->v.f32,  alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_translate(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[3])
{
    ofloat32_t		res0[4], res1[4], res2[4];

    v4f_muls(res0, v1 + 0, v2[0]);
    v4f_muls(res1, v1 + 4, v2[1]);
    v4f_muls(res2, v1 + 8, v2[2]);
    if (likely(v0 != v1))
	memcpy(v0, v1, sizeof(ofloat32_t) * 16);
    v4f_add(v0 + 12, v0 + 12, res0);
    v4f_add(v0 + 12, v0 + 12, res1);
    v4f_add(v0 + 12, v0 + 12, res2);
}

static void
native_m4f_translate(oobject_t list, oint32_t ac)
/* float32_t m4f.translate(float32_t v0[16],
			   float32_t v1[16], float32_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_VF3(alist->a2);
    m4f_translate(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_rotate(ofloat32_t v0[16], ofloat32_t v1[16],
	   ofloat32_t angle, ofloat32_t v2[3])
{
    ofloat32_t		 c, s, axis[3], tmp[3];
    ofloat32_t		*vn, rot[16], res[16], res0[4], res1[4], res2[4];

    if (unlikely(v0 == v1))
	vn = res;
    else
	vn = v0;
    c = cosf(angle);
    s = sinf(angle);

    v3f_normalize(axis, v2);

    v3f_fill(tmp, 1.0f - c);
    v3f_mul(tmp, tmp, axis);

    A4(rot, 0, 0) = c + tmp[0] * axis[0];
    A4(rot, 0, 1) =     tmp[0] * axis[1] + s * axis[2];
    A4(rot, 0, 2) =     tmp[0] * axis[2] - s * axis[1];

    A4(rot, 1, 0) =     tmp[1] * axis[0] - s * axis[2];
    A4(rot, 1, 1) = c + tmp[1] * axis[1];
    A4(rot, 1, 2) =     tmp[1] * axis[2] + s * axis[0];

    A4(rot, 2, 0) =     tmp[2] * axis[0] + s * axis[1];
    A4(rot, 2, 1) =     tmp[2] * axis[1] - s * axis[0];
    A4(rot, 2, 2) = c + tmp[2] * axis[2];

    v4f_muls(res0, v1 + 0, A4(rot, 0, 0));
    v4f_muls(res1, v1 + 4, A4(rot, 0, 1));
    v4f_muls(res2, v1 + 8, A4(rot, 0, 2));
    v4f_add(vn + 0, res0, res1);	v4f_add(vn + 0, vn + 0, res2);

    v4f_muls(res0, v1 + 0, A4(rot, 1, 0));
    v4f_muls(res1, v1 + 4, A4(rot, 1, 1));
    v4f_muls(res2, v1 + 8, A4(rot, 1, 2));
    v4f_add(vn + 4, res0, res1);	v4f_add(vn + 4, vn + 4, res2);

    v4f_muls(res0, v1 + 0, A4(rot, 2, 0));
    v4f_muls(res1, v1 + 4, A4(rot, 2, 1));
    v4f_muls(res2, v1 + 8, A4(rot, 2, 2));
    v4f_add(vn + 8, res0, res1);	v4f_add(vn + 8, vn + 8, res2);

    if (unlikely(v0 == v1))
	memcpy(v0, vn, sizeof(ofloat32_t) * 12);
    else
	memcpy(v0 + 12, v1 + 12, sizeof(ofloat32_t) * 4);
}

static void
native_m4f_rotate(oobject_t list, oint32_t ac)
/* float32_t m4f.rotate(float32_t v0[16], float32_t v1[16],
			float32_t deg, float32_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f32_vec_t	*alist;

    alist = (nat_vec_vec_f32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_VF3(alist->a3);
    m4f_rotate(alist->a0->v.f32, alist->a1->v.f32,
	       radiansf(alist->a2), alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_scale(ofloat32_t v0[16], ofloat32_t v1[16], ofloat32_t v2[3])
{
    v4f_muls(v0 + 0, v1 + 0, v2[0]);
    v4f_muls(v0 + 4, v1 + 4, v2[1]);
    v4f_muls(v0 + 8, v1 + 8, v2[2]);
    if (likely(v0 != v1))
	memcpy(v0 + 12, v1 + 12, sizeof(ofloat32_t) * 4);
}

static void
native_m4f_scale(oobject_t list, oint32_t ac)
/* float32_t m4f.scale(float32_t v0[16],
		       float32_t v1[16], float32_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_MF4(alist->a1);
    CHECK_VF3(alist->a2);
    m4f_scale(alist->a0->v.f32, alist->a1->v.f32, alist->a2->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_ortho(ofloat32_t v0[16], ofloat32_t left, ofloat32_t right,
	  ofloat32_t bottom, ofloat32_t top, ofloat32_t zNear, ofloat32_t zFar)
{
    m4f_identity(v0);
    A4(v0, 0, 0) =   2.0f / (right - left);
    A4(v0, 1, 1) =   2.0f / (top - bottom);
    A4(v0, 2, 2) = - 2.0f / (zFar - zNear);
    A4(v0, 3, 0) = - (right + left) / (right - left);
    A4(v0, 3, 1) = - (top + bottom) / (top - bottom);
    A4(v0, 3, 2) = - (zFar + zNear) / (zFar - zNear);
}

static void
native_m4f_ortho(oobject_t list, oint32_t ac)
/* float32_t m4f.ortho(float32_t v0[16], float32_t left, float32_t right,
		       float32_t bottom, float32_t top, float32_t zNear,
		       float32_t zFar)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_f32_f32_f32_f32_f32_t		*alist;

    alist = (nat_vec_f32_f32_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    m4f_ortho(alist->a0->v.f32, alist->a1, alist->a2,
	      alist->a3, alist->a4, alist->a5, alist->a6);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_frustum(ofloat32_t v0[16], ofloat32_t left,
	    ofloat32_t right, ofloat32_t bottom,
	    ofloat32_t top, ofloat32_t nearVal, ofloat32_t farVal)
{
    m4f_fill(v0, 0.0f);
    A4(v0, 0, 0) =   (2.0f * nearVal) / (right - left);
    A4(v0, 1, 1) =   (2.0f * nearVal) / (top - bottom);
    A4(v0, 2, 0) =   (right + left) / (right - left);
    A4(v0, 2, 1) = - (top + bottom) / (top - bottom);
    A4(v0, 2, 2) = - (farVal + nearVal) / (farVal - nearVal);
    A4(v0, 2, 3) = - 1.0f;
    A4(v0, 3, 2) = - (2.0f * farVal * nearVal) / (farVal - nearVal);
}

static void
native_m4f_frustum(oobject_t list, oint32_t ac)
/* float32_t m4f.frustum(float32_t v0[16], float32_t left, float32_t right,
			 float32_t bottom, float32_t top, float32_t nearVal,
			 float32_t farVal)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_f32_f32_f32_f32_f32_t		*alist;

    alist = (nat_vec_f32_f32_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    m4f_frustum(alist->a0->v.f32, alist->a1, alist->a2,
		alist->a3, alist->a4, alist->a5, alist->a6);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_perspective(ofloat32_t v0[16], ofloat32_t fovy,
		ofloat32_t aspect, ofloat32_t zNear, ofloat32_t zFar)
{
    ofloat32_t			t;

    t = tanf(fovy / 2.0f);
    m4f_fill(v0, 0.0f);
    A4(v0, 0, 0) = 1.0f / (aspect * t);
    A4(v0, 1, 1) = 1.0f / t;
    A4(v0, 2, 2) = - (zFar + zNear) / (zFar - zNear);
    A4(v0, 2, 3) = - 1.0f;
    A4(v0, 3, 2) = - (2.0f * zFar * zNear) / (zFar - zNear);
}

static void
native_m4f_perspective(oobject_t list, oint32_t ac)
/* float32_t m4f.perspective(float32_t v0[16], float32_t fovy, float32_t aspect,
			     float32_t zNear, float32_t zFar)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f32_f32_f32_f32_t	*alist;

    alist = (nat_vec_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    m4f_perspective(alist->a0->v.f32,
		    radiansf(alist->a1), alist->a2, alist->a3, alist->a4);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_pickMatrix(ofloat32_t v0[16], ofloat32_t center[2],
	       ofloat32_t delta[2], oint32_t viewport[4])
{
    ofloat32_t		tmp[3];

    m4f_identity(v0);
    tmp[0] = viewport[2] - 2.0f * (center[0] - viewport[0]) / delta[0];
    tmp[1] = viewport[3] - 2.0f * (center[1] - viewport[1]) / delta[1];
    tmp[2] = 0;
    m4f_translate(v0, v0, tmp);
    tmp[0] = viewport[2] / delta[0];
    tmp[1] = viewport[3] / delta[1];
    tmp[2] = 1.0f;
    m4f_scale(v0, v0, tmp);
}

static void
native_m4f_pickMatrix(oobject_t list, oint32_t ac)
/* float32_t m4f.pickMatrix(float32_t v0[16], float32_t center[2],
			    float32_t delta[2], int32_t viewport[4])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_VF2(alist->a1);
    CHECK_VF2(alist->a2);
    CHECK_VI4(alist->a3);
    m4f_pickMatrix(alist->a0->v.f32, alist->a1->v.f32,
		   alist->a2->v.f32, alist->a3->v.i32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
m4f_lookAt(ofloat32_t v0[16],
	   ofloat32_t eye[3], ofloat32_t center[3], ofloat32_t up[3])
{
    ofloat32_t		f[3], s[3], u[3];

    v3f_sub(f, center, eye);
    v3f_normalize(f, f);

    v3f_cross(u, f, up);
    v3f_normalize(s, u);

    v3f_cross(u, s, f);

    A4(v0, 0, 0) =  s[0];
    A4(v0, 1, 0) =  s[1];
    A4(v0, 2, 0) =  s[2];

    A4(v0, 0, 1) =  u[0];
    A4(v0, 1, 1) =  u[1];
    A4(v0, 2, 1) =  u[2];

    A4(v0, 0, 2) = -f[0];
    A4(v0, 1, 2) = -f[1];
    A4(v0, 2, 2) = -f[2];

    A4(v0, 3, 0) = -v3f_dot(s, eye);
    A4(v0, 3, 1) = -v3f_dot(u, eye);
    A4(v0, 3, 2) =  v3f_dot(f, eye);

    A4(v0, 0, 3) = A4(v0, 1, 3) = A4(v0, 2, 3) = 0.0f;
    A4(v0, 3, 3) = 1.0f;
}

static void
native_m4f_lookAt(oobject_t list, oint32_t ac)
/* float32_t m4f.lookAt(float32_t v[16], float32_t eye[3],
			float32_t center[3], float32_t up[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MF4(alist->a0);
    CHECK_VF3(alist->a1);
    CHECK_VF3(alist->a2);
    CHECK_VF3(alist->a3);
    m4f_lookAt(alist->a0->v.f32, alist->a1->v.f32,
	       alist->a2->v.f32, alist->a3->v.f32);
    r0->t = t_vector|t_float32;
    r0->v.o = alist->a0;
}

static void
native_m2d_set(oobject_t list, oint32_t ac)
/* float64_t m2d.set(float64_t v0[4], float64_t s0,
		     float64_t s1, float64_t s2, float64_t s3)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_f64_f64_t	*alist;

    alist = (nat_vec_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_fill(ofloat64_t v0[4], ofloat64_t s0)
{
    v0[0] = v0[1] = v0[2] = v0[3] = s0;
}

static void
native_m2d_fill(oobject_t list, oint32_t ac)
/* float64_t m2d.fill(float64_t v0[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    m2d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_identity(ofloat64_t v0[4])
{
    A2(v0, 0, 1) = A2(v0, 1, 0) = 0.0;
    A2(v0, 0, 0) = A2(v0, 1, 1) = 1.0;
}

static void
native_m2d_identity(oobject_t list, oint32_t ac)
/* float64_t m2d.identity(float64_t v0[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    m2d_identity(alist->a0->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_transpose(oobject_t list, oint32_t ac)
/* float64_t m2d.transpose(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    if (likely(v0 != v1)) {
	A2(v0, 0, 0) = A2(v1, 0, 0);
	A2(v0, 0, 1) = A2(v1, 1, 0);
	A2(v0, 1, 0) = A2(v1, 0, 1);
	A2(v0, 1, 1) = A2(v1, 1, 1);
    }
    else {
	v2 = A3(v1, 0, 1);
	A2(v0, 0, 1) = A2(v1, 1, 0);
	A2(v0, 1, 0) = v2;
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_copy(oobject_t list, oint32_t ac)
/* float64_t m2d.copy(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MD2(alist->a1);
	memcpy(alist->a0->v.f64, alist->a1->v.f64, sizeof(double) * 4);
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
m2d_det_inverse(ofloat64_t v0[4], ofloat64_t v1[4])
{
    ofloat64_t		s00 = A2(v1, 0, 0);
    ofloat64_t		s01 = A2(v1, 0, 1);
    ofloat64_t		s10 = A2(v1, 1, 0);
    ofloat64_t		s11 = A2(v1, 1, 1);
    ofloat64_t		det;

    det = s00 * s11 - s10 * s01;

    if (v0) {
	v0[0] =  s11 / det;
	v0[1] = -s01 / det;
	v0[2] = -s10 / det;
	v0[3] =  s00 / det;
    }

    return (det);
}

static void
native_m2d_det(oobject_t list, oint32_t ac)
/* float64_t m2d.det(float64_t v0[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MD2(alist->a0);
    r0->v.d = m2d_det_inverse(null, alist->a0->v.f64);
}

static void
native_m2d_adds(oobject_t list, oint32_t ac)
/* float64_t m2d.adds(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] + v2;		v0[1] = v1[1] + v2;
    v0[2] = v1[2] + v2;		v0[3] = v1[3] + v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_subs(oobject_t list, oint32_t ac)
/* float64_t m2d.subs(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] - v2;		v0[1] = v1[1] - v2;
    v0[2] = v1[2] - v2;		v0[3] = v1[3] - v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_muls(oobject_t list, oint32_t ac)
/* float64_t m2d.muls(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] * v2;		v0[1] = v1[1] * v2;
    v0[2] = v1[2] * v2;		v0[3] = v1[3] * v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_divs(oobject_t list, oint32_t ac)
/* float64_t m2d.divs(float64_t v0[4], float64_t v1[4], float64_t s0)[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] / v2;		v0[1] = v1[1] / v2;
    v0[2] = v1[2] / v2;		v0[3] = v1[3] / v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_inverse(oobject_t list, oint32_t ac)
/* float64_t m2d.inverse(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    CHECK_MD2(alist->a1);
    (void)m2d_det_inverse(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m2d_eq(oobject_t list, oint32_t ac)
/* uint8_t m2d.eq(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD2(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] == v1[2] && v0[3] == v1[3];
    }
    else
	r0->v.w = true;
}

static void
native_m2d_ne(oobject_t list, oint32_t ac)
/* uint8_t m2d.ne(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD2(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD2(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3];
    }
    else
	r0->v.w = false;
}

static void
native_m2d_neg(oobject_t list, oint32_t ac)
/* float64_t m2d.neg(float64_t v0[4], float64_t v1[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD2(alist->a1);
    v1 = alist->a1->v.f64;
    v0[0] = -v1[0];	v0[1] = -v1[1];
    v0[2] = -v1[2];	v0[3] = -v1[3];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_add(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] + v2[0];	v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];	v0[3] = v1[3] + v2[3];
}

static void
native_m2d_add(oobject_t list, oint32_t ac)
/* float64_t m2d.add(float64_t v0[4],
		     float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    CHECK_MD2(alist->a1);
    CHECK_MD2(alist->a2);
    m2d_add(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_sub(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    v0[0] = v1[0] - v2[0];	v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];	v0[3] = v1[3] - v2[3];
}

static void
native_m2d_sub(oobject_t list, oint32_t ac)
/* float64_t m2d.sub(float64_t v0[4],
		     float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    CHECK_MD2(alist->a1);
    CHECK_MD2(alist->a2);
    m2d_sub(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_mul(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    double		a00, a01;
    double		a10, a11;
    double		b00, b01;
    double		b10, b11;

    a00 = A2(v1, 0, 0);		a01 = A2(v1, 0, 1);
    a10 = A2(v1, 1, 0);		a11 = A2(v1, 1, 1);
    b00 = A2(v2, 0, 0);		b01 = A2(v2, 0, 1);
    b10 = A2(v2, 1, 0);		b11 = A2(v2, 1, 1);

    A2(v0, 0, 0) = a00 * b00 + a10 * b01;
    A2(v0, 0, 1) = a01 * b00 + a11 * b01;
    A2(v0, 1, 0) = a00 * b10 + a10 * b11;
    A2(v0, 1, 1) = a01 * b10 + a11 * b11;
}

static void
native_m2d_mul(oobject_t list, oint32_t ac)
/* float64_t m2d.mul(float64_t v0[4],
		     float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    CHECK_MD2(alist->a1);
    CHECK_MD2(alist->a2);
    m2d_mul(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m2d_div(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4])
{
    ofloat64_t			 inv[4];

    (void)m2d_det_inverse(inv, v2);
    m2d_mul(v0, v1, inv);
}

static void
native_m2d_div(oobject_t list, oint32_t ac)
/* float64_t m2d.div(float64_t v0[4],
		     float64_t v1[4], float64_t v2[4])[4]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD2(alist->a0);
    CHECK_MD2(alist->a1);
    CHECK_MD2(alist->a2);
    m2d_div(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_set(oobject_t list, oint32_t ac)
/* float64_t m3d.set(float64_t v0[9],
		     float64_t  s0, float64_t  s1, float64_t  s2,
		     float64_t  s3, float64_t  s4, float64_t  s5,
		     float64_t  s6, float64_t  s7, float64_t  s8)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_f64_f64_f64_f64_f64_f64_f64_t *alist;

    alist = (nat_vec_f64_f64_f64_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    v0[0] = alist->a1;
    v0[1] = alist->a2;
    v0[2] = alist->a3;
    v0[3] = alist->a4;
    v0[4] = alist->a5;
    v0[5] = alist->a6;
    v0[6] = alist->a7;
    v0[7] = alist->a8;
    v0[8] = alist->a9;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_fill(ofloat64_t v0[9], ofloat64_t s0)
{
    v0[0] = v0[1] = v0[2] =
    v0[3] = v0[4] = v0[5] =
    v0[6] = v0[7] = v0[8] = s0;
}

static void
native_m3d_fill(oobject_t list, oint32_t ac)
/* float64_t m3d.fill(float64_t v0[9], float64_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    m3d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_identity(ofloat64_t v0[9])
{
    v0[1] = v0[2] = v0[3] = v0[5] = v0[6] = v0[7] = 0.0;
    v0[0] = v0[4] = v0[8] = 1.0;
}

static void
native_m3d_identity(oobject_t list, oint32_t ac)
/* float64_t m3d.identity(float64_t v0[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    m3d_identity(alist->a0->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_transpose(oobject_t list, oint32_t ac)
/* float64_t m3d.transpose(float64_t v0[9], float64_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    if (likely(v0 != v1)) {
	A3(v0, 0, 0) = A3(v1, 0, 0);
	A3(v0, 0, 1) = A3(v1, 1, 0);
	A3(v0, 0, 2) = A3(v1, 2, 0);
	A3(v0, 1, 0) = A3(v1, 0, 1);
	A3(v0, 1, 1) = A3(v1, 1, 1);
	A3(v0, 1, 2) = A3(v1, 2, 1);
	A3(v0, 2, 0) = A3(v1, 0, 2);
	A3(v0, 2, 1) = A3(v1, 1, 2);
	A3(v0, 2, 2) = A3(v1, 2, 2);
    }
    else {
	v2 = A3(v1, 0, 1);
	A3(v0, 0, 1) = A3(v1, 1, 0);
	A3(v0, 1, 0) = v2;
	v2 = A3(v1, 0, 2);
	A3(v0, 0, 2) = A3(v1, 2, 0);
	A3(v0, 2, 0) = v2;
	v2 = A3(v1, 1, 2);
	A3(v0, 1, 2) = A3(v1, 2, 1);
	A3(v0, 2, 1) = v2;
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_copy(oobject_t list, oint32_t ac)
/* float64_t m3d.copy(float64_t v0[9], float64_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MD3(alist->a1);
	memcpy(alist->a0->v.f64, alist->a1->v.f64, sizeof(double) * 9);
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
m3d_det_inverse(ofloat64_t v0[9], ofloat64_t v1[9])
{
    ofloat64_t		s00 = A3(v1, 0, 0);
    ofloat64_t		s01 = A3(v1, 0, 1);
    ofloat64_t		s02 = A3(v1, 0, 2);
    ofloat64_t		s10 = A3(v1, 1, 0);
    ofloat64_t		s11 = A3(v1, 1, 1);
    ofloat64_t		s12 = A3(v1, 1, 2);
    ofloat64_t		s20 = A3(v1, 2, 0);
    ofloat64_t		s21 = A3(v1, 2, 1);
    ofloat64_t		s22 = A3(v1, 2, 2);
    ofloat64_t		inv[9];
    ofloat64_t		det;

    if (v0) {
	inv[0] = s11 * s22 - s12 * s21;
	inv[1] = s21 * s02 - s22 * s01;
	inv[2] = s01 * s12 - s02 * s11;
	inv[3] = s20 * s12 - s10 * s22;
	inv[4] = s00 * s22 - s20 * s02;
	inv[5] = s10 * s02 - s00 * s12;
	inv[6] = s21 * s10 - s11 * s20;
	inv[7] = s01 * s20 - s21 * s00;
	inv[8] = s11 * s00 - s01 * s10;
    }

    det = s00 * (s11 * s22 - s12 * s21) -
	  s01 * (s10 * s22 - s12 * s20) +
	  s02 * (s10 * s21 - s11 * s20);

    if (v0) {
	v0[0] = inv[0] / det;
	v0[1] = inv[1] / det;
	v0[2] = inv[2] / det;
	v0[3] = inv[3] / det;
	v0[4] = inv[4] / det;
	v0[5] = inv[5] / det;
	v0[6] = inv[6] / det;
	v0[7] = inv[7] / det;
	v0[8] = inv[8] / det;
    }

    return (det);
}

static void
native_m3d_det(oobject_t list, oint32_t ac)
/* float64_t m3d.det(float64_t v0[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MD3(alist->a0);
    r0->v.d = m3d_det_inverse(null, alist->a0->v.f64);
}

static void
native_m3d_adds(oobject_t list, oint32_t ac)
/* float64_t m3d.adds(float64_t v0[9], float64_t v1[9], float64_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] + v2;		v0[1] = v1[1] + v2;
    v0[2] = v1[2] + v2;		v0[3] = v1[3] + v2;
    v0[4] = v1[4] + v2;		v0[5] = v1[5] + v2;
    v0[6] = v1[6] + v2;		v0[7] = v1[7] + v2;
    v0[8] = v1[8] + v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_subs(oobject_t list, oint32_t ac)
/* float64_t m3d.subs(float64_t v0[9], float64_t v1[9], float64_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] - v2;		v0[1] = v1[1] - v2;
    v0[2] = v1[2] - v2;		v0[3] = v1[3] - v2;
    v0[4] = v1[4] - v2;		v0[5] = v1[5] - v2;
    v0[6] = v1[6] - v2;		v0[7] = v1[7] - v2;
    v0[8] = v1[8] - v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_muls(oobject_t list, oint32_t ac)
/* float64_t m3d.muls(float64_t v0[9], float64_t v1[9], float64_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] * v2;		v0[1] = v1[1] * v2;
    v0[2] = v1[2] * v2;		v0[3] = v1[3] * v2;
    v0[4] = v1[4] * v2;		v0[5] = v1[5] * v2;
    v0[6] = v1[6] * v2;		v0[7] = v1[7] * v2;
    v0[8] = v1[8] * v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_divs(oobject_t list, oint32_t ac)
/* float64_t m3d.divs(float64_t v0[9], float64_t v1[9], float64_t s0)[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[0] = v1[0] / v2;		v0[1] = v1[1] / v2;
    v0[2] = v1[2] / v2;		v0[3] = v1[3] / v2;
    v0[4] = v1[4] / v2;		v0[5] = v1[5] / v2;
    v0[6] = v1[6] / v2;		v0[7] = v1[7] / v2;
    v0[8] = v1[8] / v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_inverse(oobject_t list, oint32_t ac)
/* float64_t m3d.inverse(float64_t v0[9], float64_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    CHECK_MD3(alist->a1);
    (void)m3d_det_inverse(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m3d_eq(oobject_t list, oint32_t ac)
/* uint8_t m3d.eq(float64_t v0[9], float64_t v1[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD3(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] == v1[0] && v0[1] == v1[1] &&
		  v0[2] == v1[2] && v0[3] == v1[3] &&
		  v0[4] == v1[4] && v0[5] == v1[5] &&
		  v0[6] == v1[6] && v0[7] == v1[7] &&
		  v0[8] == v1[8];
    }
    else
	r0->v.w = true;
}

static void
native_m3d_ne(oobject_t list, oint32_t ac)
/* uint8_t m3d.ne(float64_t v0[9], float64_t v1[9]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD3(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD3(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[0] != v1[0] || v0[1] != v1[1] ||
		  v0[2] != v1[2] || v0[3] != v1[3] ||
		  v0[4] != v1[4] || v0[5] != v1[5] ||
		  v0[6] != v1[6] || v0[7] != v1[7] ||
		  v0[8] != v1[8];
    }
    else
	r0->v.w = false;
}

static void
native_m3d_neg(oobject_t list, oint32_t ac)
/* float64_t m3d.neg(float64_t v0[9], float64_t v1[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD3(alist->a1);
    v1 = alist->a1->v.f64;
    v0[0] = -v1[0];	v0[1] = -v1[1];		v0[2] = -v1[2];
    v0[3] = -v1[3];	v0[4] = -v1[4];		v0[5] = -v1[5];
    v0[6] = -v1[6];	v0[7] = -v1[7];		v0[8] = -v1[8];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_add(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9])
{
    v0[0] = v1[0] + v2[0];	v0[1] = v1[1] + v2[1];
    v0[2] = v1[2] + v2[2];	v0[3] = v1[3] + v2[3];
    v0[4] = v1[4] + v2[4];	v0[5] = v1[5] + v2[5];
    v0[6] = v1[6] + v2[6];	v0[7] = v1[7] + v2[7];
    v0[8] = v1[8] + v2[8];
}

static void
native_m3d_add(oobject_t list, oint32_t ac)
/* float64_t m3d.add(float64_t v0[9],
		     float64_t v1[9], float64_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    CHECK_MD3(alist->a1);
    CHECK_MD3(alist->a2);
    m3d_add(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_sub(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9])
{
    v0[0] = v1[0] - v2[0];	v0[1] = v1[1] - v2[1];
    v0[2] = v1[2] - v2[2];	v0[3] = v1[3] - v2[3];
    v0[4] = v1[4] - v2[4];	v0[5] = v1[5] - v2[5];
    v0[6] = v1[6] - v2[6];	v0[7] = v1[7] - v2[7];
    v0[8] = v1[8] - v2[8];
}

static void
native_m3d_sub(oobject_t list, oint32_t ac)
/* float64_t m3d.sub(float64_t v0[9],
		     float64_t v1[9], float64_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    CHECK_MD3(alist->a1);
    CHECK_MD3(alist->a2);
    m3d_sub(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_mul(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9])
{
    double		a00, a01, a02;
    double		a10, a11, a12;
    double		a20, a21, a22;
    double		b00, b01, b02;
    double		b10, b11, b12;
    double		b20, b21, b22;

    a00 = A3(v1, 0, 0);		a01 = A3(v1, 0, 1);	a02 = A3(v1, 0, 2);
    a10 = A3(v1, 1, 0);		a11 = A3(v1, 1, 1);	a12 = A3(v1, 1, 2);
    a20 = A3(v1, 2, 0);		a21 = A3(v1, 2, 1);	a22 = A3(v1, 2, 2);
    b00 = A3(v2, 0, 0);		b01 = A3(v2, 0, 1);	b02 = A3(v2, 0, 2);
    b10 = A3(v2, 1, 0);		b11 = A3(v2, 1, 1);	b12 = A3(v2, 1, 2);
    b20 = A3(v2, 2, 0);		b21 = A3(v2, 2, 1);	b22 = A3(v2, 2, 2);

    A3(v0, 0, 0) = a00 * b00 + a10 * b01 + a20 * b02;
    A3(v0, 0, 1) = a01 * b00 + a11 * b01 + a21 * b02;
    A3(v0, 0, 2) = a02 * b00 + a12 * b01 + a22 * b02;
    A3(v0, 1, 0) = a00 * b10 + a10 * b11 + a20 * b12;
    A3(v0, 1, 1) = a01 * b10 + a11 * b11 + a21 * b12;
    A3(v0, 1, 2) = a02 * b10 + a12 * b11 + a22 * b12;
    A3(v0, 2, 0) = a00 * b20 + a10 * b21 + a20 * b22;
    A3(v0, 2, 1) = a01 * b20 + a11 * b21 + a21 * b22;
    A3(v0, 2, 2) = a02 * b20 + a12 * b21 + a22 * b22;
}

static void
native_m3d_mul(oobject_t list, oint32_t ac)
/* float64_t m3d.mul(float64_t v0[9],
		     float64_t v1[9], float64_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    CHECK_MD3(alist->a1);
    CHECK_MD3(alist->a2);
    m3d_mul(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m3d_div(ofloat64_t v0[9], ofloat64_t v1[9], ofloat64_t v2[9])
{
    ofloat64_t			 inv[9];

    (void)m3d_det_inverse(inv, v2);
    m3d_mul(v0, v1, inv);
}

static void
native_m3d_div(oobject_t list, oint32_t ac)
/* float64_t m3d.div(float64_t v0[9],
		     float64_t v1[9], float64_t v2[9])[9]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD3(alist->a0);
    CHECK_MD3(alist->a1);
    CHECK_MD3(alist->a2);
    m3d_div(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_set(oobject_t list, oint32_t ac)
/* float64_t m4d.set(float64_t v0[16],
		     float64_t  s0, float64_t  s1, float64_t  s2, float64_t  s3,
		     float64_t  s4, float64_t  s5, float64_t  s6, float64_t  s7,
		     float64_t  s8, float64_t  s9, float64_t s10, float64_t s11,
		     float64_t s12, float64_t s13, float64_t s14, float64_t s15)
		    [16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    double			*v0;
    nat_vec_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_t *alist;

    alist = (nat_vec_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    v0[ 0] = alist->a1;
    v0[ 1] = alist->a2;
    v0[ 2] = alist->a3;
    v0[ 3] = alist->a4;
    v0[ 4] = alist->a5;
    v0[ 5] = alist->a6;
    v0[ 6] = alist->a7;
    v0[ 7] = alist->a8;
    v0[ 8] = alist->a9;
    v0[ 9] = alist->a10;
    v0[10] = alist->a11;
    v0[11] = alist->a12;
    v0[12] = alist->a13;
    v0[13] = alist->a14;
    v0[14] = alist->a15;
    v0[15] = alist->a16;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_fill(ofloat64_t v0[16], ofloat64_t s0)
{
    v0[ 0] = v0[ 1] = v0[ 2] = v0[ 3] =
    v0[ 4] = v0[ 5] = v0[ 6] = v0[ 7] =
    v0[ 8] = v0[ 9] = v0[10] = v0[11] =
    v0[12] = v0[13] = v0[14] = v0[15] =	s0;
}

static void
native_m4d_fill(oobject_t list, oint32_t ac)
/* float64_t m4d.fill(float64_t v0[16], float64_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_t		*alist;

    alist = (nat_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    m4d_fill(alist->a0->v.f64, alist->a1);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_identity(ofloat64_t v0[16])
{
    v0[ 1] = v0[ 2] = v0[ 3] = v0[ 4] =
    v0[ 6] = v0[ 7] = v0[ 8] = v0[ 9] =
    v0[11] = v0[12] = v0[13] = v0[14] = 0.0;
    v0[ 0] = v0[ 5] = v0[10] = v0[15] = 1.0;
}

static void
native_m4d_identity(oobject_t list, oint32_t ac)
/* float64_t m4d.identity(float64_t v0[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    m4d_identity(alist->a0->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_copy(oobject_t list, oint32_t ac)
/* float64_t m4d.copy(float64_t v0[16], float64_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	CHECK_MD4(alist->a1);
	memcpy(alist->a0->v.f64, alist->a1->v.f64, sizeof(double) * 16);
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static ofloat64_t
m4d_det_inverse(ofloat64_t v0[16], ofloat64_t v1[16])
{
    ofloat64_t sf00 = A4(v1, 2, 2) * A4(v1, 3, 3) - A4(v1, 2, 3) * A4(v1, 3, 2);
    ofloat64_t sf01 = A4(v1, 1, 2) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 2);
    ofloat64_t sf02 = A4(v1, 1, 2) * A4(v1, 2, 3) - A4(v1, 1, 3) * A4(v1, 2, 2);
    ofloat64_t sf03 = A4(v1, 0, 2) * A4(v1, 3, 3) - A4(v1, 0, 3) * A4(v1, 3, 2);
    ofloat64_t sf04 = A4(v1, 0, 2) * A4(v1, 2, 3) - A4(v1, 0, 3) * A4(v1, 2, 2);
    ofloat64_t sf05 = A4(v1, 0, 2) * A4(v1, 1, 3) - A4(v1, 0, 3) * A4(v1, 1, 2);
    ofloat64_t sf06 = A4(v1, 2, 1) * A4(v1, 3, 3) - A4(v1, 2, 3) * A4(v1, 3, 1);
    ofloat64_t sf07 = A4(v1, 1, 1) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 1);
    ofloat64_t sf08 = A4(v1, 1, 1) * A4(v1, 2, 3) - A4(v1, 1, 3) * A4(v1, 2, 1);
    ofloat64_t sf09 = A4(v1, 0, 1) * A4(v1, 3, 3) - A4(v1, 0, 3) * A4(v1, 3, 1);
    ofloat64_t sf10 = A4(v1, 0, 1) * A4(v1, 2, 3) - A4(v1, 0, 3) * A4(v1, 2, 1);
    ofloat64_t sf11 = A4(v1, 1, 1) * A4(v1, 3, 3) - A4(v1, 1, 3) * A4(v1, 3, 1);
    ofloat64_t sf12 = A4(v1, 0, 1) * A4(v1, 1, 3) - A4(v1, 0, 3) * A4(v1, 1, 1);
    ofloat64_t sf13 = A4(v1, 2, 1) * A4(v1, 3, 2) - A4(v1, 2, 2) * A4(v1, 3, 1);
    ofloat64_t sf14 = A4(v1, 1, 1) * A4(v1, 3, 2) - A4(v1, 1, 2) * A4(v1, 3, 1);
    ofloat64_t sf15 = A4(v1, 1, 1) * A4(v1, 2, 2) - A4(v1, 1, 2) * A4(v1, 2, 1);
    ofloat64_t sf16 = A4(v1, 0, 1) * A4(v1, 3, 2) - A4(v1, 0, 2) * A4(v1, 3, 1);
    ofloat64_t sf17 = A4(v1, 0, 1) * A4(v1, 2, 2) - A4(v1, 0, 2) * A4(v1, 2, 1);
    ofloat64_t sf18 = A4(v1, 0, 1) * A4(v1, 1, 2) - A4(v1, 0, 2) * A4(v1, 1, 1);
    ofloat64_t		inv[16];
    ofloat64_t		det;

    inv[ 0] = + A4(v1, 1, 1) * sf00 - A4(v1, 2, 1) * sf01 + A4(v1, 3, 1) * sf02;
    inv[ 1] = - A4(v1, 0, 1) * sf00 + A4(v1, 2, 1) * sf03 - A4(v1, 3, 1) * sf04;
    inv[ 2] = + A4(v1, 0, 1) * sf01 - A4(v1, 1, 1) * sf03 + A4(v1, 3, 1) * sf05;
    inv[ 3] = - A4(v1, 0, 1) * sf02 + A4(v1, 1, 1) * sf04 - A4(v1, 2, 1) * sf05;
    inv[ 4] = - A4(v1, 1, 0) * sf00 + A4(v1, 2, 0) * sf01 - A4(v1, 3, 0) * sf02;
    inv[ 5] = + A4(v1, 0, 0) * sf00 - A4(v1, 2, 0) * sf03 + A4(v1, 3, 0) * sf04;
    inv[ 6] = - A4(v1, 0, 0) * sf01 + A4(v1, 1, 0) * sf03 - A4(v1, 3, 0) * sf05;
    inv[ 7] = + A4(v1, 0, 0) * sf02 - A4(v1, 1, 0) * sf04 + A4(v1, 2, 0) * sf05;
    inv[ 8] = + A4(v1, 1, 0) * sf06 - A4(v1, 2, 0) * sf07 + A4(v1, 3, 0) * sf08;
    inv[ 9] = - A4(v1, 0, 0) * sf06 + A4(v1, 2, 0) * sf09 - A4(v1, 3, 0) * sf10;
    inv[10] = + A4(v1, 0, 0) * sf11 - A4(v1, 1, 0) * sf09 + A4(v1, 3, 0) * sf12;
    inv[11] = - A4(v1, 0, 0) * sf08 + A4(v1, 1, 0) * sf10 - A4(v1, 2, 0) * sf12;
    inv[12] = - A4(v1, 1, 0) * sf13 + A4(v1, 2, 0) * sf14 - A4(v1, 3, 0) * sf15;
    inv[13] = + A4(v1, 0, 0) * sf13 - A4(v1, 2, 0) * sf16 + A4(v1, 3, 0) * sf17;
    inv[14] = - A4(v1, 0, 0) * sf14 + A4(v1, 1, 0) * sf16 - A4(v1, 3, 0) * sf18;
    inv[15] = + A4(v1, 0, 0) * sf15 - A4(v1, 1, 0) * sf17 + A4(v1, 2, 0) * sf18;
    det = A4(v1, 0, 0) * A4(inv, 0, 0) +
	  A4(v1, 1, 0) * A4(inv, 0, 1) +
	  A4(v1, 2, 0) * A4(inv, 0, 2) +
	  A4(v1, 3, 0) * A4(inv, 0, 3);

    if (v0) {
	v0[ 0] = inv[ 0] / det;
	v0[ 1] = inv[ 1] / det;
	v0[ 2] = inv[ 2] / det;
	v0[ 3] = inv[ 3] / det;
	v0[ 4] = inv[ 4] / det;
	v0[ 5] = inv[ 5] / det;
	v0[ 6] = inv[ 6] / det;
	v0[ 7] = inv[ 7] / det;
	v0[ 8] = inv[ 8] / det;
	v0[ 9] = inv[ 9] / det;
	v0[10] = inv[10] / det;
	v0[11] = inv[11] / det;
	v0[12] = inv[12] / det;
	v0[13] = inv[13] / det;
	v0[14] = inv[14] / det;
	v0[15] = inv[15] / det;
    }

    return (det);
}

static void
native_m4d_det(oobject_t list, oint32_t ac)
/* float64_t m4d.det(float64_t v0[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_t			*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_MD4(alist->a0);
    r0->v.d = m4d_det_inverse(null, alist->a0->v.f64);
}

static void
native_m4d_transpose(oobject_t list, oint32_t ac)
/* float64_t m4d.transpose(float64_t v0[16], float64_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    if (likely(v0 != v1)) {
	A4(v0, 0, 0) = A4(v1, 0, 0);
	A4(v0, 0, 1) = A4(v1, 1, 0);
	A4(v0, 0, 2) = A4(v1, 2, 0);
	A4(v0, 0, 3) = A4(v1, 3, 0);
	A4(v0, 1, 0) = A4(v1, 0, 1);
	A4(v0, 1, 1) = A4(v1, 1, 1);
	A4(v0, 1, 2) = A4(v1, 2, 1);
	A4(v0, 1, 3) = A4(v1, 3, 1);
	A4(v0, 2, 0) = A4(v1, 0, 2);
	A4(v0, 2, 1) = A4(v1, 1, 2);
	A4(v0, 2, 2) = A4(v1, 2, 2);
	A4(v0, 2, 3) = A4(v1, 3, 2);
	A4(v0, 3, 0) = A4(v1, 0, 3);
	A4(v0, 3, 1) = A4(v1, 1, 3);
	A4(v0, 3, 2) = A4(v1, 2, 3);
	A4(v0, 3, 3) = A4(v1, 3, 3);
    }
    else {
	v2 = A4(v1, 0, 1);
	A4(v0, 0, 1) = A4(v1, 1, 0);
	A4(v0, 1, 0) = v2;
	v2 = A4(v1, 0, 2);
	A4(v0, 0, 2) = A4(v1, 2, 0);
	A4(v0, 2, 0) = v2;
	v2 = A4(v1, 0, 3);
	A4(v0, 0, 3) = A4(v1, 3, 0);
	A4(v0, 3, 0) = v2;
	v2 = A4(v1, 1, 2);
	A4(v0, 1, 2) = A4(v1, 2, 1);
	A4(v0, 2, 1) = v2;
	v2 = A4(v1, 1, 3);
	A4(v0, 1, 3) = A4(v1, 3, 1);
	A4(v0, 3, 1) = v2;
	v2 = A4(v1, 2, 3);
	A4(v0, 2, 3) = A4(v1, 3, 2);
	A4(v0, 3, 2) = v2;
    }
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_adds(oobject_t list, oint32_t ac)
/* float64_t m4d.adds(float64_t v0[16], float64_t v1[16], float64_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] + v2;	v0[ 1] = v1[ 1] + v2;
    v0[ 2] = v1[ 2] + v2;	v0[ 3] = v1[ 3] + v2;
    v0[ 4] = v1[ 4] + v2;	v0[ 5] = v1[ 5] + v2;
    v0[ 6] = v1[ 6] + v2;	v0[ 7] = v1[ 7] + v2;
    v0[ 8] = v1[ 8] + v2;	v0[ 9] = v1[ 9] + v2;
    v0[10] = v1[10] + v2;	v0[11] = v1[11] + v2;
    v0[12] = v1[12] + v2;	v0[13] = v1[13] + v2;
    v0[14] = v1[14] + v2;	v0[15] = v1[15] + v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_subs(oobject_t list, oint32_t ac)
/* float64_t m4d.subs(float64_t v0[16], float64_t v1[16], float64_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] - v2;	v0[ 1] = v1[ 1] - v2;
    v0[ 2] = v1[ 2] - v2;	v0[ 3] = v1[ 3] - v2;
    v0[ 4] = v1[ 4] - v2;	v0[ 5] = v1[ 5] - v2;
    v0[ 6] = v1[ 6] - v2;	v0[ 7] = v1[ 7] - v2;
    v0[ 8] = v1[ 8] - v2;	v0[ 9] = v1[ 9] - v2;
    v0[10] = v1[10] - v2;	v0[11] = v1[11] - v2;
    v0[12] = v1[12] - v2;	v0[13] = v1[13] - v2;
    v0[14] = v1[14] - v2;	v0[15] = v1[15] - v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_muls(oobject_t list, oint32_t ac)
/* float64_t m4d.muls(float64_t v0[16], float64_t v1[16], float64_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] * v2;	v0[ 1] = v1[ 1] * v2;
    v0[ 2] = v1[ 2] * v2;	v0[ 3] = v1[ 3] * v2;
    v0[ 4] = v1[ 4] * v2;	v0[ 5] = v1[ 5] * v2;
    v0[ 6] = v1[ 6] * v2;	v0[ 7] = v1[ 7] * v2;
    v0[ 8] = v1[ 8] * v2;	v0[ 9] = v1[ 9] * v2;
    v0[10] = v1[10] * v2;	v0[11] = v1[11] * v2;
    v0[12] = v1[12] * v2;	v0[13] = v1[13] * v2;
    v0[14] = v1[14] * v2;	v0[15] = v1[15] * v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_divs(oobject_t list, oint32_t ac)
/* float64_t m4d.divs(float64_t v0[16], float64_t v1[16], float64_t s0)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_t		*alist;
    double			*v0, *v1, v2;

    alist = (nat_vec_vec_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    v2 = alist->a2;
    v0[ 0] = v1[ 0] / v2;	v0[ 1] = v1[ 1] / v2;
    v0[ 2] = v1[ 2] / v2;	v0[ 3] = v1[ 3] / v2;
    v0[ 4] = v1[ 4] / v2;	v0[ 5] = v1[ 5] / v2;
    v0[ 6] = v1[ 6] / v2;	v0[ 7] = v1[ 7] / v2;
    v0[ 8] = v1[ 8] / v2;	v0[ 9] = v1[ 9] / v2;
    v0[10] = v1[10] / v2;	v0[11] = v1[11] / v2;
    v0[12] = v1[12] / v2;	v0[13] = v1[13] / v2;
    v0[14] = v1[14] / v2;	v0[15] = v1[15] / v2;
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_inverse(oobject_t list, oint32_t ac)
/* float64_t m4d.inverse(float64_t v0[16], float64_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    (void)m4d_det_inverse(alist->a0->v.f64, alist->a1->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
native_m4d_eq(oobject_t list, oint32_t ac)
/* uint8_t m4d.eq(float64_t v0[16], float64_t v1[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD4(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[ 0] == v1[ 0] && v0[ 1] == v1[ 1] &&
		  v0[ 2] == v1[ 2] && v0[ 3] == v1[ 3] &&
		  v0[ 4] == v1[ 4] && v0[ 5] == v1[ 5] &&
		  v0[ 6] == v1[ 6] && v0[ 7] == v1[ 7] &&
		  v0[ 8] == v1[ 8] && v0[ 9] == v1[ 9] &&
		  v0[10] == v1[10] && v0[11] == v1[11] &&
		  v0[12] == v1[12] && v0[13] == v1[13] &&
		  v0[14] == v1[14] && v0[15] == v1[15];
    }
    else
	r0->v.w = true;
}

static void
native_m4d_ne(oobject_t list, oint32_t ac)
/* uint8_t m4d.ne(float64_t v0[16], float64_t v1[16]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    ofloat64_t			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_MD4(alist->a0);
    if (likely(alist->a0 != alist->a1)) {
	v0 = alist->a0->v.f64;
	CHECK_MD4(alist->a1);
	v1 = alist->a1->v.f64;
	r0->v.w = v0[ 0] != v1[ 0] || v0[ 1] != v1[ 1] ||
		  v0[ 2] != v1[ 2] || v0[ 3] != v1[ 3] ||
		  v0[ 4] != v1[ 4] || v0[ 5] != v1[ 5] ||
		  v0[ 6] != v1[ 6] || v0[ 7] != v1[ 7] ||
		  v0[ 8] != v1[ 8] || v0[ 9] != v1[ 9] ||
		  v0[10] != v1[10] || v0[11] != v1[11] ||
		  v0[12] != v1[12] || v0[13] != v1[13] ||
		  v0[14] != v1[14] || v0[15] != v1[15];
    }
    else
	r0->v.w = false;
}

static void
native_m4d_neg(oobject_t list, oint32_t ac)
/* float64_t m4d.neg(float64_t v0[16], float64_t v1[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_MD4(alist->a1);
    v1 = alist->a1->v.f64;
    v0[ 0] = -v1[ 0]; v0[ 1] = -v1[ 1]; v0[ 2] = -v1[ 2]; v0[ 3] = -v1[ 3];
    v0[ 4] = -v1[ 4]; v0[ 5] = -v1[ 5]; v0[ 6] = -v1[ 6]; v0[ 7] = -v1[ 7];
    v0[ 8] = -v1[ 8]; v0[ 9] = -v1[ 9]; v0[10] = -v1[10]; v0[11] = -v1[11];
    v0[12] = -v1[12]; v0[13] = -v1[13]; v0[14] = -v1[14]; v0[15] = -v1[15];
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_add(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16])
{
    v0[ 0] = v1[ 0] + v2[ 0];	v0[ 1] = v1[ 1] + v2[ 1];
    v0[ 2] = v1[ 2] + v2[ 2];	v0[ 3] = v1[ 3] + v2[ 3];
    v0[ 4] = v1[ 4] + v2[ 4];	v0[ 5] = v1[ 5] + v2[ 5];
    v0[ 6] = v1[ 6] + v2[ 6];	v0[ 7] = v1[ 7] + v2[ 7];
    v0[ 8] = v1[ 8] + v2[ 8];	v0[ 9] = v1[ 9] + v2[ 9];
    v0[10] = v1[10] + v2[10];	v0[11] = v1[11] + v2[11];
    v0[12] = v1[12] + v2[12];	v0[13] = v1[13] + v2[13];
    v0[14] = v1[14] + v2[14];	v0[15] = v1[15] + v2[15];
}

static void
native_m4d_add(oobject_t list, oint32_t ac)
/* float64_t m4d.add(float64_t v0[16],
		     float64_t v1[16], float64_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_MD4(alist->a2);
    m4d_add(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_sub(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16])
{
    v0[ 0] = v1[ 0] - v2[ 0];	v0[ 1] = v1[ 1] - v2[ 1];
    v0[ 2] = v1[ 2] - v2[ 2];	v0[ 3] = v1[ 3] - v2[ 3];
    v0[ 4] = v1[ 4] - v2[ 4];	v0[ 5] = v1[ 5] - v2[ 5];
    v0[ 6] = v1[ 6] - v2[ 6];	v0[ 7] = v1[ 7] - v2[ 7];
    v0[ 8] = v1[ 8] - v2[ 8];	v0[ 9] = v1[ 9] - v2[ 9];
    v0[10] = v1[10] - v2[10];	v0[11] = v1[11] - v2[11];
    v0[12] = v1[12] - v2[12];	v0[13] = v1[13] - v2[13];
    v0[14] = v1[14] - v2[14];	v0[15] = v1[15] - v2[15];
}

static void
native_m4d_sub(oobject_t list, oint32_t ac)
/* float64_t m4d.sub(float64_t v0[16],
		     float64_t v1[16], float64_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_MD4(alist->a2);
    m4d_sub(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_mul(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16])
{
    double		a00, a01, a02, a03;
    double		a10, a11, a12, a13;
    double		a20, a21, a22, a23;
    double		a30, a31, a32, a33;
    double		b00, b01, b02, b03;
    double		b10, b11, b12, b13;
    double		b20, b21, b22, b23;
    double		b30, b31, b32, b33;

    a00 = A4(v1, 0, 0);	a01 = A4(v1, 0, 1);
    a02 = A4(v1, 0, 2);	a03 = A4(v1, 0, 3);
    a10 = A4(v1, 1, 0);	a11 = A4(v1, 1, 1);
    a12 = A4(v1, 1, 2);	a13 = A4(v1, 1, 3);
    a20 = A4(v1, 2, 0);	a21 = A4(v1, 2, 1);
    a22 = A4(v1, 2, 2);	a23 = A4(v1, 2, 3);
    a30 = A4(v1, 3, 0);	a31 = A4(v1, 3, 1);
    a32 = A4(v1, 3, 2);	a33 = A4(v1, 3, 3);
    b00 = A4(v2, 0, 0);	b01 = A4(v2, 0, 1);
    b02 = A4(v2, 0, 2);	b03 = A4(v2, 0, 3);
    b10 = A4(v2, 1, 0);	b11 = A4(v2, 1, 1);
    b12 = A4(v2, 1, 2);	b13 = A4(v2, 1, 3);
    b20 = A4(v2, 2, 0);	b21 = A4(v2, 2, 1);
    b22 = A4(v2, 2, 2);	b23 = A4(v2, 2, 3);
    b30 = A4(v2, 3, 0);	b31 = A4(v2, 3, 1);
    b32 = A4(v2, 3, 2);	b33 = A4(v2, 3, 3);
    A4(v0, 0, 0) = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    A4(v0, 0, 1) = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    A4(v0, 0, 2) = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    A4(v0, 0, 3) = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    A4(v0, 1, 0) = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    A4(v0, 1, 1) = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    A4(v0, 1, 2) = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    A4(v0, 1, 3) = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    A4(v0, 2, 0) = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    A4(v0, 2, 1) = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    A4(v0, 2, 2) = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    A4(v0, 2, 3) = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    A4(v0, 3, 0) = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    A4(v0, 3, 1) = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    A4(v0, 3, 2) = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    A4(v0, 3, 3) = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

static void
native_m4d_mul(oobject_t list, oint32_t ac)
/* float64_t m4d.mul(float64_t v0[16],
		     float64_t v1[16], float64_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_MD4(alist->a2);
    m4d_mul(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_div(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[16])
{
    ofloat64_t			 inv[16];

    (void)m4d_det_inverse(inv, v2);
    m4d_mul(v0, v1, inv);
}

static void
native_m4d_div(oobject_t list, oint32_t ac)
/* float64_t m4d.div(float64_t v0[16],
		     float64_t v1[16], float64_t v2[16])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_MD4(alist->a2);
    m4d_div(alist->a0->v.f64,  alist->a1->v.f64,  alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_translate(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[3])
{
    ofloat64_t		res0[4], res1[4], res2[4];

    v4d_muls(res0, v1 + 0, v2[0]);
    v4d_muls(res1, v1 + 4, v2[1]);
    v4d_muls(res2, v1 + 8, v2[2]);
    if (likely(v0 != v1))
	memcpy(v0, v1, sizeof(ofloat64_t) * 16);
    v4d_add(v0 + 12, v0 + 12, res0);
    v4d_add(v0 + 12, v0 + 12, res1);
    v4d_add(v0 + 12, v0 + 12, res2);
}

static void
native_m4d_translate(oobject_t list, oint32_t ac)
/* float64_t m4d.translate(float64_t v0[16],
			   float64_t v1[16], float64_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_VD3(alist->a2);
    m4d_translate(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_rotate(ofloat64_t v0[16], ofloat64_t v1[16],
	   ofloat64_t angle, ofloat64_t v2[3])
{
    ofloat64_t		 c, s, axis[3], tmp[3];
    ofloat64_t		*vn, rot[16], res[16], res0[4], res1[4], res2[4];

    if (unlikely(v0 == v1))
	vn = res;
    else
	vn = v0;
    c = cos(angle);
    s = sin(angle);

    v3d_normalize(axis, v2);

    v3d_fill(tmp, 1.0 - c);
    v3d_mul(tmp, tmp, axis);

    A4(rot, 0, 0) = c + tmp[0] * axis[0];
    A4(rot, 0, 1) =     tmp[0] * axis[1] + s * axis[2];
    A4(rot, 0, 2) =     tmp[0] * axis[2] - s * axis[1];

    A4(rot, 1, 0) =     tmp[1] * axis[0] - s * axis[2];
    A4(rot, 1, 1) = c + tmp[1] * axis[1];
    A4(rot, 1, 2) =     tmp[1] * axis[2] + s * axis[0];

    A4(rot, 2, 0) =     tmp[2] * axis[0] + s * axis[1];
    A4(rot, 2, 1) =     tmp[2] * axis[1] - s * axis[0];
    A4(rot, 2, 2) = c + tmp[2] * axis[2];

    v4d_muls(res0, v1 + 0, A4(rot, 0, 0));
    v4d_muls(res1, v1 + 4, A4(rot, 0, 1));
    v4d_muls(res2, v1 + 8, A4(rot, 0, 2));
    v4d_add(vn + 0, res0, res1);	v4d_add(vn + 0, vn + 0, res2);

    v4d_muls(res0, v1 + 0, A4(rot, 1, 0));
    v4d_muls(res1, v1 + 4, A4(rot, 1, 1));
    v4d_muls(res2, v1 + 8, A4(rot, 1, 2));
    v4d_add(vn + 4, res0, res1);	v4d_add(vn + 4, vn + 4, res2);

    v4d_muls(res0, v1 + 0, A4(rot, 2, 0));
    v4d_muls(res1, v1 + 4, A4(rot, 2, 1));
    v4d_muls(res2, v1 + 8, A4(rot, 2, 2));
    v4d_add(vn + 8, res0, res1);	v4d_add(vn + 8, vn + 8, res2);

    if (unlikely(v0 == v1))
	memcpy(v0, vn, sizeof(ofloat64_t) * 12);
    else
	memcpy(v0 + 12, v1 + 12, sizeof(ofloat64_t) * 4);
}

static void
native_m4d_rotate(oobject_t list, oint32_t ac)
/* float64_t m4d.rotate(float64_t v0[16], float64_t v1[16],
			float64_t deg, float64_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_f64_vec_t	*alist;

    alist = (nat_vec_vec_f64_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_VD3(alist->a3);
    m4d_rotate(alist->a0->v.f64, alist->a1->v.f64,
	       radians(alist->a2), alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_scale(ofloat64_t v0[16], ofloat64_t v1[16], ofloat64_t v2[3])
{
    v4d_muls(v0 + 0, v1 + 0, v2[0]);
    v4d_muls(v0 + 4, v1 + 4, v2[1]);
    v4d_muls(v0 + 8, v1 + 8, v2[2]);
    if (likely(v0 != v1))
	memcpy(v0 + 12, v1 + 12, sizeof(ofloat64_t) * 4);
}

static void
native_m4d_scale(oobject_t list, oint32_t ac)
/* float64_t m4d.scale(float64_t v0[16],
		       float64_t v1[16], float64_t v2[3])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_MD4(alist->a1);
    CHECK_VD3(alist->a2);
    m4d_scale(alist->a0->v.f64, alist->a1->v.f64, alist->a2->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_ortho(ofloat64_t v0[16], ofloat64_t left, ofloat64_t right,
	  ofloat64_t bottom, ofloat64_t top, ofloat64_t zNear, ofloat64_t zFar)
{
    m4d_identity(v0);
    A4(v0, 0, 0) =   2.0 / (right - left);
    A4(v0, 1, 1) =   2.0 / (top - bottom);
    A4(v0, 2, 2) = - 2.0 / (zFar - zNear);
    A4(v0, 3, 0) = - (right + left) / (right - left);
    A4(v0, 3, 1) = - (top + bottom) / (top - bottom);
    A4(v0, 3, 2) = - (zFar + zNear) / (zFar - zNear);
}

static void
native_m4d_ortho(oobject_t list, oint32_t ac)
/* float64_t m4d.ortho(float64_t v0[16], float64_t left, float64_t right,
		       float64_t bottom, float64_t top, float64_t zNear,
		       float64_t zFar)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_f64_f64_f64_f64_f64_t		*alist;

    alist = (nat_vec_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    m4d_ortho(alist->a0->v.f64, alist->a1, alist->a2,
	      alist->a3, alist->a4, alist->a5, alist->a6);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_frustum(ofloat64_t v0[16], ofloat64_t left,
	    ofloat64_t right, ofloat64_t bottom,
	    ofloat64_t top, ofloat64_t nearVal, ofloat64_t farVal)
{
    m4d_fill(v0, 0.0);
    A4(v0, 0, 0) =   (2.0 * nearVal) / (right - left);
    A4(v0, 1, 1) =   (2.0 * nearVal) / (top - bottom);
    A4(v0, 2, 0) =   (right + left) / (right - left);
    A4(v0, 2, 1) = - (top + bottom) / (top - bottom);
    A4(v0, 2, 2) = - (farVal + nearVal) / (farVal - nearVal);
    A4(v0, 2, 3) = - 1.0;
    A4(v0, 3, 2) = - (2.0 * farVal * nearVal) / (farVal - nearVal);
}

static void
native_m4d_frustum(oobject_t list, oint32_t ac)
/* float64_t m4d.frustum(float64_t v0[16], float64_t left, float64_t right,
			 float64_t bottom, float64_t top, float64_t nearVal,
			 float64_t farVal)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_f64_f64_f64_f64_f64_t		*alist;

    alist = (nat_vec_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    m4d_frustum(alist->a0->v.f64, alist->a1, alist->a2,
		alist->a3, alist->a4, alist->a5, alist->a6);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_perspective(ofloat64_t v0[16], ofloat64_t fovy,
		ofloat64_t aspect, ofloat64_t zNear, ofloat64_t zFar)
{
    ofloat64_t			t;

    t = tan(fovy / 2.0);
    m4d_fill(v0, 0.0);
    A4(v0, 0, 0) = 1.0 / (aspect * t);
    A4(v0, 1, 1) = 1.0 / t;
    A4(v0, 2, 2) = - (zFar + zNear) / (zFar - zNear);
    A4(v0, 2, 3) = - 1.0;
    A4(v0, 3, 2) = - (2.0 * zFar * zNear) / (zFar - zNear);
}

static void
native_m4d_perspective(oobject_t list, oint32_t ac)
/* float64_t m4d.perspective(float64_t v0[16], float64_t fovy, float64_t aspect,
			     float64_t zNear, float64_t zFar)[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_f64_f64_f64_f64_t	*alist;

    alist = (nat_vec_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    m4d_perspective(alist->a0->v.f64,
		    radians(alist->a1), alist->a2, alist->a3, alist->a4);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_pickMatrix(ofloat64_t v0[16], ofloat64_t center[2],
	       ofloat64_t delta[2], oint32_t viewport[4])
{
    ofloat64_t		tmp[3];

    m4d_identity(v0);
    tmp[0] = viewport[2] - 2.0 * (center[0] - viewport[0]) / delta[0];
    tmp[1] = viewport[3] - 2.0 * (center[1] - viewport[1]) / delta[1];
    tmp[2] = 0;
    m4d_translate(v0, v0, tmp);
    tmp[0] = viewport[2] / delta[0];
    tmp[1] = viewport[3] / delta[1];
    tmp[2] = 1.0;
    m4d_scale(v0, v0, tmp);
}

static void
native_m4d_pickMatrix(oobject_t list, oint32_t ac)
/* float64_t m4d.pickMatrix(float64_t v0[16], float64_t center[2],
			    float64_t delta[2], int32_t viewport[4])[16]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_VD2(alist->a1);
    CHECK_VD2(alist->a2);
    CHECK_VI4(alist->a3);
    m4d_pickMatrix(alist->a0->v.f64, alist->a1->v.f64,
		   alist->a2->v.f64, alist->a3->v.i32);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}

static void
m4d_lookAt(ofloat64_t v0[16], ofloat64_t eye[3],
	   ofloat64_t center[3], ofloat64_t up[3])
{
    ofloat64_t		f[3], s[3], u[3];

    v3d_sub(f, center, eye);
    v3d_normalize(f, f);

    v3d_cross(u, f, up);
    v3d_normalize(s, u);

    v3d_cross(u, s, f);

    A4(v0, 0, 0) =  s[0];
    A4(v0, 1, 0) =  s[1];
    A4(v0, 2, 0) =  s[2];

    A4(v0, 0, 1) =  u[0];
    A4(v0, 1, 1) =  u[1];
    A4(v0, 2, 1) =  u[2];

    A4(v0, 0, 2) = -f[0];
    A4(v0, 1, 2) = -f[1];
    A4(v0, 2, 2) = -f[2];

    A4(v0, 3, 0) = -v3d_dot(s, eye);
    A4(v0, 3, 1) = -v3d_dot(u, eye);
    A4(v0, 3, 2) =  v3d_dot(f, eye);

    A4(v0, 0, 3) = A4(v0, 1, 3) = A4(v0, 2, 3) = 0.0;
    A4(v0, 3, 3) = 1.0;
}

static void
native_m4d_lookAt(oobject_t list, oint32_t ac)
/* float64_t m4d.lookAt(float64_t v[0], float64_t eye[3],
			float64_t center[3], float64_t up[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_vec_t	*alist;

    alist = (nat_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_MD4(alist->a0);
    CHECK_VD3(alist->a1);
    CHECK_VD3(alist->a2);
    CHECK_VD3(alist->a3);
    m4d_lookAt(alist->a0->v.f64, alist->a1->v.f64,
	       alist->a2->v.f64, alist->a3->v.f64);
    r0->t = t_vector|t_float64;
    r0->v.o = alist->a0;
}
