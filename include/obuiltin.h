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

#ifndef _obuiltin_h
#define _obuiltin_h

#include "otypes.h"

#define define_builtin0(TYPE, NAME)					\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin1(TYPE, NAME, A0)					\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin2(TYPE, NAME, A0, A1)				\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin3(TYPE, NAME, A0, A1, A2)				\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin4(TYPE, NAME, A0, A1, A2, A3)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin5(TYPE, NAME, A0, A1, A2, A3, A4)			\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin6(TYPE, NAME, A0, A1, A2, A3, A4, A5)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin7(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6)		\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin8(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6, A7)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin9(TYPE, NAME, A0, A1, A2, A3, A4, A5, A6, A7, A8)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	onew_argument(builtin, A8);					\
	oend_builtin(builtin);						\
    } while (0)
#define define_builtin10(TYPE,NAME,A0,A1,A2,A3,A4,A5,A6,A7,A8,A9)	\
    do {								\
	builtin = onew_builtin(#NAME, native_##NAME, TYPE, false);	\
	onew_argument(builtin, A0);					\
	onew_argument(builtin, A1);					\
	onew_argument(builtin, A2);					\
	onew_argument(builtin, A3);					\
	onew_argument(builtin, A4);					\
	onew_argument(builtin, A5);					\
	onew_argument(builtin, A6);					\
	onew_argument(builtin, A7);					\
	onew_argument(builtin, A8);					\
	onew_argument(builtin, A9);					\
	oend_builtin(builtin);						\
    } while (0)

#define bad_arg_type(A, T)						\
    (alist->A == null || otype(alist->A) != (T))
#define bad_arg_type_field(A, T, F)					\
    (alist->A == null || otype(alist->A) != (T) || alist->A->F == null)
#define bad_arg_type_length(A, T, L)					\
    (alist->A == null || otype(alist->A) != (T) || alist->A->length != L)

/*
 * Types
 */
#define TYPE1(T0, N)							\
typedef struct {							\
    T0			 a0;						\
} nat_##N##_t
TYPE1(oint8_t,		i8);
TYPE1(ouint8_t,		u8);
TYPE1(oint16_t,		i16);
TYPE1(oint32_t,		i32);
TYPE1(ouint32_t,	u32);
TYPE1(ofloat32_t,	f32);
TYPE1(ofloat64_t,	f64);
TYPE1(ovector_t*,	vec);
#if SDL
TYPE1(ochunk_t*,	chu);
TYPE1(ocontext_t*,	ctx);
TYPE1(oevent_t*,	evt);
TYPE1(ofont_t*,		fnt);
TYPE1(omusic_t*,	mus);
TYPE1(onurbs_t*,	nur);
TYPE1(oquadric_t*,	qua);
TYPE1(orenderer_t*,	ren);
TYPE1(osurface_t*,	srf);
TYPE1(otesselator_t*,	tes);
TYPE1(otexture_t*,	tex);
TYPE1(otimer_t*,	tmr);
TYPE1(owindow_t*,	win);
#endif
#undef TYPE1

#define TYPE2(T0,T1, N)							\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
} nat_##N##_t
TYPE2(oint8_t,oint8_t,
      i8_i8);
TYPE2(oint16_t,oint16_t,
      i16_i16);
TYPE2(oint32_t,oint8_t,
      i32_i8);
TYPE2(oint32_t,ouint8_t,
      i32_u8);
TYPE2(oint32_t,ouint16_t,
      i32_u16);
TYPE2(oint32_t,oint32_t,
      i32_i32);
TYPE2(oint32_t,ovector_t*,
      i32_vec);
TYPE2(ouint32_t,oint32_t,
      u32_i32);
TYPE2(ouint32_t,ouint32_t,
      u32_u32);
TYPE2(ouint32_t,ofloat32_t,
      u32_f32);
TYPE2(ouint32_t,ovector_t*,
      u32_vec);
TYPE2(ouint32_t,oobject_t,
      u32_obj);
TYPE2(ofloat32_t,ofloat32_t,
      f32_f32);
TYPE2(ofloat64_t,ofloat64_t,
      f64_f64);
TYPE2(ovector_t*,oint32_t,
      vec_i32);
TYPE2(ovector_t*,ovector_t*,
      vec_vec);
#if SDL
TYPE2(ochunk_t*,oint32_t,
      chu_i32);
TYPE2(ofont_t*,ouint16_t,
      fnt_u16);
TYPE2(omusic_t*,oint32_t,
      mus_i32);
TYPE2(oquadric_t*,ouint8_t,
      qua_u8);
TYPE2(oquadric_t*,ouint32_t,
      qua_u32);
TYPE2(orenderer_t*,ovector_t*,
      ren_vec);
TYPE2(orenderer_t*,osurface_t*,
      ren_srf);
TYPE2(otesselator_t*,ouint32_t,
      tes_u32);
TYPE2(otesselator_t*,ovector_t*,
      tes_vec);
TYPE2(owindow_t*,ocontext_t*,
      win_ctx);
#endif
#undef TYPE2

#define TYPE3(T0,T1,T2, N)						\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
} nat_##N##_t
TYPE3(oint8_t,oint8_t,oint8_t,
      i8_i8_i8);
TYPE3(ouint8_t,ouint8_t,ouint8_t,
      u8_u8_u8);
TYPE3(oint16_t,oint16_t,oint16_t,
      i16_i16_i16);
TYPE3(ouint16_t,ouint16_t,ouint16_t,
      u16_u16_u16);
TYPE3(oint32_t,ouint8_t,ouint8_t,
      i32_u8_u8);
TYPE3(oint32_t,oint16_t,ouint8_t,
      i32_i16_u8);
TYPE3(oint32_t,oint32_t,oint32_t,
      i32_i32_i32);
TYPE3(oint32_t,ofloat64_t,ofloat64_t,
      i32_f64_f64);
TYPE3(ouint32_t,oint32_t,oint32_t,
      u32_i32_i32);
TYPE3(ouint32_t,oint32_t,ouint32_t,
      u32_i32_u32);
TYPE3(ouint32_t,oint32_t,ovector_t*,
      u32_i32_vec);
TYPE3(ouint32_t,ouint32_t,ovector_t*,
      u32_u32_vec);
TYPE3(ouint32_t,ouint32_t,oint32_t,
      u32_u32_i32);
TYPE3(ouint32_t,ouint32_t,ouint32_t,
      u32_u32_u32);
TYPE3(ouint32_t,ouint32_t,ofloat32_t,
      u32_u32_f32);
TYPE3(ofloat32_t,ofloat32_t,ofloat32_t,
      f32_f32_f32);
TYPE3(ofloat64_t,ofloat64_t,ofloat64_t,
      f64_f64_f64);
#if SDL
TYPE3(oint32_t,ochunk_t*,oint32_t,
      i32_chu_i32);
TYPE3(ofont_t*,ouint16_t,ouint16_t,
      fnt_u16_u16);
TYPE3(ofont_t*,ouint16_t,ocolor_t*,
      fnt_u16_col);
TYPE3(ofont_t*,ovector_t*,ocolor_t*,
      fnt_vec_col);
TYPE3(ofont_t*,ovector_t*,opoint_t*,
      fnt_vec_pnt);
TYPE3(omusic_t*,oint32_t,oint32_t,
      mus_i32_i32);
TYPE3(onurbs_t*,ouint32_t,ofloat32_t,
      nur_u32_f32);
TYPE3(onurbs_t*,ouint32_t,ovector_t*,
      nur_u32_vec);
TYPE3(onurbs_t*,ovector_t*,ouint32_t,
      nur_vec_u32);
TYPE3(orenderer_t*,oint32_t,oint32_t,
      ren_i32_i32);
TYPE3(osurface_t*,oint32_t,oint32_t,
      srf_i32_i32);
TYPE3(otesselator_t*,ouint32_t,ofloat64_t,
      tes_u32_f64);
TYPE3(otesselator_t*,ouint32_t,ovector_t*,
      tes_u32_vec);
TYPE3(owindow_t*,oint32_t,ouint32_t,
      win_i32_u32);
#endif
#undef TYPE3

#define TYPE4(T0,T1,T2,T3, N)						\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
} nat_##N##_t
TYPE4(oint8_t, oint8_t, oint8_t, oint8_t,
      i8_i8_i8_i8);
TYPE4(ouint8_t, ouint8_t, ouint8_t, ouint8_t,
      u8_u8_u8_u8);
TYPE4(oint16_t, oint16_t, oint16_t, oint16_t,
      i16_i16_i16_i16);
TYPE4(ouint16_t, ouint16_t, ouint16_t, ouint16_t,
      u16_u16_u16_u16);
TYPE4(oint32_t, ouint16_t, oint32_t, oint32_t,
      i32_u16_i32_i32);
TYPE4(oint32_t, oint32_t, oint32_t, oint32_t,
      i32_i32_i32_i32);
TYPE4(oint32_t, oint32_t, ouint32_t, ovector_t*,
      i32_i32_u32_vec);
TYPE4(ouint32_t, oint32_t, ouint32_t, ovector_t*,
      u32_i32_u32_vec);
TYPE4(ouint32_t, ouint32_t, ouint32_t, ouint32_t,
      u32_u32_u32_u32);
TYPE4(ofloat32_t, ofloat32_t, ofloat32_t, ofloat32_t,
      f32_f32_f32_f32);
TYPE4(ofloat64_t, ofloat64_t, ofloat64_t, ofloat64_t,
      f64_f64_f64_f64);
#if SDL
TYPE4(oint32_t, ochunk_t*, oint32_t, oint32_t,
      i32_chu_i32_i32);
TYPE4(osurface_t*, orect_t*, osurface_t*, orect_t*,
      srf_rec_srf_rec);
TYPE4(ofont_t*, ouint16_t, ocolor_t*, ocolor_t*,
      fnt_u16_col_col);
TYPE4(ofont_t*, ovector_t*, ocolor_t*, ouint32_t,
      fnt_vec_col_u32);
TYPE4(ofont_t*, ovector_t*, ocolor_t*, ocolor_t*,
      fnt_vec_col_col);
TYPE4(onurbs_t*, ovector_t*, ovector_t*, ovector_t*,
      nur_vec_vec_vec);
TYPE4(oquadric_t*, ofloat64_t, oint32_t, oint32_t,
      qua_f64_i32_i32);
TYPE4(orenderer_t*, ouint32_t, oint32_t, oint32_t,
      ren_u32_i32_i32);
TYPE4(orenderer_t*, otexture_t*, orect_t*, orect_t*,
      ren_tex_rec_rec);
TYPE4(otesselator_t*, ofloat64_t, ofloat64_t, ofloat64_t,
      tes_f64_f64_f64);
#endif
#undef TYPE4

#define TYPE5(T0,T1,T2,T3,T4, N)					\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
} nat_##N##_t
TYPE5(oint32_t, oint32_t, oint32_t, oint32_t, ouint32_t,
      i32_i32_i32_i32_u32);
TYPE5(ouint32_t, oint32_t, oint32_t, oint32_t, oint32_t,
      u32_i32_i32_i32_i32);
TYPE5(ouint32_t, oint32_t, oint32_t, ouint32_t, ovector_t*,
      u32_i32_i32_u32_vec);
TYPE5(ouint32_t, ofloat64_t, ofloat64_t, oint32_t, ovector_t*,
      u32_f64_f64_i32_vec);
TYPE5(ofloat64_t, ofloat64_t, ofloat64_t, ofloat64_t, ovector_t*,
      f64_f64_f64_f64_vec);
#if SDL
TYPE5(onurbs_t*, ovector_t*, ovector_t*, oint32_t, ouint32_t,
      nur_vec_vec_i32_u32);
TYPE5(oquadric_t*, ofloat64_t, ofloat64_t, oint32_t, oint32_t,
      qua_f64_f64_i32_i32);
TYPE5(orenderer_t*, oint32_t, oint32_t, oint32_t, oint32_t,
      ren_i32_i32_i32_i32);
TYPE5(orenderer_t*, ouint32_t, oint32_t, oint32_t, oint32_t,
      ren_u32_i32_i32_i32);
TYPE5(osurface_t*, oint32_t, oint32_t, oint32_t, oint32_t,
      srf_i32_i32_i32_i32);
#endif
#undef TYPE5

#define TYPE6(T0,T1,T2,T3,T4,T5, N)					\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
    T5			 a5;						\
} nat_##N##_t
TYPE6(oint32_t, oint32_t, oint32_t, oint32_t, ouint32_t, ovector_t*,
      i32_i32_i32_i32_u32_vec);
TYPE6(oint32_t, ofloat64_t, ofloat64_t, oint32_t, ofloat64_t, ofloat64_t,
      i32_f64_f64_i32_f64_f64);
TYPE6(ouint32_t, oint32_t, oint32_t, oint32_t, oint32_t, oint32_t,
      u32_i32_i32_i32_i32_i32);
TYPE6(ouint32_t, oint32_t, ouint32_t, oint32_t, oint32_t, oint32_t,
      u32_i32_u32_i32_i32_i32);
TYPE6(ouint32_t, oint32_t, oint32_t, oint32_t, ouint32_t, ovector_t*,
      u32_i32_i32_i32_u32_vec);
TYPE6(ofloat64_t, ofloat64_t, ofloat64_t, ofloat64_t, ofloat64_t, ofloat64_t,
      f64_f64_f64_f64_f64_f64);
TYPE6(ovector_t*, oint32_t, oint32_t, oint32_t, oint32_t, ouint32_t,
      vec_i32_i32_i32_i32_u32);
#if SDL
TYPE6(oquadric_t*, ofloat64_t, ofloat64_t, ofloat64_t, oint32_t, oint32_t,
      qua_f64_f64_f64_i32_i32);
#endif
#undef TYPE6

#define TYPE7(T0,T1,T2,T3,T4,T5,T6, N)					\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
    T5			 a5;						\
    T6			 a6;						\
} nat_##N##_t
TYPE7(oint32_t,oint32_t,ofloat32_t,ofloat32_t,ofloat32_t,ofloat32_t,ovector_t*,
      i32_i32_f32_f32_f32_f32_vec);
TYPE7(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,
      u32_i32_i32_i32_i32_i32_i32);
TYPE7(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,oint32_t,
      u32_i32_i32_i32_i32_u32_i32);
TYPE7(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,ovector_t*,
      u32_i32_i32_i32_i32_u32_vec);
TYPE7(ouint32_t,oint32_t,ofloat32_t,oint32_t,oint32_t,ouint32_t,ovector_t*,
      u32_i32_f32_i32_i32_u32_vec);
TYPE7(ouint32_t,oint32_t,oint32_t,ovector_t*,oint32_t,oint32_t,ovector_t*,
      u32_i32_i32_vec_i32_i32_vec);
TYPE7(ouint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,
      u32_i32_u32_i32_i32_i32_i32);
TYPE7(ofloat64_t,ofloat64_t,ofloat64_t,ovector_t*,ovector_t*,ovector_t*,ovector_t*,
      f64_f64_f64_vec_vec_vec_vec);
#if SDL
TYPE7(orenderer_t*,otexture_t*,orect_t*,orect_t*,ofloat64_t,opoint_t*,oint32_t,
      ren_tex_rec_rec_f64_pnt_i32);
TYPE7(onurbs_t*,ovector_t*,ovector_t*,ovector_t*,oint32_t,oint32_t,ouint32_t,
      nur_vec_vec_vec_i32_i32_u32);
TYPE7(oquadric_t*,ofloat64_t,ofloat64_t,oint32_t,oint32_t,ofloat64_t,ofloat64_t,
      qua_f64_f64_i32_i32_f64_f64);
#endif
#undef TYPE7

#define TYPE8(T0,T1,T2,T3,T4,T5,T6,T7, N)				\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
    T5			 a5;						\
    T6			 a6;						\
    T7			 a7;						\
} nat_##N##_t
TYPE8(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,
      u32_i32_i32_i32_i32_i32_i32_i32);
TYPE8(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,ovector_t*,
      u32_i32_i32_i32_i32_i32_u32_vec);
TYPE8(ouint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,oint32_t,
      u32_i32_u32_i32_i32_i32_i32_i32);
TYPE8(ouint32_t,ofloat64_t,ofloat64_t,oint32_t,ofloat64_t,ofloat64_t,oint32_t,ovector_t*,
      u32_f64_f64_i32_f64_f64_i32_vec);
TYPE8(ouint32_t,oint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,oint32_t,ovector_t*,
      u32_i32_i32_u32_i32_i32_i32_vec);
#undef TYPE8

#define TYPE9(T0,T1,T2,T3,T4,T5,T6,T7,T8, N)				\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
    T5			 a5;						\
    T6			 a6;						\
    T7			 a7;						\
    T8			 a8;						\
} nat_##N##_t
TYPE9(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,ovector_t*,
      u32_i32_i32_i32_i32_u32_i32_i32_vec);
TYPE9(ouint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,oint32_t,ovector_t*,
      u32_i32_i32_i32_u32_i32_i32_i32_vec);
TYPE9(ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,
      f64_f64_f64_f64_f64_f64_f64_f64_f64);
#undef TYPE9

#define TYPE10(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9, N)			\
typedef struct {							\
    T0			 a0;						\
    T1			 a1;						\
    T2			 a2;						\
    T3			 a3;						\
    T4			 a4;						\
    T5			 a5;						\
    T6			 a6;						\
    T7			 a7;						\
    T8			 a8;						\
    T9			 a9;						\
} nat_##N##_t
TYPE10(ouint32_t,oint32_t,oint32_t,oint32_t,oint32_t,ouint32_t,oint32_t,oint32_t,oint32_t,ovector_t*,
       u32_i32_i32_i32_i32_u32_i32_i32_i32_vec);
TYPE10(ofloat64_t,ofloat64_t,ofloat64_t,ofloat64_t,ovector_t*,ovector_t*,ovector_t*,ofloat64_t,ofloat64_t,ovector_t*,
       f64_f64_f64_f64_vec_vec_vec_f64_f64_vec);
#undef TYPE10

#endif /* _obuiltin_h */
