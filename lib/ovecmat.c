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

/*
 * Other than trivial operations, a good share of the code here is direct
 * translation from glm (http://glm.g-truc.net)
 */

/*
 * Prototypes
 */
static void native_v2f_set(oobject_t list, oint32_t ac);
static void v2f_fill(ofloat32_t v0[2], ofloat32_t s0);
static void native_v2f_fill(oobject_t list, oint32_t ac);
static void native_v2f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v2f_length(ofloat32_t v0[2]);
static void native_v2f_length(oobject_t list, oint32_t ac);
static void native_v2f_dot(oobject_t list, oint32_t ac);
static void v2f_normalize(ofloat32_t v0[2], ofloat32_t v1[2]);
static void native_v2f_normalize(oobject_t list, oint32_t ac);
static void v2f_muls(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t s0);
static void native_v2f_muls(oobject_t list, oint32_t ac);
static void native_v2f_neg(oobject_t list, oint32_t ac);
static void v2f_add(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_add(oobject_t list, oint32_t ac);
static void v2f_sub(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_sub(oobject_t list, oint32_t ac);
static void v2f_mul(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_mul(oobject_t list, oint32_t ac);
static void v2f_div(ofloat32_t v0[2], ofloat32_t v1[2], ofloat32_t v2[2]);
static void native_v2f_div(oobject_t list, oint32_t ac);

static void native_v3f_set(oobject_t list, oint32_t ac);
static void v3f_fill(ofloat32_t v0[3], ofloat32_t s0);
static void native_v3f_fill(oobject_t list, oint32_t ac);
static void native_v3f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v3f_length(ofloat32_t v0[3]);
static void native_v3f_length(oobject_t list, oint32_t ac);
static void native_v3f_dot(oobject_t list, oint32_t ac);
static void v3f_normalize(ofloat32_t v0[3], ofloat32_t v1[3]);
static void native_v3f_normalize(oobject_t list, oint32_t ac);
static void v3f_muls(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t s0);
static void native_v3f_muls(oobject_t list, oint32_t ac);
static void native_v3f_cross(oobject_t list, oint32_t ac);
static void native_v3f_neg(oobject_t list, oint32_t ac);
static void v3f_add(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_add(oobject_t list, oint32_t ac);
static void v3f_sub(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_sub(oobject_t list, oint32_t ac);
static void v3f_mul(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_mul(oobject_t list, oint32_t ac);
static void v3f_div(ofloat32_t v0[3], ofloat32_t v1[3], ofloat32_t v2[3]);
static void native_v3f_div(oobject_t list, oint32_t ac);

static void native_v4f_set(oobject_t list, oint32_t ac);
static void v4f_fill(ofloat32_t v0[4], ofloat32_t s0);
static void native_v4f_fill(oobject_t list, oint32_t ac);
static void native_v4f_copy(oobject_t list, oint32_t ac);
static ofloat32_t v4f_length(ofloat32_t v0[4]);
static void native_v4f_length(oobject_t list, oint32_t ac);
static void native_v4f_dot(oobject_t list, oint32_t ac);
static void v4f_normalize(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_v4f_normalize(oobject_t list, oint32_t ac);
static void v4f_muls(ofloat32_t v0[4], ofloat32_t v1[1], ofloat32_t s0);
static void native_v4f_muls(oobject_t list, oint32_t ac);
static void native_v4f_neg(oobject_t list, oint32_t ac);
static void v4f_add(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_add(oobject_t list, oint32_t ac);
static void v4f_sub(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_sub(oobject_t list, oint32_t ac);
static void v4f_mul(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_mul(oobject_t list, oint32_t ac);
static void v4f_div(ofloat32_t v0[4], ofloat32_t v1[4], ofloat32_t v2[4]);
static void native_v4f_div(oobject_t list, oint32_t ac);

static void native_v2d_set(oobject_t list, oint32_t ac);
static void v2d_fill(ofloat64_t v0[2], ofloat64_t s0);
static void native_v2d_fill(oobject_t list, oint32_t ac);
static void native_v2d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v2d_length(ofloat64_t v0[2]);
static void native_v2d_length(oobject_t list, oint32_t ac);
static void native_v2d_dot(oobject_t list, oint32_t ac);
static void v2d_normalize(ofloat64_t v0[2], ofloat64_t v1[2]);
static void native_v2d_normalize(oobject_t list, oint32_t ac);
static void v2d_muls(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t s0);
static void native_v2d_muls(oobject_t list, oint32_t ac);
static void native_v2d_neg(oobject_t list, oint32_t ac);
static void v2d_add(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_add(oobject_t list, oint32_t ac);
static void v2d_sub(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_sub(oobject_t list, oint32_t ac);
static void v2d_mul(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_mul(oobject_t list, oint32_t ac);
static void v2d_div(ofloat64_t v0[2], ofloat64_t v1[2], ofloat64_t v2[2]);
static void native_v2d_div(oobject_t list, oint32_t ac);

static void native_v3d_set(oobject_t list, oint32_t ac);
static void v3d_fill(ofloat64_t v0[3], ofloat64_t s0);
static void native_v3d_fill(oobject_t list, oint32_t ac);
static void native_v3d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v3d_length(ofloat64_t v0[3]);
static void native_v3d_length(oobject_t list, oint32_t ac);
static void native_v3d_dot(oobject_t list, oint32_t ac);
static void v3d_normalize(ofloat64_t v0[3], ofloat64_t v1[3]);
static void native_v3d_normalize(oobject_t list, oint32_t ac);
static void v3d_muls(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t s0);
static void native_v3d_muls(oobject_t list, oint32_t ac);
static void native_v3d_cross(oobject_t list, oint32_t ac);
static void native_v3d_neg(oobject_t list, oint32_t ac);
static void v3d_add(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_add(oobject_t list, oint32_t ac);
static void v3d_sub(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_sub(oobject_t list, oint32_t ac);
static void v3d_mul(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_mul(oobject_t list, oint32_t ac);
static void v3d_div(ofloat64_t v0[3], ofloat64_t v1[3], ofloat64_t v2[3]);
static void native_v3d_div(oobject_t list, oint32_t ac);

static void native_v4d_set(oobject_t list, oint32_t ac);
static void v4d_fill(ofloat64_t v0[4], ofloat64_t s0);
static void native_v4d_fill(oobject_t list, oint32_t ac);
static void native_v4d_copy(oobject_t list, oint32_t ac);
static ofloat64_t v4d_length(ofloat64_t v0[4]);
static void native_v4d_length(oobject_t list, oint32_t ac);
static void native_v4d_dot(oobject_t list, oint32_t ac);
static void v4d_normalize(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_v4d_normalize(oobject_t list, oint32_t ac);
static void v4d_muls(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t s0);
static void native_v4d_muls(oobject_t list, oint32_t ac);
static void native_v4d_neg(oobject_t list, oint32_t ac);
static void v4d_add(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_add(oobject_t list, oint32_t ac);
static void v4d_sub(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_sub(oobject_t list, oint32_t ac);
static void v4d_mul(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_mul(oobject_t list, oint32_t ac);
static void v4d_div(ofloat64_t v0[4], ofloat64_t v1[4], ofloat64_t v2[4]);
static void native_v4d_div(oobject_t list, oint32_t ac);

static void native_m2f_set(oobject_t list, oint32_t ac);
static void m2f_fill(ofloat32_t v0[4], ofloat32_t s0);
static void native_m2f_fill(oobject_t list, oint32_t ac);
static void m2f_identity(ofloat32_t v0[4]);
static void native_m2f_identity(oobject_t list, oint32_t ac);
static void native_m2f_copy(oobject_t list, oint32_t ac);
static ofloat32_t m2f_det_inverse(ofloat32_t v0[4], ofloat32_t v1[4]);
static void native_m2f_det(oobject_t list, oint32_t ac);
static void native_m2f_transpose(oobject_t list, oint32_t ac);
static void native_m2f_muls(oobject_t list, oint32_t ac);
static void native_m2f_inverse(oobject_t list, oint32_t ac);
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
static void native_m3f_muls(oobject_t list, oint32_t ac);
static void native_m3f_inverse(oobject_t list, oint32_t ac);
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
static void native_m4f_muls(oobject_t list, oint32_t ac);
static void native_m4f_inverse(oobject_t list, oint32_t ac);
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

static void native_m2d_set(oobject_t list, oint32_t ac);
static void m2d_fill(ofloat64_t v0[4], ofloat64_t s0);
static void native_m2d_fill(oobject_t list, oint32_t ac);
static void m2d_identity(ofloat64_t v0[4]);
static void native_m2d_identity(oobject_t list, oint32_t ac);
static void native_m2d_copy(oobject_t list, oint32_t ac);
static ofloat64_t m2d_det_inverse(ofloat64_t v0[4], ofloat64_t v1[4]);
static void native_m2d_det(oobject_t list, oint32_t ac);
static void native_m2d_transpose(oobject_t list, oint32_t ac);
static void native_m2d_muls(oobject_t list, oint32_t ac);
static void native_m2d_inverse(oobject_t list, oint32_t ac);
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
static void native_m3d_muls(oobject_t list, oint32_t ac);
static void native_m3d_inverse(oobject_t list, oint32_t ac);
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
static void native_m4d_muls(oobject_t list, oint32_t ac);
static void native_m4d_inverse(oobject_t list, oint32_t ac);
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

/*
 * Implementation
 */
void
init_vecmat(void)
{
    orecord_t			*record;
    obuiltin_t			*builtin;

    record = current_record;

#define t_f				t_float32
#define t_vf				(t_vector|t_float32)
#define t_d				t_float64
#define t_vd				(t_vector|t_float64)

    current_record = v2f_record;
    define_nsbuiltin3(t_vf, v2f_, set, t_vf, t_f, t_f);
    define_nsbuiltin2(t_vf, v2f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v2f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v2f_, length, t_vf);
    define_nsbuiltin2(t_d, v2f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v2f_, normalize, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, v2f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v2f_, div, t_vf, t_vf, t_vf);

    current_record = v3f_record;
    define_nsbuiltin4(t_vf, v3f_, set, t_vf, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, v3f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v3f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v3f_, length, t_vf);
    define_nsbuiltin2(t_d, v3f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, normalize, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin3(t_vf, v3f_, cross, t_vf, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v3f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v3f_, div, t_vf, t_vf, t_vf);

    current_record = v4f_record;
    define_nsbuiltin5(t_vf, v4f_, set, t_vf, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, v4f_, fill, t_vf, t_f);
    define_nsbuiltin2(t_vf, v4f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v4f_, length, t_vf);
    define_nsbuiltin2(t_d, v4f_, dot, t_vf, t_vf);
    define_nsbuiltin2(t_vf, v4f_, normalize, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, v4f_, neg, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, add, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, sub, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, mul, t_vf, t_vf, t_vf);
    define_nsbuiltin3(t_vf, v4f_, div, t_vf, t_vf, t_vf);

    current_record = v2d_record;
    define_nsbuiltin3(t_vd, v2d_, set, t_vd, t_d, t_d);
    define_nsbuiltin2(t_vd, v2d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v2d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, v2d_, length, t_vd);
    define_nsbuiltin2(t_d, v2d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v2d_, normalize, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, v2d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v2d_, div, t_vd, t_vd, t_vd);

    current_record = v3d_record;
    define_nsbuiltin4(t_vd, v3d_, set, t_vd, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, v3d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v3d_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, v3d_, length, t_vd);
    define_nsbuiltin2(t_d, v3d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, normalize, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin3(t_vd, v3d_, cross, t_vd, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v3d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v3d_, div, t_vd, t_vd, t_vd);

    current_record = v4d_record;
    define_nsbuiltin5(t_vd, v4d_, set, t_vd, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, v4d_, fill, t_vd, t_d);
    define_nsbuiltin2(t_vd, v4d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, v4d_, length, t_vd);
    define_nsbuiltin2(t_d, v4d_, dot, t_vd, t_vd);
    define_nsbuiltin2(t_vd, v4d_, normalize, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, v4d_, neg, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, add, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, sub, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, mul, t_vd, t_vd, t_vd);
    define_nsbuiltin3(t_vd, v4d_, div, t_vd, t_vd, t_vd);

    current_record = m2f_record;
    define_nsbuiltin5(t_vf, m2f_, set,
		      t_vf, t_f, t_f, t_f, t_f);
    define_nsbuiltin2(t_vf, m2f_, fill, t_vf, t_f);
    define_nsbuiltin1(t_vf, m2f_, identity, t_vf);
    define_nsbuiltin2(t_vf, m2f_, copy, t_vf, t_vf);
    define_nsbuiltin1(t_d, m2f_, det, t_vf);
    define_nsbuiltin2(t_vf, m2f_, transpose, t_vf, t_vf);
    define_nsbuiltin3(t_vf, m2f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m2f_, inverse, t_vf, t_vf);
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
    define_nsbuiltin3(t_vf, m3f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m3f_, inverse, t_vf, t_vf);
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
    define_nsbuiltin3(t_vf, m4f_, muls, t_vf, t_vf, t_f);
    define_nsbuiltin2(t_vf, m4f_, inverse, t_vf, t_vf);
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

    current_record = m2d_record;
    define_nsbuiltin5(t_vd, m2d_, set,
		      t_vd, t_d, t_d, t_d, t_d);
    define_nsbuiltin2(t_vd, m2d_, fill, t_vd, t_d);
    define_nsbuiltin1(t_vd, m2d_, identity, t_vd);
    define_nsbuiltin2(t_vd, m2d_, copy, t_vd, t_vd);
    define_nsbuiltin1(t_d, m2d_, det, t_vd);
    define_nsbuiltin2(t_vd, m2d_, transpose, t_vd, t_vd);
    define_nsbuiltin3(t_vd, m2d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m2d_, inverse, t_vd, t_vd);
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
    define_nsbuiltin3(t_vd, m3d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m3d_, inverse, t_vd, t_vd);
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
    define_nsbuiltin3(t_vd, m4d_, muls, t_vd, t_vd, t_d);
    define_nsbuiltin2(t_vd, m4d_, inverse, t_vd, t_vd);
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

    current_record = record;
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

static void
native_v2f_dot(oobject_t list, oint32_t ac)
/* float64_t v2f.dot(float32_t v0[2], float32_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF2(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF2(alist->a1);
    v1 = alist->a1->v.f32;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1];
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
native_v2f_neg(oobject_t list, oint32_t ac)
/* float32_t v3f.neg(float32_t v0[2], float32_t v1[2])[2]; */
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

static void
native_v3f_dot(oobject_t list, oint32_t ac)
/* float64_t v3f.dot(float32_t v0[3], float32_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF3(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF3(alist->a1);
    v1 = alist->a1->v.f32;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
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
native_v3f_cross(oobject_t list, oint32_t ac)
/* float32_t v3f.cross(float32_t v0[3], float32_t v1[3], float32_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;
    float			*v0, *v1, *v2, v[9];

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VF3(alist->a0);
    CHECK_VF3(alist->a1);
    v1 = alist->a1->v.f32;
    CHECK_VF3(alist->a2);
    v2 = alist->a2->v.f32;
    if (likely(alist->a0 != alist->a1 && alist->a0 != alist->a2))
	v0 = alist->a0->v.f32;
    else
	v0 = v;
    v0[0] = v1[1] * v2[2] - v1[2] * v2[1];
    v0[1] = v1[2] * v2[0] - v1[0] * v2[2];
    v0[2] = v1[0] * v2[1] - v1[1] * v2[0];
    if (unlikely(v0 != alist->a0->v.f32)) {
	v0[0] = v[0];
	v0[1] = v[1];
	v0[2] = v[2];
    }
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

static void
native_v4f_dot(oobject_t list, oint32_t ac)
/* float64_t v4f.dot(float32_t v0[4], float32_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    float			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VF4(alist->a0);
    v0 = alist->a0->v.f32;
    CHECK_VF4(alist->a1);
    v1 = alist->a1->v.f32;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
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

static void
native_v2d_dot(oobject_t list, oint32_t ac)
/* float64_t v2d.dot(float64_t v0[2], float64_t v1[2]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD2(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD2(alist->a1);
    v1 = alist->a1->v.f64;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1];
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

static void
native_v3d_dot(oobject_t list, oint32_t ac)
/* float64_t v3d.dot(float64_t v0[3], float64_t v1[3]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD3(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD3(alist->a1);
    v1 = alist->a1->v.f64;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
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
native_v3d_cross(oobject_t list, oint32_t ac)
/* float64_t v3d.cross(float64_t v0[3], float64_t v1[3], float64_t v2[3])[3]; */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_vec_t		*alist;
    double			*v0, *v1, *v2, v[9];

    alist = (nat_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_VD3(alist->a0);
    CHECK_VD3(alist->a1);
    v1 = alist->a1->v.f64;
    CHECK_VD3(alist->a2);
    v2 = alist->a2->v.f64;
    if (likely(alist->a0 != alist->a1 && alist->a0 != alist->a2))
	v0 = alist->a0->v.f64;
    else
	v0 = v;
    v0[0] = v1[1] * v2[2] - v1[2] * v2[1];
    v0[1] = v1[2] * v2[0] - v1[0] * v2[2];
    v0[2] = v1[0] * v2[1] - v1[1] * v2[0];
    if (unlikely(v0 != alist->a0->v.f64)) {
	v0[0] = v[0];
	v0[1] = v[1];
	v0[2] = v[2];
    }
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

static void
native_v4d_dot(oobject_t list, oint32_t ac)
/* float64_t v4d.dot(float64_t v0[4], float64_t v1[4]); */
{
    GET_THREAD_SELF()
    oregister_t			*r0;
    nat_vec_vec_t		*alist;
    double			*v0, *v1;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_float64;
    CHECK_VD4(alist->a0);
    v0 = alist->a0->v.f64;
    CHECK_VD4(alist->a1);
    v1 = alist->a1->v.f64;
    r0->v.d = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2] + v0[3] * v1[3];
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
