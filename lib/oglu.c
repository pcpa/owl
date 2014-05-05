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

#define check_mult(x, y)						\
    do {								\
	if (0x7fffffff / (y) < (x))					\
	    ovm_raise(except_not_a_32_bits_integer);			\
    } while (0)

/*
 * Prototypes
 */
static void native_BeginCurve(oobject_t list, oint32_t ac);
static void native_BeginPolygon(oobject_t list, oint32_t ac);
static void native_BeginSurface(oobject_t list, oint32_t ac);
static void native_BeginTrim(oobject_t list, oint32_t ac);
static void native_Build1DMipmapLevels(oobject_t list, oint32_t ac);
static void native_Build1DMipmaps(oobject_t list, oint32_t ac);
static void native_Build2DMipmapLevels(oobject_t list, oint32_t ac);
static void native_Build2DMipmaps(oobject_t list, oint32_t ac);
static void native_Build3DMipmapLevels(oobject_t list, oint32_t ac);
static void native_Build3DMipmaps(oobject_t list, oint32_t ac);
static void native_CheckExtension(oobject_t list, oint32_t ac);
static void native_Cylinder(oobject_t list, oint32_t ac);
static void native_DeleteNurbsRenderer(oobject_t list, oint32_t ac);
static void native_DeleteQuadric(oobject_t list, oint32_t ac);
static void native_DeleteTess(oobject_t list, oint32_t ac);
static void native_Disk(oobject_t list, oint32_t ac);
static void native_EndCurve(oobject_t list, oint32_t ac);
static void native_EndPolygon(oobject_t list, oint32_t ac);
static void native_EndSurface(oobject_t list, oint32_t ac);
static void native_EndTrim(oobject_t list, oint32_t ac);
static void native_ErrorString(oobject_t list, oint32_t ac);
static void native_GetNurbsProperty(oobject_t list, oint32_t ac);
static void native_GetString(oobject_t list, oint32_t ac);
static void native_GetTessProperty(oobject_t list, oint32_t ac);
static void native_LoadSamplingMatrices(oobject_t list, oint32_t ac);
static void native_LookAt(oobject_t list, oint32_t ac);
static void native_NewNurbsRenderer(oobject_t list, oint32_t ac);
static void native_NewQuadric(oobject_t list, oint32_t ac);
static void native_NewTess(oobject_t list, oint32_t ac);
static void native_NextContour(oobject_t list, oint32_t ac);
#if 0
/* No callbacks, should be implemented internally? */
/* FIXME still a TODO to callback in the jit code (not too difficult but not
 * too easy either; easier than creating multiple threads, but should be
 * made on a cheaper/faster way) */
static void native_NurbsCallback(oobject_t list, oint32_t ac);
static void native_NurbsCallbackData(oobject_t list, oint32_t ac);
static void native_NurbsCallbackDataEXT(oobject_t list, oint32_t ac);
#endif
static void native_NurbsCurve(oobject_t list, oint32_t ac);
static void native_NurbsProperty(oobject_t list, oint32_t ac);
static void native_NurbsSurface(oobject_t list, oint32_t ac);
static void native_Ortho2D(oobject_t list, oint32_t ac);
static void native_PartialDisk(oobject_t list, oint32_t ac);
static void native_Perspective(oobject_t list, oint32_t ac);
static void native_PickMatrix(oobject_t list, oint32_t ac);
static void native_Project(oobject_t list, oint32_t ac);
static void native_PwlCurve(oobject_t list, oint32_t ac);
#if 0
static void native_QuadricCallback(oobject_t list, oint32_t ac);
#endif
static void native_QuadricDrawStyle(oobject_t list, oint32_t ac);
static void native_QuadricNormals(oobject_t list, oint32_t ac);
static void native_QuadricOrientation(oobject_t list, oint32_t ac);
static void native_QuadricTexture(oobject_t list, oint32_t ac);
static void native_ScaleImage(oobject_t list, oint32_t ac);
static void native_Sphere(oobject_t list, oint32_t ac);
static void native_TessBeginContour(oobject_t list, oint32_t ac);
static void native_TessBeginPolygon(oobject_t list, oint32_t ac);
#if 0
static void native_TessCallback(oobject_t list, oint32_t ac);
#endif
static void native_TessEndContour(oobject_t list, oint32_t ac);
static void native_TessEndPolygon(oobject_t list, oint32_t ac);
static void native_TessNormal(oobject_t list, oint32_t ac);
static void native_TessProperty(oobject_t list, oint32_t ac);
static void native_TessVertex(oobject_t list, oint32_t ac);
static void native_UnProject(oobject_t list, oint32_t ac);
static void native_UnProject4(oobject_t list, oint32_t ac);

/*
 * Initialization
 */
orecord_t	*glu_record;

static struct {
    char	*name;
    int		 value;
} glu[] = {
    /* StringName */
    { "VERSION",			GLU_VERSION },
    { "EXTENSIONS",			GLU_EXTENSIONS },
    /* ErrorCode */
    { "INVALID_ENUM",			GLU_INVALID_ENUM },
    { "INVALID_VALUE",			GLU_INVALID_VALUE },
    { "OUT_OF_MEMORY",			GLU_OUT_OF_MEMORY },
    { "INCOMPATIBLE_GL_VERSION",	GLU_INCOMPATIBLE_GL_VERSION }, 
    { "INVALID_OPERATION",		GLU_INVALID_OPERATION },
    /* NurbsDisplay */
    { "OUTLINE_POLYGON",		GLU_OUTLINE_POLYGON },
    { "OUTLINE_PATCH",			GLU_OUTLINE_PATCH },
    /* NurbsCallback */
    { "NURBS_ERROR",			GLU_NURBS_ERROR }, 
    { "ERROR",				GLU_ERROR },
    { "NURBS_BEGIN",			GLU_NURBS_BEGIN },
    { "NURBS_BEGIN_EXT",		GLU_NURBS_BEGIN_EXT },
    { "NURBS_VERTEX",			GLU_NURBS_VERTEX },
    { "NURBS_VERTEX_EXT",		GLU_NURBS_VERTEX_EXT },
    { "NURBS_NORMAL",			GLU_NURBS_NORMAL },
    { "NURBS_NORMAL_EXT",		GLU_NURBS_NORMAL_EXT },
    { "NURBS_COLOR",			GLU_NURBS_COLOR },
    { "NURBS_COLOR_EXT",		GLU_NURBS_COLOR_EXT },
    { "NURBS_TEXTURE_COORD",		GLU_NURBS_TEXTURE_COORD },
    { "NURBS_TEX_COORD_EXT",		GLU_NURBS_TEX_COORD_EXT },
    { "NURBS_END",			GLU_NURBS_END },
    { "NURBS_END_EXT",			GLU_NURBS_END_EXT },
    { "NURBS_BEGIN_DATA",		GLU_NURBS_BEGIN_DATA },
    { "NURBS_BEGIN_DATA_EXT",		GLU_NURBS_BEGIN_DATA_EXT },
    { "NURBS_VERTEX_DATA",		GLU_NURBS_VERTEX_DATA },
    { "NURBS_VERTEX_DATA_EXT",		GLU_NURBS_VERTEX_DATA_EXT },
    { "NURBS_NORMAL_DATA",		GLU_NURBS_NORMAL_DATA },
    { "NURBS_NORMAL_DATA_EXT",		GLU_NURBS_NORMAL_DATA_EXT },
    { "NURBS_COLOR_DATA",		GLU_NURBS_COLOR_DATA },
    { "NURBS_COLOR_DATA_EXT",		GLU_NURBS_COLOR_DATA_EXT },
    { "NURBS_TEXTURE_COORD_DATA",	GLU_NURBS_TEXTURE_COORD_DATA },
    { "NURBS_TEX_COORD_DATA_EXT",	GLU_NURBS_TEX_COORD_DATA_EXT },
    { "NURBS_END_DATA",			GLU_NURBS_END_DATA },
    { "NURBS_END_DATA_EXT",		GLU_NURBS_END_DATA_EXT },
    /* NurbsError */
    { "NURBS_ERROR1",			GLU_NURBS_ERROR1 },
    { "NURBS_ERROR2",			GLU_NURBS_ERROR2 },
    { "NURBS_ERROR3",			GLU_NURBS_ERROR3 },
    { "NURBS_ERROR4",			GLU_NURBS_ERROR4 },
    { "NURBS_ERROR5",			GLU_NURBS_ERROR5 },
    { "NURBS_ERROR6",			GLU_NURBS_ERROR6 },
    { "NURBS_ERROR7",			GLU_NURBS_ERROR7 },
    { "NURBS_ERROR8",			GLU_NURBS_ERROR8 },
    { "NURBS_ERROR9",			GLU_NURBS_ERROR9 },
    { "NURBS_ERROR10",			GLU_NURBS_ERROR10 },
    { "NURBS_ERROR11",			GLU_NURBS_ERROR11 },
    { "NURBS_ERROR12",			GLU_NURBS_ERROR12 },
    { "NURBS_ERROR13",			GLU_NURBS_ERROR13 },
    { "NURBS_ERROR14",			GLU_NURBS_ERROR14 },
    { "NURBS_ERROR15",			GLU_NURBS_ERROR15 },
    { "NURBS_ERROR16",			GLU_NURBS_ERROR16 },
    { "NURBS_ERROR17",			GLU_NURBS_ERROR17 },
    { "NURBS_ERROR18",			GLU_NURBS_ERROR18 },
    { "NURBS_ERROR19",			GLU_NURBS_ERROR19 },
    { "NURBS_ERROR20",			GLU_NURBS_ERROR20 },
    { "NURBS_ERROR21",			GLU_NURBS_ERROR21 },
    { "NURBS_ERROR22",			GLU_NURBS_ERROR22 },
    { "NURBS_ERROR23",			GLU_NURBS_ERROR23 },
    { "NURBS_ERROR24",			GLU_NURBS_ERROR24 },
    { "NURBS_ERROR25",			GLU_NURBS_ERROR25 },
    { "NURBS_ERROR26",			GLU_NURBS_ERROR26 },
    { "NURBS_ERROR27",			GLU_NURBS_ERROR27 },
    { "NURBS_ERROR28",			GLU_NURBS_ERROR28 },
    { "NURBS_ERROR29",			GLU_NURBS_ERROR29 },
    { "NURBS_ERROR30",			GLU_NURBS_ERROR30 },
    { "NURBS_ERROR31",			GLU_NURBS_ERROR31 },
    { "NURBS_ERROR32",			GLU_NURBS_ERROR32 },
    { "NURBS_ERROR33",			GLU_NURBS_ERROR33 },
    { "NURBS_ERROR34",			GLU_NURBS_ERROR34 },
    { "NURBS_ERROR35",			GLU_NURBS_ERROR35 },
    { "NURBS_ERROR36",			GLU_NURBS_ERROR36 },
    { "NURBS_ERROR37",			GLU_NURBS_ERROR37 },
    /* NurbsProperty */
    { "AUTO_LOAD_MATRIX",		GLU_AUTO_LOAD_MATRIX },
    { "CULLING",			GLU_CULLING },
    { "SAMPLING_TOLERANCE",		GLU_SAMPLING_TOLERANCE },
    { "DISPLAY_MODE",			GLU_DISPLAY_MODE },
    { "PARAMETRIC_TOLERANCE",		GLU_PARAMETRIC_TOLERANCE },
    { "SAMPLING_METHOD",		GLU_SAMPLING_METHOD },
    { "U_STEP",				GLU_U_STEP },
    { "V_STEP",				GLU_V_STEP },
    { "NURBS_MODE",			GLU_NURBS_MODE },
    { "NURBS_MODE_EXT",			GLU_NURBS_MODE_EXT },
    { "NURBS_TESSELLATOR",		GLU_NURBS_TESSELLATOR },
    { "NURBS_TESSELLATOR_EXT",		GLU_NURBS_TESSELLATOR_EXT },
    { "NURBS_RENDERER",			GLU_NURBS_RENDERER },
    { "NURBS_RENDERER_EXT",		GLU_NURBS_RENDERER_EXT },
    /* NurbsSampling */
    { "OBJECT_PARAMETRIC_ERROR",	GLU_OBJECT_PARAMETRIC_ERROR },
    { "OBJECT_PARAMETRIC_ERROR_EXT",	GLU_OBJECT_PARAMETRIC_ERROR_EXT },
    { "OBJECT_PATH_LENGTH",		GLU_OBJECT_PATH_LENGTH },
    { "OBJECT_PATH_LENGTH_EXT",		GLU_OBJECT_PATH_LENGTH_EXT },
    { "PATH_LENGTH",			GLU_PATH_LENGTH },
    { "PARAMETRIC_ERROR",		GLU_PARAMETRIC_ERROR },
    { "DOMAIN_DISTANCE",		GLU_DOMAIN_DISTANCE },
    /* NurbsTrim */
    { "MAP1_TRIM_2",			GLU_MAP1_TRIM_2 },
    { "MAP1_TRIM_3",			GLU_MAP1_TRIM_3 },
    /* QuadricDrawStyle */
    { "POINT",				GLU_POINT },
    { "LINE",				GLU_LINE },
    { "FILL",				GLU_FILL },
    { "SILHOUETTE",			GLU_SILHOUETTE },
    /* QuadricNormal */
    { "SMOOTH",				GLU_SMOOTH },
    { "FLAT",				GLU_FLAT },
    { "NONE",				GLU_NONE },
    /* QuadricOrientation */
    { "OUTSIDE",			GLU_OUTSIDE },
    { "INSIDE",				GLU_INSIDE },
    /* TessCallback */
    { "TESS_BEGIN",			GLU_TESS_BEGIN },
    { "BEGIN",				GLU_BEGIN },
    { "TESS_VERTEX",			GLU_TESS_VERTEX },
    { "VERTEX",				GLU_VERTEX },
    { "TESS_END",			GLU_TESS_END },
    { "END",				GLU_END },
    { "TESS_ERROR",			GLU_TESS_ERROR },
    { "TESS_EDGE_FLAG",			GLU_TESS_EDGE_FLAG },
    { "EDGE_FLAG",			GLU_EDGE_FLAG },
    { "TESS_COMBINE",			GLU_TESS_COMBINE },
    { "TESS_BEGIN_DATA",		GLU_TESS_BEGIN_DATA },
    { "TESS_VERTEX_DATA",		GLU_TESS_VERTEX_DATA },
    { "TESS_END_DATA",			GLU_TESS_END_DATA },
    { "TESS_ERROR_DATA",		GLU_TESS_ERROR_DATA },
    { "TESS_EDGE_FLAG_DATA",		GLU_TESS_EDGE_FLAG_DATA },
    { "TESS_COMBINE_DATA",		GLU_TESS_COMBINE_DATA },
    /* TessContour */
    { "CW",				GLU_CW },
    { "CCW",				GLU_CCW },
    { "INTERIOR",			GLU_INTERIOR },
    { "EXTERIOR",			GLU_EXTERIOR },
    { "UNKNOWN",			GLU_UNKNOWN },
    /* TessProperty */
    { "TESS_WINDING_RULE",		GLU_TESS_WINDING_RULE },
    { "TESS_BOUNDARY_ONLY",		GLU_TESS_BOUNDARY_ONLY },
    { "TESS_TOLERANCE",			GLU_TESS_TOLERANCE },
    /* TessError */
    { "TESS_ERROR1",			GLU_TESS_ERROR1 },
    { "TESS_ERROR2",			GLU_TESS_ERROR2 },
    { "TESS_ERROR3",			GLU_TESS_ERROR3 },
    { "TESS_ERROR4",			GLU_TESS_ERROR4 },
    { "TESS_ERROR5",			GLU_TESS_ERROR5 },
    { "TESS_ERROR6",			GLU_TESS_ERROR6 },
    { "TESS_ERROR7",			GLU_TESS_ERROR7 },
    { "TESS_ERROR8",			GLU_TESS_ERROR8 },
    { "TESS_MISSING_BEGIN_POLYGON",	GLU_TESS_MISSING_BEGIN_POLYGON },
    { "TESS_MISSING_BEGIN_CONTOUR",	GLU_TESS_MISSING_BEGIN_CONTOUR },
    { "TESS_MISSING_END_POLYGON",	GLU_TESS_MISSING_END_POLYGON },
    { "TESS_MISSING_END_CONTOUR",	GLU_TESS_MISSING_END_CONTOUR },
    { "TESS_COORD_TOO_LARGE",		GLU_TESS_COORD_TOO_LARGE },
    { "TESS_NEED_COMBINE_CALLBACK",	GLU_TESS_NEED_COMBINE_CALLBACK },
    /* TessWinding */
    { "TESS_WINDING_ODD",		GLU_TESS_WINDING_ODD },
    { "TESS_WINDING_NONZERO",		GLU_TESS_WINDING_NONZERO },
    { "TESS_WINDING_POSITIVE",		GLU_TESS_WINDING_POSITIVE },
    { "TESS_WINDING_NEGATIVE",		GLU_TESS_WINDING_NEGATIVE },
    { "TESS_WINDING_ABS_GEQ_TWO",	GLU_TESS_WINDING_ABS_GEQ_TWO },
};

/*
 * Implementation
 */
void
init_glu(void)
{
    char			*string;
    oword_t			 offset;
    osymbol_t			*symbol;
    orecord_t			*record;
    ovector_t			*vector;
    obuiltin_t			*builtin;

    for (offset = 0; offset < osize(glu); ++offset) {
	string = glu[offset].name;
	onew_constant(glu_record, oget_string((ouint8_t *)string,
					      strlen(string)),
		      glu[offset].value);
    }

#if __WORDSIZE == 32
#  define word_string		"int32_t"
#  define pointer_string	"uint32_t"
#else
#  define word_string		"int64_t"
#  define pointer_string	"uint64_t"
#endif

#define add_field(type, name)						\
    do {								\
	vector = oget_string((ouint8_t *)type, strlen(type));		\
	symbol = oget_identifier(vector);				\
	if (!symbol->base) {						\
	    symbol = oget_symbol(sdl_record, vector);			\
	    /*assert(symbol && symbol->type);*/				\
	}								\
	vector = oget_string((ouint8_t *)name, strlen(name));		\
	(void)onew_symbol(record, vector, symbol->tag);			\
    } while (0)

    record = type_vector->v.ptr[t_nurbs];
    add_field(pointer_string,	"*nurbs*");
    oend_record(record);

    record = type_vector->v.ptr[t_quadric];
    add_field(pointer_string,	"*quadric*");
    oend_record(record);

    record = type_vector->v.ptr[t_tesselator];
    add_field(pointer_string,	"*tesselator*");
    oend_record(record);

    record = current_record;
    current_record = glu_record;

    define_builtin1(t_void, BeginCurve, t_nurbs);
    define_builtin1(t_void, BeginPolygon, t_tesselator);
    define_builtin1(t_void, BeginSurface, t_nurbs);
    define_builtin1(t_void, BeginTrim, t_nurbs);
    define_builtin7(t_void, Build1DMipmapLevels,
		    t_uint32, t_int32, t_uint32, t_int32,
		    t_int32, t_int32, t_vector|t_uint8);
    define_builtin4(t_void, Build1DMipmaps,
		    t_uint32, t_int32, t_uint32, t_vector|t_uint8);
    define_builtin8(t_void, Build2DMipmapLevels,
		    t_uint32, t_int32, t_int32, t_uint32, t_int32,
		    t_int32, t_int32, t_vector|t_uint8);
    define_builtin5(t_void, Build2DMipmaps,
		    t_uint32, t_int32, t_int32, t_uint32,
		    t_vector|t_uint8);
    define_builtin9(t_void, Build3DMipmapLevels,
		    t_uint32, t_int32, t_int32, t_int32, t_uint32,
		    t_int32, t_int32, t_int32, t_vector|t_uint8);
    define_builtin6(t_void, Build3DMipmaps,
		    t_uint32, t_int32, t_int32, t_int32, t_uint32,
		    t_vector|t_uint8);
    define_builtin2(t_uint8, CheckExtension, t_string, t_string);
    define_builtin6(t_void, Cylinder, t_quadric, t_float64, t_float64,
		    t_float64, t_int32, t_int32);
    define_builtin1(t_void,  DeleteNurbsRenderer, t_nurbs);
    define_builtin1(t_void,  DeleteQuadric, t_quadric);
    define_builtin1(t_void,  DeleteTess, t_tesselator);
    define_builtin5(t_void,  Disk,
		    t_quadric, t_float64, t_float64, t_int32, t_int32);
    define_builtin1(t_void,  EndCurve, t_nurbs);
    define_builtin1(t_void,  EndPolygon, t_tesselator);
    define_builtin1(t_void,  EndSurface, t_nurbs);
    define_builtin1(t_void,  EndTrim, t_nurbs);
    define_builtin1(t_string, ErrorString, t_uint32);
    define_builtin3(t_void,  GetNurbsProperty,
		    t_nurbs, t_uint32, t_vector|t_float32);
    define_builtin1(t_string, GetString, t_uint32);
    define_builtin3(t_void,  GetTessProperty,
		    t_tesselator, t_uint32, t_vector|t_float64);
    define_builtin4(t_void,  LoadSamplingMatrices,
		    t_nurbs, t_vector|t_float64, t_vector|t_float64,
		    t_vector|t_int32);
    define_builtin9(t_void,  LookAt,
		    t_float64, t_float64, t_float64,
		    t_float64, t_float64, t_float64,
		    t_float64, t_float64, t_float64);
    define_builtin0(t_nurbs, NewNurbsRenderer);
    define_builtin0(t_quadric, NewQuadric);
    define_builtin0(t_tesselator, NewTess);
    define_builtin2(t_void, NextContour, t_tesselator, t_uint32);
    define_builtin5(t_void, NurbsCurve,
		    t_nurbs, t_vector|t_float32, t_vector|t_float32,
		    t_int32, t_uint32);
    define_builtin3(t_void, NurbsProperty, t_nurbs, t_uint32, t_float32);
    define_builtin7(t_void, NurbsSurface,
		    t_nurbs, t_vector|t_float32, t_vector|t_float32,
		    t_vector|t_float32, t_int32, t_int32, t_uint32);
    define_builtin4(t_void,  Ortho2D,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin7(t_void,  PartialDisk,
		    t_quadric, t_float64, t_float64, t_int32, t_int32,
		    t_float64, t_float64);
    define_builtin4(t_void,  Perspective,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin5(t_void, PickMatrix,
		    t_float64, t_float64, t_float64, t_float64,
		    t_vector|t_int32);
    define_builtin7(t_void, Project,
		    t_float64, t_float64, t_float64,
		    t_vector|t_float64, t_vector|t_float64, t_vector|t_int32,
		    t_vector|t_float64);
    define_builtin3(t_void, PwlCurve, t_nurbs, t_vector|t_float32, t_uint32);
    define_builtin2(t_void, QuadricNormals, t_quadric, t_uint32);
    define_builtin2(t_void, QuadricOrientation, t_quadric, t_uint32);
    define_builtin2(t_void, QuadricTexture, t_quadric, t_uint8);
    define_builtin7(t_void, ScaleImage,
		    t_uint32, t_int32, t_int32, t_vector|t_uint8, t_int32,
		    t_int32, t_vector|t_uint8);
    define_builtin2(t_void, QuadricDrawStyle, t_quadric, t_uint32);
    define_builtin4(t_void,  Sphere,
		    t_quadric, t_float64, t_int32, t_int32);
    define_builtin1(t_void, TessBeginContour, t_tesselator);
    define_builtin1(t_void, TessBeginPolygon, t_tesselator);
    define_builtin1(t_void, TessEndContour, t_tesselator);
    define_builtin1(t_void, TessEndPolygon, t_tesselator);
    define_builtin4(t_void, TessNormal,
		    t_tesselator, t_float64, t_float64, t_float64);
    define_builtin3(t_void, TessProperty,
		    t_tesselator, t_uint32, t_float64);
    define_builtin2(t_void, TessVertex, t_tesselator, t_vector|t_float64);
    define_builtin7(t_int32, UnProject,
		    t_float64, t_float64, t_float64, t_vector|t_float64,
		    t_vector|t_float64, t_vector|t_int32, t_vector|t_float64);
    define_builtin10(t_int32, UnProject4,
		     t_float64, t_float64, t_float64, t_float64,
		     t_vector|t_float64, t_vector|t_float64, t_vector|t_int32,
		     t_float64, t_float64, t_vector|t_float64);

    current_record = record;
}

void
finish_glu(void)
{
}

static void
native_BeginCurve(oobject_t list, oint32_t ac)
/* void BeginCurve(nurb_t *nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluBeginCurve(alist->a0->__nurbs);
}

static void
native_BeginPolygon(oobject_t list, oint32_t ac)
/* void BeginPolygon(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluBeginPolygon(alist->a0->__tesselator);
}

static void
native_BeginSurface(oobject_t list, oint32_t ac)
/* void BeginSurface(nurbs_t nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluBeginSurface(alist->a0->__nurbs);
}

static void
native_BeginTrim(oobject_t list, oint32_t ac)
/* void BeginTrim(nurbs_t nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluBeginTrim(alist->a0->__nurbs);
}

static void
native_Build1DMipmapLevels(oobject_t list, oint32_t ac)
/* void Build1DMipmapLevels(uint32_t target,
			    int32_t width, uint32_t format, int32_t level,
			    int32_t base, int32_t max, uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_i32_i32_i32_vec_t	*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_u32_i32_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width is a power of 2 */
    if (alist->a1 & (alist->a1 - 1))
	ovm_raise(except_invalid_argument);
    switch (alist->a2 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    length = alist->a1;		/* width */
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, 2);
	    length = alist->a1 * 2;
	    format = 2;
	    break;
	case GL_RGB:
	    check_mult(alist->a1, 3);
	    length = alist->a1 * 3;
	    format = 3;
	    break;
	case GL_RGBA:
	    check_mult(alist->a1, 4);
	    length = alist->a1 * 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a6, length);
    gluBuild1DMipmapLevels(alist->a0, format, alist->a1, alist->a2,
			   GL_UNSIGNED_BYTE, alist->a3, alist->a4, alist->a5,
			   alist->a6->v.u8);
}

static void
native_Build1DMipmaps(oobject_t list, oint32_t ac)
/* void Build1DMipmaps(uint32_t target,
		       int32_t width, uint32_t format, uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_vec_t		*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width is a power of 2 */
    if (alist->a1 & (alist->a1 - 1))
	ovm_raise(except_invalid_argument);
    switch (alist->a2 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    length = alist->a2;		/* width */
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, 2);
	    length = alist->a1 * 2;
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a1, 3);
	    length = alist->a1 * 3;
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a1, 4);
	    length = alist->a1 * 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a3, length);
    gluBuild1DMipmaps(alist->a0, format, alist->a1, alist->a2,
		      GL_UNSIGNED_BYTE, alist->a3->v.u8);
}

static void
native_Build2DMipmapLevels(oobject_t list, oint32_t ac)
/* void Build2DMipmapLevels(uint32_t target,
			    int32_t width, int32_t height, uint32_t format,
			    int32_t level, int32_t base, int32_t max,
			    uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_u32_i32_i32_i32_vec_t	*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_i32_u32_i32_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width and height are powers of 2 */
    if ((alist->a1 & (alist->a1 - 1)) || (alist->a2 & (alist->a2 - 1)))
	ovm_raise(except_invalid_argument);
    switch (alist->a3 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 2);
	    length *= 2;
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 3);
	    length *= 3;
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 4);
	    length *= 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a7);
    CHECK_TYPE(alist->a7, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a7, length);
    gluBuild2DMipmapLevels(alist->a0, format, alist->a1, alist->a2,
			   alist->a3, GL_UNSIGNED_BYTE, alist->a4, alist->a5,
			   alist->a6, alist->a7->v.u8);
}

static void
native_Build2DMipmaps(oobject_t list, oint32_t ac)
/* void Build2DMipmaps(uint32_t target,
		       int32_t width, int32_t height, uint32_t format,
		       uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_u32_vec_t		*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width and height are powers of 2 */
    if ((alist->a1 & (alist->a1 - 1)) || (alist->a2 & (alist->a2 - 1)))
	ovm_raise(except_invalid_argument);
    switch (alist->a3 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 2);
	    length *= 2;
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 3);
	    length *= 3;
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, 4);
	    length *= 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a4, length);
    gluBuild2DMipmaps(alist->a0, format, alist->a1, alist->a2, alist->a3,
		      GL_UNSIGNED_BYTE, alist->a4->v.u8);
}

static void
native_Build3DMipmapLevels(oobject_t list, oint32_t ac)
/* void Build3DMipmapLevels(uint32_t target,
			    int32_t width, int32_t height, int32_t depth,
			    uint32_t format, int32_t level, int32_t base,
			    int32_t max, uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_i32_i32_i32_vec_t	*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_u32_i32_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width, height and depth are powers of 2 */
    if ((alist->a1 & (alist->a1 - 1)) ||
	(alist->a2 & (alist->a2 - 1)) || 
	(alist->a3 & (alist->a3 - 1)))
	ovm_raise(except_invalid_argument);
    switch (alist->a4 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 2);
	    length *= 2;
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 3);
	    length *= 3;
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 4);
	    length *= 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a8);
    CHECK_TYPE(alist->a8, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a8, length);
    gluBuild3DMipmapLevels(alist->a0, format, alist->a1, alist->a2,
			   alist->a3, alist->a4, GL_UNSIGNED_BYTE, alist->a5,
			   alist->a6, alist->a7, alist->a8->v.u8);
}

static void
native_Build3DMipmaps(oobject_t list, oint32_t ac)
/* void Build3DMipmaps(uint32_t target,
		       int32_t width, int32_t height, int32_t depth,
		       uint32_t format, uint8_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_vec_t	*alist;
    ouint32_t				 format;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    /* Validate width, height and depth are powers of 2 */
    if ((alist->a1 & (alist->a1 - 1)) ||
	(alist->a2 & (alist->a2 - 1)) ||
	(alist->a3 & (alist->a3 - 1)))
	ovm_raise(except_invalid_argument);
    switch (alist->a4 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 2);
	    length *= 2;
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 3);
	    length *= 3;
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a1, alist->a2);
	    length = alist->a1 * alist->a2;
	    check_mult(length, alist->a3);
	    length *= alist->a3;
	    check_mult(length, 4);
	    length *= 4;
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0->t = t_void;
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a5, length);
    gluBuild3DMipmaps(alist->a0, format, alist->a1, alist->a2, alist->a3,
		      alist->a4, GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_CheckExtension(oobject_t list, oint32_t ac)
/* uint8_t CheckExtension(string_t extName, string_t extString); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_vec_t			*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_string);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    /* XXX know internals about 16 byte alignment, and if already aligned,
     * cause it to reallocate (what is a noop if previously reallocated)
     * to pad with 1-15 zeroes, i.e. make a "C" string */
    if (!(alist->a0->length & 15)) {
	orenew_vector(alist->a0, alist->a0->length + 1);
	--alist->a0->length;
    }
    if (!(alist->a1->length & 15)) {
	orenew_vector(alist->a1, alist->a1->length + 1);
	--alist->a1->length;
    }
    r0->t = t_word;
    r0->v.w = gluCheckExtension(alist->a0->v.u8, alist->a1->v.u8);
}

static void
native_Cylinder(oobject_t list, oint32_t ac)
/* void Cylinder(quadric_t quad, float64_t base, float64_t top,
		 float64_t height, int32_t slices, int32_t stacks); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_f64_f64_f64_i32_i32_t	*alist;

    alist = (nat_qua_f64_f64_f64_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluCylinder(alist->a0->__quadric, alist->a1, alist->a2, alist->a3,
		alist->a4, alist->a5);
}

static void
native_DeleteNurbsRenderer(oobject_t list, oint32_t ac)
/* void DeleteNurbsRenderer(nurbs_t nurbs); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_nurbs);
	if (alist->a0->__nurbs) {
	    gluDeleteNurbsRenderer(alist->a0->__nurbs);
	    alist->a0->__nurbs = null;
	}
    }
}

static void
native_DeleteQuadric(oobject_t list, oint32_t ac)
/* void DeleteQuadric(quadric_t quadric); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_t				*alist;

    alist = (nat_qua_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_quadric);
	if (alist->a0->__quadric) {
	    gluDeleteQuadric(alist->a0->__quadric);
	    alist->a0->__quadric = null;
	}
    }
}

static void
native_DeleteTess(oobject_t list, oint32_t ac)
/* void DeleteTess(tesselator_t tesselator); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0) {
	CHECK_TYPE(alist->a0, t_tesselator);
	if (alist->a0->__tesselator) {
	    gluDeleteTess(alist->a0->__tesselator);
	    alist->a0->__tesselator = null;
	}
    }
}

static void
native_Disk(oobject_t list, oint32_t ac)
/* void Disk(quadric_t quad, float64_t inner, float64_t outer,
	     int32_t slices, int32_t loops); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_f64_f64_i32_i32_t		*alist;

    alist = (nat_qua_f64_f64_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluDisk(alist->a0->__quadric, alist->a1, alist->a2, alist->a3, alist->a4);
}

static void
native_EndCurve(oobject_t list, oint32_t ac)
/* void EndCurve(nurbs_t nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluEndCurve(alist->a0->__nurbs);
}

static void
native_EndPolygon(oobject_t list, oint32_t ac)
/* void EndPolygon(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluEndPolygon(alist->a0->__tesselator);
}

static void
native_EndSurface(oobject_t list, oint32_t ac)
/* void EndSurface(nurbs_t nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluEndSurface(alist->a0->__nurbs);
}

static void
native_EndTrim(oobject_t list, oint32_t ac)
/* void EndTrim(nurbs_t nurb); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_t				*alist;

    alist = (nat_nur_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluEndTrim(alist->a0->__nurbs);
}

/* FIXME this should be a constant string */
static void
native_ErrorString(oobject_t list, oint32_t ac)
/* string_t ErrorString(uint32_t error); */
{
    GET_THREAD_SELF()
    const GLubyte			*gv;
    ovector_t				*ov;
    oregister_t				*r0;
    nat_u32_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_u32_t *)list;
    if ((gv = gluErrorString(alist->a0))) {
	onew_vector(&thread_self->obj, t_uint8, strlen((char *)gv));
	ov = (ovector_t *)thread_self->obj;
	memcpy(ov->v.u8, gv, ov->length);
	r0->v.o = thread_self->obj;
	r0->t = t_string;
    }
    else
	r0->t = t_void;
}

static void
native_GetNurbsProperty(oobject_t list, oint32_t ac)
/* void GetNurbsProperty(nurbs_t *nurb, uint32_t property, float32_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_u32_vec_t			*alist;

    alist = (nat_nur_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    /* Verify in case of extensions or unexpected future properties return
     * more than one value */
    switch (alist->a1) {
	case GLU_NURBS_MODE:
	case GLU_SAMPLING_METHOD:
	case GLU_SAMPLING_TOLERANCE:
	case GLU_PARAMETRIC_TOLERANCE:
	case GLU_U_STEP:
	case GLU_V_STEP:
	case GLU_DISPLAY_MODE:
	case GLU_CULLING:
	case GLU_AUTO_LOAD_MATRIX:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a2->length != 1)
	orenew_vector(alist->a2, 1);
    r0->t = t_void;
    gluGetNurbsProperty(alist->a0->__nurbs, alist->a1, alist->a2->v.f32);
}

/* FIXME this should be a constant string */
static void
native_GetString(oobject_t list, oint32_t ac)
/* string_t GetString(uint32_t name); */
{
    GET_THREAD_SELF()
    const GLubyte			*gv;
    ovector_t				*ov;
    oregister_t				*r0;
    nat_u32_t				*alist;

    r0 = &thread_self->r0;
    alist = (nat_u32_t *)list;
    if ((gv = gluGetString(alist->a0))) {
	onew_vector(&thread_self->obj, t_uint8, strlen((char *)gv));
	ov = (ovector_t *)thread_self->obj;
	memcpy(ov->v.u8, gv, ov->length);
	r0->v.o = thread_self->obj;
	r0->t = t_string;
    }
    else
	r0->t = t_void;
}

static void
native_GetTessProperty(oobject_t list, oint32_t ac)
/* void GetTessProperty(tesselator_t *tess, uint32_t which,
		        float64_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_u32_vec_t			*alist;

    alist = (nat_tes_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float64);
    /* Verify in case of extensions or unexpected future properties return
     * more than one value */
    switch (alist->a1) {
	case GLU_TESS_WINDING_RULE:
	case GLU_TESS_BOUNDARY_ONLY:
	case GLU_TESS_TOLERANCE:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a2->length != 1)
	orenew_vector(alist->a2, 1);
    r0->t = t_void;
    gluGetTessProperty(alist->a0->__tesselator, alist->a1, alist->a2->v.f64);
}

static void
native_LoadSamplingMatrices(oobject_t list, oint32_t ac)
/* void LoadSamplingMatrices(nurbs_t *nurb, float32_t model[],
			     float32_t perspective[], int32_t view[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_vec_vec_vec_t		*alist;

    alist = (nat_nur_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
    CHECK_LENGTH(alist->a1, 16);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    CHECK_LENGTH(alist->a2, 16);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_int32);
    CHECK_LENGTH(alist->a3, 4);
    r0->t = t_void;
    gluLoadSamplingMatrices(alist->a0->__nurbs, alist->a1->v.f32,
			    alist->a2->v.f32, alist->a3->v.i32);
}

static void
native_LookAt(oobject_t list, oint32_t ac)
/* LookAt(float64_t eyeX, float64_t eyeY, float64_t eyeZ,
	  float64_t centerX, float64_t centerY, float64_t centerZ);
	  float64_t upX, float64_t upY, float64_t upZ); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_f64_f64_f64_f64_f64_t	*alist;

    alist = (nat_f64_f64_f64_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    gluLookAt(alist->a0, alist->a1, alist->a2,
	      alist->a3, alist->a4, alist->a5,
	      alist->a6, alist->a7, alist->a8);
}

static void
native_NewNurbsRenderer(oobject_t list, oint32_t ac)
/* nurbs_t NewNurbsRenderer(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    GLUnurbs				*gn;
    onurbs_t				*on;

    r0 = &thread_self->r0;
    if ((gn = gluNewNurbsRenderer())) {
	onew_object(&thread_self->obj, t_nurbs, sizeof(onurbs_t));
	on = (onurbs_t *)thread_self->obj;
	on->__nurbs = gn;
	r0->v.o = thread_self->obj;
	r0->t = t_nurbs;
    }
    else
	r0->t = t_void;
}

static void
native_NewQuadric(oobject_t list, oint32_t ac)
/* quadric_t NewQuadric(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    GLUquadric				*gq;
    oquadric_t				*oq;

    r0 = &thread_self->r0;
    if ((gq = gluNewQuadric())) {
	onew_object(&thread_self->obj, t_quadric, sizeof(oquadric_t));
	oq = (oquadric_t *)thread_self->obj;
	oq->__quadric = gq;
	r0->v.o = thread_self->obj;
	r0->t = t_quadric;
    }
    else
	r0->t = t_void;
}

static void
native_NewTess(oobject_t list, oint32_t ac)
/* tesselator_t NewTess(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    GLUtesselator			*gt;
    otesselator_t			*ot;

    r0 = &thread_self->r0;
    if ((gt = gluNewTess())) {
	onew_object(&thread_self->obj, t_tesselator, sizeof(otesselator_t));
	ot = (otesselator_t *)thread_self->obj;
	ot->__tesselator = gt;
	r0->v.o = thread_self->obj;
	r0->t = t_tesselator;
    }
    else
	r0->t = t_void;
}

static void
native_NextContour(oobject_t list, oint32_t ac)
/* void NextContour(tesselator_t tess, uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_u32_t			*alist;

    alist = (nat_tes_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluNextContour(alist->a0->__tesselator, alist->a1);
}

static void
native_NurbsCurve(oobject_t list, oint32_t ac)
/* void NurbsCurve(nurbs_t nurb, float32_t knots[], float32_t control[],
		   int32_t order, uint32_t type);  */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_vec_vec_i32_u32_t		*alist;
    oword_t				 length;

    alist = (nat_nur_vec_vec_i32_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    length = alist->a1->length;
    if (alist->a3 < length)
	ovm_raise(except_out_of_bounds);
    /* FIXME this minimum vector length check is probably wrong */
    length -= alist->a3;
    switch (alist->a4) {
	case GLU_MAP1_TRIM_2:
	    check_mult(length, 2);
	    length *= 2;
	    break;
	case GLU_MAP1_TRIM_3:
	    check_mult(length, 3);
	    length *= 3;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    CHECK_BOUNDS(alist->a2, length);
    r0->t = t_void;
    gluNurbsCurve(alist->a0->__nurbs, alist->a1->length, alist->a1->v.f32,
		  0, alist->a2->v.f32, alist->a3, alist->a4);
}

static void
native_Ortho2D(oobject_t list, oint32_t ac)
/* void Ortho2D(float64_t left, float64_t right,
		float64_t bottom, float64_t top);*/
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    gluOrtho2D(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_NurbsProperty(oobject_t list, oint32_t ac)
/* void NurbsProperty(nurbs_t nurb, uint32_t property, float32_t value); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_u32_f32_t			*alist;

    alist = (nat_nur_u32_f32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    r0->t = t_void;
    gluNurbsProperty(alist->a0->__nurbs, alist->a1, alist->a2);
}

static void
native_NurbsSurface(oobject_t list, oint32_t ac)
/* void NurbsSurface(nurbs_t nurb, float32_t sKnots[], float32_t tKnots[],
		     float32_t control[], int32_t sOrder, int32_t tOrder,
		     uint32_t type);  */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_vec_vec_vec_i32_i32_u32_t	*alist;
    oword_t				 length;

    alist = (nat_nur_vec_vec_vec_i32_i32_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_float32);
    /* FIXME this minimum vector length check is probably wrong */
    check_mult(alist->a1->length, alist->a2->length);
    length = alist->a1->length * alist->a2->length;
    check_mult(length, 2);
    length *= 2;
    CHECK_BOUNDS(alist->a3, length);
    r0->t = t_void;
    gluNurbsSurface(alist->a0->__nurbs, alist->a1->length, alist->a1->v.f32,
		    alist->a2->length, alist->a2->v.f32, 0, 0, alist->a3->v.f32,
		    alist->a4, alist->a5, alist->a6);
}

static void
native_PartialDisk(oobject_t list, oint32_t ac)
/* void PartialDisk(quadric_t quad, float64_t inner, float64_t outer,
		    int32_t slices, int32_t loops, float64_t start,
		    float64_t sweep); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_f64_f64_i32_i32_f64_f64_t	*alist;

    alist = (nat_qua_f64_f64_i32_i32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluPartialDisk(alist->a0->__quadric, alist->a1, alist->a2, alist->a3,
		   alist->a4, alist->a5, alist->a6);
}

static void
native_Perspective(oobject_t list, oint32_t ac)
/* static void Perspective(float64_t fovy, float64_t aspec,
			   float64_t zNear, float64_t zFar); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    gluPerspective(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_PickMatrix(oobject_t list, oint32_t ac)
/* void PickMatrix(float64_t x, float64_t y, float64_t delX, float64_t delY,
		   int32_t viewport[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_vec_t		*alist;

    alist = (nat_f64_f64_f64_f64_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_vector|t_int32);
    if (alist->a4->length != 4)
	orenew_vector(alist->a4, 4);
    r0->t = t_void;
    gluPickMatrix(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4->v.i32);
}

static void
native_Project(oobject_t list, oint32_t ac)
/* void Project(float64_t objX, float64_t objY, float64_t objZ,
		float64_t model[], float64_t proj[], int32_t view[],
		float64_t winXYZ[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_vec_vec_vec_vec_t	*alist;

    alist = (nat_f64_f64_f64_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_float64);
    CHECK_LENGTH(alist->a3, 16);
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_vector|t_float64);
    CHECK_LENGTH(alist->a4, 16);
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_int32);
    CHECK_LENGTH(alist->a5, 4);
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_float64);
    if (alist->a6->length != 3)
	orenew_vector(alist->a6, 3);
    r0->t = t_void;
    gluProject(alist->a0, alist->a1, alist->a2, alist->a3->v.f64,
	       alist->a4->v.f64, alist->a5->v.i32, alist->a6->v.f64 + 0,
	       alist->a6->v.f64 + 1, alist->a6->v.f64 + 2);
}

static void
native_PwlCurve(oobject_t list, oint32_t ac)
/* void PwlCurve(nurbs_t nurb, float32_t data[], uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_nur_vec_u32_t			*alist;
    oword_t				 length;

    alist = (nat_nur_vec_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_nurbs);
    CHECK_NULL(alist->a0->__nurbs);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
    switch (alist->a2) {
	case GLU_MAP1_TRIM_2:
	    length = alist->a1->length / 2;
	    break;
	case GLU_MAP1_TRIM_3:
	    length = alist->a1->length / 3;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    gluPwlCurve(alist->a0->__nurbs, length, alist->a1->v.f32, 0, alist->a2);
}

static void
native_QuadricDrawStyle(oobject_t list, oint32_t ac)
/* void QuadricDrawStyle(quadric_t quad, uint32_t draw); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_u32_t			*alist;

    alist = (nat_qua_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluQuadricDrawStyle(alist->a0->__quadric, alist->a1);
}

static void
native_QuadricNormals(oobject_t list, oint32_t ac)
/* void QuadricNormals(quadric_t quad, uint32_t normal); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_u32_t			*alist;

    alist = (nat_qua_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluQuadricNormals(alist->a0->__quadric, alist->a1);
}

static void
native_QuadricOrientation(oobject_t list, oint32_t ac)
/* void QuadricOrientation(quadric_t quad, uint32_t orientation); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_u32_t			*alist;

    alist = (nat_qua_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluQuadricOrientation(alist->a0->__quadric, alist->a1);
}

static void
native_QuadricTexture(oobject_t list, oint32_t ac)
/* void QuadricTexture(quadric_t quad, uint8_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_u8_t			*alist;

    alist = (nat_qua_u8_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluQuadricTexture(alist->a0->__quadric, alist->a1);
}

static void
native_ScaleImage(oobject_t list, oint32_t ac)
/* void ScaleImage(uint32_t format,
		   int32_t wIn, int32_t hIn, uint8_t dataIn[],
		   int32_t wOut, int32_t hOut, uint8_t dataOut[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_vec_i32_i32_vec_t	*alist;
    oword_t				 ilen, olen;

    alist = (nat_u32_i32_i32_vec_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint8);
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_uint8);
    check_mult(alist->a1, alist->a2);
    ilen = alist->a1 * alist->a2;
    check_mult(alist->a4, alist->a5);
    olen = alist->a4 * alist->a5;
    switch (alist->a0 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_DEPTH_COMPONENT:
	    break;
	case GL_LUMINANCE_ALPHA:
	    check_mult(ilen, 2);
	    ilen *= 2;
	    check_mult(olen, 2);
	    olen *= 2;
	    break;
	case GL_RGB:
	    check_mult(ilen, 3);
	    ilen *= 3;
	    check_mult(olen, 3);
	    olen *= 3;
	    break;
	case GL_RGBA:
	    check_mult(ilen, 4);
	    ilen *= 4;
	    check_mult(olen, 4);
	    olen *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    if (alist->a3->length < ilen)
	ovm_raise(except_invalid_argument);
    if (alist->a6->length < olen)
	orenew_vector(alist->a6, olen);
    r0->t = t_void;
    gluScaleImage(alist->a0, alist->a1, alist->a2, GL_UNSIGNED_BYTE,
		  alist->a3->v.u8, alist->a4, alist->a5, GL_UNSIGNED_BYTE,
		  alist->a6->v.u8);
}

static void
native_Sphere(oobject_t list, oint32_t ac)
/* void Sphere(quadric_t quad, float64_t radius,
	       int32_t slices, int32_t stacks); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_qua_f64_i32_i32_t		*alist;

    alist = (nat_qua_f64_i32_i32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_quadric);
    CHECK_NULL(alist->a0->__quadric);
    r0->t = t_void;
    gluSphere(alist->a0->__quadric, alist->a1, alist->a2, alist->a3);
}


static void
native_TessBeginContour(oobject_t list, oint32_t ac)
/* void TessBeginContour(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessBeginContour(alist->a0->__tesselator);
}

static void
native_TessBeginPolygon(oobject_t list, oint32_t ac)
/* void TessBeginPolygon(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessBeginPolygon(alist->a0->__tesselator, null);
}

static void
native_TessEndContour(oobject_t list, oint32_t ac)
/* void TessEndContour(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessEndContour(alist->a0->__tesselator);
}

static void
native_TessEndPolygon(oobject_t list, oint32_t ac)
/* void TessEndPolygon(tesselator_t tess); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_t				*alist;

    alist = (nat_tes_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessEndPolygon(alist->a0->__tesselator);
}

static void
native_TessNormal(oobject_t list, oint32_t ac)
/* void TessNormal(tesselator_t tess, float64_t valueX,
		   float64_t valueY, float64_t valueZ); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_f64_f64_f64_t		*alist;

    alist = (nat_tes_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessNormal(alist->a0->__tesselator, alist->a1, alist->a2, alist->a3);
}

static void
native_TessProperty(oobject_t list, oint32_t ac)
/* void TessProperty(tesselator_t tess, uint32_t which, float64_t data); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_u32_f64_t			*alist;

    alist = (nat_tes_u32_f64_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    r0->t = t_void;
    gluTessProperty(alist->a0->__tesselator, alist->a1, alist->a2);
}

static void
native_TessVertex(oobject_t list, oint32_t ac)
/* void TessVertex(tesselator_t tess, float64_t location[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_tes_vec_t			*alist;

    alist = (nat_tes_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_tesselator);
    CHECK_NULL(alist->a0->__tesselator);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float64);
    CHECK_LENGTH(alist->a1, 3);
    r0->t = t_void;
    gluTessVertex(alist->a0->__tesselator, alist->a1->v.f64, null);
}

static void
native_UnProject(oobject_t list, oint32_t ac)
/* int32_t UnProject(float64_t winX, float64_t winY, float64_t winZ,
		    float64_t model[16], float64_t proj[16],
		    int32_t view[4], float64_t objXYZ[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_vec_vec_vec_vec_t	*alist;

    alist = (nat_f64_f64_f64_vec_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_float64);
    CHECK_LENGTH(alist->a3, 16);
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_vector|t_float64);
    CHECK_LENGTH(alist->a4, 16);
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_int32);
    CHECK_LENGTH(alist->a5, 4);
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_float64);
    if (alist->a6->length != 3)
	orenew_vector(alist->a6, 3);
    r0->t = t_word;
    r0->v.w = gluUnProject(alist->a0, alist->a1, alist->a2, alist->a3->v.f64,
			   alist->a4->v.f64, alist->a5->v.i32,
			   alist->a6->v.f64 + 0,
			   alist->a6->v.f64 + 1,
			   alist->a6->v.f64 + 2);
}

static void
native_UnProject4(oobject_t list, oint32_t ac)
/* int32_t UnProject4(float64_t winX, float64_t winY, float64_t winZ,
		      float64_t clipW, float64_t model[16], float64_t proj[16],
		      int32_t view[4], float64_t nearVal, float64_t farVal,
		      float64_t objXYZW[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_vec_vec_vec_f64_f64_vec_t	*alist;

    alist = (nat_f64_f64_f64_f64_vec_vec_vec_f64_f64_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_vector|t_float64);
    CHECK_LENGTH(alist->a4, 16);
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_float64);
    CHECK_LENGTH(alist->a5, 16);
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_int32);
    CHECK_LENGTH(alist->a6, 4);
    CHECK_NULL(alist->a9);
    CHECK_TYPE(alist->a9, t_vector|t_float64);
    if (alist->a9->length != 4)
	orenew_vector(alist->a9, 4);
    r0->t = t_word;
    r0->v.w = gluUnProject4(alist->a0, alist->a1, alist->a2, alist->a3,
			    alist->a4->v.f64, alist->a5->v.f64,
			    alist->a6->v.i32, alist->a7, alist->a8,
			    alist->a9->v.f64 + 0, alist->a9->v.f64 + 1,
			    alist->a9->v.f64 + 2, alist->a9->v.f64 + 3);
}
