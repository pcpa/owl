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
/* Miscellaneous */
static void native_ClearIndex(oobject_t list, oint32_t ac);
static void native_ClearColor(oobject_t list, oint32_t ac);
static void native_Clear(oobject_t list, oint32_t ac);
static void native_IndexMask(oobject_t list, oint32_t ac);
static void native_ColorMask(oobject_t list, oint32_t ac);
static void native_AlphaFunc(oobject_t list, oint32_t ac);
static void native_BlendFunc(oobject_t list, oint32_t ac);
static void native_LogicOp(oobject_t list, oint32_t ac);
static void native_CullFace(oobject_t list, oint32_t ac);
static void native_FrontFace(oobject_t list, oint32_t ac);
static void native_PointSize(oobject_t list, oint32_t ac);
static void native_LineWidth(oobject_t list, oint32_t ac);
static void native_LineStipple(oobject_t list, oint32_t ac);
static void native_PolygonMode(oobject_t list, oint32_t ac);
static void native_PolygonOffset(oobject_t list, oint32_t ac);
static void native_PolygonStipple(oobject_t list, oint32_t ac);
static void native_GetPolygonStipple(oobject_t list, oint32_t ac);
static void native_EdgeFlag(oobject_t list, oint32_t ac);
static void native_Scissor(oobject_t list, oint32_t ac);
static void native_ClipPlane(oobject_t list, oint32_t ac);
static void native_GetClipPlane(oobject_t list, oint32_t ac);
static void native_DrawBuffer(oobject_t list, oint32_t ac);
static void native_ReadBuffer(oobject_t list, oint32_t ac);
static void native_Enable(oobject_t list, oint32_t ac);
static void native_Disable(oobject_t list, oint32_t ac);
static void native_IsEnabled(oobject_t list, oint32_t ac);
static void native_EnableClientState(oobject_t list, oint32_t ac);
static void native_DisableClientState(oobject_t list, oint32_t ac);
static void native_GetBooleanv(oobject_t list, oint32_t ac);
static void native_GetFloatv(oobject_t list, oint32_t ac);
static void native_GetDoublev(oobject_t list, oint32_t ac);
static void native_GetIntegerv(oobject_t list, oint32_t ac);
static void native_PushAttrib(oobject_t list, oint32_t ac);
static void native_PopAttrib(oobject_t list, oint32_t ac);
static void native_PushClientAttrib(oobject_t list, oint32_t ac);
static void native_PopClientAttrib(oobject_t list, oint32_t ac);
static void native_RenderMode(oobject_t list, oint32_t ac);
static void native_GetError(oobject_t list, oint32_t ac);
static void native_GetString(oobject_t list, oint32_t ac);
static void native_Finish(oobject_t list, oint32_t ac);
static void native_Flush(oobject_t list, oint32_t ac);
static void native_Hint(oobject_t list, oint32_t ac);
/* Depth Buffer */
static void native_ClearDepth(oobject_t list, oint32_t ac);
static void native_DepthFunc(oobject_t list, oint32_t ac);
static void native_DepthMask(oobject_t list, oint32_t ac);
static void native_DepthRange(oobject_t list, oint32_t ac);
/* Accumulation Buffer */
static void native_ClearAccum(oobject_t list, oint32_t ac);
static void native_Accum(oobject_t list, oint32_t ac);
/* Transformation */
static void native_MatrixMode(oobject_t list, oint32_t ac);
static void native_Ortho(oobject_t list, oint32_t ac);
static void native_Frustum(oobject_t list, oint32_t ac);
static void native_Viewport(oobject_t list, oint32_t ac);
static void native_PushMatrix(oobject_t list, oint32_t ac);
static void native_PopMatrix(oobject_t list, oint32_t ac);
static void native_LoadIdentity(oobject_t list, oint32_t ac);
static void native_LoadMatrix(oobject_t list, oint32_t ac);
static void native_MultMatrix(oobject_t list, oint32_t ac);
static void native_Rotate(oobject_t list, oint32_t ac);
static void native_Scale(oobject_t list, oint32_t ac);
static void native_Translate(oobject_t list, oint32_t ac);
/* Display Lists */
static void native_IsList(oobject_t list, oint32_t ac);
static void native_DeleteLists(oobject_t list, oint32_t ac);
static void native_GenLists(oobject_t list, oint32_t ac);
static void native_NewList(oobject_t list, oint32_t ac);
static void native_EndList(oobject_t list, oint32_t ac);
static void native_CallList(oobject_t list, oint32_t ac);
static void native_CallLists(oobject_t list, oint32_t ac);
static void native_ListBase(oobject_t list, oint32_t ac);
/* Drawing Functions */
static void native_Begin(oobject_t list, oint32_t ac);
static void native_End(oobject_t list, oint32_t ac);
static void native_Vertex2(oobject_t list, oint32_t ac);
static void native_Vertex3(oobject_t list, oint32_t ac);
static void native_Vertex4(oobject_t list, oint32_t ac);
static void native_Vertex2v(oobject_t list, oint32_t ac);
static void native_Vertex3v(oobject_t list, oint32_t ac);
static void native_Vertex4v(oobject_t list, oint32_t ac);
static void native_Normal3(oobject_t list, oint32_t ac);
static void native_Normal3v(oobject_t list, oint32_t ac);
static void native_Index(oobject_t list, oint32_t ac);
static void native_Color3(oobject_t list, oint32_t ac);
static void native_Color4(oobject_t list, oint32_t ac);
static void native_Color3v(oobject_t list, oint32_t ac);
static void native_Color4v(oobject_t list, oint32_t ac);
static void native_TexCoord1(oobject_t list, oint32_t ac);
static void native_TexCoord2(oobject_t list, oint32_t ac);
static void native_TexCoord3(oobject_t list, oint32_t ac);
static void native_TexCoord4(oobject_t list, oint32_t ac);
static void native_TexCoord2v(oobject_t list, oint32_t ac);
static void native_TexCoord3v(oobject_t list, oint32_t ac);
static void native_TexCoord4v(oobject_t list, oint32_t ac);
static void native_RasterPos2(oobject_t list, oint32_t ac);
static void native_RasterPos3(oobject_t list, oint32_t ac);
static void native_RasterPos4(oobject_t list, oint32_t ac);
static void native_RasterPos2v(oobject_t list, oint32_t ac);
static void native_RasterPos3v(oobject_t list, oint32_t ac);
static void native_RasterPos4v(oobject_t list, oint32_t ac);
static void native_Rect(oobject_t list, oint32_t ac);
static void native_Rectv(oobject_t list, oint32_t ac);
/* Lighting */
static void native_ShadeModel(oobject_t list, oint32_t ac);
static void native_Light(oobject_t list, oint32_t ac);
static void native_Lightv(oobject_t list, oint32_t ac);
static void native_GetLightv(oobject_t list, oint32_t ac);
static void native_LightModel(oobject_t list, oint32_t ac);
static void native_LightModelv(oobject_t list, oint32_t ac);
static void native_Material(oobject_t list, oint32_t ac);
static void native_Materialv(oobject_t list, oint32_t ac);
static void native_GetMaterialv(oobject_t list, oint32_t ac);
static void native_ColorMaterial(oobject_t list, oint32_t ac);
/* Raster functions */
static void native_PixelZoom(oobject_t list, oint32_t ac);
static void native_PixelStore(oobject_t list, oint32_t ac);
static void native_PixelTransfer(oobject_t list, oint32_t ac);
static void native_PixelMapv(oobject_t list, oint32_t ac);
static void native_GetPixelMapv(oobject_t list, oint32_t ac);
static void native_Bitmap(oobject_t list, oint32_t ac);
static void native_ReadPixels(oobject_t list, oint32_t ac);
static void native_ReadBitmap(oobject_t list, oint32_t ac);
static void native_DrawPixels(oobject_t list, oint32_t ac);
static void native_DrawBitmap(oobject_t list, oint32_t ac);
static void native_CopyPixels(oobject_t list, oint32_t ac);
/* Stenciling */
static void native_StencilFunc(oobject_t list, oint32_t ac);
static void native_StencilMask(oobject_t list, oint32_t ac);
static void native_StencilOp(oobject_t list, oint32_t ac);
static void native_ClearStencil(oobject_t list, oint32_t ac);
/* Texture mapping */
static void native_TexGen(oobject_t list, oint32_t ac);
static void native_TexGenv(oobject_t list, oint32_t ac);
static void native_GetTexGenv(oobject_t list, oint32_t ac);
static void native_TexEnv(oobject_t list, oint32_t ac);
static void native_TexEnvv(oobject_t list, oint32_t ac);
static void native_GetTexEnvv(oobject_t list, oint32_t ac);
static void native_TexParameter(oobject_t list, oint32_t ac);
static void native_TexParameterv(oobject_t list, oint32_t ac);
static void native_GetTexParameterv(oobject_t list, oint32_t ac);
static void native_GetTexLevelParameter(oobject_t list, oint32_t ac);
static void native_TexImage1D(oobject_t list, oint32_t ac);
static void native_TexImage2D(oobject_t list, oint32_t ac);
static void native_GetTexImage(oobject_t list, oint32_t ac);
static void native_GenTextures(oobject_t list, oint32_t ac);
static void native_DeleteTextures(oobject_t list, oint32_t ac);
static void native_BindTexture(oobject_t list, oint32_t ac);
static void native_PrioritizeTextures(oobject_t list, oint32_t ac);
static void native_AreTexturesResident(oobject_t list, oint32_t ac);
static void native_IsTexture(oobject_t list, oint32_t ac);
static void native_TexSubImage1D(oobject_t list, oint32_t ac);
static void native_TexSubImage2D(oobject_t list, oint32_t ac);
static void native_CopyTexImage1D(oobject_t list, oint32_t ac);
static void native_CopyTexImage2D(oobject_t list, oint32_t ac);
static void native_CopyTexSubImage1D(oobject_t list, oint32_t ac);
static void native_CopyTexSubImage2D(oobject_t list, oint32_t ac);
/* Evaluators */
static void native_Map1(oobject_t list, oint32_t ac);
static void native_Map2(oobject_t list, oint32_t ac);
static void native_GetMapCoeff(oobject_t list, oint32_t ac);
static void native_GetMapOrder(oobject_t list, oint32_t ac);
static void native_GetMapDomain(oobject_t list, oint32_t ac);
static void native_EvalCoord1(oobject_t list, oint32_t ac);
static void native_EvalCoord2(oobject_t list, oint32_t ac);
static void native_MapGrid1(oobject_t list, oint32_t ac);
static void native_MapGrid2(oobject_t list, oint32_t ac);
static void native_EvalPoint1(oobject_t list, oint32_t ac);
static void native_EvalPoint2(oobject_t list, oint32_t ac);
static void native_EvalMesh1(oobject_t list, oint32_t ac);
static void native_EvalMesh2(oobject_t list, oint32_t ac);
/* Fog */
static void native_Fog(oobject_t list, oint32_t ac);
static void native_Fogv(oobject_t list, oint32_t ac);
/* Selection and Feedback */
static void native_FeedbackBuffer(oobject_t list, oint32_t ac);
static void native_PassThrough(oobject_t list, oint32_t ac);
static void native_SelectBuffer(oobject_t list, oint32_t ac);
static void native_InitNames(oobject_t list, oint32_t ac);
static void native_LoadName(oobject_t list, oint32_t ac);
static void native_PushName(oobject_t list, oint32_t ac);
static void native_PopName(oobject_t list, oint32_t ac);
/* GL_ARB_multitexture */
static void native_ActiveTextureARB(oobject_t list, oint32_t ac);
static void native_ClientActiveTextureARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord1ARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord1vARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord2ARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord2vARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord3ARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord3vARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord4ARB(oobject_t list, oint32_t ac);
static void native_MultiTexCoord4vARB(oobject_t list, oint32_t ac);

/* glext */
static void native_WindowPos2(oobject_t list, oint32_t ac);
static void native_WindowPos2v(oobject_t list, oint32_t ac);
static void native_WindowPos3(oobject_t list, oint32_t ac);
static void native_WindowPos3v(oobject_t list, oint32_t ac);

/*
 * Initialization
 */
orecord_t	*gl_record;

static struct {
    char	*name;
    int		 value;
} gl[] = {
    /* Primitives */
    { "POINTS",				GL_POINTS },
    { "LINES",				GL_LINES },
    { "LINE_LOOP",			GL_LINE_LOOP },
    { "LINE_STRIP",			GL_LINE_STRIP },
    { "TRIANGLES",			GL_TRIANGLES },
    { "TRIANGLE_STRIP",			GL_TRIANGLE_STRIP },
    { "TRIANGLE_FAN",			GL_TRIANGLE_FAN },
    { "QUADS",				GL_QUADS },
    { "QUAD_STRIP",			GL_QUAD_STRIP },
    { "POLYGON",			GL_POLYGON },
    /* Matrix Mode */
    { "MATRIX_MODE",			GL_MATRIX_MODE },
    { "MODELVIEW",			GL_MODELVIEW },
    { "PROJECTION",			GL_PROJECTION },
    { "TEXTURE",			GL_TEXTURE },
    /* Points */
    { "POINT_SMOOTH",			GL_POINT_SMOOTH },
    { "POINT_SIZE",			GL_POINT_SIZE },
    { "POINT_SIZE_GRANULARITY",		GL_POINT_SIZE_GRANULARITY },
    { "POINT_SIZE_RANGE",		GL_POINT_SIZE_RANGE },
    /* Lines */
    { "LINE_SMOOTH",			GL_LINE_SMOOTH },
    { "LINE_STIPPLE",			GL_LINE_STIPPLE },
    { "LINE_STIPPLE_PATTERN",		GL_LINE_STIPPLE_PATTERN },
    { "LINE_STIPPLE_REPEAT",		GL_LINE_STIPPLE_REPEAT },
    { "LINE_WIDTH",			GL_LINE_WIDTH },
    { "LINE_WIDTH_GRANULARITY",		GL_LINE_WIDTH_GRANULARITY },
    { "LINE_WIDTH_RANGE",		GL_LINE_WIDTH_RANGE },
    /* Polygons */
    { "POINT",				GL_POINT },
    { "LINE",				GL_LINE },
    { "FILL",				GL_FILL },
    { "CW",				GL_CW },
    { "CCW",				GL_CCW },
    { "FRONT",				GL_FRONT },
    { "BACK",				GL_BACK },
    { "POLYGON_MODE",			GL_POLYGON_MODE },
    { "POLYGON_SMOOTH",			GL_POLYGON_SMOOTH },
    { "POLYGON_STIPPLE",		GL_POLYGON_STIPPLE },
    { "EDGE_FLAG",			GL_EDGE_FLAG },
    { "CULL_FACE",			GL_CULL_FACE },
    { "CULL_FACE_MODE",			GL_CULL_FACE_MODE },
    { "FRONT_FACE",			GL_FRONT_FACE },
    { "POLYGON_OFFSET_FACTOR",		GL_POLYGON_OFFSET_FACTOR },
    { "POLYGON_OFFSET_UNITS",		GL_POLYGON_OFFSET_UNITS },
    { "POLYGON_OFFSET_POINT",		GL_POLYGON_OFFSET_POINT },
    { "POLYGON_OFFSET_LINE",		GL_POLYGON_OFFSET_LINE },
    { "POLYGON_OFFSET_FILL",		GL_POLYGON_OFFSET_FILL },
    /* Display Lists */
    { "COMPILE",			GL_COMPILE },
    { "COMPILE_AND_EXECUTE",		GL_COMPILE_AND_EXECUTE },
    { "LIST_BASE",			GL_LIST_BASE },
    { "LIST_INDEX",			GL_LIST_INDEX },
    { "LIST_MODE",			GL_LIST_MODE },
    /* Depth buffer */
    { "NEVER",				GL_NEVER },
    { "LESS",				GL_LESS },
    { "EQUAL",				GL_EQUAL },
    { "LEQUAL",				GL_LEQUAL },
    { "GREATER",			GL_GREATER },
    { "NOTEQUAL",			GL_NOTEQUAL },
    { "GEQUAL",				GL_GEQUAL },
    { "ALWAYS",				GL_ALWAYS },
    { "DEPTH_TEST",			GL_DEPTH_TEST },
    { "DEPTH_BITS",			GL_DEPTH_BITS },
    { "DEPTH_CLEAR_VALUE",		GL_DEPTH_CLEAR_VALUE },
    { "DEPTH_FUNC",			GL_DEPTH_FUNC },
    { "DEPTH_RANGE",			GL_DEPTH_RANGE },
    { "DEPTH_WRITEMASK",		GL_DEPTH_WRITEMASK },
    { "DEPTH_COMPONENT",		GL_DEPTH_COMPONENT },
    /* Lighting */
    { "LIGHTING",			GL_LIGHTING },
    { "LIGHT0",				GL_LIGHT0 },
    { "LIGHT1",				GL_LIGHT1 },
    { "LIGHT2",				GL_LIGHT2 },
    { "LIGHT3",				GL_LIGHT3 },
    { "LIGHT4",				GL_LIGHT4 },
    { "LIGHT5",				GL_LIGHT5 },
    { "LIGHT6",				GL_LIGHT6 },
    { "LIGHT7",				GL_LIGHT7 },
    { "SPOT_EXPONENT",			GL_SPOT_EXPONENT },
    { "SPOT_CUTOFF",			GL_SPOT_CUTOFF },
    { "CONSTANT_ATTENUATION",		GL_CONSTANT_ATTENUATION },
    { "LINEAR_ATTENUATION",		GL_LINEAR_ATTENUATION },
    { "QUADRATIC_ATTENUATION",		GL_QUADRATIC_ATTENUATION },
    { "AMBIENT",			GL_AMBIENT },
    { "DIFFUSE",			GL_DIFFUSE },
    { "SPECULAR",			GL_SPECULAR },
    { "SHININESS",			GL_SHININESS },
    { "EMISSION",			GL_EMISSION },
    { "POSITION",			GL_POSITION },
    { "SPOT_DIRECTION",			GL_SPOT_DIRECTION },
    { "AMBIENT_AND_DIFFUSE",		GL_AMBIENT_AND_DIFFUSE },
    { "COLOR_INDEXES",			GL_COLOR_INDEXES },
    { "LIGHT_MODEL_TWO_SIDE",		GL_LIGHT_MODEL_TWO_SIDE },
    { "LIGHT_MODEL_LOCAL_VIEWER",	GL_LIGHT_MODEL_LOCAL_VIEWER },
    { "LIGHT_MODEL_AMBIENT",		GL_LIGHT_MODEL_AMBIENT },
    { "FRONT_AND_BACK",			GL_FRONT_AND_BACK },
    { "SHADE_MODEL",			GL_SHADE_MODEL },
    { "FLAT",				GL_FLAT },
    { "SMOOTH",				GL_SMOOTH },
    { "COLOR_MATERIAL",			GL_COLOR_MATERIAL },
    { "COLOR_MATERIAL_FACE",		GL_COLOR_MATERIAL_FACE },
    { "COLOR_MATERIAL_PARAMETER",	GL_COLOR_MATERIAL_PARAMETER },
    { "NORMALIZE",			GL_NORMALIZE },
    /* User clipping planes */
    { "CLIP_PLANE0",			GL_CLIP_PLANE0 },
    { "CLIP_PLANE1",			GL_CLIP_PLANE1 },
    { "CLIP_PLANE2",			GL_CLIP_PLANE2 },
    { "CLIP_PLANE3",			GL_CLIP_PLANE3 },
    { "CLIP_PLANE4",			GL_CLIP_PLANE4 },
    { "CLIP_PLANE5",			GL_CLIP_PLANE5 },
    /* Accumulation buffer */
    { "ACCUM_RED_BITS",			GL_ACCUM_RED_BITS },
    { "ACCUM_GREEN_BITS",		GL_ACCUM_GREEN_BITS },
    { "ACCUM_BLUE_BITS",		GL_ACCUM_BLUE_BITS },
    { "ACCUM_ALPHA_BITS",		GL_ACCUM_ALPHA_BITS },
    { "ACCUM_CLEAR_VALUE",		GL_ACCUM_CLEAR_VALUE },
    { "ACCUM",				GL_ACCUM },
    { "ADD",				GL_ADD },
    { "LOAD",				GL_LOAD },
    { "MULT",				GL_MULT },
    { "RETURN",				GL_RETURN },
    /* Alpha testing */
    { "ALPHA_TEST",			GL_ALPHA_TEST },
    { "ALPHA_TEST_REF",			GL_ALPHA_TEST_REF },
    { "ALPHA_TEST_FUNC",		GL_ALPHA_TEST_FUNC },
    /* Blending */
    { "BLEND",				GL_BLEND },
    { "BLEND_SRC",			GL_BLEND_SRC },
    { "BLEND_DST",			GL_BLEND_DST },
    { "ZERO",				GL_ZERO },
    { "ONE",				GL_ONE },
    { "SRC_COLOR",			GL_SRC_COLOR },
    { "ONE_MINUS_SRC_COLOR",		GL_ONE_MINUS_SRC_COLOR },
    { "SRC_ALPHA",			GL_SRC_ALPHA },
    { "ONE_MINUS_SRC_ALPHA",		GL_ONE_MINUS_SRC_ALPHA },
    { "DST_ALPHA",			GL_DST_ALPHA },
    { "ONE_MINUS_DST_ALPHA",		GL_ONE_MINUS_DST_ALPHA },
    { "DST_COLOR",			GL_DST_COLOR },
    { "ONE_MINUS_DST_COLOR",		GL_ONE_MINUS_DST_COLOR },
    { "SRC_ALPHA_SATURATE",		GL_SRC_ALPHA_SATURATE },
    /* Render Mode */
    { "FEEDBACK",			GL_FEEDBACK },
    { "RENDER",				GL_RENDER },
    { "SELECT",				GL_SELECT },
    /* Feedback */
    { "2D",				GL_2D },
    { "3D",				GL_3D },
    { "3D_COLOR",			GL_3D_COLOR },
    { "3D_COLOR_TEXTURE",		GL_3D_COLOR_TEXTURE },
    { "4D_COLOR_TEXTURE",		GL_4D_COLOR_TEXTURE },
    { "POINT_TOKEN",			GL_POINT_TOKEN },
    { "LINE_TOKEN",			GL_LINE_TOKEN },
    { "LINE_RESET_TOKEN",		GL_LINE_RESET_TOKEN },
    { "POLYGON_TOKEN",			GL_POLYGON_TOKEN },
    { "BITMAP_TOKEN",			GL_BITMAP_TOKEN },
    { "DRAW_PIXEL_TOKEN",		GL_DRAW_PIXEL_TOKEN },
    { "COPY_PIXEL_TOKEN",		GL_COPY_PIXEL_TOKEN },
    { "PASS_THROUGH_TOKEN",		GL_PASS_THROUGH_TOKEN },
    { "FEEDBACK_BUFFER_POINTER",	GL_FEEDBACK_BUFFER_POINTER },
    { "FEEDBACK_BUFFER_SIZE",		GL_FEEDBACK_BUFFER_SIZE },
    { "FEEDBACK_BUFFER_TYPE",		GL_FEEDBACK_BUFFER_TYPE },
    /* Selection */
    { "SELECTION_BUFFER_POINTER",	GL_SELECTION_BUFFER_POINTER },
    { "SELECTION_BUFFER_SIZE",		GL_SELECTION_BUFFER_SIZE },
    /* Fog */
    { "FOG",				GL_FOG },
    { "FOG_MODE",			GL_FOG_MODE },
    { "FOG_DENSITY",			GL_FOG_DENSITY },
    { "FOG_COLOR",			GL_FOG_COLOR },
    { "FOG_INDEX",			GL_FOG_INDEX },
    { "FOG_START",			GL_FOG_START },
    { "FOG_END",			GL_FOG_END },
    { "LINEAR",				GL_LINEAR },
    { "EXP",				GL_EXP },
    { "EXP2",				GL_EXP2 },
    /* Logic Ops */
    { "LOGIC_OP",			GL_LOGIC_OP },
    { "INDEX_LOGIC_OP",			GL_INDEX_LOGIC_OP },
    { "COLOR_LOGIC_OP",			GL_COLOR_LOGIC_OP },
    { "LOGIC_OP_MODE",			GL_LOGIC_OP_MODE },
    { "CLEAR",				GL_CLEAR },
    { "SET",				GL_SET },
    { "COPY",				GL_COPY },
    { "COPY_INVERTED",			GL_COPY_INVERTED },
    { "NOOP",				GL_NOOP },
    { "INVERT",				GL_INVERT },
    { "AND",				GL_AND },
    { "NAND",				GL_NAND },
    { "OR",				GL_OR },
    { "NOR",				GL_NOR },
    { "XOR",				GL_XOR },
    { "EQUIV",				GL_EQUIV },
    { "AND_REVERSE",			GL_AND_REVERSE },
    { "AND_INVERTED",			GL_AND_INVERTED },
    { "OR_REVERSE",			GL_OR_REVERSE },
    { "OR_INVERTED",			GL_OR_INVERTED },
    /* Stencil */
    { "STENCIL_BITS",			GL_STENCIL_BITS },
    { "STENCIL_TEST",			GL_STENCIL_TEST },
    { "STENCIL_CLEAR_VALUE",		GL_STENCIL_CLEAR_VALUE },
    { "STENCIL_FUNC",			GL_STENCIL_FUNC },
    { "STENCIL_VALUE_MASK",		GL_STENCIL_VALUE_MASK },
    { "STENCIL_FAIL",			GL_STENCIL_FAIL },
    { "STENCIL_PASS_DEPTH_FAIL",	GL_STENCIL_PASS_DEPTH_FAIL },
    { "STENCIL_PASS_DEPTH_PASS",	GL_STENCIL_PASS_DEPTH_PASS },
    { "STENCIL_REF",			GL_STENCIL_REF },
    { "STENCIL_WRITEMASK",		GL_STENCIL_WRITEMASK },
    { "STENCIL_INDEX",			GL_STENCIL_INDEX },
    { "KEEP",				GL_KEEP },
    { "REPLACE",			GL_REPLACE },
    { "INCR",				GL_INCR },
    { "DECR",				GL_DECR },
    /* Buffers, Pixel Drawing/Reading */
    { "NONE",				GL_NONE },
    { "LEFT",				GL_LEFT },
    { "RIGHT",				GL_RIGHT },
    { "FRONT_LEFT",			GL_FRONT_LEFT },
    { "FRONT_RIGHT",			GL_FRONT_RIGHT },
    { "BACK_LEFT",			GL_BACK_LEFT },
    { "BACK_RIGHT",			GL_BACK_RIGHT },
    { "AUX0",				GL_AUX0 },
    { "AUX1",				GL_AUX1 },
    { "AUX2",				GL_AUX2 },
    { "AUX3",				GL_AUX3 },
    { "COLOR_INDEX",			GL_COLOR_INDEX },
    { "RED",				GL_RED },
    { "GREEN",				GL_GREEN },
    { "BLUE",				GL_BLUE },
    { "ALPHA",				GL_ALPHA },
    { "LUMINANCE",			GL_LUMINANCE },
    { "LUMINANCE_ALPHA",		GL_LUMINANCE_ALPHA },
    { "ALPHA_BITS",			GL_ALPHA_BITS },
    { "RED_BITS",			GL_RED_BITS },
    { "GREEN_BITS",			GL_GREEN_BITS },
    { "BLUE_BITS",			GL_BLUE_BITS },
    { "INDEX_BITS",			GL_INDEX_BITS },
    { "SUBPIXEL_BITS",			GL_SUBPIXEL_BITS },
    { "AUX_BUFFERS",			GL_AUX_BUFFERS },
    { "READ_BUFFER",			GL_READ_BUFFER },
    { "DRAW_BUFFER",			GL_DRAW_BUFFER },
    { "DOUBLEBUFFER",			GL_DOUBLEBUFFER },
    { "STEREO",				GL_STEREO },
    { "BITMAP",				GL_BITMAP },
    { "COLOR",				GL_COLOR },
    { "DEPTH",				GL_DEPTH },
    { "STENCIL",			GL_STENCIL },
    { "DITHER",				GL_DITHER },
    { "RGB",				GL_RGB },
    { "RGBA",				GL_RGBA },
    /* Implementation limits */
    { "MAX_LIST_NESTING",		GL_MAX_LIST_NESTING },
    { "MAX_EVAL_ORDER",			GL_MAX_EVAL_ORDER },
    { "MAX_LIGHTS",			GL_MAX_LIGHTS },
    { "MAX_CLIP_PLANES",		GL_MAX_CLIP_PLANES },
    { "MAX_TEXTURE_SIZE",		GL_MAX_TEXTURE_SIZE },
    { "MAX_PIXEL_MAP_TABLE",		GL_MAX_PIXEL_MAP_TABLE },
    { "MAX_ATTRIB_STACK_DEPTH",		GL_MAX_ATTRIB_STACK_DEPTH },
    { "MAX_MODELVIEW_STACK_DEPTH",	GL_MAX_MODELVIEW_STACK_DEPTH },
    { "MAX_NAME_STACK_DEPTH",		GL_MAX_NAME_STACK_DEPTH },
    { "MAX_PROJECTION_STACK_DEPTH",	GL_MAX_PROJECTION_STACK_DEPTH },
    { "MAX_TEXTURE_STACK_DEPTH",	GL_MAX_TEXTURE_STACK_DEPTH },
    { "MAX_VIEWPORT_DIMS",		GL_MAX_VIEWPORT_DIMS },
    { "MAX_CLIENT_ATTRIB_STACK_DEPTH",	GL_MAX_CLIENT_ATTRIB_STACK_DEPTH },
    /* Gets */
    { "ATTRIB_STACK_DEPTH",		GL_ATTRIB_STACK_DEPTH },
    { "CLIENT_ATTRIB_STACK_DEPTH",	GL_CLIENT_ATTRIB_STACK_DEPTH },
    { "COLOR_CLEAR_VALUE",		GL_COLOR_CLEAR_VALUE },
    { "COLOR_WRITEMASK",		GL_COLOR_WRITEMASK },
    { "CURRENT_INDEX",			GL_CURRENT_INDEX },
    { "CURRENT_COLOR",			GL_CURRENT_COLOR },
    { "CURRENT_NORMAL",			GL_CURRENT_NORMAL },
    { "CURRENT_RASTER_COLOR",		GL_CURRENT_RASTER_COLOR },
    { "CURRENT_RASTER_DISTANCE",	GL_CURRENT_RASTER_DISTANCE },
    { "CURRENT_RASTER_INDEX",		GL_CURRENT_RASTER_INDEX },
    { "CURRENT_RASTER_POSITION",	GL_CURRENT_RASTER_POSITION },
    { "CURRENT_RASTER_TEXTURE_COORDS",	GL_CURRENT_RASTER_TEXTURE_COORDS },
    { "CURRENT_RASTER_POSITION_VALID",	GL_CURRENT_RASTER_POSITION_VALID },
    { "CURRENT_TEXTURE_COORDS",		GL_CURRENT_TEXTURE_COORDS },
    { "INDEX_CLEAR_VALUE",		GL_INDEX_CLEAR_VALUE },
    { "INDEX_MODE",			GL_INDEX_MODE },
    { "INDEX_WRITEMASK",		GL_INDEX_WRITEMASK },
    { "MODELVIEW_MATRIX",		GL_MODELVIEW_MATRIX },
    { "MODELVIEW_STACK_DEPTH",		GL_MODELVIEW_STACK_DEPTH },
    { "NAME_STACK_DEPTH",		GL_NAME_STACK_DEPTH },
    { "PROJECTION_MATRIX",		GL_PROJECTION_MATRIX },
    { "PROJECTION_STACK_DEPTH",		GL_PROJECTION_STACK_DEPTH },
    { "RENDER_MODE",			GL_RENDER_MODE },
    { "RGBA_MODE",			GL_RGBA_MODE },
    { "TEXTURE_MATRIX",			GL_TEXTURE_MATRIX },
    { "TEXTURE_STACK_DEPTH",		GL_TEXTURE_STACK_DEPTH },
    { "VIEWPORT",			GL_VIEWPORT },
    /* Evaluators */
    { "AUTO_NORMAL",			GL_AUTO_NORMAL },
    { "MAP1_COLOR_4",			GL_MAP1_COLOR_4 },
    { "MAP1_INDEX",			GL_MAP1_INDEX },
    { "MAP1_NORMAL",			GL_MAP1_NORMAL },
    { "MAP1_TEXTURE_COORD_1",		GL_MAP1_TEXTURE_COORD_1 },
    { "MAP1_TEXTURE_COORD_2",		GL_MAP1_TEXTURE_COORD_2 },
    { "MAP1_TEXTURE_COORD_3",		GL_MAP1_TEXTURE_COORD_3 },
    { "MAP1_TEXTURE_COORD_4",		GL_MAP1_TEXTURE_COORD_4 },
    { "MAP1_VERTEX_3",			GL_MAP1_VERTEX_3 },
    { "MAP1_VERTEX_4",			GL_MAP1_VERTEX_4 },
    { "MAP2_COLOR_4",			GL_MAP2_COLOR_4 },
    { "MAP2_INDEX",			GL_MAP2_INDEX },
    { "MAP2_NORMAL",			GL_MAP2_NORMAL },
    { "MAP2_TEXTURE_COORD_1",		GL_MAP2_TEXTURE_COORD_1 },
    { "MAP2_TEXTURE_COORD_2",		GL_MAP2_TEXTURE_COORD_2 },
    { "MAP2_TEXTURE_COORD_3",		GL_MAP2_TEXTURE_COORD_3 },
    { "MAP2_TEXTURE_COORD_4",		GL_MAP2_TEXTURE_COORD_4 },
    { "MAP2_VERTEX_3",			GL_MAP2_VERTEX_3 },
    { "MAP2_VERTEX_4",			GL_MAP2_VERTEX_4 },
    { "MAP1_GRID_DOMAIN",		GL_MAP1_GRID_DOMAIN },
    { "MAP1_GRID_SEGMENTS",		GL_MAP1_GRID_SEGMENTS },
    { "MAP2_GRID_DOMAIN",		GL_MAP2_GRID_DOMAIN },
    { "MAP2_GRID_SEGMENTS",		GL_MAP2_GRID_SEGMENTS },
    { "COEFF",				GL_COEFF },
    { "ORDER",				GL_ORDER },
    { "DOMAIN",				GL_DOMAIN },
    /* Hints */
    { "PERSPECTIVE_CORRECTION_HINT",	GL_PERSPECTIVE_CORRECTION_HINT },
    { "POINT_SMOOTH_HINT",		GL_POINT_SMOOTH_HINT },
    { "LINE_SMOOTH_HINT",		GL_LINE_SMOOTH_HINT },
    { "POLYGON_SMOOTH_HINT",		GL_POLYGON_SMOOTH_HINT },
    { "FOG_HINT",			GL_FOG_HINT },
    { "DONT_CARE",			GL_DONT_CARE },
    { "FASTEST",			GL_FASTEST },
    { "NICEST",				GL_NICEST },
    /* Scissor box */
    { "SCISSOR_BOX",			GL_SCISSOR_BOX },
    { "SCISSOR_TEST",			GL_SCISSOR_TEST },
    /* Pixel Mode / Transfer */
    { "MAP_COLOR",			GL_MAP_COLOR },
    { "MAP_STENCIL",			GL_MAP_STENCIL },
    { "INDEX_SHIFT",			GL_INDEX_SHIFT },
    { "INDEX_OFFSET",			GL_INDEX_OFFSET },
    { "RED_SCALE",			GL_RED_SCALE },
    { "RED_BIAS",			GL_RED_BIAS },
    { "GREEN_SCALE",			GL_GREEN_SCALE },
    { "GREEN_BIAS",			GL_GREEN_BIAS },
    { "BLUE_SCALE",			GL_BLUE_SCALE },
    { "BLUE_BIAS",			GL_BLUE_BIAS },
    { "ALPHA_SCALE",			GL_ALPHA_SCALE },
    { "ALPHA_BIAS",			GL_ALPHA_BIAS },
    { "DEPTH_SCALE",			GL_DEPTH_SCALE },
    { "DEPTH_BIAS",			GL_DEPTH_BIAS },
    { "PIXEL_MAP_S_TO_S_SIZE",		GL_PIXEL_MAP_S_TO_S_SIZE },
    { "PIXEL_MAP_I_TO_I_SIZE",		GL_PIXEL_MAP_I_TO_I_SIZE },
    { "PIXEL_MAP_I_TO_R_SIZE",		GL_PIXEL_MAP_I_TO_R_SIZE },
    { "PIXEL_MAP_I_TO_G_SIZE",		GL_PIXEL_MAP_I_TO_G_SIZE },
    { "PIXEL_MAP_I_TO_B_SIZE",		GL_PIXEL_MAP_I_TO_B_SIZE },
    { "PIXEL_MAP_I_TO_A_SIZE",		GL_PIXEL_MAP_I_TO_A_SIZE },
    { "PIXEL_MAP_R_TO_R_SIZE",		GL_PIXEL_MAP_R_TO_R_SIZE },
    { "PIXEL_MAP_G_TO_G_SIZE",		GL_PIXEL_MAP_G_TO_G_SIZE },
    { "PIXEL_MAP_B_TO_B_SIZE",		GL_PIXEL_MAP_B_TO_B_SIZE },
    { "PIXEL_MAP_A_TO_A_SIZE",		GL_PIXEL_MAP_A_TO_A_SIZE },
    { "PIXEL_MAP_S_TO_S",		GL_PIXEL_MAP_S_TO_S },
    { "PIXEL_MAP_I_TO_I",		GL_PIXEL_MAP_I_TO_I },
    { "PIXEL_MAP_I_TO_R",		GL_PIXEL_MAP_I_TO_R },
    { "PIXEL_MAP_I_TO_G",		GL_PIXEL_MAP_I_TO_G },
    { "PIXEL_MAP_I_TO_B",		GL_PIXEL_MAP_I_TO_B },
    { "PIXEL_MAP_I_TO_A",		GL_PIXEL_MAP_I_TO_A },
    { "PIXEL_MAP_R_TO_R",		GL_PIXEL_MAP_R_TO_R },
    { "PIXEL_MAP_G_TO_G",		GL_PIXEL_MAP_G_TO_G },
    { "PIXEL_MAP_B_TO_B",		GL_PIXEL_MAP_B_TO_B },
    { "PIXEL_MAP_A_TO_A",		GL_PIXEL_MAP_A_TO_A },
    { "PACK_ALIGNMENT",			GL_PACK_ALIGNMENT },
    { "PACK_LSB_FIRST",			GL_PACK_LSB_FIRST },
    { "PACK_ROW_LENGTH",		GL_PACK_ROW_LENGTH },
    { "PACK_SKIP_PIXELS",		GL_PACK_SKIP_PIXELS },
    { "PACK_SKIP_ROWS",			GL_PACK_SKIP_ROWS },
    { "PACK_SWAP_BYTES",		GL_PACK_SWAP_BYTES },
    { "UNPACK_ALIGNMENT",		GL_UNPACK_ALIGNMENT },
    { "UNPACK_LSB_FIRST",		GL_UNPACK_LSB_FIRST },
    { "UNPACK_ROW_LENGTH",		GL_UNPACK_ROW_LENGTH },
    { "UNPACK_SKIP_PIXELS",		GL_UNPACK_SKIP_PIXELS },
    { "UNPACK_SKIP_ROWS",		GL_UNPACK_SKIP_ROWS },
    { "UNPACK_SWAP_BYTES",		GL_UNPACK_SWAP_BYTES },
    { "ZOOM_X",				GL_ZOOM_X },
    { "ZOOM_Y",				GL_ZOOM_Y },
    /* Texture mapping */
    { "TEXTURE_ENV",			GL_TEXTURE_ENV },
    { "TEXTURE_ENV_MODE",		GL_TEXTURE_ENV_MODE },
    { "TEXTURE_1D",			GL_TEXTURE_1D },
    { "TEXTURE_2D",			GL_TEXTURE_2D },
    { "TEXTURE_WRAP_S",			GL_TEXTURE_WRAP_S },
    { "TEXTURE_WRAP_T",			GL_TEXTURE_WRAP_T },
    { "TEXTURE_MAG_FILTER",		GL_TEXTURE_MAG_FILTER },
    { "TEXTURE_MIN_FILTER",		GL_TEXTURE_MIN_FILTER },
    { "TEXTURE_ENV_COLOR",		GL_TEXTURE_ENV_COLOR },
    { "TEXTURE_GEN_S",			GL_TEXTURE_GEN_S },
    { "TEXTURE_GEN_T",			GL_TEXTURE_GEN_T },
    { "TEXTURE_GEN_R",			GL_TEXTURE_GEN_R },
    { "TEXTURE_GEN_Q",			GL_TEXTURE_GEN_Q },
    { "TEXTURE_GEN_MODE",		GL_TEXTURE_GEN_MODE },
    { "TEXTURE_BORDER_COLOR",		GL_TEXTURE_BORDER_COLOR },
    { "TEXTURE_WIDTH",			GL_TEXTURE_WIDTH },
    { "TEXTURE_HEIGHT",			GL_TEXTURE_HEIGHT },
    { "TEXTURE_BORDER",			GL_TEXTURE_BORDER },
    { "TEXTURE_COMPONENTS",		GL_TEXTURE_COMPONENTS },
    { "TEXTURE_RED_SIZE",		GL_TEXTURE_RED_SIZE },
    { "TEXTURE_GREEN_SIZE",		GL_TEXTURE_GREEN_SIZE },
    { "TEXTURE_BLUE_SIZE",		GL_TEXTURE_BLUE_SIZE },
    { "TEXTURE_ALPHA_SIZE",		GL_TEXTURE_ALPHA_SIZE },
    { "TEXTURE_LUMINANCE_SIZE",		GL_TEXTURE_LUMINANCE_SIZE },
    { "TEXTURE_INTENSITY_SIZE",		GL_TEXTURE_INTENSITY_SIZE },
    { "NEAREST_MIPMAP_NEAREST",		GL_NEAREST_MIPMAP_NEAREST },
    { "NEAREST_MIPMAP_LINEAR",		GL_NEAREST_MIPMAP_LINEAR },
    { "LINEAR_MIPMAP_NEAREST",		GL_LINEAR_MIPMAP_NEAREST },
    { "LINEAR_MIPMAP_LINEAR",		GL_LINEAR_MIPMAP_LINEAR },
    { "OBJECT_LINEAR",			GL_OBJECT_LINEAR },
    { "OBJECT_PLANE",			GL_OBJECT_PLANE },
    { "EYE_LINEAR",			GL_EYE_LINEAR },
    { "EYE_PLANE",			GL_EYE_PLANE },
    { "SPHERE_MAP",			GL_SPHERE_MAP },
    { "DECAL",				GL_DECAL },
    { "MODULATE",			GL_MODULATE },
    { "NEAREST",			GL_NEAREST },
    { "REPEAT",				GL_REPEAT },
    { "CLAMP",				GL_CLAMP },
    { "S",				GL_S },
    { "T",				GL_T },
    { "R",				GL_R },
    { "Q",				GL_Q },
    /* Utility */
    { "VENDOR",				GL_VENDOR },
    { "RENDERER",			GL_RENDERER },
    { "VERSION",			GL_VERSION },
    { "EXTENSIONS",			GL_EXTENSIONS },
    /* Errors */
    { "NO_ERROR",			GL_NO_ERROR },
    { "INVALID_ENUM",			GL_INVALID_ENUM },
    { "INVALID_VALUE",			GL_INVALID_VALUE },
    { "INVALID_OPERATION",		GL_INVALID_OPERATION },
    { "STACK_OVERFLOW",			GL_STACK_OVERFLOW },
    { "STACK_UNDERFLOW",		GL_STACK_UNDERFLOW },
    { "OUT_OF_MEMORY",			GL_OUT_OF_MEMORY },
    /* glPush/PopAttrib bits */
    { "CURRENT_BIT",			GL_CURRENT_BIT },
    { "POINT_BIT",			GL_POINT_BIT },
    { "LINE_BIT",			GL_LINE_BIT },
    { "POLYGON_BIT",			GL_POLYGON_BIT },
    { "POLYGON_STIPPLE_BIT",		GL_POLYGON_STIPPLE_BIT },
    { "PIXEL_MODE_BIT",			GL_PIXEL_MODE_BIT },
    { "LIGHTING_BIT",			GL_LIGHTING_BIT },
    { "FOG_BIT",			GL_FOG_BIT },
    { "DEPTH_BUFFER_BIT",		GL_DEPTH_BUFFER_BIT },
    { "ACCUM_BUFFER_BIT",		GL_ACCUM_BUFFER_BIT },
    { "STENCIL_BUFFER_BIT",		GL_STENCIL_BUFFER_BIT },
    { "VIEWPORT_BIT",			GL_VIEWPORT_BIT },
    { "TRANSFORM_BIT",			GL_TRANSFORM_BIT },
    { "ENABLE_BIT",			GL_ENABLE_BIT },
    { "COLOR_BUFFER_BIT",		GL_COLOR_BUFFER_BIT },
    { "HINT_BIT",			GL_HINT_BIT },
    { "EVAL_BIT",			GL_EVAL_BIT },
    { "LIST_BIT",			GL_LIST_BIT },
    { "TEXTURE_BIT",			GL_TEXTURE_BIT },
    { "SCISSOR_BIT",			GL_SCISSOR_BIT },
    { "ALL_ATTRIB_BITS",		GL_ALL_ATTRIB_BITS },
    /* OpenGL 1.1 */
    { "PROXY_TEXTURE_1D",		GL_PROXY_TEXTURE_1D },
    { "PROXY_TEXTURE_2D",		GL_PROXY_TEXTURE_2D },
    { "TEXTURE_PRIORITY",		GL_TEXTURE_PRIORITY },
    { "TEXTURE_RESIDENT",		GL_TEXTURE_RESIDENT },
    { "TEXTURE_BINDING_1D",		GL_TEXTURE_BINDING_1D },
    { "TEXTURE_BINDING_2D",		GL_TEXTURE_BINDING_2D },
    { "TEXTURE_INTERNAL_FORMAT",	GL_TEXTURE_INTERNAL_FORMAT },
    { "ALPHA4",				GL_ALPHA4 },
    { "ALPHA8",				GL_ALPHA8 },
    { "ALPHA12",			GL_ALPHA12 },
    { "ALPHA16",			GL_ALPHA16 },
    { "LUMINANCE4",			GL_LUMINANCE4 },
    { "LUMINANCE8",			GL_LUMINANCE8 },
    { "LUMINANCE12",			GL_LUMINANCE12 },
    { "LUMINANCE16",			GL_LUMINANCE16 },
    { "LUMINANCE4_ALPHA4",		GL_LUMINANCE4_ALPHA4 },
    { "LUMINANCE6_ALPHA2",		GL_LUMINANCE6_ALPHA2 },
    { "LUMINANCE8_ALPHA8",		GL_LUMINANCE8_ALPHA8 },
    { "LUMINANCE12_ALPHA4",		GL_LUMINANCE12_ALPHA4 },
    { "LUMINANCE12_ALPHA12",		GL_LUMINANCE12_ALPHA12 },
    { "LUMINANCE16_ALPHA16",		GL_LUMINANCE16_ALPHA16 },
    { "INTENSITY",			GL_INTENSITY },
    { "INTENSITY4",			GL_INTENSITY4 },
    { "INTENSITY8",			GL_INTENSITY8 },
    { "INTENSITY12",			GL_INTENSITY12 },
    { "INTENSITY16",			GL_INTENSITY16 },
    { "R3_G3_B2",			GL_R3_G3_B2 },
    { "RGB4",				GL_RGB4 },
    { "RGB5",				GL_RGB5 },
    { "RGB8",				GL_RGB8 },
    { "RGB10",				GL_RGB10 },
    { "RGB12",				GL_RGB12 },
    { "RGB16",				GL_RGB16 },
    { "RGBA2",				GL_RGBA2 },
    { "RGBA4",				GL_RGBA4 },
    { "RGB5_A1",			GL_RGB5_A1 },
    { "RGBA8",				GL_RGBA8 },
    { "RGB10_A2",			GL_RGB10_A2 },
    { "RGBA12",				GL_RGBA12 },
    { "RGBA16",				GL_RGBA16 },
    { "CLIENT_PIXEL_STORE_BIT",		GL_CLIENT_PIXEL_STORE_BIT },
    { "CLIENT_VERTEX_ARRAY_BIT",	GL_CLIENT_VERTEX_ARRAY_BIT },
    { "ALL_CLIENT_ATTRIB_BITS",		GL_ALL_CLIENT_ATTRIB_BITS },
    { "CLIENT_ALL_ATTRIB_BITS",		GL_CLIENT_ALL_ATTRIB_BITS },
    /* OpenGL 1.2 */
    { "RESCALE_NORMAL",			GL_RESCALE_NORMAL },
    { "CLAMP_TO_EDGE",			GL_CLAMP_TO_EDGE },
    { "MAX_ELEMENTS_VERTICES",		GL_MAX_ELEMENTS_VERTICES },
    { "MAX_ELEMENTS_INDICES",		GL_MAX_ELEMENTS_INDICES },
    { "BGR",				GL_BGR },
    { "BGRA",				GL_BGRA },
    { "UNSIGNED_BYTE_3_3_2",		GL_UNSIGNED_BYTE_3_3_2 },
    { "UNSIGNED_BYTE_2_3_3_REV",	GL_UNSIGNED_BYTE_2_3_3_REV },
    { "UNSIGNED_SHORT_5_6_5",		GL_UNSIGNED_SHORT_5_6_5 },
    { "UNSIGNED_SHORT_5_6_5_REV",	GL_UNSIGNED_SHORT_5_6_5_REV },
    { "UNSIGNED_SHORT_4_4_4_4",		GL_UNSIGNED_SHORT_4_4_4_4 },
    { "UNSIGNED_SHORT_4_4_4_4_REV",	GL_UNSIGNED_SHORT_4_4_4_4_REV },
    { "UNSIGNED_SHORT_5_5_5_1",		GL_UNSIGNED_SHORT_5_5_5_1 },
    { "UNSIGNED_SHORT_1_5_5_5_REV",	GL_UNSIGNED_SHORT_1_5_5_5_REV },
    { "UNSIGNED_INT_8_8_8_8",		GL_UNSIGNED_INT_8_8_8_8 },
    { "UNSIGNED_INT_8_8_8_8_REV",	GL_UNSIGNED_INT_8_8_8_8_REV },
    { "UNSIGNED_INT_10_10_10_2",	GL_UNSIGNED_INT_10_10_10_2 },
    { "UNSIGNED_INT_2_10_10_10_REV",	GL_UNSIGNED_INT_2_10_10_10_REV },
    { "LIGHT_MODEL_COLOR_CONTROL",	GL_LIGHT_MODEL_COLOR_CONTROL },
    { "SINGLE_COLOR",			GL_SINGLE_COLOR },
    { "SEPARATE_SPECULAR_COLOR",	GL_SEPARATE_SPECULAR_COLOR },
    { "TEXTURE_MIN_LOD",		GL_TEXTURE_MIN_LOD },
    { "TEXTURE_MAX_LOD",		GL_TEXTURE_MAX_LOD },
    { "TEXTURE_BASE_LEVEL",		GL_TEXTURE_BASE_LEVEL },
    { "TEXTURE_MAX_LEVEL",		GL_TEXTURE_MAX_LEVEL },
    { "SMOOTH_POINT_SIZE_RANGE",	GL_SMOOTH_POINT_SIZE_RANGE },
    { "SMOOTH_POINT_SIZE_GRANULARITY",	GL_SMOOTH_POINT_SIZE_GRANULARITY },
    { "SMOOTH_LINE_WIDTH_RANGE",	GL_SMOOTH_LINE_WIDTH_RANGE },
    { "SMOOTH_LINE_WIDTH_GRANULARITY",	GL_SMOOTH_LINE_WIDTH_GRANULARITY },
    { "ALIASED_POINT_SIZE_RANGE",	GL_ALIASED_POINT_SIZE_RANGE },
    { "ALIASED_LINE_WIDTH_RANGE",	GL_ALIASED_LINE_WIDTH_RANGE },
    { "PACK_SKIP_IMAGES",		GL_PACK_SKIP_IMAGES },
    { "PACK_IMAGE_HEIGHT",		GL_PACK_IMAGE_HEIGHT },
    { "UNPACK_SKIP_IMAGES",		GL_UNPACK_SKIP_IMAGES },
    { "UNPACK_IMAGE_HEIGHT",		GL_UNPACK_IMAGE_HEIGHT },
    { "TEXTURE_3D",			GL_TEXTURE_3D },
    { "PROXY_TEXTURE_3D",		GL_PROXY_TEXTURE_3D },
    { "TEXTURE_DEPTH",			GL_TEXTURE_DEPTH },
    { "TEXTURE_WRAP_R",			GL_TEXTURE_WRAP_R },
    { "MAX_3D_TEXTURE_SIZE",		GL_MAX_3D_TEXTURE_SIZE },
    { "TEXTURE_BINDING_3D",		GL_TEXTURE_BINDING_3D },
    /* GL_ARB_imaging */
    { "CONSTANT_COLOR",			GL_CONSTANT_COLOR },
    { "ONE_MINUS_CONSTANT_COLOR",	GL_ONE_MINUS_CONSTANT_COLOR },
    { "CONSTANT_ALPHA",			GL_CONSTANT_ALPHA },
    { "ONE_MINUS_CONSTANT_ALPHA",	GL_ONE_MINUS_CONSTANT_ALPHA },
    { "COLOR_TABLE",			GL_COLOR_TABLE },
    { "POST_CONVOLUTION_COLOR_TABLE",	GL_POST_CONVOLUTION_COLOR_TABLE },
    { "POST_COLOR_MATRIX_COLOR_TABLE",	GL_POST_COLOR_MATRIX_COLOR_TABLE },
    { "PROXY_COLOR_TABLE",		GL_PROXY_COLOR_TABLE },
    { "PROXY_POST_CONVOLUTION_COLOR_TABLE",GL_PROXY_POST_CONVOLUTION_COLOR_TABLE },
    { "PROXY_POST_COLOR_MATRIX_COLOR_TABLE",GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE },
    { "COLOR_TABLE_SCALE",		GL_COLOR_TABLE_SCALE },
    { "COLOR_TABLE_BIAS",		GL_COLOR_TABLE_BIAS },
    { "COLOR_TABLE_FORMAT",		GL_COLOR_TABLE_FORMAT },
    { "COLOR_TABLE_WIDTH",		GL_COLOR_TABLE_WIDTH },
    { "COLOR_TABLE_RED_SIZE",		GL_COLOR_TABLE_RED_SIZE },
    { "COLOR_TABLE_GREEN_SIZE",		GL_COLOR_TABLE_GREEN_SIZE },
    { "COLOR_TABLE_BLUE_SIZE",		GL_COLOR_TABLE_BLUE_SIZE },
    { "COLOR_TABLE_ALPHA_SIZE",		GL_COLOR_TABLE_ALPHA_SIZE },
    { "COLOR_TABLE_LUMINANCE_SIZE",	GL_COLOR_TABLE_LUMINANCE_SIZE },
    { "COLOR_TABLE_INTENSITY_SIZE",	GL_COLOR_TABLE_INTENSITY_SIZE },
    { "CONVOLUTION_1D",			GL_CONVOLUTION_1D },
    { "CONVOLUTION_2D",			GL_CONVOLUTION_2D },
    { "SEPARABLE_2D",			GL_SEPARABLE_2D },
    { "CONVOLUTION_BORDER_MODE",	GL_CONVOLUTION_BORDER_MODE },
    { "CONVOLUTION_FILTER_SCALE",	GL_CONVOLUTION_FILTER_SCALE },
    { "CONVOLUTION_FILTER_BIAS",	GL_CONVOLUTION_FILTER_BIAS },
    { "REDUCE",				GL_REDUCE },
    { "CONVOLUTION_FORMAT",		GL_CONVOLUTION_FORMAT },
    { "CONVOLUTION_WIDTH",		GL_CONVOLUTION_WIDTH },
    { "CONVOLUTION_HEIGHT",		GL_CONVOLUTION_HEIGHT },
    { "MAX_CONVOLUTION_WIDTH",		GL_MAX_CONVOLUTION_WIDTH },
    { "MAX_CONVOLUTION_HEIGHT",		GL_MAX_CONVOLUTION_HEIGHT },
    { "POST_CONVOLUTION_RED_SCALE",	GL_POST_CONVOLUTION_RED_SCALE },
    { "POST_CONVOLUTION_GREEN_SCALE",	GL_POST_CONVOLUTION_GREEN_SCALE },
    { "POST_CONVOLUTION_BLUE_SCALE",	GL_POST_CONVOLUTION_BLUE_SCALE },
    { "POST_CONVOLUTION_ALPHA_SCALE",	GL_POST_CONVOLUTION_ALPHA_SCALE },
    { "POST_CONVOLUTION_RED_BIAS",	GL_POST_CONVOLUTION_RED_BIAS },
    { "POST_CONVOLUTION_GREEN_BIAS",	GL_POST_CONVOLUTION_GREEN_BIAS },
    { "POST_CONVOLUTION_BLUE_BIAS",	GL_POST_CONVOLUTION_BLUE_BIAS },
    { "POST_CONVOLUTION_ALPHA_BIAS",	GL_POST_CONVOLUTION_ALPHA_BIAS },
    { "CONSTANT_BORDER",		GL_CONSTANT_BORDER },
    { "REPLICATE_BORDER",		GL_REPLICATE_BORDER },
    { "CONVOLUTION_BORDER_COLOR",	GL_CONVOLUTION_BORDER_COLOR },
    { "COLOR_MATRIX",			GL_COLOR_MATRIX },
    { "COLOR_MATRIX_STACK_DEPTH",	GL_COLOR_MATRIX_STACK_DEPTH },
    { "MAX_COLOR_MATRIX_STACK_DEPTH",	GL_MAX_COLOR_MATRIX_STACK_DEPTH },
    { "POST_COLOR_MATRIX_RED_SCALE",	GL_POST_COLOR_MATRIX_RED_SCALE },
    { "POST_COLOR_MATRIX_GREEN_SCALE",	GL_POST_COLOR_MATRIX_GREEN_SCALE },
    { "POST_COLOR_MATRIX_BLUE_SCALE",	GL_POST_COLOR_MATRIX_BLUE_SCALE },
    { "POST_COLOR_MATRIX_ALPHA_SCALE",	GL_POST_COLOR_MATRIX_ALPHA_SCALE },
    { "POST_COLOR_MATRIX_RED_BIAS",	GL_POST_COLOR_MATRIX_RED_BIAS },
    { "POST_COLOR_MATRIX_GREEN_BIAS",	GL_POST_COLOR_MATRIX_GREEN_BIAS },
    { "POST_COLOR_MATRIX_BLUE_BIAS",	GL_POST_COLOR_MATRIX_BLUE_BIAS },
    { "POST_COLOR_MATRIX_ALPHA_BIAS",	GL_POST_COLOR_MATRIX_ALPHA_BIAS },
    { "HISTOGRAM",			GL_HISTOGRAM },
    { "PROXY_HISTOGRAM",		GL_PROXY_HISTOGRAM },
    { "HISTOGRAM_WIDTH",		GL_HISTOGRAM_WIDTH },
    { "HISTOGRAM_FORMAT",		GL_HISTOGRAM_FORMAT },
    { "HISTOGRAM_RED_SIZE",		GL_HISTOGRAM_RED_SIZE },
    { "HISTOGRAM_GREEN_SIZE",		GL_HISTOGRAM_GREEN_SIZE },
    { "HISTOGRAM_BLUE_SIZE",		GL_HISTOGRAM_BLUE_SIZE },
    { "HISTOGRAM_ALPHA_SIZE",		GL_HISTOGRAM_ALPHA_SIZE },
    { "HISTOGRAM_LUMINANCE_SIZE",	GL_HISTOGRAM_LUMINANCE_SIZE },
    { "HISTOGRAM_SINK",			GL_HISTOGRAM_SINK },
    { "MINMAX",				GL_MINMAX },
    { "MINMAX_FORMAT",			GL_MINMAX_FORMAT },
    { "MINMAX_SINK",			GL_MINMAX_SINK },
    { "TABLE_TOO_LARGE",		GL_TABLE_TOO_LARGE },
    { "BLEND_EQUATION",			GL_BLEND_EQUATION },
    { "MIN",				GL_MIN },
    { "MAX",				GL_MAX },
    { "FUNC_ADD",			GL_FUNC_ADD },
    { "FUNC_SUBTRACT",			GL_FUNC_SUBTRACT },
    { "FUNC_REVERSE_SUBTRACT",		GL_FUNC_REVERSE_SUBTRACT },
    { "BLEND_COLOR",			GL_BLEND_COLOR },
    /* OpenGL 1.3 */
    /* multitexture */
    { "TEXTURE0",			GL_TEXTURE0 },
    { "TEXTURE1",			GL_TEXTURE1 },
    { "TEXTURE2",			GL_TEXTURE2 },
    { "TEXTURE3",			GL_TEXTURE3 },
    { "TEXTURE4",			GL_TEXTURE4 },
    { "TEXTURE5",			GL_TEXTURE5 },
    { "TEXTURE6",			GL_TEXTURE6 },
    { "TEXTURE7",			GL_TEXTURE7 },
    { "TEXTURE8",			GL_TEXTURE8 },
    { "TEXTURE9",			GL_TEXTURE9 },
    { "TEXTURE10",			GL_TEXTURE10 },
    { "TEXTURE11",			GL_TEXTURE11 },
    { "TEXTURE12",			GL_TEXTURE12 },
    { "TEXTURE13",			GL_TEXTURE13 },
    { "TEXTURE14",			GL_TEXTURE14 },
    { "TEXTURE15",			GL_TEXTURE15 },
    { "TEXTURE16",			GL_TEXTURE16 },
    { "TEXTURE17",			GL_TEXTURE17 },
    { "TEXTURE18",			GL_TEXTURE18 },
    { "TEXTURE19",			GL_TEXTURE19 },
    { "TEXTURE20",			GL_TEXTURE20 },
    { "TEXTURE21",			GL_TEXTURE21 },
    { "TEXTURE22",			GL_TEXTURE22 },
    { "TEXTURE23",			GL_TEXTURE23 },
    { "TEXTURE24",			GL_TEXTURE24 },
    { "TEXTURE25",			GL_TEXTURE25 },
    { "TEXTURE26",			GL_TEXTURE26 },
    { "TEXTURE27",			GL_TEXTURE27 },
    { "TEXTURE28",			GL_TEXTURE28 },
    { "TEXTURE29",			GL_TEXTURE29 },
    { "TEXTURE30",			GL_TEXTURE30 },
    { "TEXTURE31",			GL_TEXTURE31 },
    { "ACTIVE_TEXTURE",			GL_ACTIVE_TEXTURE },
    { "CLIENT_ACTIVE_TEXTURE",		GL_CLIENT_ACTIVE_TEXTURE },
    { "MAX_TEXTURE_UNITS",		GL_MAX_TEXTURE_UNITS },
    /* texture_cube_map */
    { "NORMAL_MAP",			GL_NORMAL_MAP },
    { "REFLECTION_MAP",			GL_REFLECTION_MAP },
    { "TEXTURE_CUBE_MAP",		GL_TEXTURE_CUBE_MAP },
    { "TEXTURE_BINDING_CUBE_MAP",	GL_TEXTURE_BINDING_CUBE_MAP },
    { "TEXTURE_CUBE_MAP_POSITIVE_X",	GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { "TEXTURE_CUBE_MAP_NEGATIVE_X",	GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
    { "TEXTURE_CUBE_MAP_POSITIVE_Y",	GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
    { "TEXTURE_CUBE_MAP_NEGATIVE_Y",	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
    { "TEXTURE_CUBE_MAP_POSITIVE_Z",	GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
    { "TEXTURE_CUBE_MAP_NEGATIVE_Z",	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z },
    { "PROXY_TEXTURE_CUBE_MAP",		GL_PROXY_TEXTURE_CUBE_MAP },
    { "MAX_CUBE_MAP_TEXTURE_SIZE",	GL_MAX_CUBE_MAP_TEXTURE_SIZE },
    /* texture_compression */
    { "COMPRESSED_ALPHA",		GL_COMPRESSED_ALPHA },
    { "COMPRESSED_LUMINANCE",		GL_COMPRESSED_LUMINANCE },
    { "COMPRESSED_LUMINANCE_ALPHA",	GL_COMPRESSED_LUMINANCE_ALPHA },
    { "COMPRESSED_INTENSITY",		GL_COMPRESSED_INTENSITY },
    { "COMPRESSED_RGB",			GL_COMPRESSED_RGB },
    { "COMPRESSED_RGBA",		GL_COMPRESSED_RGBA },
    { "TEXTURE_COMPRESSION_HINT",	GL_TEXTURE_COMPRESSION_HINT },
    { "TEXTURE_COMPRESSED_IMAGE_SIZE",	GL_TEXTURE_COMPRESSED_IMAGE_SIZE },
    { "TEXTURE_COMPRESSED",		GL_TEXTURE_COMPRESSED },
    { "NUM_COMPRESSED_TEXTURE_FORMATS",	GL_NUM_COMPRESSED_TEXTURE_FORMATS },
    { "COMPRESSED_TEXTURE_FORMATS",	GL_COMPRESSED_TEXTURE_FORMATS },
    /* multisample */
    { "MULTISAMPLE",			GL_MULTISAMPLE },
    { "SAMPLE_ALPHA_TO_COVERAGE",	GL_SAMPLE_ALPHA_TO_COVERAGE },
    { "SAMPLE_ALPHA_TO_ONE",		GL_SAMPLE_ALPHA_TO_ONE },
    { "SAMPLE_COVERAGE",		GL_SAMPLE_COVERAGE },
    { "SAMPLE_BUFFERS",			GL_SAMPLE_BUFFERS },
    { "SAMPLES",			GL_SAMPLES },
    { "SAMPLE_COVERAGE_VALUE",		GL_SAMPLE_COVERAGE_VALUE },
    { "SAMPLE_COVERAGE_INVERT",		GL_SAMPLE_COVERAGE_INVERT },
    { "MULTISAMPLE_BIT",		GL_MULTISAMPLE_BIT },
    /* transpose_matrix */
    { "TRANSPOSE_MODELVIEW_MATRIX",	GL_TRANSPOSE_MODELVIEW_MATRIX },
    { "TRANSPOSE_PROJECTION_MATRIX",	GL_TRANSPOSE_PROJECTION_MATRIX },
    { "TRANSPOSE_TEXTURE_MATRIX",	GL_TRANSPOSE_TEXTURE_MATRIX },
    { "TRANSPOSE_COLOR_MATRIX",		GL_TRANSPOSE_COLOR_MATRIX },
    /* texture_env_combine */
    { "COMBINE",			GL_COMBINE },
    { "COMBINE_RGB",			GL_COMBINE_RGB },
    { "COMBINE_ALPHA",			GL_COMBINE_ALPHA },
    { "SOURCE0_RGB",			GL_SOURCE0_RGB },
    { "SOURCE1_RGB",			GL_SOURCE1_RGB },
    { "SOURCE2_RGB",			GL_SOURCE2_RGB },
    { "SOURCE0_ALPHA",			GL_SOURCE0_ALPHA },
    { "SOURCE1_ALPHA",			GL_SOURCE1_ALPHA },
    { "SOURCE2_ALPHA",			GL_SOURCE2_ALPHA },
    { "OPERAND0_RGB",			GL_OPERAND0_RGB },
    { "OPERAND1_RGB",			GL_OPERAND1_RGB },
    { "OPERAND2_RGB",			GL_OPERAND2_RGB },
    { "OPERAND0_ALPHA",			GL_OPERAND0_ALPHA },
    { "OPERAND1_ALPHA",			GL_OPERAND1_ALPHA },
    { "OPERAND2_ALPHA",			GL_OPERAND2_ALPHA },
    { "RGB_SCALE",			GL_RGB_SCALE },
    { "ADD_SIGNED",			GL_ADD_SIGNED },
    { "INTERPOLATE",			GL_INTERPOLATE },
    { "SUBTRACT",			GL_SUBTRACT },
    { "CONSTANT",			GL_CONSTANT },
    { "PRIMARY_COLOR",			GL_PRIMARY_COLOR },
    { "PREVIOUS",			GL_PREVIOUS },
    /* texture_env_dot3 */
    { "DOT3_RGB",			GL_DOT3_RGB },
    { "DOT3_RGBA",			GL_DOT3_RGBA },
    /* texture_border_clamp */
    { "CLAMP_TO_BORDER",		GL_CLAMP_TO_BORDER },
    /* GL_ARB_multitexture */
    { "TEXTURE0_ARB",			GL_TEXTURE0_ARB },
    { "TEXTURE1_ARB",			GL_TEXTURE1_ARB },
    { "TEXTURE2_ARB",			GL_TEXTURE2_ARB },
    { "TEXTURE3_ARB",			GL_TEXTURE3_ARB },
    { "TEXTURE4_ARB",			GL_TEXTURE4_ARB },
    { "TEXTURE5_ARB",			GL_TEXTURE5_ARB },
    { "TEXTURE6_ARB",			GL_TEXTURE6_ARB },
    { "TEXTURE7_ARB",			GL_TEXTURE7_ARB },
    { "TEXTURE8_ARB",			GL_TEXTURE8_ARB },
    { "TEXTURE9_ARB",			GL_TEXTURE9_ARB },
    { "TEXTURE10_ARB",			GL_TEXTURE10_ARB },
    { "TEXTURE11_ARB",			GL_TEXTURE11_ARB },
    { "TEXTURE12_ARB",			GL_TEXTURE12_ARB },
    { "TEXTURE13_ARB",			GL_TEXTURE13_ARB },
    { "TEXTURE14_ARB",			GL_TEXTURE14_ARB },
    { "TEXTURE15_ARB",			GL_TEXTURE15_ARB },
    { "TEXTURE16_ARB",			GL_TEXTURE16_ARB },
    { "TEXTURE17_ARB",			GL_TEXTURE17_ARB },
    { "TEXTURE18_ARB",			GL_TEXTURE18_ARB },
    { "TEXTURE19_ARB",			GL_TEXTURE19_ARB },
    { "TEXTURE20_ARB",			GL_TEXTURE20_ARB },
    { "TEXTURE21_ARB",			GL_TEXTURE21_ARB },
    { "TEXTURE22_ARB",			GL_TEXTURE22_ARB },
    { "TEXTURE23_ARB",			GL_TEXTURE23_ARB },
    { "TEXTURE24_ARB",			GL_TEXTURE24_ARB },
    { "TEXTURE25_ARB",			GL_TEXTURE25_ARB },
    { "TEXTURE26_ARB",			GL_TEXTURE26_ARB },
    { "TEXTURE27_ARB",			GL_TEXTURE27_ARB },
    { "TEXTURE28_ARB",			GL_TEXTURE28_ARB },
    { "TEXTURE29_ARB",			GL_TEXTURE29_ARB },
    { "TEXTURE30_ARB",			GL_TEXTURE30_ARB },
    { "TEXTURE31_ARB",			GL_TEXTURE31_ARB },
    { "ACTIVE_TEXTURE_ARB",		GL_ACTIVE_TEXTURE_ARB },
    { "CLIENT_ACTIVE_TEXTURE_ARB",	GL_CLIENT_ACTIVE_TEXTURE_ARB },
    { "MAX_TEXTURE_UNITS_ARB",		GL_MAX_TEXTURE_UNITS_ARB },
    /* GL_EXEXT_texture_env_combine */
    { "COMBINE_EXT",			GL_COMBINE_EXT },
    { "COMBINE_RGB_EXT",		GL_COMBINE_RGB_EXT },
    { "COMBINE_ALPHA_EXT",		GL_COMBINE_ALPHA_EXT },
    { "RGB_SCALE_EXT",			GL_RGB_SCALE_EXT },
    { "ADD_SIGNED_EXT",			GL_ADD_SIGNED_EXT },
    { "INTERPOLATE_EXT",		GL_INTERPOLATE_EXT },
    { "CONSTANT_EXT",			GL_CONSTANT_EXT },
    { "PRIMARY_COLOR_EXT",		GL_PRIMARY_COLOR_EXT },
    { "PREVIOUS_EXT",			GL_PREVIOUS_EXT },
    { "SOURCE0_RGB_EXT",		GL_SOURCE0_RGB_EXT },
    { "SOURCE1_RGB_EXT",		GL_SOURCE1_RGB_EXT },
    { "SOURCE2_RGB_EXT",		GL_SOURCE2_RGB_EXT },
    { "SOURCE0_ALPHA_EXT",		GL_SOURCE0_ALPHA_EXT },
    { "SOURCE1_ALPHA_EXT",		GL_SOURCE1_ALPHA_EXT },
    { "SOURCE2_ALPHA_EXT",		GL_SOURCE2_ALPHA_EXT },
    { "OPERAND0_RGB_EXT",		GL_OPERAND0_RGB_EXT },
    { "OPERAND1_RGB_EXT",		GL_OPERAND1_RGB_EXT },
    { "OPERAND2_RGB_EXT",		GL_OPERAND2_RGB_EXT },
    { "OPERAND0_ALPHA_EXT",		GL_OPERAND0_ALPHA_EXT },
    { "OPERAND1_ALPHA_EXT",		GL_OPERAND1_ALPHA_EXT },
    { "OPERAND2_ALPHA_EXT",		GL_OPERAND2_ALPHA_EXT },
};

/*
 * Implementation
 */
void
init_gl(void)
{
    char			*string;
    oword_t			 offset;
    orecord_t			*record;
    obuiltin_t			*builtin;

    for (offset = 0; offset < osize(gl); ++offset) {
	string = gl[offset].name;
	onew_constant(gl_record, oget_string((ouint8_t *)string,
					     strlen(string)),
		      gl[offset].value);
    }

    record = current_record;
    current_record = gl_record;

    define_builtin1(t_void,    ClearIndex, t_float32);
    define_builtin4(t_void,    ClearColor,
		    t_float32, t_float32, t_float32, t_float32);
    define_builtin1(t_void,    Clear, t_uint32);
    define_builtin1(t_void,    IndexMask, t_uint32);
    define_builtin4(t_void,    ColorMask, t_uint8, t_uint8, t_uint8, t_uint8);
    define_builtin2(t_void,    AlphaFunc, t_uint32, t_float32);
    define_builtin2(t_void,    BlendFunc, t_uint32, t_uint32);
    define_builtin1(t_void,    LogicOp, t_uint32);
    define_builtin1(t_void,    CullFace, t_uint32);
    define_builtin1(t_void,    FrontFace, t_uint32);
    define_builtin1(t_void,    PointSize, t_float32);
    define_builtin1(t_void,    LineWidth, t_float32);
    define_builtin2(t_void,    LineStipple, t_int32, t_uint16);
    define_builtin2(t_void,    PolygonMode, t_uint32, t_uint32);
    define_builtin2(t_void,    PolygonOffset, t_float32, t_float32);
    define_builtin1(t_void,    PolygonStipple, t_vector|t_uint8);
    define_builtin1(t_void,    GetPolygonStipple, t_vector|t_uint8);
    define_builtin1(t_void,    EdgeFlag, t_uint8);
    define_builtin4(t_void,    Scissor, t_int32, t_int32, t_int32, t_int32);
    define_builtin2(t_void,    ClipPlane, t_uint32, t_vector|t_float64);
    define_builtin2(t_void,    GetClipPlane, t_uint32, t_vector|t_float64);
    define_builtin1(t_void,    DrawBuffer, t_uint32);
    define_builtin1(t_void,    ReadBuffer, t_uint32);
    define_builtin1(t_void,    Enable, t_uint32);
    define_builtin1(t_void,    Disable, t_uint32);
    define_builtin1(t_uint8,   IsEnabled, t_uint32);
    define_builtin1(t_void,    EnableClientState, t_uint32);
    define_builtin1(t_void,    DisableClientState, t_uint32);
    define_builtin2(t_void,    GetBooleanv, t_uint32, t_vector|t_uint8);
    define_builtin2(t_void,    GetDoublev, t_uint32, t_vector|t_float64);
    define_builtin2(t_void,    GetFloatv, t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    GetIntegerv, t_uint32, t_vector|t_int32);
    define_builtin1(t_void,    PushAttrib, t_uint32);
    define_builtin0(t_void,    PopAttrib);
    define_builtin1(t_void,    PushClientAttrib, t_uint32);
    define_builtin0(t_void,    PopClientAttrib);
    define_builtin1(t_void,    RenderMode, t_uint32);
    define_builtin0(t_uint32,  GetError);
    define_builtin1(t_string,  GetString, t_uint32);
    define_builtin0(t_void,    Finish);
    define_builtin0(t_void,    Flush);
    define_builtin2(t_void,    Hint, t_uint32, t_uint32);
    define_builtin1(t_void,    ClearDepth, t_float64);
    define_builtin1(t_void,    DepthFunc, t_uint32);
    define_builtin1(t_void,    DepthMask, t_uint8);
    define_builtin2(t_void,    DepthRange, t_float64, t_float64);
    define_builtin4(t_void,    ClearAccum,
		    t_float32, t_float32, t_float32, t_float32);
    define_builtin2(t_void,    Accum, t_uint32, t_float32);
    define_builtin1(t_void,    MatrixMode, t_uint32);
    define_builtin6(t_void,    Ortho,
		    t_float64, t_float64, t_float64,
		    t_float64, t_float64, t_float64);
    define_builtin6(t_void,    Frustum,
		    t_float64, t_float64, t_float64,
		    t_float64, t_float64, t_float64);
    define_builtin4(t_void,    Viewport, t_int32, t_int32, t_int32, t_int32);
    define_builtin0(t_void,    PushMatrix);
    define_builtin0(t_void,    PopMatrix);
    define_builtin0(t_void,    LoadIdentity);
    define_builtin1(t_void,    LoadMatrix, t_vector|t_float64);
    define_builtin1(t_void,    MultMatrix, t_vector|t_float64);
    define_builtin4(t_void,    Rotate,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin3(t_void,    Scale, t_float64, t_float64, t_float64);
    define_builtin3(t_void,    Translate, t_float64, t_float64, t_float64);
    define_builtin1(t_uint8,   IsList, t_uint32);
    define_builtin2(t_void,    DeleteLists, t_uint32, t_int32);
    define_builtin1(t_uint32,  GenLists, t_int32);
    define_builtin2(t_void,    NewList, t_uint32, t_uint32);
    define_builtin0(t_void,    EndList);
    define_builtin1(t_void,    CallList, t_uint32);
    define_builtin1(t_void,    CallLists, t_vector|t_uint32);
    define_builtin1(t_void,    ListBase, t_uint32);
    define_builtin1(t_void,    Begin, t_uint32);
    define_builtin0(t_void,    End);
    define_builtin2(t_void,    Vertex2, t_float64, t_float64);
    define_builtin3(t_void,    Vertex3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    Vertex4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Vertex2v, t_vector|t_float64);
    define_builtin1(t_void,    Vertex3v, t_vector|t_float64);
    define_builtin1(t_void,    Vertex4v, t_vector|t_float64);
    define_builtin3(t_void,    Normal3, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Normal3v, t_vector|t_float64);
    define_builtin1(t_void,    Index, t_int32);
    define_builtin3(t_void,    Color3, t_float32, t_float32, t_float32);
    define_builtin4(t_void,    Color4,
		    t_float32, t_float32, t_float32, t_float32);
    define_builtin1(t_void,    Color3v, t_vector|t_float32);
    define_builtin1(t_void,    Color4v, t_vector|t_float32);
    define_builtin1(t_void,    TexCoord1, t_float64);
    define_builtin2(t_void,    TexCoord2, t_float64, t_float64);
    define_builtin3(t_void,    TexCoord3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    TexCoord4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    TexCoord2v, t_vector|t_float64);
    define_builtin1(t_void,    TexCoord3v, t_vector|t_float64);
    define_builtin1(t_void,    TexCoord4v, t_vector|t_float64);
    define_builtin2(t_void,    RasterPos2, t_float64, t_float64);
    define_builtin3(t_void,    RasterPos3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    RasterPos4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    RasterPos2v, t_vector|t_float64);
    define_builtin1(t_void,    RasterPos3v, t_vector|t_float64);
    define_builtin1(t_void,    RasterPos4v, t_vector|t_float64);
    define_builtin4(t_void,    Rect,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Rectv, t_vector|t_float64);
    define_builtin1(t_void,    ShadeModel, t_uint32);
    define_builtin3(t_void,    Light, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    Lightv, t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    GetLightv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    LightModel, t_uint32, t_float32);
    define_builtin2(t_void,    LightModelv, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    Material, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    Materialv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    GetMaterialv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    ColorMaterial, t_uint32, t_uint32);
    define_builtin2(t_void,    PixelZoom, t_float32, t_float32);
    define_builtin2(t_void,    PixelStore, t_uint32, t_int32);
    define_builtin2(t_void,    PixelTransfer, t_uint32, t_float32);
    define_builtin2(t_void,    PixelMapv, t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    GetPixelMapv, t_uint32, t_vector|t_float32);
    define_builtin7(t_void,    Bitmap,
		    t_int32, t_int32, t_float32, t_float32,
		    t_float32, t_float32, t_vector|t_uint8);
    define_builtin6(t_void,    ReadPixels,
		    t_int32, t_int32, t_int32, t_int32,
		    t_uint32, t_vector|t_uint8);
    define_builtin6(t_void,    ReadBitmap,
		    t_int32, t_int32, t_int32, t_int32,
		    t_uint32, t_vector|t_uint8);
    define_builtin4(t_void,    DrawPixels,
		    t_int32, t_int32, t_uint32, t_vector|t_uint8);
    define_builtin4(t_void,    DrawBitmap,
		    t_int32, t_int32, t_uint32, t_vector|t_uint8);
    define_builtin5(t_void,    CopyPixels,
		    t_int32, t_int32, t_int32, t_int32, t_uint32);
    define_builtin3(t_void,    StencilFunc, t_int32, t_uint32, t_int32);
    define_builtin1(t_void,    StencilMask, t_int32);
    define_builtin3(t_void,    StencilOp, t_uint32, t_uint32, t_uint32);
    define_builtin1(t_void,    ClearStencil, t_int32);
    define_builtin3(t_void,    TexGen, t_uint32, t_uint32, t_float64);
    define_builtin3(t_void,    TexGenv, t_uint32, t_uint32, t_vector|t_float64);
    define_builtin3(t_void,    GetTexGenv,
		    t_uint32, t_uint32, t_vector|t_float64);
    define_builtin3(t_void,    TexEnv, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    TexEnvv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    GetTexEnvv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    TexParameter, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    TexParameterv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_void,    GetTexParameterv,
		    t_uint32, t_uint32, t_vector|t_float32);
    define_builtin3(t_int32,   GetTexLevelParameter,
		    t_uint32, t_int32, t_uint32);
    define_builtin6(t_void,    TexImage1D,
		    t_uint32, t_int32, t_int32, t_int32, t_uint32,
		    t_vector|t_uint8);
    define_builtin7(t_void,    TexImage2D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32,
		    t_uint32, t_vector|t_uint8);
    define_builtin4(t_void,    GetTexImage,
		    t_uint32, t_int32, t_uint32, t_vector|t_uint8);
    define_builtin1(t_void,    GenTextures, t_vector|t_uint32);
    define_builtin1(t_void,    DeleteTextures, t_vector|t_uint32);
    define_builtin2(t_void,    BindTexture, t_uint32, t_uint32);
    define_builtin2(t_void,    PrioritizeTextures,
		    t_vector|t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    AreTexturesResident,
		    t_vector|t_uint32, t_vector|t_uint8);
    define_builtin1(t_void,    IsTexture, t_uint32);
    define_builtin6(t_void,    TexSubImage1D,
		    t_uint32, t_int32, t_int32, t_int32, t_uint32,
		    t_vector|t_uint8);
    define_builtin7(t_void,    TexSubImage2D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32, t_uint32,
		    t_vector|t_uint8);
    define_builtin8(t_void,    CopyTexImage1D,
		    t_uint32, t_int32, t_uint32, t_int32, t_int32, t_int32,
		    t_int32, t_int32);
    define_builtin9(t_void,    CopyTexImage2D,
		    t_uint32, t_int32, t_uint32, t_int32, t_int32, t_int32,
		    t_int32, t_int32, t_int32);
    define_builtin5(t_void,    CopyTexSubImage1D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32);
    define_builtin7(t_void,    CopyTexSubImage2D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32, t_int32,
		    t_int32);
    define_builtin5(t_void,    Map1,
		    t_uint32, t_float64, t_float64, t_int32,
		    t_vector|t_float64);
    define_builtin8(t_void,    Map2,
		    t_uint32, t_float64, t_float64, t_int32,
		    t_float64, t_float64, t_int32, t_vector|t_float64);
    define_builtin2(t_void,    GetMapCoeff, t_uint32, t_vector|t_float64);
    define_builtin2(t_void,    GetMapOrder, t_uint32, t_vector|t_int32);
    define_builtin2(t_void,    GetMapDomain, t_uint32, t_vector|t_float64);
    define_builtin1(t_void,    EvalCoord1, t_float64);
    define_builtin2(t_void,    EvalCoord2, t_float64, t_float64);
    define_builtin3(t_void,    MapGrid1, t_int32, t_float64, t_float64);
    define_builtin6(t_void,    MapGrid2,
		    t_int32, t_float64, t_float64,
		    t_int32, t_float64, t_float64);
    define_builtin1(t_void,    EvalPoint1, t_int32);
    define_builtin2(t_void,    EvalPoint2, t_int32, t_int32);
    define_builtin3(t_void,    EvalMesh1, t_uint32, t_int32, t_int32);
    define_builtin5(t_void,    EvalMesh2,
		    t_uint32, t_int32, t_int32, t_int32, t_int32);
    define_builtin2(t_void,    Fog, t_uint32, t_float32);
    define_builtin2(t_void,    Fogv, t_uint32, t_vector|t_float32);
    define_builtin2(t_void,    FeedbackBuffer, t_uint32, t_vector|t_float32);
    define_builtin1(t_void,    PassThrough, t_float32);
    define_builtin1(t_void,    SelectBuffer, t_vector|t_uint32);
    define_builtin0(t_void,    InitNames);
    define_builtin1(t_void,    LoadName, t_uint32);
    define_builtin1(t_void,    PushName, t_uint32);
    define_builtin0(t_void,    PopName);

    define_builtin1(t_void,    ActiveTextureARB, t_uint32);
    define_builtin1(t_void,    ClientActiveTextureARB, t_uint32);
    define_builtin2(t_void,    MultiTexCoord1ARB, t_uint32, t_float64);
    define_builtin2(t_void,    MultiTexCoord1vARB,
		    t_uint32, t_vector|t_float64);
    define_builtin3(t_void,    MultiTexCoord2ARB,
		    t_uint32, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord2vARB,
		    t_uint32, t_vector|t_float64);
    define_builtin4(t_void,    MultiTexCoord3ARB,
		    t_uint32, t_float64, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord3vARB,
		    t_uint32, t_vector|t_float64);
    define_builtin5(t_void,    MultiTexCoord4ARB,
		    t_uint32, t_float64, t_float64, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord4vARB,
		    t_uint32, t_vector|t_float64);

    define_builtin2(t_void, WindowPos2, t_float64, t_float64);
    define_builtin1(t_void, WindowPos2v, t_vector|t_float64);
    define_builtin3(t_void, WindowPos3, t_float64, t_float64, t_float64);
    define_builtin1(t_void, WindowPos3v, t_vector|t_float64);

    current_record = record;
}

void
finish_gl(void)
{
}

static void
native_ClearIndex(oobject_t list, oint32_t ac)
/* void ClearIndex(float32_t c); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_t				*alist;

    alist = (nat_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClearIndex(alist->a0);
}

static void
native_ClearColor(oobject_t list, oint32_t ac)
/* void ClearColor(float32_t r, float32_t g, float32_t b, float32_t a); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_f32_f32_t		*alist;

    alist = (nat_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClearColor(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Clear(oobject_t list, oint32_t ac)
/* void Clear(uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClear(alist->a0);
}

static void
native_IndexMask(oobject_t list, oint32_t ac)
/* void IndexMask(uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glIndexMask(alist->a0);
}

static void
native_ColorMask(oobject_t list, oint32_t ac)
/* void ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u8_u8_u8_u8_t			*alist;

    alist = (nat_u8_u8_u8_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glColorMask(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_AlphaFunc(oobject_t list, oint32_t ac)
/* void AlphaFunc(uint32_t func, float32_t ref); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f32_t			*alist;

    alist = (nat_u32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glAlphaFunc(alist->a0, alist->a1);
}

static void
native_BlendFunc(oobject_t list, oint32_t ac)
/* void BlendFunc(uint32_t func, uint32_t dfactor); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glBlendFunc(alist->a0, alist->a1);
}

static void
native_LogicOp(oobject_t list, oint32_t ac)
/* void LogicOp(uint32_t opcode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLogicOp(alist->a0);
}

static void
native_CullFace(oobject_t list, oint32_t ac)
/* void CullFace(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCullFace(alist->a0);
}

static void
native_FrontFace(oobject_t list, oint32_t ac)
/* void FrontFace(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glFrontFace(alist->a0);
}

static void
native_PointSize(oobject_t list, oint32_t ac)
/* void PointSize(float32_t size); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_t				*alist;

    alist = (nat_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPointSize(alist->a0);
}

static void
native_LineWidth(oobject_t list, oint32_t ac)
/* void LineWidth(float32_t width); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_t				*alist;

    alist = (nat_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLineWidth(alist->a0);
}

static void
native_LineStipple(oobject_t list, oint32_t ac)
/* void LineStipple(int32_t factor, uint16_t pattern); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_u16_t			*alist;

    alist = (nat_i32_u16_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLineStipple(alist->a0, alist->a1);
}

static void
native_PolygonMode(oobject_t list, oint32_t ac)
/* void PolygonMode(uint32_t face, uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPolygonMode(alist->a0, alist->a1);
}

static void
native_PolygonOffset(oobject_t list, oint32_t ac)
/* void PolygonOffset(float32_t factor, float32_t units); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_t			*alist;

    alist = (nat_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPolygonOffset(alist->a0, alist->a1);
}

static void
native_PolygonStipple(oobject_t list, oint32_t ac)
/* void PolygonStipple(uint8_t mask[128]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_uint8, 128))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glPolygonStipple(alist->a0->v.u8);
}

static void
native_GetPolygonStipple(oobject_t list, oint32_t ac)
/* void GetPolygonStipple(uint8_t mask[128]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint8))
	ovm_raise(except_invalid_argument);
    if (alist->a0->length != 128)
	orenew_vector(alist->a0, 128);
    r0->t = t_void;
    glGetPolygonStipple(alist->a0->v.u8);
}

static void
native_EdgeFlag(oobject_t list, oint32_t ac)
/* void EdgeFlag(uint8_t flag); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u8_t				*alist;

    alist = (nat_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEdgeFlag(alist->a0);
}

static void
native_Scissor(oobject_t list, oint32_t ac)
/* void Scissor(int32_t x, int32_t y, int32_t width, int32_t height) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_t		*alist;

    alist = (nat_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glScissor(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_ClipPlane(oobject_t list, oint32_t ac)
/* void ClipPlane(uint32_t plane, float64_t equation[4]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a1, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glClipPlane(alist->a0, alist->a1->v.f64);
}

static void
native_GetClipPlane(oobject_t list, oint32_t ac)
/* void GetClipPlane(uint32_t plane, float64_t equation[4]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_float64))
	ovm_raise(except_invalid_argument);
    if (alist->a1->length != 4)
	orenew_vector(alist->a1, 4);
    r0->t = t_void;
    glGetClipPlane(alist->a0, alist->a1->v.f64);
}

static void
native_DrawBuffer(oobject_t list, oint32_t ac)
/* void DrawBuffer(uint32_t mode) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDrawBuffer(alist->a0);
}

static void
native_ReadBuffer(oobject_t list, oint32_t ac)
/* void ReadBuffer(uint32_t mode) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glReadBuffer(alist->a0);
}

static void
native_Enable(oobject_t list, oint32_t ac)
/* void Enable(uint32_t cap) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEnable(alist->a0);
}

static void
native_Disable(oobject_t list, oint32_t ac)
/* void Disable(uint32_t cap) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDisable(alist->a0);
}

static void
native_IsEnabled(oobject_t list, oint32_t ac)
/* uint8_t IsEnabled(uint32_t cap) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsEnabled(alist->a0);
}

static void
native_EnableClientState(oobject_t list, oint32_t ac)
/* void EnableClientState(uint32_t cap) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEnableClientState(alist->a0);
}

static void
native_DisableClientState(oobject_t list, oint32_t ac)
/* void DisableClientState(uint32_t cap) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDisableClientState(alist->a0);
}

static void
native_GetBooleanv(oobject_t list, oint32_t ac)
/* void GetBooleanv(uint32_t pname, uint8_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_uint8))
	ovm_raise(except_invalid_argument);
    switch (alist->a0) {
	case GL_ALPHA_TEST:
	case GL_AUTO_NORMAL:
	case GL_BLEND:
	case GL_COLOR_ARRAY:
	case GL_COLOR_LOGIC_OP:
	case GL_COLOR_MATERIAL:
	case GL_CULL_FACE_MODE:
	case GL_CURRENT_RASTER_POSITION_VALID:
	case GL_DEPTH_WRITEMASK:
	case GL_DITHER:
	case GL_DOUBLEBUFFER:
	case GL_EDGE_FLAG:
	case GL_EDGE_FLAG_ARRAY:
#if defined(GL_EXT_DEPTH_TEXTURE)
	case GL_EXT_DEPTH_TEXTURE:
#endif
#if defined(GL_EXT_SHADOW)
	case GL_EXT_SHADOW:
#endif
#if defined(GL_EXT_TEXTURE_BORDER_CLAMP)
	case GL_EXT_TEXTURE_BORDER_CLAMP:
#endif
#if defined(GL_EXT_TEXTURE3D)
	case GL_EXT_TEXTURE3D:
#endif
#if defined(GL_EXT_TEXTURE_EDGE_CLAMP)
	case GL_EXT_TEXTURE_EDGE_CLAMP:
#endif
	case GL_FOG:
#if defined(GL_HP_TEXTURING_LIGHTING)
	case GL_HP_TEXTURING_LIGHTING:
#endif
	case GL_INDEX_ARRAY:		/* XXX should not be used */
	case GL_INDEX_LOGIC_OP:
	case GL_INDEX_MODE:
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	case GL_LIGHTING:
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
	case GL_LIGHT_MODEL_TWO_SIDE:
	case GL_LINE_SMOOTH:
	case GL_LINE_STIPPLE:
	case GL_MAP1_COLOR_4:
	case GL_MAP1_INDEX:
	case GL_MAP1_NORMAL:
	case GL_MAP1_TEXTURE_COORD_1:
	case GL_MAP1_TEXTURE_COORD_2:
	case GL_MAP1_TEXTURE_COORD_3:
	case GL_MAP1_TEXTURE_COORD_4:
	case GL_MAP1_VERTEX_3:
	case GL_MAP1_VERTEX_4:
	case GL_MAP2_INDEX:
	case GL_MAP2_NORMAL:
	case GL_MAP2_TEXTURE_COORD_1:
	case GL_MAP2_TEXTURE_COORD_2:
	case GL_MAP2_TEXTURE_COORD_3:
	case GL_MAP2_TEXTURE_COORD_4:
	case GL_MAP2_VERTEX_3:
	case GL_MAP2_VERTEX_4:
	case GL_MAP_COLOR:
	case GL_MAP_STENCIL:
	case GL_NORMAL_ARRAY:		/* XXX should not be used */
	case GL_NORMALIZE:
	case GL_OCCLUSION_TEST_HP:
	case GL_OCCLUSION_TEST_RESULT_HP:
	case GL_PACK_LSB_FIRST:
	case GL_PACK_SWAP_BYTES:
	case GL_POINT_SMOOTH:
	case GL_POLYGON_OFFSET_UNITS:
	case GL_POLYGON_OFFSET_FILL:
	case GL_POLYGON_OFFSET_LINE:
	case GL_POLYGON_OFFSET_POINT:
	case GL_POLYGON_SMOOTH:
	case GL_POLYGON_STIPPLE:
	case GL_RGBA_MODE:
	case GL_SCISSOR_TEST:
	case GL_STENCIL_TEST:
	case GL_STEREO:
	case GL_TEXTURE_1D:
	case GL_TEXTURE_2D:
	case GL_TEXTURE_COORD_ARRAY:	/* XXX should not be used */
	case GL_TEXTURE_GEN_Q:
	case GL_TEXTURE_GEN_R:
	case GL_TEXTURE_GEN_S:
	case GL_TEXTURE_GEN_T:
	case GL_UNPACK_LSB_FIRST:
	case GL_UNPACK_SWAP_BYTES:
	case GL_VERTEX_ARRAY:		/* XXX should not be used */
	    length = 1;
	    break;
#if defined(GL_VISIBILITY_TEST_HP)
	case GL_VISIBILITY_TEST_HP:
	    length = 2;
	    break;
#endif
	case GL_COLOR_WRITEMASK:
	    length = 4;
	    break;
	default:
	    /* FIXME add more valid entries with proper bounds checking */
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetBooleanv(alist->a0, alist->a1->v.u8);
}

static void
native_GetDoublev(oobject_t list, oint32_t ac)
/* void GetDoublev(uint32_t pname, float64_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_float64))
	ovm_raise(except_invalid_argument);
    switch (alist->a0) {
	case GL_CURRENT_RASTER_DISTANCE:
	case GL_DEPTH_CLEAR_VALUE:
	    length = 1;
	    break;
	case GL_DEPTH_RANGE:
	case GL_MAP1_GRID_DOMAIN:
	    length = 2;
	    break;
	case GL_CURRENT_NORMAL:
	    length = 3;
	    break;
	case GL_CURRENT_RASTER_POSITION:
	case GL_CURRENT_RASTER_TEXTURE_COORDS:
	case GL_CURRENT_TEXTURE_COORDS:
	case GL_MAP2_GRID_DOMAIN:
	    length = 4;
	    break;
	case GL_MODELVIEW_MATRIX:
	case GL_PROJECTION_MATRIX:
	case GL_TEXTURE_MATRIX:
	    length = 16;
	    break;
	default:
	    /* FIXME add more valid entries with proper bounds checking */
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetDoublev(alist->a0, alist->a1->v.f64);
}

static void
native_GetFloatv(oobject_t list, oint32_t ac)
/* void GetFloatv(uint32_t pname, float32_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    switch (alist->a0) {
	case GL_ALPHA_BIAS:
	case GL_ALPHA_SCALE:
	case GL_ALPHA_TEST_REF:
	case GL_AUX_BUFFERS:
	case GL_BLUE_BIAS:
	case GL_BLUE_SCALE:
	case GL_DEPTH_BIAS:
	case GL_DEPTH_SCALE:
	case GL_FOG_DENSITY:
	case GL_FOG_END:
	case GL_FOG_START:
	case GL_GREEN_BIAS:
	case GL_GREEN_SCALE:
	case GL_LINE_WIDTH:
	case GL_LINE_WIDTH_GRANULARITY:
	case GL_POINT_SIZE:
	case GL_POINT_SIZE_GRANULARITY:
	case GL_POINT_SIZE_RANGE:
	case GL_POLYGON_OFFSET_FACTOR:
	case GL_POLYGON_OFFSET_UNITS:
	case GL_RED_BIAS:
	case GL_RED_SCALE:
	case GL_ZOOM_X:
	case GL_ZOOM_Y:
	    length = 1;
	    break;
	case GL_LINE_WIDTH_RANGE:
	    length = 2;
	    break;
	case GL_ACCUM_CLEAR_VALUE:
	case GL_COLOR_CLEAR_VALUE:
	case GL_CURRENT_COLOR:
	case GL_CURRENT_RASTER_COLOR:
	case GL_FOG_COLOR:
	case GL_LIGHT_MODEL_AMBIENT:
	    length = 4;
	    break;
	    /* for the sake of glu helpers that use float32_t only */
	case GL_MODELVIEW_MATRIX:
	case GL_PROJECTION_MATRIX:
	    length = 16;
	    break;
	default:
	    /* FIXME add more valid entries with proper bounds checking */
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetFloatv(alist->a0, alist->a1->v.f32);
}

static void
native_GetIntegerv(oobject_t list, oint32_t ac)
/* void GetIntegerv(uint32_t pname, int32_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_int32))
	ovm_raise(except_invalid_argument);
    switch (alist->a0) {
	case GL_ACCUM_ALPHA_BITS:
	case GL_ACCUM_BLUE_BITS:
	case GL_ACCUM_GREEN_BITS:
	case GL_ACCUM_RED_BITS:
	case GL_ALPHA_BITS:
	case GL_ALPHA_TEST_FUNC:
	case GL_ATTRIB_STACK_DEPTH:
	case GL_BLEND_DST:
	case GL_BLEND_SRC:
	case GL_BLUE_BITS:
#if defined(GL_BUFFER_SWAP_MODE_HINT_HP)
	case GL_BUFFER_SWAP_MODE_HINT_HP:
#endif
	case GL_CLIENT_ATTRIB_STACK_DEPTH:
	case GL_COLOR_ARRAY_SIZE:	/* XXX should not be used */
	case GL_COLOR_ARRAY_STRIDE:	/* XXX should not be used */
	case GL_COLOR_ARRAY_TYPE:	/* XXX should not be used */
	case GL_COLOR_MATERIAL_FACE:
	case GL_COLOR_MATERIAL_PARAMETER:
	case GL_CURRENT_INDEX:
	case GL_CURRENT_RASTER_INDEX:
	case GL_DEPTH_BITS:
	case GL_DEPTH_FUNC:
	case GL_DRAW_BUFFER:
	case GL_FOG_HINT:
	case GL_FOG_INDEX:
	case GL_FOG_MODE:
	case GL_FRONT_FACE:
	case GL_GREEN_BITS:
	case GL_INDEX_ARRAY_STRIDE:	/* XXX should not be used */
	case GL_INDEX_ARRAY_TYPE:	/* XXX should not be used */
	case GL_INDEX_BITS:
	case GL_INDEX_CLEAR_VALUE:
	case GL_INDEX_OFFSET:
	case GL_INDEX_SHIFT:
#if defined(GL_INDEX_WRITE_MASK)
	case GL_INDEX_WRITE_MASK:
#endif
	case GL_LINE_SMOOTH_HINT:
	case GL_LINE_STIPPLE_PATTERN:	/* 16 bit pattern */
	case GL_LINE_STIPPLE_REPEAT:
	case GL_LIST_BASE:
	case GL_LIST_INDEX:
	case GL_LIST_MODE:
	case GL_MAP1_GRID_SEGMENTS:
	case GL_MATRIX_MODE:
	case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
	case GL_MAX_ATTRIB_STACK_DEPTH:
	case GL_MAX_CLIP_PLANES:
	case GL_MAX_EVAL_ORDER:		/* at least 8 */
	case GL_MAX_LIGHTS:		/* at least 8 */
	case GL_MAX_LIST_NESTING:	/* at least 64 */
	case GL_MAX_MODELVIEW_STACK_DEPTH:/* at least 32 */
	case GL_MAX_NAME_STACK_DEPTH:	/* at least 64 */
	case GL_MAX_PIXEL_MAP_TABLE:	/* at least 32 */
	case GL_MAX_PROJECTION_STACK_DEPTH:/* at least 2 */
	case GL_MAX_TEXTURE_SIZE:
	case GL_MAX_TEXTURE_STACK_DEPTH:/* at least 2 */
	case GL_MODELVIEW_STACK_DEPTH:
	case GL_NAME_STACK_DEPTH:
	case GL_NORMAL_ARRAY_STRIDE:	/* XXX should not be used */
	case GL_NORMAL_ARRAY_TYPE:	/* XXX should not be used */
	case GL_PACK_ALIGNMENT:
	case GL_PACK_ROW_LENGTH:
	case GL_PACK_SKIP_PIXELS:
	case GL_PACK_SKIP_ROWS:
	case GL_PERSPECTIVE_CORRECTION_HINT:
	case GL_PIXEL_MAP_A_TO_A_SIZE:
	case GL_PIXEL_MAP_B_TO_B_SIZE:
	case GL_PIXEL_MAP_G_TO_G_SIZE:
	case GL_PIXEL_MAP_I_TO_A_SIZE:
	case GL_PIXEL_MAP_I_TO_B_SIZE:
	case GL_PIXEL_MAP_I_TO_G_SIZE:
	case GL_PIXEL_MAP_I_TO_I_SIZE:
	case GL_PIXEL_MAP_I_TO_R_SIZE:
	case GL_PIXEL_MAP_R_TO_R_SIZE:
	case GL_PIXEL_MAP_S_TO_S_SIZE:
	case GL_POINT_SMOOTH_HINT:
	case GL_POLYGON_MODE:
	case GL_POLYGON_SMOOTH_HINT:
	case GL_PROJECTION_STACK_DEPTH:
	case GL_READ_BUFFER:
	case GL_RED_BITS:
	case GL_RENDER_MODE:
	case GL_SHADE_MODEL:
	case GL_STENCIL_BITS:
	case GL_STENCIL_CLEAR_VALUE:
	case GL_STENCIL_FAIL:
	case GL_STENCIL_FUNC:
	case GL_STENCIL_PASS_DEPTH_FAIL:
	case GL_STENCIL_PASS_DEPTH_PASS:
	case GL_STENCIL_REF:
	case GL_STENCIL_VALUE_MASK:
	case GL_STENCIL_WRITEMASK:
	case GL_SUBPIXEL_BITS:
#if defined(GL_TEXTURE_1D_BINDING)
	case GL_TEXTURE_1D_BINDING:
#endif
#if defined(GL_TEXTURE_2D_BINDING)
	case GL_TEXTURE_2D_BINDING:
#endif
	case GL_TEXTURE_COORD_ARRAY_SIZE:/* XXX should not be used */
	case GL_TEXTURE_COORD_ARRAY_STRIDE:/* XXX should not be used */
	case GL_TEXTURE_COORD_ARRAY_TYPE:/* XXX should not be used */
	case GL_TEXTURE_STACK_DEPTH:
	case GL_UNPACK_ALIGNMENT:
	case GL_UNPACK_ROW_LENGTH:
	case GL_UNPACK_SKIP_PIXELS:
	case GL_UNPACK_SKIP_ROWS:
	case GL_VERTEX_ARRAY_SIZE:	/* XXX should not be used */
	case GL_VERTEX_ARRAY_STRIDE:	/* XXX should not be used */
	case GL_VERTEX_ARRAY_TYPE:	/* XXX should not be used */

	case GL_MAX_TEXTURE_UNITS_ARB:

	    length = 1;
	    break;
	case GL_MAP2_GRID_SEGMENTS:
	case GL_MAX_VIEWPORT_DIMS:
	    length = 2;
	    break;
	case GL_VIEWPORT:
	case GL_SCISSOR_BOX:
	    length = 4;
	    break;
	default:
	    /* FIXME add more valid entries with proper bounds checking */
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetIntegerv(alist->a0, alist->a1->v.i32);
}

static void
native_PushAttrib(oobject_t list, oint32_t ac)
/* void PushAttrib(uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPushAttrib(alist->a0);
}

static void
native_PopAttrib(oobject_t list, oint32_t ac)
/* void PopAttrib(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopAttrib();
}

static void
native_PushClientAttrib(oobject_t list, oint32_t ac)
/* void PushClientAttrib(uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPushClientAttrib(alist->a0);
}

static void
native_PopClientAttrib(oobject_t list, oint32_t ac)
/* void PopClientAttrib(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopClientAttrib();
}

static void
native_RenderMode(oobject_t list, oint32_t ac)
/* int32_t RenderMode(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glRenderMode(alist->a0);
}

static void
native_GetError(oobject_t list, oint32_t ac)
/* uint32_t GetError(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glGetError();
}

static void
native_GetString(oobject_t list, oint32_t ac)
/* string_t GetString(uint32_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    const GLubyte			*str;
    ovector_t				*vec;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    if ((str = glGetString(alist->a0))) {
	onew_vector(&thread_self->obj, t_uint8, strlen((char *)str));
	vec = (ovector_t *)thread_self->obj;
	memcpy(vec->v.u8, str, vec->length);
	r0->v.o = vec;
	r0->t = t_string;
    }
    else
	r0->t = t_void;
}

static void
native_Finish(oobject_t list, oint32_t ac)
/* void Finish(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glFinish();
}

static void
native_Flush(oobject_t list, oint32_t ac)
/* void Flush(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glFlush();
}

static void
native_Hint(oobject_t list, oint32_t ac)
/* void Hint(uint32_t target, uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glHint(alist->a0, alist->a1);
}

static void
native_ClearDepth(oobject_t list, oint32_t ac)
/* void ClearDepth(float64_t depth); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_t				*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClearDepth(alist->a0);
}

static void
native_DepthFunc(oobject_t list, oint32_t ac)
/* void DepthFunc(uint32_t func); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDepthFunc(alist->a0);
}

static void
native_DepthMask(oobject_t list, oint32_t ac)
/* void DepthMask(uint8_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u8_t				*alist;

    alist = (nat_u8_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDepthMask(alist->a0);
}

static void
native_DepthRange(oobject_t list, oint32_t ac)
/* void DepthRange(float64_t near_val, float64_t far_val); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDepthRange(alist->a0, alist->a1);
}

static void
native_ClearAccum(oobject_t list, oint32_t ac)
/* void ClearAccum(float32_t r, float32_t g, float32_t b, float32_t a); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_f32_f32_t		*alist;

    alist = (nat_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClearAccum(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Accum(oobject_t list, oint32_t ac)
/* void Accum(uint32_t op, float32_t value); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f32_t			*alist;

    alist = (nat_u32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glAccum(alist->a0, alist->a1);
}


static void
native_MatrixMode(oobject_t list, oint32_t ac)
/* void MatrixMode(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMatrixMode(alist->a0);
}

static void
native_Ortho(oobject_t list, oint32_t ac)
/* void Ortho(float64_t left, float64_t right,
	      float64_t bottom, float64_t top,
	      float64_t near_val, float64_t far_val); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_f64_f64_t	*alist;

    alist = (nat_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glOrtho(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4, alist->a5);
}

static void
native_Frustum(oobject_t list, oint32_t ac)
/* void Frustum(float64_t left, float64_t right,
		float64_t bottom, float64_t top,
		float64_t near_val, float64_t far_val); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_f64_f64_t	*alist;

    alist = (nat_f64_f64_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glFrustum(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4, alist->a5);
}

static void
native_Viewport(oobject_t list, oint32_t ac)
/* void Viewport(int32_t, int32_t y, int32_t width, int32_t height); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_t		*alist;

    alist = (nat_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glViewport(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_PushMatrix(oobject_t list, oint32_t ac)
/* void PushMatrix(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPushMatrix();
}

static void
native_PopMatrix(oobject_t list, oint32_t ac)
/* void PopMatrix(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopMatrix();
}

static void
native_LoadIdentity(oobject_t list, oint32_t ac)
/* void LoadIdentity(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glLoadIdentity();
}

static void
native_LoadMatrix(oobject_t list, oint32_t ac)
/* void LoadMatrix(float64_t m[16]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 16))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glLoadMatrixd(alist->a0->v.f64);
}

static void
native_MultMatrix(oobject_t list, oint32_t ac)
/* void MultMatrix(float64_t m[16]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 16))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMultMatrixd(alist->a0->v.f64);
}

static void
native_Rotate(oobject_t list, oint32_t ac)
/* void Rotate(float64_t angle, float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glRotated(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Scale(oobject_t list, oint32_t ac)
/* void Scale(float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glScaled(alist->a0, alist->a1, alist->a2);
}

static void
native_Translate(oobject_t list, oint32_t ac)
/* void Translate(float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTranslated(alist->a0, alist->a1, alist->a2);
}

static void
native_IsList(oobject_t list, oint32_t ac)
/* uint8_t IsList(uint32_t list); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsList(alist->a0);
}

static void
native_DeleteLists(oobject_t list, oint32_t ac)
/* void DeleteLists(uint32_t list, int32_t range); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_t			*alist;

    alist = (nat_u32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDeleteLists(alist->a0, alist->a1);
}

static void
native_GenLists(oobject_t list, oint32_t ac)
/* uint32_t GenLists(int32_t range); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glGenLists(alist->a0);
}

static void
native_NewList(oobject_t list, oint32_t ac)
/* void NewList(uint32_t list, uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glNewList(alist->a0, alist->a1);
}

static void
native_EndList(oobject_t list, oint32_t ac)
/* void EndList(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glEndList();
}

static void
native_CallList(oobject_t list, oint32_t ac)
/* void CallList(uint32_t list); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCallList(alist->a0);
}

static void
native_CallLists(oobject_t list, oint32_t ac)
/* void CallLists(uint32_t lists[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint32))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glCallLists(alist->a0->length, GL_UNSIGNED_INT, alist->a0->v.u32);
}

static void
native_ListBase(oobject_t list, oint32_t ac)
/* void ListBase(uint32_t base); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glListBase(alist->a0);
}

static void
native_Begin(oobject_t list, oint32_t ac)
/* void Begin(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glBegin(alist->a0);
}

static void
native_End(oobject_t list, oint32_t ac)
/* void End(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glEnd();
}

static void
native_Vertex2(oobject_t list, oint32_t ac)
/* void Vertex2(float64_t x, float64_t y); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertex2d(alist->a0, alist->a1);
}

static void
native_Vertex3(oobject_t list, oint32_t ac)
/* void Vertex3(float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertex3d(alist->a0, alist->a1, alist->a2);
}

static void
native_Vertex4(oobject_t list, oint32_t ac)
/* void Vertex4(float64_t x, float64_t y, float64_t z, float64_t w); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertex4d(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Vertex2v(oobject_t list, oint32_t ac)
/* void Vertex2v(float64_t v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 2))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glVertex2dv(alist->a0->v.f64);
}

static void
native_Vertex3v(oobject_t list, oint32_t ac)
/* void Vertex3v(float64_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glVertex3dv(alist->a0->v.f64);
}

static void
native_Vertex4v(oobject_t list, oint32_t ac)
/* void Vertex4v(float64_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glVertex4dv(alist->a0->v.f64);
}

static void
native_Normal3(oobject_t list, oint32_t ac)
/* void Normal3(float64_t nx, float64_t ny, float64_t nz); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glNormal3d(alist->a0, alist->a1, alist->a2);
}

static void
native_Normal3v(oobject_t list, oint32_t ac)
/* void Normal3v(float64_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glNormal3dv(alist->a0->v.f64);
}

static void
native_Index(oobject_t list, oint32_t ac)
/* void Index(int32_t c); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glIndexi(alist->a0);
}

static void
native_Color3(oobject_t list, oint32_t ac)
/* void Color3(float32_t r, float32_t g, float32_t b); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_f32_t			*alist;

    alist = (nat_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glColor3f(alist->a0, alist->a1, alist->a2);
}

static void
native_Color4(oobject_t list, oint32_t ac)
/* void Color4d(float32_t r, float32_t g, float32_t b, float32_t a); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_f32_f32_t		*alist;

    alist = (nat_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glColor4f(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Color3v(oobject_t list, oint32_t ac)
/* void Color3v(float32_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float32, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glColor3fv(alist->a0->v.f32);
}

static void
native_Color4v(oobject_t list, oint32_t ac)
/* void Color4v(float32_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float32, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glColor4fv(alist->a0->v.f32);
}

static void
native_TexCoord1(oobject_t list, oint32_t ac)
/* void TexCoord1(float64_t s); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_t				*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTexCoord1d(alist->a0);
}

static void
native_TexCoord2(oobject_t list, oint32_t ac)
/* void TexCoord2(float64_t s, float64_t t); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTexCoord2d(alist->a0, alist->a1);
}

static void
native_TexCoord3(oobject_t list, oint32_t ac)
/* void TexCoord3(float64_t s, float64_t t, float64_t r); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTexCoord3d(alist->a0, alist->a1, alist->a2);
}

static void
native_TexCoord4(oobject_t list, oint32_t ac)
/* void TexCoord4(float64_t s, float64_t t, float64_t r, float64_t q); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTexCoord4d(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_TexCoord2v(oobject_t list, oint32_t ac)
/* void TexCoord2v(float64_t v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 2))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexCoord2dv(alist->a0->v.f64);
}

static void
native_TexCoord3v(oobject_t list, oint32_t ac)
/* void TexCoord3v(float64_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexCoord3dv(alist->a0->v.f64);
}

static void
native_TexCoord4v(oobject_t list, oint32_t ac)
/* void TexCoord4v(float64_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexCoord4dv(alist->a0->v.f64);
}

static void
native_RasterPos2(oobject_t list, oint32_t ac)
/* void RasterPos2(float64_t x, float64_t y); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glRasterPos2d(alist->a0, alist->a1);
}

static void
native_RasterPos3(oobject_t list, oint32_t ac)
/* void RasterPos3(float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glRasterPos3d(alist->a0, alist->a1, alist->a2);
}

static void
native_RasterPos4(oobject_t list, oint32_t ac)
/* void RasterPos3(float64_t x, float64_t y, float64_t z, float64_t w); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glRasterPos4d(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_RasterPos2v(oobject_t list, oint32_t ac)
/* void RasterPos2v(float64_t v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 2))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glRasterPos2dv(alist->a0->v.f64);
}

static void
native_RasterPos3v(oobject_t list, oint32_t ac)
/* void RasterPos3v(float64_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glRasterPos3dv(alist->a0->v.f64);
}

static void
native_RasterPos4v(oobject_t list, oint32_t ac)
/* void RasterPos4v(float64_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glRasterPos4dv(alist->a0->v.f64);
}


static void
native_Rect(oobject_t list, oint32_t ac)
/* void Rect(float64_t x1, float64_t y1, float64_t x2, float64_t y2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_f64_t		*alist;

    alist = (nat_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glRectd(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Rectv(oobject_t list, oint32_t ac)
/* void Rectv(float64_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a0, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glRectdv(alist->a0->v.f64, alist->a0->v.f64 + 2);
}

static void
native_ShadeModel(oobject_t list, oint32_t ac)
/* void ShadeModel(uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glShadeModel(alist->a0);
}

static void
native_Light(oobject_t list, oint32_t ac)
/* void Light(uint32_t light, uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_f32_t			*alist;

    alist = (nat_u32_u32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLightf(alist->a0, alist->a1, alist->a2);
}

static void
native_Lightv(oobject_t list, oint32_t ac)
/* void Lightv(uint32_t light, uint32_t pname, float32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
	    length = 4;
	    break;
	case GL_SPOT_DIRECTION:
	    length = 3;
	    break;
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
	    length = 1;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a2, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glLightfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_GetLightv(oobject_t list, oint32_t ac)
/* void GetLightv(uint32_t light, uint32_t pname, float32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
	    length = 4;
	    break;
	case GL_SPOT_DIRECTION:
	    length = 3;
	    break;
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
	    length = 1;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a2, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    if (alist->a2->length != length)
	orenew_vector(alist->a2, length);
    r0->t = t_void;
    glGetLightfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_LightModel(oobject_t list, oint32_t ac)
/* void LightModel(uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f32_t			*alist;

    alist = (nat_u32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLightModelf(alist->a0, alist->a1);
}

static void
native_LightModelv(oobject_t list, oint32_t ac)
/* void LightModelv(uint32_t pname, float32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_LIGHT_MODEL_AMBIENT:
	    length = 4;
	    break;
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
	case GL_LIGHT_MODEL_TWO_SIDE:
	    length = 1;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a1, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glLightModelfv(alist->a0, alist->a1->v.f32);
}

static void
native_Material(oobject_t list, oint32_t ac)
/* void Material(uint32_t face, uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_f32_t			*alist;

    alist = (nat_u32_u32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMaterialf(alist->a0, alist->a1, alist->a2);
}

static void
native_Materialv(oobject_t list, oint32_t ac)
/* void Materialv(uint32_t face, uint32_t pname, float32_t paramm[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_EMISSION:
	case GL_AMBIENT_AND_DIFFUSE:
	    length = 4;
	    break;
	case GL_SHININESS:
	    length = 1;
	    break;
	case GL_COLOR_INDEXES:
	    length = 3;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a2, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMaterialfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_GetMaterialv(oobject_t list, oint32_t ac)
/* void GetMaterialv(uint32_t face, uint32_t pname, float32_t paramm[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_EMISSION:
	case GL_AMBIENT_AND_DIFFUSE:
	    length = 4;
	    break;
	case GL_SHININESS:
	    length = 1;
	    break;
	case GL_COLOR_INDEXES:
	    length = 3;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a2, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    if (alist->a2->length != length)
	orenew_vector(alist->a2, length);
    r0->t = t_void;
    glGetMaterialfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_ColorMaterial(oobject_t list, oint32_t ac)
/* void ColorMaterial(uint32_t face, uint32_t mode); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glColorMaterial(alist->a0, alist->a1);
}

static void
native_PixelZoom(oobject_t list, oint32_t ac)
/* void PixelZoom(float32_t xfactor, float32_t yfactor); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_f32_t			*alist;

    alist = (nat_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPixelZoom(alist->a0, alist->a1);
}

static void
native_PixelStore(oobject_t list, oint32_t ac)
/* void PixelStore(uint32_t pname, int32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_t			*alist;

    alist = (nat_u32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPixelStorei(alist->a0, alist->a1);
}

static void
native_PixelTransfer(oobject_t list, oint32_t ac)
/* void PixelTransfer(uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f32_t			*alist;

    alist = (nat_u32_f32_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	    /* boolean */
	case GL_MAP_COLOR:
	case GL_MAP_STENCIL:
	    /* integer */
	case GL_INDEX_SHIFT:
	case GL_INDEX_OFFSET:
	    /* float */
	case GL_RED_SCALE:
	case GL_GREEN_SCALE:
	case GL_BLUE_SCALE:
	case GL_ALPHA_SCALE:
	case GL_DEPTH_SCALE:
	case GL_RED_BIAS:
	case GL_GREEN_BIAS:
	case GL_BLUE_BIAS:
	case GL_ALPHA_BIAS:
	case GL_DEPTH_BIAS:
	case GL_POST_COLOR_MATRIX_RED_SCALE:
	case GL_POST_COLOR_MATRIX_GREEN_SCALE:
	case GL_POST_COLOR_MATRIX_BLUE_SCALE:
	case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
	case GL_POST_COLOR_MATRIX_RED_BIAS:
	case GL_POST_COLOR_MATRIX_GREEN_BIAS:
	case GL_POST_COLOR_MATRIX_BLUE_BIAS:
	case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
	case GL_POST_CONVOLUTION_RED_SCALE:
	case GL_POST_CONVOLUTION_GREEN_SCALE:
	case GL_POST_CONVOLUTION_BLUE_SCALE:
	case GL_POST_CONVOLUTION_ALPHA_SCALE:
	case GL_POST_CONVOLUTION_RED_BIAS:
	case GL_POST_CONVOLUTION_GREEN_BIAS:
	case GL_POST_CONVOLUTION_BLUE_BIAS:
	case GL_POST_CONVOLUTION_ALPHA_BIAS:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glPixelTransferf(alist->a0, alist->a1);
}

static void
native_PixelMapv(oobject_t list, oint32_t ac)
/* void PixelMapfv(uint32_t map, float32_t values[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a1, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    switch (alist->a0) {
	    /* unsigned integer */
	case GL_PIXEL_MAP_I_TO_I:
	case GL_PIXEL_MAP_S_TO_S:
	case GL_PIXEL_MAP_I_TO_R:
	case GL_PIXEL_MAP_I_TO_G:
	case GL_PIXEL_MAP_I_TO_B:
	case GL_PIXEL_MAP_I_TO_A:
	    /* float */
	case GL_PIXEL_MAP_R_TO_R:
	case GL_PIXEL_MAP_G_TO_G:
	case GL_PIXEL_MAP_B_TO_B:
	case GL_PIXEL_MAP_A_TO_A:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glPixelMapfv(alist->a0, alist->a1->length, alist->a1->v.f32);
}

static void
native_GetPixelMapv(oobject_t list, oint32_t ac)
/* void GetPixelMapv(uint32_t map, float32_t values[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    int					 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    glGetIntegerv(alist->a0, &length);
    if (bad_arg_type(a1, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    switch (alist->a0) {
	    /* unsigned integer */
	case GL_PIXEL_MAP_I_TO_I:
	case GL_PIXEL_MAP_S_TO_S:
	case GL_PIXEL_MAP_I_TO_R:
	case GL_PIXEL_MAP_I_TO_G:
	case GL_PIXEL_MAP_I_TO_B:
	case GL_PIXEL_MAP_I_TO_A:
	    /* float */
	case GL_PIXEL_MAP_R_TO_R:
	case GL_PIXEL_MAP_G_TO_G:
	case GL_PIXEL_MAP_B_TO_B:
	case GL_PIXEL_MAP_A_TO_A:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glGetPixelMapfv(alist->a0, alist->a1->v.f32);
}

static void
native_Bitmap(oobject_t list, oint32_t ac)
/* void Bitmap(int32_t width, int32_t height,
	       float32_t xorig, float32_t yorig,
	       float32_t xmove, float32_t ymove,
	       uint8_t bitmap[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_f32_f32_f32_f32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_i32_i32_f32_f32_f32_f32_vec_t *)list;
    r0 = &thread_self->r0;
    length = (alist->a0 + 7) & ~7;
    check_mult(length, alist->a1);
    length = (length * alist->a1) >> 3;
    if (bad_arg_type(a6, t_vector|t_uint8) || alist->a6->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glBitmap(alist->a0, alist->a1, alist->a2, alist->a3,
	     alist->a4, alist->a5, alist->a6->v.u8);
}

static void
native_ReadPixels(oobject_t list, oint32_t ac)
/* void ReadPixels(int32_t x, int32_t y, int32_t width, int32_t height,
		   uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a5, t_vector|t_uint8))
	ovm_raise(except_invalid_argument);
    check_mult(alist->a2, alist->a3);
    length = alist->a2 * alist->a3;
    switch (alist->a4) {
	case GL_RGB:
	case GL_BGR:
	    check_mult(length, 3);
	    length *= 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(length, 4);
	    length *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a5->length != length)
	orenew_vector(alist->a5, length);
    r0->t = t_void;
    glReadPixels(alist->a0, alist->a1, alist->a2, alist->a3,
		 alist->a4, GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_ReadBitmap(oobject_t list, oint32_t ac)
/* void ReadBitmap(int32_t x, int32_t y, int32_t width, int32_t height,
		   uint32_t format, uint8_t bitmap[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a5, t_vector|t_uint8))
	ovm_raise(except_invalid_argument);
    length = (alist->a2 + 7) & ~7;
    check_mult(length, alist->a3);
    length = (length * alist->a3) >> 3;
    if (alist->a5->length != length)
	orenew_vector(alist->a5, length);
    r0->t = t_void;
    glReadPixels(alist->a0, alist->a1, alist->a2, alist->a3,
		 alist->a4, GL_BITMAP, alist->a5->v.u8);
}

static void
native_DrawPixels(oobject_t list, oint32_t ac)
/* void DrawPixels(int32_t width, int32_t height,
		   uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_u32_vec_t		*alist;
    oword_t				 length;

    alist = (nat_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    check_mult(alist->a0, alist->a1);
    length = alist->a0 * alist->a1;
    switch (alist->a2) {
	case GL_RGB:
	case GL_BGR:
	    check_mult(length, 3);
	    length *= 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(length, 4);
	    length *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a3, t_vector|t_uint8) || alist->a3->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glDrawPixels(alist->a0, alist->a1, alist->a2, GL_UNSIGNED_BYTE,
		 alist->a3->v.u8);
}

static void
native_DrawBitmap(oobject_t list, oint32_t ac)
/* void DrawBitmap(int32_t width, int32_t height,
		   uint32_t format, uint8_t bitmap[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_u32_vec_t		*alist;
    oword_t				 length;

    alist = (nat_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;

    length = (alist->a0 + 7) & ~7;
    check_mult(length, alist->a1);
    length = (length * alist->a1) >> 3;

    if (bad_arg_type(a3, t_vector|t_uint8) || alist->a3->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glDrawPixels(alist->a0, alist->a1, alist->a2, GL_BITMAP, alist->a3->v.u8);
}

static void
native_CopyPixels(oobject_t list, oint32_t ac)
/* void CopyPixels(int32_t x, int32_t y,
		   int32_t width, int32_t height, uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_t		*alist;

    alist = (nat_i32_i32_i32_i32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCopyPixels(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}


static void
native_StencilFunc(oobject_t list, oint32_t ac)
/* void StencilFunc(uint32_t func, int32_t ref, uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_t			*alist;

    alist = (nat_u32_i32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilFunc(alist->a0, alist->a1, alist->a2);
}

static void
native_StencilMask(oobject_t list, oint32_t ac)
/* void StencilMask(uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilMask(alist->a0);
}

static void
native_StencilOp(oobject_t list, oint32_t ac)
/* void StencilOp(uint32_t fail uint32_t zfail, uint32_t zpass); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_u32_t			*alist;

    alist = (nat_u32_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilOp(alist->a0, alist->a1, alist->a2);
}

static void
native_ClearStencil(oobject_t list, oint32_t ac)
/* void ClearStencil(int32_t s); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClearStencil(alist->a0);
}

static void
native_TexGen(oobject_t list, oint32_t ac)
/* void TexGen(uint32_t coord, uint32_t pname, float64_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_f64_t			*alist;

    alist = (nat_u32_u32_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glTexGend(alist->a0, alist->a1, alist->a2);
}

static void
native_TexGenv(oobject_t list, oint32_t ac)
/* void TexGenv(uint32_t coord, uint32_t pname, float64_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	case GL_TEXTURE_GEN_MODE:
	    length = 1;
	    break;
	case GL_OBJECT_PLANE:
	case GL_EYE_PLANE:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a2, t_vector|t_float64, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexGendv(alist->a0, alist->a1, alist->a2->v.f64);
}

static void
native_GetTexGenv(oobject_t list, oint32_t ac)
/* void GetTexGenv(uint32_t coord, uint32_t pname, float64_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a2, t_vector|t_float64))
	ovm_raise(except_invalid_argument);
    switch (alist->a1) {
	case GL_TEXTURE_GEN_MODE:
	    length = 1;
	    break;
	case GL_OBJECT_PLANE:
	case GL_EYE_PLANE:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a2->length != length)
	orenew_vector(alist->a2, length);
    r0->t = t_void;
    glGetTexGendv(alist->a0, alist->a1, alist->a2->v.f64);
}

static void
native_TexEnv(oobject_t list, oint32_t ac)
/* void TexEnv(uint32_t target, uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_f32_t			*alist;

    alist = (nat_u32_u32_f32_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_ENV_MODE:
	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_SRC0_RGB:
	case GL_SRC1_RGB:
	case GL_SRC2_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_ALPHA:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND1_RGB:
	case GL_OPERAND2_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_ALPHA:
	case GL_COORD_REPLACE:
	    /* float */
	case GL_TEXTURE_LOD_BIAS:
	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glTexEnvf(alist->a0, alist->a1, alist->a2);
}

static void
native_TexEnvv(oobject_t list, oint32_t ac)
/* void TexEnvv(uint32_t target, uint32_t pname, float32_t param[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_ENV_MODE:
	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_SRC0_RGB:
	case GL_SRC1_RGB:
	case GL_SRC2_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_ALPHA:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND1_RGB:
	case GL_OPERAND2_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_ALPHA:
	case GL_COORD_REPLACE:
	    /* float */
	case GL_TEXTURE_LOD_BIAS:
	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
	    length = 1;
	    break;
	    /* float[4] */
	case GL_TEXTURE_ENV_COLOR:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a2, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexEnvfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_GetTexEnvv(oobject_t list, oint32_t ac)
/* void GetTexEnvv(uint32_t target, uint32_t pname, float32_t param[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a2, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_ENV_MODE:
	case GL_COMBINE_RGB:
	case GL_COMBINE_ALPHA:
	case GL_SRC0_RGB:
	case GL_SRC1_RGB:
	case GL_SRC2_RGB:
	case GL_SRC0_ALPHA:
	case GL_SRC1_ALPHA:
	case GL_SRC2_ALPHA:
	case GL_OPERAND0_RGB:
	case GL_OPERAND1_RGB:
	case GL_OPERAND2_RGB:
	case GL_OPERAND0_ALPHA:
	case GL_OPERAND1_ALPHA:
	case GL_OPERAND2_ALPHA:
	case GL_COORD_REPLACE:
	    /* float */
	case GL_TEXTURE_LOD_BIAS:
	case GL_RGB_SCALE:
	case GL_ALPHA_SCALE:
	    length = 1;
	    break;
	    /* float[4] */
	case GL_TEXTURE_ENV_COLOR:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (alist->a2->length != length)
	orenew_vector(alist->a2, length);
    r0->t = t_void;
    glGetTexEnvfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_TexParameter(oobject_t list, oint32_t ac)
/* void TexParameter(uint32_t target, uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_f32_t			*alist;

    alist = (nat_u32_u32_f32_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_MIN_FILTER:
	case GL_TEXTURE_MAG_FILTER:
	case GL_TEXTURE_BASE_LEVEL:
	case GL_TEXTURE_MAX_LEVEL:
	case GL_TEXTURE_WRAP_S:
	case GL_TEXTURE_WRAP_T:
	case GL_TEXTURE_WRAP_R:
	case GL_TEXTURE_COMPARE_MODE:
	case GL_TEXTURE_COMPARE_FUNC:
	case GL_DEPTH_TEXTURE_MODE:
	case GL_GENERATE_MIPMAP:
	    /* float */
	case GL_TEXTURE_MIN_LOD:
	case GL_TEXTURE_MAX_LOD:
	case GL_TEXTURE_PRIORITY:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glTexParameterf(alist->a0, alist->a1, alist->a2);
}

static void
native_TexParameterv(oobject_t list, oint32_t ac)
/* void TexParameterv(uint32_t target, uint32_t pname, float32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_MIN_FILTER:
	case GL_TEXTURE_MAG_FILTER:
	case GL_TEXTURE_BASE_LEVEL:
	case GL_TEXTURE_MAX_LEVEL:
	case GL_TEXTURE_WRAP_S:
	case GL_TEXTURE_WRAP_T:
	case GL_TEXTURE_WRAP_R:
	case GL_TEXTURE_COMPARE_MODE:
	case GL_TEXTURE_COMPARE_FUNC:
	case GL_DEPTH_TEXTURE_MODE:
	case GL_GENERATE_MIPMAP:
	    /* float */
	case GL_TEXTURE_MIN_LOD:
	case GL_TEXTURE_MAX_LOD:
	case GL_TEXTURE_PRIORITY:
	    length = 1;
	    break;
	    /* float[4]*/
	case GL_TEXTURE_BORDER_COLOR:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type_length(a2, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexParameterfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_GetTexParameterv(oobject_t list, oint32_t ac)
/* void GetTexParameterv(uint32_t target, uint32_t pname,
			 float32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a1) {
	    /* integer */
	case GL_TEXTURE_MIN_FILTER:
	case GL_TEXTURE_MAG_FILTER:
	case GL_TEXTURE_BASE_LEVEL:
	case GL_TEXTURE_MAX_LEVEL:
	case GL_TEXTURE_WRAP_S:
	case GL_TEXTURE_WRAP_T:
	case GL_TEXTURE_WRAP_R:
	case GL_TEXTURE_COMPARE_MODE:
	case GL_TEXTURE_COMPARE_FUNC:
	case GL_DEPTH_TEXTURE_MODE:
	case GL_GENERATE_MIPMAP:
	    /* float */
	case GL_TEXTURE_MIN_LOD:
	case GL_TEXTURE_MAX_LOD:
	case GL_TEXTURE_PRIORITY:
	    length = 1;
	    break;
	    /* float[4] */
	case GL_TEXTURE_BORDER_COLOR:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    if (bad_arg_type_length(a2, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    if (alist->a2->length != length)
	orenew_vector(alist->a2, length);
    glGetTexParameterfv(alist->a0, alist->a1, alist->a2->v.f32);
}

static void
native_GetTexLevelParameter(oobject_t list, oint32_t ac)
/* int32_t GetTexLevelParameter(uint32_t target,
				int32_t level, uint32_t pname); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_t			*alist;
    GLint				 value[4];

    alist = (nat_u32_i32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    glGetTexLevelParameteriv(alist->a0, alist->a1, alist->a2, value);
    r0->v.w = value[0];
}

static void
native_TexImage1D(oobject_t list, oint32_t ac)
/* void TexImage1D(uint32_t target, int32_t level,
		   int32_t width, int32_t border, uint32_t format,
		   uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;

    /* width must be !2^n - (border * 2)" for an integer "n" */
    length = alist->a2 /* width */ - (alist->a3 /* border */ << 1);
    if (length & (length - 1))
	ovm_raise(except_invalid_argument);
    switch (alist->a4 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
	case GL_DEPTH_COMPONENT:
	    length = alist->a2;		/* width */
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a2, 3);
	    length = alist->a2 * 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a2, 4);
	    length = alist->a2 * 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    if (bad_arg_type_length(a5, t_vector|t_uint8, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexImage1D(alist->a0, alist->a1, alist->a4, alist->a2, alist->a3,
		 alist->a4, GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_TexImage2D(oobject_t list, oint32_t ac)
/* void TexImage2D(uint32_t target, int32_t level,
		   int32_t width, int32_t height, int32_t border,
		   uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    /* width and height must be !2^n - (border * 2)" for an integer "n" */
    length = alist->a2 /* width */ - (alist->a4 /* border */ << 1);
    if (length & (length - 1))
	ovm_raise(except_invalid_argument);
    /* width and height must be !2^n - (border * 2)" for an integer "n" */
    length = alist->a3 /* height */ - (alist->a4 /* border */ << 1);
    if (length & (length - 1))
	ovm_raise(except_invalid_argument);
    switch (alist->a5 /* format */) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
	case GL_DEPTH_COMPONENT:
	    check_mult(alist->a2, alist->a3);
	    length = alist->a2 * alist->a3;
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(alist->a2, alist->a3);
	    length = alist->a2 * alist->a3;
	    check_mult(length, 3);
	    length *= 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(alist->a2, alist->a3);
	    length = alist->a2 * alist->a3;
	    check_mult(length, 4);
	    length *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    if (bad_arg_type_length(a6, t_vector|t_uint8, length))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexImage2D(alist->a0, alist->a1, alist->a5, alist->a2, alist->a3,
		 alist->a4, alist->a5, GL_UNSIGNED_BYTE, alist->a6->v.u8);
}

static void
native_GetTexImage(oobject_t list, oint32_t ac)
/* void GetTexImage(uint32_t target, int32_t level, uint32_t format,
		    uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_vec_t		*alist;
    oword_t				 mult;
    oword_t				 width;
    oword_t				 height;
    oword_t				 length;
    GLint				 value[4];

    alist = (nat_u32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    height = mult = 1;
    switch (alist->a0) {
	case GL_TEXTURE_1D:
	    glGetTexLevelParameteriv(GL_TEXTURE_1D, alist->a1,
				     GL_TEXTURE_WIDTH, value);
	    width = value[0];
	    break;
	case GL_TEXTURE_2D:
	    glGetTexLevelParameteriv(GL_TEXTURE_2D, alist->a1,
				     GL_TEXTURE_WIDTH, value);
	    width = value[0];
	    glGetTexLevelParameteriv(GL_TEXTURE_2D, alist->a1,
				     GL_TEXTURE_HEIGHT, value);
	    height = value[1];
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    switch (alist->a2) {
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
	    break;
	case GL_RGB:
	case GL_BGR:
	    mult = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    mult = 4;
	    break;
    }
    check_mult(width, height);
    length = width * height;
    check_mult(length, mult);
    length *= mult;
    if (bad_arg_type(a3, t_vector|t_uint8))
	ovm_raise(except_invalid_argument);
    if (alist->a3->length != length)
	orenew_vector(alist->a3, length);
    r0->t = t_void;
    glGetTexImage(alist->a0, alist->a1, alist->a2, GL_UNSIGNED_BYTE,
		  alist->a3->v.u8);
}

static void
native_GenTextures(oobject_t list, oint32_t ac)
/* void GenTextures(uint32_t textures[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint32))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glGenTextures(alist->a0->length, alist->a0->v.u32);
}

static void
native_DeleteTextures(oobject_t list, oint32_t ac)
/* void DeleteTextures(uint32_t textures[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint32))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glDeleteTextures(alist->a0->length, alist->a0->v.u32);
}

static void
native_BindTexture(oobject_t list, oint32_t ac)
/* void BindTexture(uint32_t target, uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glBindTexture(alist->a0, alist->a1);
}

static void
native_PrioritizeTextures(oobject_t list, oint32_t ac)
/* void PrioritizeTextures(uint32_t textures[], float32_t priorities[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_vec_t			*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint32) ||
	bad_arg_type(a0, t_vector|t_float32) ||
	alist->a0->length != alist->a1->length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glPrioritizeTextures(alist->a0->length, alist->a0->v.u32, alist->a1->v.f32);
}

static void
native_AreTexturesResident(oobject_t list, oint32_t ac)
/* void AreTexturesResident(uint32_t textures[], uint8_t residences[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_vec_t			*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type(a0, t_vector|t_uint32) ||
	bad_arg_type(a1, t_vector|t_uint8) ||
	alist->a0->length != alist->a1->length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glAreTexturesResident(alist->a0->length, alist->a0->v.u32, alist->a1->v.u8);
}

static void
native_IsTexture(oobject_t list, oint32_t ac)
/* uint8_t IsTexture(uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsTexture(alist->a0);
}

static void
native_TexSubImage1D(oobject_t list, oint32_t ac)
/* void TexSubImage1D(uint32_t target, int32_t level, int32_t xoffset,
		      int32_t width, uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    length = alist->a3;
    switch (alist->a4) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
	case GL_DEPTH_COMPONENT:
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(length, 3);
	    length *= 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(length, 4);
	    length *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a5, t_vector|t_uint8) || alist->a5->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexSubImage1D(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4,
		    GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_TexSubImage2D(oobject_t list, oint32_t ac)
/* void TexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
		      int32_t yoffset, int32_t width, int32_t height,
		      uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_i32_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    check_mult(alist->a3, alist->a4);
    length = alist->a3 * alist->a4;
    switch (alist->a6) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
	case GL_DEPTH_COMPONENT:
	    break;
	case GL_RGB:
	case GL_BGR:
	    check_mult(length, 3);
	    length *= 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    check_mult(length, 4);
	    length *= 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a7, t_vector|t_uint8) || alist->a7->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glTexSubImage2D(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4,
		    alist->a5, alist->a6, GL_UNSIGNED_BYTE, alist->a7->v.u8);
}

static void
native_CopyTexImage1D(oobject_t list, oint32_t ac)
/* void CopyTexImage1D(uint32_t target, int32_t level, int32_t format,
		       int32_t x, int32_t y, int32_t width, int32_t border); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_i32_i32_i32_i32_t	*alist;

    alist = (nat_u32_i32_u32_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCopyTexImage1D(alist->a0, alist->a1, alist->a2, alist->a3,
		     alist->a4, alist->a5, alist->a6);
}

static void
native_CopyTexImage2D(oobject_t list, oint32_t ac)
/* void CopyTexImage2D(uint32_t target, int32_t level, int32_t format,
		       int32_t x, int32_t y, int32_t width, int32_t height,
		       int32_t border); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_i32_i32_i32_i32_i32_t	*alist;

    alist = (nat_u32_i32_u32_i32_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCopyTexImage2D(alist->a0, alist->a1, alist->a2, alist->a3,
		     alist->a4, alist->a5, alist->a6, alist->a7);
}

static void
native_CopyTexSubImage1D(oobject_t list, oint32_t ac)
/* void CopyTexSubImage1D(uint32_t target, int32_t level, int32_t xoffset,
			  int32_t x, int32_t y, int32_t width); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_i32_t	*alist;

    alist = (nat_u32_i32_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCopyTexSubImage1D(alist->a0, alist->a1, alist->a2, alist->a3,
			alist->a4, alist->a5);
}

static void
native_CopyTexSubImage2D(oobject_t list, oint32_t ac)
/* void CopyTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
		          int32_t yoffset, int32_t x, int32_t y,
			  int32_t width, int32_t height); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_i32_i32_i32_t	*alist;

    alist = (nat_u32_i32_i32_i32_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCopyTexSubImage2D(alist->a0, alist->a1, alist->a2, alist->a3,
			alist->a4, alist->a5, alist->a6, alist->a7);
}

static void
native_Map1(oobject_t list, oint32_t ac)
/* void Map1(uint32_t target, float64_t u1, float64_t u2,
	     int32_t order, float64_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 mult;
    nat_u32_f64_f64_i32_vec_t		*alist;
    oword_t				 length;

    alist = (nat_u32_f64_f64_i32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_MAP1_INDEX:
	case GL_MAP1_TEXTURE_COORD_1:
	    mult = 1;
	    break;
	case GL_MAP1_TEXTURE_COORD_2:
	    mult = 2;
	    break;
	case GL_MAP1_VERTEX_3:
	case GL_MAP1_NORMAL:
	case GL_MAP1_TEXTURE_COORD_3:
	    mult = 3;
	    break;
	case GL_MAP1_VERTEX_4:
	case GL_MAP1_COLOR_4:
	case GL_MAP1_TEXTURE_COORD_4:
	    mult = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    check_mult(alist->a3, mult);
    length = alist->a3 * mult;
    if (bad_arg_type(a4, t_vector|t_float64) || alist->a4->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMap1d(alist->a0, alist->a1, alist->a2, 0, alist->a3, alist->a4->v.f64);
}

static void
native_Map2(oobject_t list, oint32_t ac)
/* void Map2(uint32_t target,
	     float64_t u1, float64_t u2, int32_t uorder,
	     float64_t v1, float64_t v2, int32_t vorder,
	     float64_t points[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 mult;
    nat_u32_f64_f64_i32_f64_f64_i32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_f64_f64_i32_f64_f64_i32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_MAP2_INDEX:
	case GL_MAP2_TEXTURE_COORD_1:
	    mult = 1;
	    break;
	case GL_MAP2_TEXTURE_COORD_2:
	    mult = 2;
	    break;
	case GL_MAP2_VERTEX_3:
	case GL_MAP2_NORMAL:
	case GL_MAP2_TEXTURE_COORD_3:
	    mult = 3;
	    break;
	case GL_MAP2_VERTEX_4:
	case GL_MAP2_COLOR_4:
	case GL_MAP2_TEXTURE_COORD_4:
	    mult = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    check_mult(alist->a3, alist->a6);
    length = alist->a3 * alist->a6;
    check_mult(length, mult);
    length *= mult;
    if (bad_arg_type(a7, t_vector|t_float64) || alist->a7->length < length)
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMap2d(alist->a0, alist->a1, alist->a2, 0, alist->a3,
	    alist->a4, alist->a5, 0, alist->a6, alist->a7->v.f64);
}

static void
native_GetMapCoeff(oobject_t list, oint32_t ac)
/* void GetMapCoeff(uint32_t target, float64_t v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 dim;
    oword_t				 mult;
    nat_u32_vec_t			*alist;
    oword_t				 length;
    GLint				 order[2];

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_MAP1_INDEX:
	case GL_MAP1_TEXTURE_COORD_1:
	    dim = 1;
	    mult = 1;
	    break;
	case GL_MAP1_TEXTURE_COORD_2:
	    dim = 1;
	    mult = 2;
	    break;
	case GL_MAP1_VERTEX_3:
	case GL_MAP1_NORMAL:
	case GL_MAP1_TEXTURE_COORD_3:
	    dim = 1;
	    mult = 3;
	    break;
	case GL_MAP1_VERTEX_4:
	case GL_MAP1_COLOR_4:
	case GL_MAP1_TEXTURE_COORD_4:
	    dim = 1;
	    mult = 4;
	    break;
	case GL_MAP2_INDEX:
	case GL_MAP2_TEXTURE_COORD_1:
	    dim = 2;
	    mult = 1;
	    break;
	case GL_MAP2_TEXTURE_COORD_2:
	    dim = 2;
	    mult = 2;
	    break;
	case GL_MAP2_VERTEX_3:
	case GL_MAP2_NORMAL:
	case GL_MAP2_TEXTURE_COORD_3:
	    dim = 2;
	    mult = 3;
	    break;
	case GL_MAP2_VERTEX_4:
	case GL_MAP2_COLOR_4:
	case GL_MAP2_TEXTURE_COORD_4:
	    dim = 2;
	    mult = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    glGetMapiv(alist->a0, GL_ORDER, order);
    length = order[0];
    if (dim == 2) {
	check_mult(length, order[1]);
	length *= order[1];
    }
    check_mult(length, mult);
    length *= mult;
    if (bad_arg_type(a1, t_vector|t_float64))
	ovm_raise(except_invalid_argument);
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetMapdv(alist->a0, GL_COEFF, alist->a1->v.f64);
}

static void
native_GetMapOrder(oobject_t list, oint32_t ac)
/* void GetMapOrder(uint32_t target, int32_t v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 order;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_MAP1_INDEX:
	case GL_MAP1_TEXTURE_COORD_1:
	case GL_MAP1_TEXTURE_COORD_2:
	case GL_MAP1_VERTEX_3:
	case GL_MAP1_NORMAL:
	case GL_MAP1_TEXTURE_COORD_3:
	case GL_MAP1_VERTEX_4:
	case GL_MAP1_COLOR_4:
	case GL_MAP1_TEXTURE_COORD_4:
	    order = 1;
	    break;
	case GL_MAP2_INDEX:
	case GL_MAP2_TEXTURE_COORD_1:
	case GL_MAP2_TEXTURE_COORD_2:
	case GL_MAP2_VERTEX_3:
	case GL_MAP2_NORMAL:
	case GL_MAP2_TEXTURE_COORD_3:
	case GL_MAP2_VERTEX_4:
	case GL_MAP2_COLOR_4:
	case GL_MAP2_TEXTURE_COORD_4:
	    order = 2;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a1, t_vector|t_int32))
	ovm_raise(except_invalid_argument);
    if (alist->a1->length != order)
	orenew_vector(alist->a1, order);
    r0->t = t_void;
    glGetMapiv(alist->a0, GL_ORDER, alist->a1->v.i32);
}

static void
native_GetMapDomain(oobject_t list, oint32_t ac)
/* void GetMapDomain(uint32_t target, float64_t v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    switch (alist->a0) {
	case GL_MAP1_INDEX:
	case GL_MAP1_TEXTURE_COORD_1:
	case GL_MAP1_TEXTURE_COORD_2:
	case GL_MAP1_VERTEX_3:
	case GL_MAP1_NORMAL:
	case GL_MAP1_TEXTURE_COORD_3:
	case GL_MAP1_VERTEX_4:
	case GL_MAP1_COLOR_4:
	case GL_MAP1_TEXTURE_COORD_4:
	    length = 2;
	    break;
	case GL_MAP2_INDEX:
	case GL_MAP2_TEXTURE_COORD_1:
	case GL_MAP2_TEXTURE_COORD_2:
	case GL_MAP2_VERTEX_3:
	case GL_MAP2_NORMAL:
	case GL_MAP2_TEXTURE_COORD_3:
	case GL_MAP2_VERTEX_4:
	case GL_MAP2_COLOR_4:
	case GL_MAP2_TEXTURE_COORD_4:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    if (bad_arg_type(a1, t_vector|t_float64))
	ovm_raise(except_invalid_argument);
    if (alist->a1->length != length)
	orenew_vector(alist->a1, length);
    r0->t = t_void;
    glGetMapdv(alist->a0, GL_DOMAIN, alist->a1->v.f64);
}


static void
native_EvalCoord1(oobject_t list, oint32_t ac)
/* void EvalCoord1(float64_t u); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_t				*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalCoord1d(alist->a0);
}

static void
native_EvalCoord2(oobject_t list, oint32_t ac)
/* void EvalCoord2(float64_t u, float64_t v); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalCoord2d(alist->a0, alist->a1);
}

static void
native_MapGrid1(oobject_t list, oint32_t ac)
/* void MapGrid1(int32_t un, float64_t u1, float64_t u2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f64_f64_t			*alist;

    alist = (nat_i32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMapGrid1d(alist->a0, alist->a1, alist->a2);
}

static void
native_MapGrid2(oobject_t list, oint32_t ac)
/* void MapGrid2(int32_t un, float64_t u1, float64_t u2,
		 int32_t vn, float64_t v1, float64_t v2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f64_f64_i32_f64_f64_t	*alist;

    alist = (nat_i32_f64_f64_i32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMapGrid2d(alist->a0, alist->a1, alist->a2,
		alist->a3, alist->a4, alist->a5);
}

static void
native_EvalPoint1(oobject_t list, oint32_t ac)
/* void EvalPoint1(int32_t i); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalPoint1(alist->a0);
}

static void
native_EvalPoint2(oobject_t list, oint32_t ac)
/* void EvalPoint2(int32_t i, int32_t j); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_t			*alist;

    alist = (nat_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalPoint2(alist->a0, alist->a1);
}

static void
native_EvalMesh1(oobject_t list, oint32_t ac)
/* void EvalMesh1(uint32_t mode, int32_t i1, int32_t i2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_t			*alist;

    alist = (nat_u32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalMesh1(alist->a0, alist->a1, alist->a2);
}

static void
native_EvalMesh2(oobject_t list, oint32_t ac)
/* void EvalMesh2(uint32_t mode, int32_t i1, int32_t i2,
		  int32_t j1, int32_t j2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_t		*alist;

    alist = (nat_u32_i32_i32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEvalMesh2(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}

/* Fog */
static void
native_Fog(oobject_t list, oint32_t ac)
/* void Fog(uint32_t pname, float32_t param); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f32_t			*alist;

    alist = (nat_u32_f32_t *)list;
    switch (alist->a0) {
	case GL_FOG_MODE:
	case GL_FOG_DENSITY:
	case GL_FOG_START:
	case GL_FOG_END:
	case GL_FOG_INDEX:
	case GL_FOG_COORD_SRC:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    r0 = &thread_self->r0;
    r0->t = t_void;
    glFogf(alist->a0, alist->a1);
}

static void
native_Fogv(oobject_t list, oint32_t ac)
/* void Fogv(uint32_t pname, float32_t param[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    switch (alist->a0) {
	case GL_FOG_MODE:
	case GL_FOG_DENSITY:
	case GL_FOG_START:
	case GL_FOG_END:
	case GL_FOG_INDEX:
	case GL_FOG_COORD_SRC:
	    length = 1;
	    break;
	case GL_FOG_COLOR:
	    length = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }

    if (bad_arg_type_length(a1, t_vector|t_float32, length))
	ovm_raise(except_invalid_argument);
    glFogfv(alist->a0, alist->a1->v.f32);
}

static void
native_FeedbackBuffer(oobject_t list, oint32_t ac)
/* void FeedbackBuffer(uint32_t type, float32_t buffer[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (bad_arg_type(a1, t_vector|t_float32))
	ovm_raise(except_invalid_argument);
    glFeedbackBuffer(alist->a1->length, alist->a0, alist->a1->v.f32);
}

static void
native_PassThrough(oobject_t list, oint32_t ac)
/* void PassThrough(float32_t token); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f32_t				*alist;

    alist = (nat_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPassThrough(alist->a0);
}

static void
native_SelectBuffer(oobject_t list, oint32_t ac)
/* void SelectBuffer(int32_t buffer[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (bad_arg_type(a0, t_vector|t_uint32))
	ovm_raise(except_invalid_argument);
    glSelectBuffer(alist->a0->length, alist->a0->v.u32);
}

static void
native_InitNames(oobject_t list, oint32_t ac)
/* void InitNames(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glInitNames();
}

static void
native_LoadName(oobject_t list, oint32_t ac)
/* void LoadName(uint32_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLoadName(alist->a0);
}

static void
native_PushName(oobject_t list, oint32_t ac)
/* void PushName(uint32_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glPushName(alist->a0);
}

static void
native_PopName(oobject_t list, oint32_t ac)
/* void PopName(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopName();
}

static void
native_ActiveTextureARB(oobject_t list, oint32_t ac)
/* void ActiveTextureARB(uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glActiveTextureARB(alist->a0);
}

static void
native_ClientActiveTextureARB(oobject_t list, oint32_t ac)
/* void ClientActiveTextureARB(uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClientActiveTextureARB(alist->a0);
}

static void
native_MultiTexCoord1ARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord1ARB(uint32_t target, float64_t s); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_t			*alist;

    alist = (nat_u32_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord1dARB(alist->a0, alist->a1);
}

static void
native_MultiTexCoord1vARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord1vARB(uint32_t target, float64_t v[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a1, t_vector|t_float64, 1))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMultiTexCoord1dvARB(alist->a0, alist->a1->v.f64);
}

static void
native_MultiTexCoord2ARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord2ARB(uint32_t target, float64_t s, float64_t t); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_t			*alist;

    alist = (nat_u32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord2dARB(alist->a0, alist->a1, alist->a2);
}

static void
native_MultiTexCoord2vARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord2vARB(uint32_t target, float64_t v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a1, t_vector|t_float64, 2))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMultiTexCoord2dvARB(alist->a0, alist->a1->v.f64);
}

static void
native_MultiTexCoord3ARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord3ARB(uint32_t target, float64_t s,
			  float64_t t, float64_t r); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord3dARB(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_MultiTexCoord3vARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord3vARB(uint32_t target, float64_t v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a1, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMultiTexCoord3dvARB(alist->a0, alist->a1->v.f64);
}

static void
native_MultiTexCoord4ARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord4ARB(uint32_t target, float64_t s,
			  float64_t t, float64_t r, float64_t q); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord4dARB(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}

static void
native_MultiTexCoord4vARB(oobject_t list, oint32_t ac)
/* void MultiTexCoord4vARB(uint32_t target, float64_t v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    if (bad_arg_type_length(a1, t_vector|t_float64, 4))
	ovm_raise(except_invalid_argument);
    r0->t = t_void;
    glMultiTexCoord4dvARB(alist->a0, alist->a1->v.f64);
}

static void
native_WindowPos2(oobject_t list, oint32_t ac)
/* void WindowPos2(float64_t x, float64_t y); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_t			*alist;

    alist = (nat_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glWindowPos2d(alist->a0, alist->a1);
}

static void
native_WindowPos2v(oobject_t list, oint32_t ac)
/* void WindowPos2v(float64_t xy[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (bad_arg_type_length(a0, t_vector|t_float64, 2))
	ovm_raise(except_invalid_argument);
    glWindowPos2dv(alist->a0->v.f64);
}

static void
native_WindowPos3(oobject_t list, oint32_t ac)
/* void WindowPos3(float64_t x, float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_f64_f64_t			*alist;

    alist = (nat_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glWindowPos3d(alist->a0, alist->a1, alist->a2);
}

static void
native_WindowPos3v(oobject_t list, oint32_t ac)
/* void WindowPos3v(float64_t xyz[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (bad_arg_type_length(a0, t_vector|t_float64, 3))
	ovm_raise(except_invalid_argument);
    glWindowPos3dv(alist->a0->v.f64);
}
