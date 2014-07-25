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
#define make_vec_text(V)						\
    do {								\
	if (!(V->length & 15)) {					\
	    orenew_vector(V, V->length + 1);				\
	    --V->length;						\
	}								\
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
/* Vertex Arrays */
static void native_VertexPointer(oobject_t list, oint32_t ac);
static void native_NormalPointer(oobject_t list, oint32_t ac);
static void native_ColorPointer(oobject_t list, oint32_t ac);
static void native_IndexPointer(oobject_t list, oint32_t ac);
static void native_TexCoordPointer(oobject_t list, oint32_t ac);
static void native_EdgeFlagPointer(oobject_t list, oint32_t ac);
#if 0	/* FIXME */
static void native_glGetPointerv(oobject_t list, oint32_t ac);
#endif
static void native_ArrayElement(oobject_t list, oint32_t ac);
static void native_DrawArrays(oobject_t list, oint32_t ac);
static void native_DrawElements(oobject_t list, oint32_t ac);
#if 0	/* FIXME */
static void native_InterleavedArrays(oobject_t list, oint32_t ac);
#endif
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
static void GetMapCoeff(uint32_t target, ovector_t *v);
static void GetMapOrder(uint32_t target, ovector_t *v);
static void GetMapDomain(uint32_t target, ovector_t *v);
static void native_GetMapv(oobject_t list, oint32_t ac);
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
static void native_ActiveTexture(oobject_t list, oint32_t ac);
static void native_ClientActiveTexture(oobject_t list, oint32_t ac);
static void native_MultiTexCoord1(oobject_t list, oint32_t ac);
static void native_MultiTexCoord1v(oobject_t list, oint32_t ac);
static void native_MultiTexCoord2(oobject_t list, oint32_t ac);
static void native_MultiTexCoord2v(oobject_t list, oint32_t ac);
static void native_MultiTexCoord3(oobject_t list, oint32_t ac);
static void native_MultiTexCoord3v(oobject_t list, oint32_t ac);
static void native_MultiTexCoord4(oobject_t list, oint32_t ac);
static void native_MultiTexCoord4v(oobject_t list, oint32_t ac);

/* glext */
static void native_FogCoord(oobject_t list, oint32_t ac);
static void native_WindowPos2(oobject_t list, oint32_t ac);
static void native_WindowPos2v(oobject_t list, oint32_t ac);
static void native_WindowPos3(oobject_t list, oint32_t ac);
static void native_WindowPos3v(oobject_t list, oint32_t ac);
static void native_GenQueries(oobject_t list, oint32_t ac);
static void native_DeleteQueries(oobject_t list, oint32_t ac);
static void native_IsQuery(oobject_t list, oint32_t ac);
static void native_BeginQuery(oobject_t list, oint32_t ac);
static void native_EndQuery(oobject_t list, oint32_t ac);
static void native_GetQueryv(oobject_t list, oint32_t ac);
static void native_GetQueryObjectv(oobject_t list, oint32_t ac);
static void native_BindBuffer(oobject_t list, oint32_t ac);
static void native_GenBuffers(oobject_t list, oint32_t ac);
static void native_DeleteBuffers(oobject_t list, oint32_t ac);
static void native_IsBuffer(oobject_t list, oint32_t ac);
static void native_BufferData(oobject_t list, oint32_t ac);
static void native_BufferSubData(oobject_t list, oint32_t ac);
static void native_GetBufferSubData(oobject_t list, oint32_t ac);
#if 0
static void native_MapBuffer(oobject_t list, oint32_t ac);
static void native_UnmapBuffer(oobject_t list, oint32_t ac);
#endif
static void native_GetBufferParameterv(oobject_t list, oint32_t ac);
#if 0
static void native_GetBufferPointerv(oobject_t list, oint32_t ac);
#endif

static void native_BlendEquationSeparate(oobject_t list, oint32_t ac);
static void native_DrawBuffers(oobject_t list, oint32_t ac);
static void native_StencilOpSeparate(oobject_t list, oint32_t ac);
static void native_StencilFuncSeparate(oobject_t list, oint32_t ac);
static void native_StencilMaskSeparate(oobject_t list, oint32_t ac);
static void native_AttachShader(oobject_t list, oint32_t ac);
static void native_BindAttribLocation(oobject_t list, oint32_t ac);
static void native_CompileShader(oobject_t list, oint32_t ac);
static void native_CreateProgram(oobject_t list, oint32_t ac);
static void native_CreateShader(oobject_t list, oint32_t ac);
static void native_DeleteProgram(oobject_t list, oint32_t ac);
static void native_DeleteShader(oobject_t list, oint32_t ac);
static void native_DetachShader(oobject_t list, oint32_t ac);
static void native_DetachShader(oobject_t list, oint32_t ac);
static void native_DisableVertexAttribArray(oobject_t list, oint32_t ac);
static void native_EnableVertexAttribArray(oobject_t list, oint32_t ac);
static void native_GetActiveAttrib(oobject_t list, oint32_t ac);
static void native_GetActiveUniform(oobject_t list, oint32_t ac);
static void native_GetAttachedShaders(oobject_t list, oint32_t ac);
static void native_GetAttribLocation(oobject_t list, oint32_t ac);
static void native_GetProgramv(oobject_t list, oint32_t ac);
static void native_GetProgramInfoLog(oobject_t list, oint32_t ac);
static void native_GetShaderv(oobject_t list, oint32_t ac);
static void native_GetShaderInfoLog(oobject_t list, oint32_t ac);
static void native_GetShaderSource(oobject_t list, oint32_t ac);
static void native_GetUniformLocation(oobject_t list, oint32_t ac);
#if 0
static void native_GetUniformv(oobject_t list, oint32_t ac);
static void native_glGetVertexAttribv(oobject_t list, oint32_t ac);
static void native_glGetVertexAttribPointerv(oobject_t list, oint32_t ac);
#endif
static void native_IsProgram(oobject_t list, oint32_t ac);
static void native_IsShader(oobject_t list, oint32_t ac);
static void native_LinkProgram(oobject_t list, oint32_t ac);
static void native_ShaderSource(oobject_t list, oint32_t ac);
static void native_UseProgram(oobject_t list, oint32_t ac);
static void native_Uniform1(oobject_t list, oint32_t ac);
static void native_Uniform2(oobject_t list, oint32_t ac);
static void native_Uniform3(oobject_t list, oint32_t ac);
static void native_Uniform4(oobject_t list, oint32_t ac);
static void native_Uniform1v(oobject_t list, oint32_t ac);
static void native_Uniform2v(oobject_t list, oint32_t ac);
static void native_Uniform3v(oobject_t list, oint32_t ac);
static void native_Uniform4v(oobject_t list, oint32_t ac);
static void native_UniformMatrix2v(oobject_t list, oint32_t ac);
static void native_UniformMatrix3v(oobject_t list, oint32_t ac);
static void native_UniformMatrix4v(oobject_t list, oint32_t ac);
static void native_ValidateProgram(oobject_t list, oint32_t ac);
static void native_VertexAttrib1(oobject_t list, oint32_t ac);
static void native_VertexAttrib2(oobject_t list, oint32_t ac);
static void native_VertexAttrib3(oobject_t list, oint32_t ac);
static void native_VertexAttrib4(oobject_t list, oint32_t ac);
static void native_VertexAttrib1v(oobject_t list, oint32_t ac);
static void native_VertexAttrib2v(oobject_t list, oint32_t ac);
static void native_VertexAttrib3v(oobject_t list, oint32_t ac);
static void native_VertexAttrib4v(oobject_t list, oint32_t ac);
static void native_VertexAttrib4Nv(oobject_t list, oint32_t ac);
static void native_VertexAttribPointer(oobject_t list, oint32_t ac);

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
    /* Vertex Arrays */
    { "VERTEX_ARRAY",			GL_VERTEX_ARRAY },
    { "NORMAL_ARRAY",			GL_NORMAL_ARRAY },
    { "COLOR_ARRAY",			GL_COLOR_ARRAY },
    { "INDEX_ARRAY",			GL_INDEX_ARRAY },
    { "TEXTURE_COORD_ARRAY",		GL_TEXTURE_COORD_ARRAY },
    { "EDGE_FLAG_ARRAY",		GL_EDGE_FLAG_ARRAY },
    { "VERTEX_ARRAY_SIZE",		GL_VERTEX_ARRAY_SIZE },
    { "VERTEX_ARRAY_TYPE",		GL_VERTEX_ARRAY_TYPE },
    { "VERTEX_ARRAY_STRIDE",		GL_VERTEX_ARRAY_STRIDE },
    { "NORMAL_ARRAY_TYPE",		GL_NORMAL_ARRAY_TYPE },
    { "NORMAL_ARRAY_STRIDE",		GL_NORMAL_ARRAY_STRIDE },
    { "COLOR_ARRAY_SIZE",		GL_COLOR_ARRAY_SIZE },
    { "COLOR_ARRAY_TYPE",		GL_COLOR_ARRAY_TYPE },
    { "COLOR_ARRAY_STRIDE",		GL_COLOR_ARRAY_STRIDE },
    { "INDEX_ARRAY_TYPE",		GL_INDEX_ARRAY_TYPE },
    { "INDEX_ARRAY_STRIDE",		GL_INDEX_ARRAY_STRIDE },
    { "TEXTURE_COORD_ARRAY_SIZE",	GL_TEXTURE_COORD_ARRAY_SIZE },
    { "TEXTURE_COORD_ARRAY_TYPE",	GL_TEXTURE_COORD_ARRAY_TYPE },
    { "TEXTURE_COORD_ARRAY_STRIDE",	GL_TEXTURE_COORD_ARRAY_STRIDE },
    { "EDGE_FLAG_ARRAY_STRIDE",		GL_EDGE_FLAG_ARRAY_STRIDE },
    { "VERTEX_ARRAY_POINTER",		GL_VERTEX_ARRAY_POINTER },
    { "NORMAL_ARRAY_POINTER",		GL_NORMAL_ARRAY_POINTER },
    { "COLOR_ARRAY_POINTER",		GL_COLOR_ARRAY_POINTER },
    { "INDEX_ARRAY_POINTER",		GL_INDEX_ARRAY_POINTER },
    { "TEXTURE_COORD_ARRAY_POINTER",	GL_TEXTURE_COORD_ARRAY_POINTER },
    { "EDGE_FLAG_ARRAY_POINTER",	GL_EDGE_FLAG_ARRAY_POINTER },
    { "V2F",				GL_V2F },
    { "V3F",				GL_V3F },
    { "C4UB_V2F",			GL_C4UB_V2F },
    { "C4UB_V3F",			GL_C4UB_V3F },
    { "C3F_V3F",			GL_C3F_V3F },
    { "N3F_V3F",			GL_N3F_V3F },
    { "C4F_N3F_V3F",			GL_C4F_N3F_V3F },
    { "T2F_V3F",			GL_T2F_V3F },
    { "T4F_V4F",			GL_T4F_V4F },
    { "T2F_C4UB_V3F",			GL_T2F_C4UB_V3F },
    { "T2F_C3F_V3F",			GL_T2F_C3F_V3F },
    { "T2F_N3F_V3F",			GL_T2F_N3F_V3F },
    { "T2F_C4F_N3F_V3F",		GL_T2F_C4F_N3F_V3F },
    { "T4F_C4F_N3F_V4F",		GL_T4F_C4F_N3F_V4F },
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
    { "FOG_COORDINATE_SOURCE",		GL_FOG_COORDINATE_SOURCE },
    { "FOG_COORDINATE",			GL_FOG_COORDINATE },

    { "BUFFER_SIZE",			GL_BUFFER_SIZE },
    { "BUFFER_USAGE",			GL_BUFFER_USAGE },
    { "QUERY_COUNTER_BITS",		GL_QUERY_COUNTER_BITS },
    { "CURRENT_QUERY",			GL_CURRENT_QUERY },
    { "QUERY_RESULT",			GL_QUERY_RESULT },
    { "QUERY_RESULT_AVAILABLE",		GL_QUERY_RESULT_AVAILABLE },
    { "ARRAY_BUFFER",			GL_ARRAY_BUFFER },
    { "ELEMENT_ARRAY_BUFFER",		GL_ELEMENT_ARRAY_BUFFER },
    { "ARRAY_BUFFER_BINDING",		GL_ARRAY_BUFFER_BINDING },
    { "ELEMENT_ARRAY_BUFFER_BINDING",	GL_ELEMENT_ARRAY_BUFFER_BINDING },
    { "VERTEX_ATTRIB_ARRAY_BUFFER_BINDING",GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING },
    { "READ_ONLY",			GL_READ_ONLY },
    { "WRITE_ONLY",			GL_WRITE_ONLY },
    { "READ_WRITE",			GL_READ_WRITE },
    { "BUFFER_ACCESS",			GL_BUFFER_ACCESS },
    { "BUFFER_MAPPED",			GL_BUFFER_MAPPED },
    { "BUFFER_MAP_POINTER",		GL_BUFFER_MAP_POINTER },
    { "STREAM_DRAW",			GL_STREAM_DRAW },
    { "STREAM_READ",			GL_STREAM_READ },
    { "STREAM_COPY",			GL_STREAM_COPY },
    { "STATIC_DRAW",			GL_STATIC_DRAW },
    { "STATIC_READ",			GL_STATIC_READ },
    { "STATIC_COPY",			GL_STATIC_COPY },
    { "DYNAMIC_DRAW",			GL_DYNAMIC_DRAW },
    { "DYNAMIC_READ",			GL_DYNAMIC_READ },
    { "DYNAMIC_COPY",			GL_DYNAMIC_COPY },
    { "SAMPLES_PASSED",			GL_SAMPLES_PASSED },
    { "SRC1_ALPHA",			GL_SRC1_ALPHA },
    { "VERTEX_ARRAY_BUFFER_BINDING",	GL_VERTEX_ARRAY_BUFFER_BINDING },
    { "NORMAL_ARRAY_BUFFER_BINDING",	GL_NORMAL_ARRAY_BUFFER_BINDING },
    { "COLOR_ARRAY_BUFFER_BINDING",	GL_COLOR_ARRAY_BUFFER_BINDING },
    { "INDEX_ARRAY_BUFFER_BINDING",	GL_INDEX_ARRAY_BUFFER_BINDING },
    { "TEXTURE_COORD_ARRAY_BUFFER_BINDING",GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING },
    { "EDGE_FLAG_ARRAY_BUFFER_BINDING",	GL_EDGE_FLAG_ARRAY_BUFFER_BINDING },
    { "SECONDARY_COLOR_ARRAY_BUFFER_BINDING",GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING },
    { "FOG_COORDINATE_ARRAY_BUFFER_BINDING",GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING },
    { "WEIGHT_ARRAY_BUFFER_BINDING",	GL_WEIGHT_ARRAY_BUFFER_BINDING },
    { "FOG_COORD_SRC",			GL_FOG_COORD_SRC },
    { "FOG_COORD",			GL_FOG_COORD },
    { "CURRENT_FOG_COORD",		GL_CURRENT_FOG_COORD },
    { "FOG_COORD_ARRAY_TYPE",		GL_FOG_COORD_ARRAY_TYPE },
    { "FOG_COORD_ARRAY_STRIDE",		GL_FOG_COORD_ARRAY_STRIDE },
    { "FOG_COORD_ARRAY_POINTER",	GL_FOG_COORD_ARRAY_POINTER },
    { "FOG_COORD_ARRAY",		GL_FOG_COORD_ARRAY },
    { "FOG_COORD_ARRAY_BUFFER_BINDING",	GL_FOG_COORD_ARRAY_BUFFER_BINDING },
    { "SRC0_RGB",			GL_SRC0_RGB },
    { "SRC1_RGB",			GL_SRC1_RGB },
    { "SRC2_RGB",			GL_SRC2_RGB },
    { "SRC0_ALPHA",			GL_SRC0_ALPHA },
    { "SRC2_ALPHA",			GL_SRC2_ALPHA },

    /* GL version 2.0 */
    { "BLEND_EQUATION_RGB",		GL_BLEND_EQUATION_RGB },
    { "VERTEX_ATTRIB_ARRAY_ENABLED",	GL_VERTEX_ATTRIB_ARRAY_ENABLED },
    { "VERTEX_ATTRIB_ARRAY_SIZE",	GL_VERTEX_ATTRIB_ARRAY_SIZE },
    { "VERTEX_ATTRIB_ARRAY_STRIDE",	GL_VERTEX_ATTRIB_ARRAY_STRIDE },
    { "VERTEX_ATTRIB_ARRAY_TYPE",	GL_VERTEX_ATTRIB_ARRAY_TYPE },
    { "CURRENT_VERTEX_ATTRIB",		GL_CURRENT_VERTEX_ATTRIB },
    { "VERTEX_PROGRAM_POINT_SIZE",	GL_VERTEX_PROGRAM_POINT_SIZE },
    { "VERTEX_ATTRIB_ARRAY_POINTER",	GL_VERTEX_ATTRIB_ARRAY_POINTER },
    { "STENCIL_BACK_FUNC",		GL_STENCIL_BACK_FUNC },
    { "STENCIL_BACK_FAIL",		GL_STENCIL_BACK_FAIL },
    { "STENCIL_BACK_PASS_DEPTH_FAIL",	GL_STENCIL_BACK_PASS_DEPTH_FAIL },
    { "STENCIL_BACK_PASS_DEPTH_PASS",	GL_STENCIL_BACK_PASS_DEPTH_PASS },
    { "MAX_DRAW_BUFFERS",		GL_MAX_DRAW_BUFFERS },
    { "DRAW_BUFFER0",			GL_DRAW_BUFFER0 },
    { "DRAW_BUFFER1",			GL_DRAW_BUFFER1 },
    { "DRAW_BUFFER2",			GL_DRAW_BUFFER2 },
    { "DRAW_BUFFER3",			GL_DRAW_BUFFER3 },
    { "DRAW_BUFFER4",			GL_DRAW_BUFFER4 },
    { "DRAW_BUFFER5",			GL_DRAW_BUFFER5 },
    { "DRAW_BUFFER6",			GL_DRAW_BUFFER6 },
    { "DRAW_BUFFER7",			GL_DRAW_BUFFER7 },
    { "DRAW_BUFFER8",			GL_DRAW_BUFFER8 },
    { "DRAW_BUFFER9",			GL_DRAW_BUFFER9 },
    { "DRAW_BUFFER10",			GL_DRAW_BUFFER10 },
    { "DRAW_BUFFER11",			GL_DRAW_BUFFER11 },
    { "DRAW_BUFFER12",			GL_DRAW_BUFFER12 },
    { "DRAW_BUFFER13",			GL_DRAW_BUFFER13 },
    { "DRAW_BUFFER14",			GL_DRAW_BUFFER14 },
    { "DRAW_BUFFER15",			GL_DRAW_BUFFER15 },
    { "BLEND_EQUATION_ALPHA",		GL_BLEND_EQUATION_ALPHA },
    { "MAX_VERTEX_ATTRIBS",		GL_MAX_VERTEX_ATTRIBS },
    { "VERTEX_ATTRIB_ARRAY_NORMALIZED",	GL_VERTEX_ATTRIB_ARRAY_NORMALIZED },
    { "MAX_TEXTURE_IMAGE_UNITS",	GL_MAX_TEXTURE_IMAGE_UNITS },
    { "FRAGMENT_SHADER",		GL_FRAGMENT_SHADER },
    { "VERTEX_SHADER",			GL_VERTEX_SHADER },
    { "MAX_FRAGMENT_UNIFORM_COMPONENTS",GL_MAX_FRAGMENT_UNIFORM_COMPONENTS },
    { "MAX_VERTEX_UNIFORM_COMPONENTS",	GL_MAX_VERTEX_UNIFORM_COMPONENTS },
    { "MAX_VARYING_FLOATS",		GL_MAX_VARYING_FLOATS },
    { "MAX_VERTEX_TEXTURE_IMAGE_UNITS",	GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS },
    { "MAX_COMBINED_TEXTURE_IMAGE_UNITS",GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS },
    { "SHADER_TYPE",			GL_SHADER_TYPE },
    { "FLOAT_VEC2",			GL_FLOAT_VEC2 },
    { "FLOAT_VEC3",			GL_FLOAT_VEC3 },
    { "FLOAT_VEC4",			GL_FLOAT_VEC4 },
    { "INT_VEC2",			GL_INT_VEC2 },
    { "INT_VEC3",			GL_INT_VEC3 },
    { "INT_VEC4",			GL_INT_VEC4 },
    { "BOOL",				GL_BOOL },
    { "BOOL_VEC2",			GL_BOOL_VEC2 },
    { "BOOL_VEC3",			GL_BOOL_VEC3 },
    { "BOOL_VEC4",			GL_BOOL_VEC4 },
    { "FLOAT_MAT2",			GL_FLOAT_MAT2 },
    { "FLOAT_MAT3",			GL_FLOAT_MAT3 },
    { "FLOAT_MAT4",			GL_FLOAT_MAT4 },
    { "SAMPLER_1D",			GL_SAMPLER_1D },
    { "SAMPLER_2D",			GL_SAMPLER_2D },
    { "SAMPLER_3D",			GL_SAMPLER_3D },
    { "SAMPLER_CUBE",			GL_SAMPLER_CUBE },
    { "SAMPLER_1D_SHADOW",		GL_SAMPLER_1D_SHADOW },
    { "SAMPLER_2D_SHADOW",		GL_SAMPLER_2D_SHADOW },
    { "DELETE_STATUS",			GL_DELETE_STATUS },
    { "COMPILE_STATUS",			GL_COMPILE_STATUS },
    { "LINK_STATUS",			GL_LINK_STATUS },
    { "VALIDATE_STATUS",		GL_VALIDATE_STATUS },
    { "INFO_LOG_LENGTH",		GL_INFO_LOG_LENGTH },
    { "ATTACHED_SHADERS",		GL_ATTACHED_SHADERS },
    { "ACTIVE_UNIFORMS",		GL_ACTIVE_UNIFORMS },
    { "ACTIVE_UNIFORM_MAX_LENGTH",	GL_ACTIVE_UNIFORM_MAX_LENGTH },
    { "SHADER_SOURCE_LENGTH",		GL_SHADER_SOURCE_LENGTH },
    { "ACTIVE_ATTRIBUTES",		GL_ACTIVE_ATTRIBUTES },
    { "ACTIVE_ATTRIBUTE_MAX_LENGTH",	GL_ACTIVE_ATTRIBUTE_MAX_LENGTH },
    { "FRAGMENT_SHADER_DERIVATIVE_HINT",GL_FRAGMENT_SHADER_DERIVATIVE_HINT },
    { "SHADING_LANGUAGE_VERSION",	GL_SHADING_LANGUAGE_VERSION },
    { "CURRENT_PROGRAM",		GL_CURRENT_PROGRAM },
    { "POINT_SPRITE_COORD_ORIGIN",	GL_POINT_SPRITE_COORD_ORIGIN },
    { "LOWER_LEFT",			GL_LOWER_LEFT },
    { "UPPER_LEFT",			GL_UPPER_LEFT },
    { "STENCIL_BACK_REF",		GL_STENCIL_BACK_REF },
    { "STENCIL_BACK_VALUE_MASK",	GL_STENCIL_BACK_VALUE_MASK },
    { "STENCIL_BACK_WRITEMASK",		GL_STENCIL_BACK_WRITEMASK },
    { "VERTEX_PROGRAM_TWO_SIDE",	GL_VERTEX_PROGRAM_TWO_SIDE },
    { "POINT_SPRITE",			GL_POINT_SPRITE },
    { "COORD_REPLACE",			GL_COORD_REPLACE },
    { "MAX_TEXTURE_COORDS",		GL_MAX_TEXTURE_COORDS },
};

typedef oint32_t	obuffer_offset_t;
#define ARRAY_BUFFER_OFFSET			0
#define ARRAY_BUFFER_LENGTH			1
#define ELEMENT_ARRAY_BUFFER_OFFSET		2
#define ELEMENT_ARRAY_BUFFER_LENGTH		3
#define PIXEL_PACK_BUFFER_OFFSET		4
#define PIXEL_PACK_BUFFER_LENGTH		5
#define PIXEL_UNPACK_BUFFER_OFFSET		6
#define PIXEL_UNPACK_BUFFER_LENGTH		7
#define BUFFER_VECTOR_SIZE			(sizeof(obuffer_offset_t) * 8)
static ohashtable_t		*buffer_table;
static ovector_t		*int_vector;
static ovector_t		*str_vector;

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

    oadd_root((oobject_t *)&buffer_table);
    onew_hashtable(&buffer_table, 4);
    oadd_root((oobject_t *)&int_vector);
    oadd_root((oobject_t *)&str_vector);

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
    define_builtin1(t_void,    LoadMatrix, t_vector);
    define_builtin1(t_void,    MultMatrix, t_vector);
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
    define_builtin1(t_void,    CallLists, t_vector);
    define_builtin1(t_void,    ListBase, t_uint32);
    define_builtin1(t_void,    Begin, t_uint32);
    define_builtin0(t_void,    End);
    define_builtin2(t_void,    Vertex2, t_float64, t_float64);
    define_builtin3(t_void,    Vertex3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    Vertex4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Vertex2v, t_vector);
    define_builtin1(t_void,    Vertex3v, t_vector);
    define_builtin1(t_void,    Vertex4v, t_vector);
    define_builtin3(t_void,    Normal3, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Normal3v, t_vector);
    define_builtin1(t_void,    Index, t_int32);
    define_builtin3(t_void,    Color3, t_float32, t_float32, t_float32);
    define_builtin4(t_void,    Color4,
		    t_float32, t_float32, t_float32, t_float32);
    define_builtin1(t_void,    Color3v, t_vector);
    define_builtin1(t_void,    Color4v, t_vector);
    define_builtin1(t_void,    TexCoord1, t_float64);
    define_builtin2(t_void,    TexCoord2, t_float64, t_float64);
    define_builtin3(t_void,    TexCoord3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    TexCoord4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    TexCoord2v, t_vector);
    define_builtin1(t_void,    TexCoord3v, t_vector);
    define_builtin1(t_void,    TexCoord4v, t_vector);
    define_builtin2(t_void,    RasterPos2, t_float64, t_float64);
    define_builtin3(t_void,    RasterPos3, t_float64, t_float64, t_float64);
    define_builtin4(t_void,    RasterPos4,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    RasterPos2v, t_vector);
    define_builtin1(t_void,    RasterPos3v, t_vector);
    define_builtin1(t_void,    RasterPos4v, t_vector);
    define_builtin4(t_void,    Rect,
		    t_float64, t_float64, t_float64, t_float64);
    define_builtin1(t_void,    Rectv, t_vector);
    define_builtin3(t_void,     VertexPointer, t_int32, t_int32, t_vector);
    define_builtin2(t_void,     NormalPointer, t_int32, t_vector);
    define_builtin3(t_void,     ColorPointer, t_int32, t_int32, t_vector);
    define_builtin2(t_void,     IndexPointer, t_int32, t_vector);
    define_builtin3(t_void,     TexCoordPointer, t_int32, t_int32, t_vector);
    define_builtin2(t_void,     EdgeFlagPointer, t_int32, t_vector);
    define_builtin1(t_void,     ArrayElement, t_int32);
    define_builtin3(t_void,     DrawArrays, t_uint32, t_int32, t_int32);
    define_builtin2(t_void,     DrawElements, t_uint32, t_vector);
    define_builtin1(t_void,    ShadeModel, t_uint32);
    define_builtin3(t_void,    Light, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    Lightv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    GetLightv, t_uint32, t_uint32, t_vector);
    define_builtin2(t_void,    LightModel, t_uint32, t_float32);
    define_builtin2(t_void,    LightModelv, t_uint32, t_vector);
    define_builtin3(t_void,    Material, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    Materialv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    GetMaterialv, t_uint32, t_uint32, t_vector);
    define_builtin2(t_void,    ColorMaterial, t_uint32, t_uint32);
    define_builtin2(t_void,    PixelZoom, t_float32, t_float32);
    define_builtin2(t_void,    PixelStore, t_uint32, t_int32);
    define_builtin2(t_void,    PixelTransfer, t_uint32, t_float32);
    define_builtin2(t_void,    PixelMapv, t_uint32, t_vector);
    define_builtin2(t_void,    GetPixelMapv, t_uint32, t_vector);
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
    define_builtin3(t_void,    TexGenv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    GetTexGenv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    TexEnv, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    TexEnvv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    GetTexEnvv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    TexParameter, t_uint32, t_uint32, t_float32);
    define_builtin3(t_void,    TexParameterv, t_uint32, t_uint32, t_vector);
    define_builtin3(t_void,    GetTexParameterv, t_uint32, t_uint32, t_vector);
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
    define_builtin8(t_void,    TexSubImage2D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32, t_int32,
		    t_uint32, t_vector|t_uint8);
    define_builtin7(t_void,    CopyTexImage1D,
		    t_uint32, t_int32, t_uint32, t_int32, t_int32, t_int32,
		    t_int32);
    define_builtin8(t_void,    CopyTexImage2D,
		    t_uint32, t_int32, t_uint32, t_int32, t_int32, t_int32,
		    t_int32, t_int32);
    define_builtin6(t_void,    CopyTexSubImage1D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32, t_int32);
    define_builtin8(t_void,    CopyTexSubImage2D,
		    t_uint32, t_int32, t_int32, t_int32, t_int32, t_int32,
		    t_int32, t_int32);
    define_builtin5(t_void,    Map1,
		    t_uint32, t_float64, t_float64, t_int32, t_vector);
    define_builtin8(t_void,    Map2,
		    t_uint32, t_float64, t_float64, t_int32,
		    t_float64, t_float64, t_int32, t_vector);
    define_builtin3(t_void,    GetMapv, t_uint32, t_uint32, t_vector);
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
    define_builtin2(t_void,    Fogv, t_uint32, t_vector);
    define_builtin2(t_void,    FeedbackBuffer, t_uint32, t_vector|t_float32);
    define_builtin1(t_void,    PassThrough, t_float32);
    define_builtin1(t_void,    SelectBuffer, t_vector|t_uint32);
    define_builtin0(t_void,    InitNames);
    define_builtin1(t_void,    LoadName, t_uint32);
    define_builtin1(t_void,    PushName, t_uint32);
    define_builtin0(t_void,    PopName);
    define_builtin1(t_void,    ActiveTexture, t_uint32);
    define_builtin1(t_void,    ClientActiveTexture, t_uint32);
    define_builtin2(t_void,    MultiTexCoord1, t_uint32, t_float64);
    define_builtin2(t_void,    MultiTexCoord1v, t_uint32, t_vector);
    define_builtin3(t_void,    MultiTexCoord2,
		    t_uint32, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord2v, t_uint32, t_vector);
    define_builtin4(t_void,    MultiTexCoord3,
		    t_uint32, t_float64, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord3v, t_uint32, t_vector);
    define_builtin5(t_void,    MultiTexCoord4,
		    t_uint32, t_float64, t_float64, t_float64, t_float64);
    define_builtin2(t_void,    MultiTexCoord4v, t_uint32, t_vector);
    define_builtin1(t_void,  FogCoord, t_float64);
    define_builtin2(t_void,  WindowPos2, t_float64, t_float64);
    define_builtin1(t_void,  WindowPos2v, t_vector);
    define_builtin3(t_void,  WindowPos3, t_float64, t_float64, t_float64);
    define_builtin1(t_void,  WindowPos3v, t_vector);
    define_builtin1(t_void,  GenQueries, t_uint32|t_vector);
    define_builtin1(t_void,  DeleteQueries, t_vector|t_uint32);
    define_builtin1(t_uint8, IsQuery, t_uint32);
    define_builtin2(t_void,  BeginQuery, t_uint32, t_uint32);
    define_builtin1(t_void,  EndQuery, t_uint32);
    define_builtin3(t_void,  GetQueryv, t_uint32, t_uint32, t_vector|t_uint32);
    define_builtin3(t_void,  GetQueryObjectv,
		    t_uint32, t_uint32, t_vector|t_uint32);
    define_builtin2(t_void,  BindBuffer, t_uint32, t_uint32);
    define_builtin1(t_void,  GenBuffers, t_vector|t_uint32);
    define_builtin1(t_void,  DeleteBuffers, t_vector|t_uint32);
    define_builtin1(t_uint8, IsBuffer, t_uint32);
    define_builtin3(t_void,  BufferData, t_uint32, t_vector, t_uint32);
    define_builtin3(t_void,  BufferSubData, t_uint32, t_word, t_vector);
    define_builtin3(t_void,  GetBufferSubData, t_uint32, t_word, t_vector);
    define_builtin3(t_void,  GetBufferParameterv,
		    t_uint32, t_word, t_vector|t_int32);

    define_builtin2(t_void,   BlendEquationSeparate, t_uint32, t_uint32);
    define_builtin1(t_void,   DrawBuffers, t_vector|t_uint32);
    define_builtin4(t_void,   StencilOpSeparate,
		    t_uint32, t_uint32, t_uint32, t_uint32);
    define_builtin4(t_void,   StencilFuncSeparate,
		    t_uint32, t_uint32, t_uint32, t_uint32);
    define_builtin2(t_void,   StencilMaskSeparate, t_uint32, t_uint32);
    define_builtin2(t_void,   AttachShader, t_uint32, t_uint32);
    define_builtin3(t_void,   BindAttribLocation, t_uint32, t_uint32, t_string);
    define_builtin1(t_void,   CompileShader, t_uint32);
    define_builtin0(t_uint32, CreateProgram);
    define_builtin1(t_uint32, CreateShader, t_uint32);
    define_builtin1(t_void,   DeleteProgram, t_uint32);
    define_builtin1(t_void,   DeleteShader, t_uint32);
    define_builtin2(t_void,   DetachShader, t_uint32, t_uint32);
    define_builtin1(t_void,   DisableVertexAttribArray, t_uint32);
    define_builtin1(t_void,   EnableVertexAttribArray, t_uint32);
    define_builtin5(t_void,   GetActiveAttrib,
		    t_uint32, t_uint32,
		    t_vector|t_int32, t_vector|t_uint32, t_string);
    define_builtin5(t_void,   GetActiveUniform,
		    t_uint32, t_uint32,
		    t_vector|t_int32, t_vector|t_uint32, t_string);
    define_builtin2(t_void,   GetAttachedShaders, t_uint32, t_vector|t_uint32);
    define_builtin2(t_int32,  GetAttribLocation, t_uint32, t_string);
    define_builtin3(t_void,   GetProgramv,
		    t_uint32, t_uint32, t_vector|t_int32);
    define_builtin2(t_void,   GetProgramInfoLog, t_uint32, t_string);
    define_builtin3(t_void,   GetShaderv, t_uint32, t_uint32, t_vector|t_int32);
    define_builtin2(t_void,   GetShaderInfoLog, t_uint32, t_string);
    define_builtin2(t_void,   GetShaderSource, t_uint32, t_string);
    define_builtin2(t_int32,  GetUniformLocation, t_uint32, t_string);
    define_builtin1(t_void,   IsProgram, t_uint32);
    define_builtin1(t_void,   IsShader, t_uint32);
    define_builtin1(t_void,   LinkProgram, t_uint32);
    define_builtin2(t_void,   ShaderSource, t_uint32, t_vector|t_string);
    define_builtin1(t_void,   UseProgram, t_uint32);
    define_builtin2(t_void,   Uniform1, t_int32, t_float32);
    define_builtin3(t_void,   Uniform2,
		    t_int32, t_float32, t_float32);
    define_builtin4(t_void,   Uniform3,
		    t_int32, t_float32, t_float32, t_float32);
    define_builtin5(t_void,   Uniform4,
		    t_int32, t_float32, t_float32, t_float32, t_float32);
    define_builtin2(t_void,   Uniform1v, t_int32, t_vector);
    define_builtin2(t_void,   Uniform2v, t_int32, t_vector);
    define_builtin2(t_void,   Uniform3v, t_int32, t_vector);
    define_builtin2(t_void,   Uniform4v, t_int32, t_vector);
    define_builtin3(t_void,   UniformMatrix2v,
		    t_int32, t_uint8, t_vector|t_float32);
    define_builtin3(t_void,   UniformMatrix3v,
		    t_int32, t_uint8, t_vector|t_float32);
    define_builtin3(t_void,   UniformMatrix4v,
		    t_int32, t_uint8, t_vector|t_float32);
    define_builtin1(t_void,   ValidateProgram, t_uint32);
    define_builtin2(t_void,   VertexAttrib1, t_uint32, t_float64);
    define_builtin3(t_void,   VertexAttrib2, t_uint32, t_float64, t_float64);
    define_builtin4(t_void,   VertexAttrib3,
		    t_uint32, t_float64, t_float64, t_float64);
    define_builtin5(t_void,   VertexAttrib4,
		    t_uint32, t_float64, t_float64, t_float64, t_float64);
    define_builtin2(t_void,   VertexAttrib1v, t_uint32, t_vector);
    define_builtin2(t_void,   VertexAttrib2v, t_uint32, t_vector);
    define_builtin2(t_void,   VertexAttrib3v, t_uint32, t_vector);
    define_builtin2(t_void,   VertexAttrib4v, t_uint32, t_vector);
    define_builtin2(t_void,   VertexAttrib4Nv, t_uint32, t_vector);
    define_builtin4(t_void,   VertexAttribPointer,
		    t_uint32, t_int32, t_int32, t_vector);

    current_record = record;
}

void
finish_gl(void)
{
    orem_root((oobject_t *)&int_vector);
    orem_root((oobject_t *)&str_vector);
    orem_root((oobject_t *)&buffer_table);
}

static void
native_ClearIndex(oobject_t list, oint32_t ac)
/* void gl.ClearIndex(float32_t c); */
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
/* void gl.ClearColor(float32_t r, float32_t g, float32_t b, float32_t a); */
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
/* void gl.Clear(uint32_t mask); */
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
/* void gl.IndexMask(uint32_t mask); */
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
/* void gl.ColorMask(uint8_t r, uint8_t g, uint8_t b, uint8_t a); */
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
/* void gl.AlphaFunc(uint32_t func, float32_t ref); */
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
/* void gl.BlendFunc(uint32_t func, uint32_t dfactor); */
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
/* void gl.LogicOp(uint32_t opcode); */
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
/* void gl.CullFace(uint32_t mode); */
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
/* void gl.FrontFace(uint32_t mode); */
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
/* void gl.PointSize(float32_t size); */
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
/* void gl.LineWidth(float32_t width); */
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
/* void gl.LineStipple(int32_t factor, uint16_t pattern); */
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
/* void gl.PolygonMode(uint32_t face, uint32_t mode); */
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
/* void gl.PolygonOffset(float32_t factor, float32_t units); */
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
/* void gl.PolygonStipple(uint8_t mask[128]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint8);
    CHECK_LENGTH(alist->a0, 128);
    r0->t = t_void;
    glPolygonStipple(alist->a0->v.u8);
}

static void
native_GetPolygonStipple(oobject_t list, oint32_t ac)
/* void gl.GetPolygonStipple(uint8_t mask[128]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint8);
    if (alist->a0->length != 128)
	orenew_vector(alist->a0, 128);
    r0->t = t_void;
    glGetPolygonStipple(alist->a0->v.u8);
}

static void
native_EdgeFlag(oobject_t list, oint32_t ac)
/* void gl.EdgeFlag(uint8_t flag); */
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
/* void gl.Scissor(int32_t x, int32_t y, int32_t width, int32_t height) */
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
/* void gl.ClipPlane(uint32_t plane, float64_t equation[4]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float64);
    CHECK_LENGTH(alist->a1, 4);
    r0->t = t_void;
    glClipPlane(alist->a0, alist->a1->v.f64);
}

static void
native_GetClipPlane(oobject_t list, oint32_t ac)
/* void gl.GetClipPlane(uint32_t plane, float64_t equation[4]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float64);
    if (alist->a1->length != 4)
	orenew_vector(alist->a1, 4);
    r0->t = t_void;
    glGetClipPlane(alist->a0, alist->a1->v.f64);
}

static void
native_DrawBuffer(oobject_t list, oint32_t ac)
/* void gl.DrawBuffer(uint32_t mode) */
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
/* void gl.ReadBuffer(uint32_t mode) */
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
/* void gl.Enable(uint32_t cap) */
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
/* void gl.Disable(uint32_t cap) */
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
/* uint8_t gl.IsEnabled(uint32_t cap) */
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
/* void gl.EnableClientState(uint32_t cap) */
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
/* void gl.DisableClientState(uint32_t cap) */
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
/* void gl.GetBooleanv(uint32_t pname, uint8_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint8);
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
	case GL_INDEX_ARRAY:
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
	case GL_NORMAL_ARRAY:
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
	case GL_TEXTURE_COORD_ARRAY:
	case GL_TEXTURE_GEN_Q:
	case GL_TEXTURE_GEN_R:
	case GL_TEXTURE_GEN_S:
	case GL_TEXTURE_GEN_T:
	case GL_UNPACK_LSB_FIRST:
	case GL_UNPACK_SWAP_BYTES:
	case GL_VERTEX_ARRAY:
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
/* void gl.GetDoublev(uint32_t pname, float64_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float64);
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
/* void gl.GetFloatv(uint32_t pname, float32_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
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
/* void gl.GetIntegerv(uint32_t pname, int32_t params[]) */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_int32);
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
	case GL_COLOR_ARRAY_SIZE:
	case GL_COLOR_ARRAY_STRIDE:
	case GL_COLOR_ARRAY_TYPE:
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
	case GL_INDEX_ARRAY_STRIDE:
	case GL_INDEX_ARRAY_TYPE:
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
	case GL_NORMAL_ARRAY_STRIDE:
	case GL_NORMAL_ARRAY_TYPE:
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
	case GL_TEXTURE_COORD_ARRAY_SIZE:
	case GL_TEXTURE_COORD_ARRAY_STRIDE:
	case GL_TEXTURE_COORD_ARRAY_TYPE:
	case GL_TEXTURE_STACK_DEPTH:
	case GL_UNPACK_ALIGNMENT:
	case GL_UNPACK_ROW_LENGTH:
	case GL_UNPACK_SKIP_PIXELS:
	case GL_UNPACK_SKIP_ROWS:
	case GL_VERTEX_ARRAY_SIZE:
	case GL_VERTEX_ARRAY_STRIDE:
	case GL_VERTEX_ARRAY_TYPE:
	case GL_MAX_TEXTURE_UNITS:
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
/* void gl.PushAttrib(uint32_t mask); */
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
/* void gl.PopAttrib(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopAttrib();
}

static void
native_PushClientAttrib(oobject_t list, oint32_t ac)
/* void gl.PushClientAttrib(uint32_t mask); */
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
/* void gl.PopClientAttrib(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopClientAttrib();
}

static void
native_RenderMode(oobject_t list, oint32_t ac)
/* int32_t gl.RenderMode(uint32_t mode); */
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
/* uint32_t gl.GetError(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glGetError();
}

static void
native_GetString(oobject_t list, oint32_t ac)
/* string_t gl.GetString(uint32_t name); */
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
/* void gl.Finish(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glFinish();
}

static void
native_Flush(oobject_t list, oint32_t ac)
/* void gl.Flush(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glFlush();
}

static void
native_Hint(oobject_t list, oint32_t ac)
/* void gl.Hint(uint32_t target, uint32_t mode); */
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
/* void gl.ClearDepth(float64_t depth); */
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
/* void gl.DepthFunc(uint32_t func); */
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
/* void gl.DepthMask(uint8_t mask); */
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
/* void gl.DepthRange(float64_t near_val, float64_t far_val); */
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
/* void gl.ClearAccum(float32_t r, float32_t g, float32_t b, float32_t a); */
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
/* void gl.Accum(uint32_t op, float32_t value); */
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
/* void gl.MatrixMode(uint32_t mode); */
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
/* void gl.Ortho(float64_t left, float64_t right,
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
/* void gl.Frustum(float64_t left, float64_t right,
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
/* void gl.Viewport(int32_t, int32_t y, int32_t width, int32_t height); */
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
/* void gl.PushMatrix(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPushMatrix();
}

static void
native_PopMatrix(oobject_t list, oint32_t ac)
/* void gl.PopMatrix(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopMatrix();
}

static void
native_LoadIdentity(oobject_t list, oint32_t ac)
/* void gl.LoadIdentity(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glLoadIdentity();
}

static void
native_LoadMatrix(oobject_t list, oint32_t ac)
/* void gl.LoadMatrix((float64_t|float64_t) m[16]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0 == null)			goto fail;
    switch (otype(alist->a0)) {
	case t_vector|t_float32:
	    if (alist->a0->length != 16)	goto fail;
	    glLoadMatrixf(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    if (alist->a0->length != 16)	goto fail;
	    glLoadMatrixd(alist->a0->v.f64);
	    break;
	default:
	fail:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_MultMatrix(oobject_t list, oint32_t ac)
/* void gl.MultMatrix((float64_t|float64_t) m[16]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a0 == null)			goto fail;
    switch (otype(alist->a0)) {
	case t_vector|t_float32:
	    if (alist->a0->length != 16)	goto fail;
	    glMultMatrixf(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    if (alist->a0->length != 16)	goto fail;
	    glMultMatrixd(alist->a0->v.f64);
	    break;
	default:
	fail:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Rotate(oobject_t list, oint32_t ac)
/* void gl.Rotate(float64_t angle, float64_t x, float64_t y, float64_t z); */
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
/* void gl.Scale(float64_t x, float64_t y, float64_t z); */
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
/* void gl.Translate(float64_t x, float64_t y, float64_t z); */
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
/* uint8_t gl.IsList(uint32_t list); */
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
/* void gl.DeleteLists(uint32_t list, int32_t range); */
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
/* uint32_t gl.GenLists(int32_t range); */
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
/* void gl.NewList(uint32_t list, uint32_t mode); */
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
/* void gl.EndList(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glEndList();
}

static void
native_CallList(oobject_t list, oint32_t ac)
/* void gl.CallList(uint32_t list); */
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
/* void gl.CallLists((uint8_t|uint16_t|uint32_t) lists[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    ouint32_t				 type;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    switch (otype(alist->a0)) {
	case t_vector|t_uint8:
	    type = GL_UNSIGNED_BYTE;
	    break;
	case t_vector|t_uint16:
	    type = GL_UNSIGNED_SHORT;
	    break;
	case t_vector|t_uint32:
	    type = GL_UNSIGNED_INT;
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    r0->t = t_void;
    glCallLists(alist->a0->length, type, alist->a0->v.u8);
}

static void
native_ListBase(oobject_t list, oint32_t ac)
/* void gl.ListBase(uint32_t base); */
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
/* void gl.Begin(uint32_t mode); */
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
/* void gl.End(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glEnd();
}

static void
native_Vertex2(oobject_t list, oint32_t ac)
/* void gl.Vertex2(float64_t x, float64_t y); */
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
/* void gl.Vertex3(float64_t x, float64_t y, float64_t z); */
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
/* void gl.Vertex4(float64_t x, float64_t y, float64_t z, float64_t w); */
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
/* void gl.Vertex2v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 2);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glVertex2sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glVertex2iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glVertex2fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertex2dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Vertex3v(oobject_t list, oint32_t ac)
/* void gl.Vertex3v((int16_t|int32_t|float32_t|float64_t) v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glVertex3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glVertex3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glVertex3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertex3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Vertex4v(oobject_t list, oint32_t ac)
/* void gl.Vertex4v((int16_t|int32_t|float32_t|float64_t) v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 4);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glVertex4sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glVertex4iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glVertex4fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertex4dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Normal3(oobject_t list, oint32_t ac)
/* void gl.Normal3(float64_t nx, float64_t ny, float64_t nz); */
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
/* void gl.Normal3v((int8_t|int16_t|int32_t|float32_t|float64_t) v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int8:
	    glNormal3bv(alist->a0->v.i8);
	    break;
	case t_vector|t_int16:
	    glNormal3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glNormal3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glNormal3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glNormal3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Index(oobject_t list, oint32_t ac)
/* void gl.Index(int32_t c); */
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
/* void gl.Color3(float32_t r, float32_t g, float32_t b); */
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
/* void gl.Color4(float32_t r, float32_t g, float32_t b, float32_t a); */
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
/* void gl.Color3v((int8_t|uint8_t|int16_t|uint16_t|int32_t|uint32_t|
		    float32_t|float64_t) v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int8:
	    glColor3bv(alist->a0->v.i8);
	    break;
	case t_vector|t_uint8:
	    glColor3ubv(alist->a0->v.u8);
	    break;
	case t_vector|t_int16:
	    glColor3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_uint16:
	    glColor3usv(alist->a0->v.u16);
	    break;
	case t_vector|t_int32:
	    glColor3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_uint32:
	    glColor3uiv(alist->a0->v.u32);
	    break;
	case t_vector|t_float32:
	    glColor3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glColor3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Color4v(oobject_t list, oint32_t ac)
/* void gl.Color4v((int8_t|uint8_t|int16_t|uint16_t|int32_t|uint32_t|
		    float32_t|float64_t) v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 4);
    switch (otype(alist->a0)) {
	case t_vector|t_int8:
	    glColor4bv(alist->a0->v.i8);
	    break;
	case t_vector|t_uint8:
	    glColor4ubv(alist->a0->v.u8);
	    break;
	case t_vector|t_int16:
	    glColor4sv(alist->a0->v.i16);
	    break;
	case t_vector|t_uint16:
	    glColor4usv(alist->a0->v.u16);
	    break;
	case t_vector|t_int32:
	    glColor4iv(alist->a0->v.i32);
	    break;
	case t_vector|t_uint32:
	    glColor4uiv(alist->a0->v.u32);
	    break;
	case t_vector|t_float32:
	    glColor4fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glColor4dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_TexCoord1(oobject_t list, oint32_t ac)
/* void gl.TexCoord1(float64_t s); */
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
/* void gl.TexCoord2(float64_t s, float64_t t); */
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
/* void gl.TexCoord3(float64_t s, float64_t t, float64_t r); */
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
/* void gl.TexCoord4(float64_t s, float64_t t, float64_t r, float64_t q); */
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
/* void gl.TexCoord2v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 2);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glTexCoord2sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glTexCoord2iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexCoord2fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glTexCoord2dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_TexCoord3v(oobject_t list, oint32_t ac)
/* void gl.TexCoord3v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glTexCoord3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glTexCoord3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexCoord3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glTexCoord3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_TexCoord4v(oobject_t list, oint32_t ac)
/* void gl.TexCoord4v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 4);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glTexCoord4sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glTexCoord4iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexCoord4fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glTexCoord4dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_RasterPos2(oobject_t list, oint32_t ac)
/* void gl.RasterPos2(float64_t x, float64_t y); */
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
/* void gl.RasterPos3(float64_t x, float64_t y, float64_t z); */
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
/* void gl.RasterPos3(float64_t x, float64_t y, float64_t z, float64_t w); */
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
/* void gl.RasterPos2v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 2);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glRasterPos2sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glRasterPos2iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glRasterPos2fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glRasterPos2dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_RasterPos3v(oobject_t list, oint32_t ac)
/* void gl.RasterPos3v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glRasterPos3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glRasterPos3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glRasterPos3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glRasterPos3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_RasterPos4v(oobject_t list, oint32_t ac)
/* void gl.RasterPos4v((int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 4);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glRasterPos4sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glRasterPos4iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glRasterPos4fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glRasterPos4dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Rect(oobject_t list, oint32_t ac)
/* void gl.Rect(float64_t x1, float64_t y1, float64_t x2, float64_t y2); */
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
/* void gl.Rectv((int16_t|int32_t|float32_t|float64_t) v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 4);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glRectsv(alist->a0->v.i16, alist->a0->v.i16 + 2);
	    break;
	case t_vector|t_int32:
	    glRectiv(alist->a0->v.i32, alist->a0->v.i32 + 2);
	    break;
	case t_vector|t_float32:
	    glRectfv(alist->a0->v.f32, alist->a0->v.f32 + 2);
	    break;
	case t_vector|t_float64:
	    glRectdv(alist->a0->v.f64, alist->a0->v.f64 + 2);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexPointer(oobject_t list, oint32_t ac)
/* void gl.VertexPointer(int32_t size, int32_t stride,
			 (int16_t|int32_t|float32_t|float64_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    oword_t				 offset;
    oword_t				 stride;
    obuffer_offset_t			*vector;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;

    if (alist->a2 && (otype(alist->a2) & t_vector)) {
	if (alist->a0 < 2 || alist->a0 > 0)	/* size */
	    ovm_raise(except_invalid_argument);
	switch (otype(alist->a2)) {
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a1 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glVertexPointer(alist->a0, type, stride, alist->a2->v.obj);
    }
    else {
	if (alist->a2 == null)
	    offset = 0;
	else if (otype(alist->a2) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a2;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_SHORT:
		stride = alist->a1 << 1;
		break;
	    case GL_INT:		case GL_FLOAT:
		stride = alist->a1 << 2;
		break;
	    case GL_DOUBLE:
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glVertexPointer(alist->a0, type, stride, (oobject_t)offset);
    }
}

static void
native_NormalPointer(oobject_t list, oint32_t ac)
/* void gl.NormalPointer(int32_t stride,
			 (int8_t|int16_t|int32_t|float32_t|float64_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    oword_t				 offset;
    oword_t				 stride;
    obuffer_offset_t			*vector;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;

    if (alist->a1 && (otype(alist->a1) & t_vector)) {
	switch (otype(alist->a1)) {
	    case t_vector|t_int8:
		type = GL_BYTE;
		stride = alist->a0;
		break;
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a0 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a0 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a0 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a0 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glNormalPointer(type, stride, alist->a1->v.obj);
    }
    else {
	if (alist->a1 == null)
	    offset = 0;
	else if (otype(alist->a1) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a1;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_BYTE:
		stride = alist->a0;
		break;
	    case GL_SHORT:
		stride = alist->a0 << 1;
		break;
	    case GL_INT:		case GL_FLOAT:
		stride = alist->a0 << 2;
		break;
	    case GL_DOUBLE:
		stride = alist->a0 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glNormalPointer(type, stride, (oobject_t)offset);
    }
}

static void
native_ColorPointer(oobject_t list, oint32_t ac)
/* void gl.ColorPointer(int32_t size, int32_t stride,
			(int8_t|uint8_t|int16_t|t_uint16_t|int32_t|t_uint32_t|
			 float32_t|float64_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    oword_t				 offset;
    oword_t				 stride;
    obuffer_offset_t			*vector;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a2 && (otype(alist->a2) & t_vector)) {
	if (alist->a0 < 3 || alist->a0 > 4)	    /* size */
	    ovm_raise(except_invalid_argument);
	CHECK_VECTOR(alist->a2);
	switch (otype(alist->a2)) {
	    case t_vector|t_int8:
		type = GL_BYTE;
		stride = alist->a1;
		break;
	    case t_vector|t_uint8:
		type = GL_UNSIGNED_BYTE;
		stride = alist->a1;
		break;
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a1 << 1;
		break;
	    case t_vector|t_uint16:
		type = GL_UNSIGNED_SHORT;
		stride = alist->a1 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_uint32:
		type = GL_UNSIGNED_INT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glColorPointer(alist->a0, type, stride, alist->a2->v.obj);
    }
    else {
	if (alist->a2 == null)
	    offset = 0;
	else if (otype(alist->a2) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a2;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_BYTE:		case GL_UNSIGNED_BYTE:
		stride = alist->a1;
		break;
	    case GL_SHORT:		case GL_UNSIGNED_SHORT:
		stride = alist->a1 << 1;
		break;
	    case GL_INT:		case GL_UNSIGNED_INT:
	    case GL_FLOAT:
		stride = alist->a1 << 2;
		break;
	    case GL_DOUBLE:
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glColorPointer(alist->a0, type, stride, (oobject_t)offset);
    }
}

static void
native_IndexPointer(oobject_t list, oint32_t ac)
/* void gl.IndexPointer(int32_t stride,
			(uint8_t|int16_t|int32_t|float32_t|float64_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    oword_t				 offset;
    oword_t				 stride;
    obuffer_offset_t			*vector;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a1 && (otype(alist->a1) & t_vector)) {
	switch (otype(alist->a1)) {
	    case t_vector|t_uint8:
		type = GL_UNSIGNED_BYTE;
		stride = alist->a0;
		break;
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a0 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a0 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a0 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a0 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glIndexPointer(type, stride, alist->a1->v.obj);
    }
    else {
	if (alist->a1 == null)
	    offset = 0;
	else if (otype(alist->a1) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a1;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_UNSIGNED_BYTE:
		stride = alist->a0;
		break;
	    case GL_SHORT:
		stride = alist->a0 << 1;
		break;
	    case GL_INT:		case GL_FLOAT:
		stride = alist->a0 << 2;
		break;
	    case GL_DOUBLE:
		stride = alist->a0 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glIndexPointer(type, alist->a0, (oobject_t)offset);
    }
}

static void
native_TexCoordPointer(oobject_t list, oint32_t ac)
/* void gl.TexCoordPointer(int32_t size, int32_t stride,
			   (int16_t|t_uint16_t|int32_t|
			    float32_t|float64_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    oword_t				 offset;
    oword_t				 stride;
    obuffer_offset_t			*vector;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;

    if (alist->a2 && (otype(alist->a2) & t_vector)) {
	if (alist->a0 < 2 || alist->a0 > 4)	    /* size */
	    ovm_raise(except_invalid_argument);
	switch (otype(alist->a2)) {
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a1 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a1 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glTexCoordPointer(alist->a0, type, stride, alist->a2->v.obj);
    }
    else {
	if (alist->a2 == null)
	    offset = 0;
	else if (otype(alist->a2) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a2;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_SHORT:
		stride = alist->a1 << 1;
		break;
	    case GL_INT:		case GL_FLOAT:
		stride = alist->a1 << 2;
		break;
	    case GL_DOUBLE:
		stride = alist->a1 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glTexCoordPointer(alist->a0, type, alist->a1, (oobject_t)offset);
    }
}

static void
native_EdgeFlagPointer(oobject_t list, oint32_t ac)
/* void gl.EdgeFlagPointer(int32_t stride, uint8_t ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a1 == null) {
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	glEdgeFlagPointer(alist->a0, null);
    }
    else {
	CHECK_TYPE(alist->a1, t_vector|t_uint8);
	glEdgeFlagPointer(alist->a0, alist->a1->v.obj);
    }
}

static void
native_ArrayElement(oobject_t list, oint32_t ac)
/* void gl.ArrayElement(int32_t i); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_t				*alist;

    alist = (nat_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glArrayElement(alist->a0);
}

static void
native_DrawArrays(oobject_t list, oint32_t ac)
/* void gl.DrawArrays(uint32_t mode, int32_t first, int32_t count); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_t			*alist;

    alist = (nat_u32_i32_i32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDrawArrays(alist->a0, alist->a1, alist->a2);
}

static void
native_DrawElements(oobject_t list, oint32_t ac)
/* void gl.DrawElements(uint32_t mode, (uint8_t|t_uint16_t|uint32_t) ptr[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oint32_t				 type;
    nat_i32_vec_t			*alist;

    /* FIXME somehow keep track of vector length and cause
     * a failure if glArrayElement, etc is called with an
     * invalid index/length */
    alist = (nat_i32_vec_t *)list;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1)) {
	case t_vector|t_uint8:
	    type = GL_UNSIGNED_BYTE;
	    break;
	case t_vector|t_uint16:
	    type = GL_UNSIGNED_SHORT;
	    break;
	case t_vector|t_uint32:
	    type = GL_UNSIGNED_INT;
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDrawElements(alist->a0, alist->a1->length, type, alist->a1->v.obj);
}

static void
native_ShadeModel(oobject_t list, oint32_t ac)
/* void gl.ShadeModel(uint32_t mode); */
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
/* void gl.Light(uint32_t light, uint32_t pname, float32_t param); */
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
/* void gl.Lightv(uint32_t light, uint32_t pname,
		  (int32_t|float32_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    CHECK_LENGTH(alist->a2, length);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glLightiv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    glLightfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetLightv(oobject_t list, oint32_t ac)
/* void gl.GetLightv(uint32_t light, uint32_t pname,
		     (int32_t|float32_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetLightiv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetLightfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_LightModel(oobject_t list, oint32_t ac)
/* void gl.LightModel(uint32_t pname, float32_t param); */
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
/* void gl.LightModelv(uint32_t pname, (int32_t|float32_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, length);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glLightModeliv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glLightModelfv(alist->a0, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Material(oobject_t list, oint32_t ac)
/* void gl.Material(uint32_t face, uint32_t pname, float32_t param); */
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
/* void gl.Materialv(uint32_t face, uint32_t pname,
		     (int32_t|float32_t) paramm[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    CHECK_LENGTH(alist->a2, length);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glMaterialiv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    glMaterialfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetMaterialv(oobject_t list, oint32_t ac)
/* void gl.GetMaterialv(uint32_t face, uint32_t pname,
			(int32_t|float32_t) paramm[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetMaterialiv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetMaterialfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_ColorMaterial(oobject_t list, oint32_t ac)
/* void gl.ColorMaterial(uint32_t face, uint32_t mode); */
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
/* void gl.PixelZoom(float32_t xfactor, float32_t yfactor); */
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
/* void gl.PixelStore(uint32_t pname, int32_t param); */
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
/* void gl.PixelTransfer(uint32_t pname, float32_t param); */
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
/* void gl.PixelMapfv(uint32_t map, (uint16_t|uint32_t|float32_t) values[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1)) {
	case t_vector|t_uint16:
	    glPixelMapusv(alist->a0, alist->a1->length, alist->a1->v.u16);
	    break;
	case t_vector|t_uint32:
	    glPixelMapuiv(alist->a0, alist->a1->length, alist->a1->v.u32);
	    break;
	case t_vector|t_float32:
	    glPixelMapfv(alist->a0, alist->a1->length, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetPixelMapv(oobject_t list, oint32_t ac)
/* void gl.GetPixelMapv(uint32_t map,
			(uint16_t|uint32_t|float32_t) values[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    int					 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glGetIntegerv(alist->a0, &length);
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
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1)) {
	case t_vector|t_uint16:
	    if (alist->a1->length != length)
		orenew_vector(alist->a1, length);
	    glGetPixelMapusv(alist->a0, alist->a1->v.u16);
	    break;
	case t_vector|t_uint32:
	    if (alist->a1->length != length)
		orenew_vector(alist->a1, length);
	    glGetPixelMapuiv(alist->a0, alist->a1->v.u32);
	    break;
	case t_vector|t_float32:
	    if (alist->a1->length != length)
		orenew_vector(alist->a1, length);
	    glGetPixelMapfv(alist->a0, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Bitmap(oobject_t list, oint32_t ac)
/* void gl.Bitmap(int32_t width, int32_t height,
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
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a6, length);
    r0->t = t_void;
    glBitmap(alist->a0, alist->a1, alist->a2, alist->a3,
	     alist->a4, alist->a5, alist->a6->v.u8);
}

static void
native_ReadPixels(oobject_t list, oint32_t ac)
/* void gl.ReadPixels(int32_t x, int32_t y, int32_t width, int32_t height,
		      uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_uint8);
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
/* void gl.ReadBitmap(int32_t x, int32_t y, int32_t width, int32_t height,
		      uint32_t format, uint8_t bitmap[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_i32_i32_i32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_uint8);
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
/* void gl.DrawPixels(int32_t width, int32_t height,
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
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a3, length);
    r0->t = t_void;
    glDrawPixels(alist->a0, alist->a1, alist->a2, GL_UNSIGNED_BYTE,
		 alist->a3->v.u8);
}

static void
native_DrawBitmap(oobject_t list, oint32_t ac)
/* void gl.DrawBitmap(int32_t width, int32_t height,
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

    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a3, length);
    r0->t = t_void;
    glDrawPixels(alist->a0, alist->a1, alist->a2, GL_BITMAP, alist->a3->v.u8);
}

static void
native_CopyPixels(oobject_t list, oint32_t ac)
/* void gl.CopyPixels(int32_t x, int32_t y,
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
/* void gl.StencilFunc(uint32_t func, int32_t ref, uint32_t mask); */
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
/* void gl.StencilMask(uint32_t mask); */
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
/* void gl.StencilOp(uint32_t fail uint32_t zfail, uint32_t zpass); */
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
/* void gl.ClearStencil(int32_t s); */
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
/* void gl.TexGen(uint32_t coord, uint32_t pname, float64_t param); */
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
/* void gl.TexGenv(uint32_t coord, uint32_t pname,
		   (int32_t|float32_t|float64_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    CHECK_LENGTH(alist->a2, length);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glTexGeniv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexGenfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	case t_vector|t_float64:
	    glTexGendv(alist->a0, alist->a1, alist->a2->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetTexGenv(oobject_t list, oint32_t ac)
/* void gl.GetTexGenv(uint32_t coord, uint32_t pname, float64_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexGeniv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexGenfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	case t_vector|t_float64:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexGendv(alist->a0, alist->a1, alist->a2->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_TexEnv(oobject_t list, oint32_t ac)
/* void gl.TexEnv(uint32_t target, uint32_t pname, float32_t param); */
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
/* void gl.TexEnvv(uint32_t target, uint32_t pname,
		   (int32_t|float32_t) param[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    if (alist->a2 == null ||
	!(otype(alist->a2) & t_vector) || alist->a2->length != length)
	ovm_raise(except_invalid_argument);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glTexEnviv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexEnvfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

static void
native_GetTexEnvv(oobject_t list, oint32_t ac)
/* void gl.GetTexEnvv(uint32_t target, uint32_t pname,
		      (int32_t|float32_t) param[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexEnviv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexEnvfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_TexParameter(oobject_t list, oint32_t ac)
/* void gl.TexParameter(uint32_t target, uint32_t pname, float32_t param); */
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
/* void gl.TexParameterv(uint32_t target, uint32_t pname,
			 (int32_t|float32_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    CHECK_LENGTH(alist->a2, length);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glTexParameteriv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    glTexParameterfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetTexParameterv(oobject_t list, oint32_t ac)
/* void gl.GetTexParameterv(uint32_t target, uint32_t pname,
			    (int32_t|float32_t) params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;
    oword_t				 length;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexParameteriv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_float32:
	    if (alist->a2->length != length)
		orenew_vector(alist->a2, length);
	    glGetTexParameterfv(alist->a0, alist->a1, alist->a2->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetTexLevelParameter(oobject_t list, oint32_t ac)
/* int32_t gl.GetTexLevelParameter(uint32_t target,
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
/* void gl.TexImage1D(uint32_t target, int32_t level,
		      int32_t width, int32_t border, uint32_t format,
		      uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 format;
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
	case GL_DEPTH_COMPONENT:
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    check_mult(alist->a2, format);
    length = alist->a2 * format;
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a5, length);
    r0->t = t_void;
    glTexImage1D(alist->a0, alist->a1, format, alist->a2, alist->a3,
		 alist->a4, GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_TexImage2D(oobject_t list, oint32_t ac)
/* void gl.TexImage2D(uint32_t target, int32_t level,
		      int32_t width, int32_t height, int32_t border,
		      uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 format;
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
	case GL_DEPTH_COMPONENT:
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    check_mult(alist->a2, alist->a3);
    length = alist->a2 * alist->a3;
    check_mult(length, format);
    length *= format;
    CHECK_NULL(alist->a6);
    CHECK_TYPE(alist->a6, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a6, length);
    r0->t = t_void;
    glTexImage2D(alist->a0, alist->a1, format, alist->a2, alist->a3,
		 alist->a4, alist->a5, GL_UNSIGNED_BYTE, alist->a6->v.u8);
}

static void
native_GetTexImage(oobject_t list, oint32_t ac)
/* void gl.GetTexImage(uint32_t target, int32_t level, uint32_t format,
		       uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_u32_vec_t		*alist;
    oword_t				 width;
    oword_t				 height;
    oword_t				 format;
    oword_t				 length;
    GLint				 value[4];

    alist = (nat_u32_i32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    height = 1;
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
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
	    break;
    }
    check_mult(width, height);
    length = width * height;
    check_mult(length, format);
    length *= format;
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint8);
    if (alist->a3->length != length)
	orenew_vector(alist->a3, length);
    r0->t = t_void;
    glGetTexImage(alist->a0, alist->a1, alist->a2, GL_UNSIGNED_BYTE,
		  alist->a3->v.u8);
}

static void
native_GenTextures(oobject_t list, oint32_t ac)
/* void gl.GenTextures(uint32_t textures[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    r0->t = t_void;
    glGenTextures(alist->a0->length, alist->a0->v.u32);
}

static void
native_DeleteTextures(oobject_t list, oint32_t ac)
/* void gl.DeleteTextures(uint32_t textures[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    r0->t = t_void;
    glDeleteTextures(alist->a0->length, alist->a0->v.u32);
}

static void
native_BindTexture(oobject_t list, oint32_t ac)
/* void gl.BindTexture(uint32_t target, uint32_t texture); */
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
/* void gl.PrioritizeTextures(uint32_t textures[], float32_t priorities[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_vec_t			*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
    CHECK_LENGTH(alist->a0, alist->a1->length);
    r0->t = t_void;
    glPrioritizeTextures(alist->a0->length, alist->a0->v.u32, alist->a1->v.f32);
}

static void
native_AreTexturesResident(oobject_t list, oint32_t ac)
/* void gl.AreTexturesResident(uint32_t textures[], uint8_t residences[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_vec_t			*alist;

    alist = (nat_vec_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint8);
    CHECK_LENGTH(alist->a0, alist->a1->length);
    r0->t = t_void;
    glAreTexturesResident(alist->a0->length, alist->a0->v.u32, alist->a1->v.u8);
}

static void
native_IsTexture(oobject_t list, oint32_t ac)
/* uint8_t gl.IsTexture(uint32_t texture); */
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
/* void gl.TexSubImage1D(uint32_t target, int32_t level, int32_t xoffset,
			 int32_t width, uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 format;
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
	case GL_DEPTH_COMPONENT:
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    check_mult(length, format);
    length *= format;
    CHECK_NULL(alist->a5);
    CHECK_TYPE(alist->a5, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a5, length);
    r0->t = t_void;
    glTexSubImage1D(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4,
		    GL_UNSIGNED_BYTE, alist->a5->v.u8);
}

static void
native_TexSubImage2D(oobject_t list, oint32_t ac)
/* void gl.TexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
			 int32_t yoffset, int32_t width, int32_t height,
			 uint32_t format, uint8_t pixels[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_i32_i32_i32_i32_i32_u32_vec_t	*alist;
    oword_t				 format;
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
	case GL_DEPTH_COMPONENT:
	    format = 1;
	    break;
	case GL_LUMINANCE_ALPHA:
	    format = 2;
	    break;
	case GL_RGB:
	case GL_BGR:
	    format = 3;
	    break;
	case GL_RGBA:
	case GL_BGRA:
	    format = 4;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    check_mult(length, format);
    length *= format;
    CHECK_NULL(alist->a7);
    CHECK_TYPE(alist->a7, t_vector|t_uint8);
    CHECK_BOUNDS(alist->a7, length);
    r0->t = t_void;
    glTexSubImage2D(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4,
		    alist->a5, alist->a6, GL_UNSIGNED_BYTE, alist->a7->v.u8);
}

static void
native_CopyTexImage1D(oobject_t list, oint32_t ac)
/* void gl.CopyTexImage1D(uint32_t target, int32_t level, int32_t format,
			  int32_t x, int32_t y, int32_t width,
			  int32_t border); */
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
/* void gl.CopyTexImage2D(uint32_t target, int32_t level, int32_t format,
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
/* void gl.CopyTexSubImage1D(uint32_t target, int32_t level, int32_t xoffset,
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
/* void gl.CopyTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
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
/* void gl.Map1(uint32_t target, float64_t u1, float64_t u2,
		int32_t order, (float32_t|float64_t) points[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 mult;
    nat_u32_f64_f64_i32_vec_t		*alist;
    oword_t				 length;

    alist = (nat_u32_f64_f64_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a4);
    CHECK_VECTOR(alist->a4);
    CHECK_LENGTH(alist->a4, length);
    switch (otype(alist->a4)) {
	case t_vector|t_float32:
	    glMap1f(alist->a0, alist->a1, alist->a2, 0,
		    alist->a3, alist->a4->v.f32);
	    break;
	case t_vector|t_float64:
	    glMap1d(alist->a0, alist->a1, alist->a2, 0,
		    alist->a3, alist->a4->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Map2(oobject_t list, oint32_t ac)
/* void gl.Map2(uint32_t target,
		float64_t u1, float64_t u2, int32_t uorder,
		float64_t v1, float64_t v2, int32_t vorder,
		(float32_t|float64_t) points[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 mult;
    nat_u32_f64_f64_i32_f64_f64_i32_vec_t	*alist;
    oword_t				 length;

    alist = (nat_u32_f64_f64_i32_f64_f64_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
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
    CHECK_NULL(alist->a7);
    CHECK_VECTOR(alist->a7);
    CHECK_LENGTH(alist->a7, length);
    switch (otype(alist->a7)) {
	case t_vector|t_float32:
	    glMap2f(alist->a0, alist->a1, alist->a2, 0, alist->a3,
		    alist->a4, alist->a5, 0, alist->a6, alist->a7->v.f32);
	    break;
	case t_vector|t_float64:
	    glMap2d(alist->a0, alist->a1, alist->a2, 0, alist->a3,
		    alist->a4, alist->a5, 0, alist->a6, alist->a7->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
GetMapCoeff(uint32_t target, ovector_t *v)
{
    oword_t				 dim;
    oword_t				 mult;
    oword_t				 length;
    GLint				 order[2];

    switch (target) {
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
    glGetMapiv(target, GL_ORDER, order);
    length = order[0];
    if (dim == 2) {
	check_mult(length, order[1]);
	length *= order[1];
    }
    check_mult(length, mult);
    length *= mult;
    switch (otype(v)) {
	case t_vector|t_int32:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapiv(target, GL_COEFF, v->v.i32);
	    break;
	case t_vector|t_float32:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapfv(target, GL_COEFF, v->v.f32);
	    break;
	case t_vector|t_float64:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapdv(target, GL_COEFF, v->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
GetMapOrder(uint32_t target, ovector_t *v)
{
    oword_t				 order;

    switch (target) {
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
    switch (otype(v)) {
	case t_vector|t_int32:
	    if (v->length != order)
		orenew_vector(v, order);
	    glGetMapiv(target, GL_ORDER, v->v.i32);
	    break;
	case t_vector|t_float32:
	    if (v->length != order)
		orenew_vector(v, order);
	    glGetMapfv(target, GL_ORDER, v->v.f32);
	    break;
	case t_vector|t_float64:
	    if (v->length != order)
		orenew_vector(v, order);
	    glGetMapdv(target, GL_ORDER, v->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
GetMapDomain(uint32_t target, ovector_t *v)
{
    oword_t				 length;

    switch (target) {
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
    switch (otype(v)) {
	case t_vector|t_int32:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapiv(target, GL_DOMAIN, v->v.i32);
	    break;
	case t_vector|t_float32:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapfv(target, GL_DOMAIN, v->v.f32);
	    break;
	case t_vector|t_float64:
	    if (v->length != length)
		orenew_vector(v, length);
	    glGetMapdv(target, GL_DOMAIN, v->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GetMapv(oobject_t list, oint32_t ac)
/* void gl.GetMapv(uint32_t target, uint32_t query,
		   (int32_t|float32_t|float64_t) v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (alist->a1) {
	case GL_COEFF:
	    GetMapCoeff(alist->a0, alist->a2);
	    break;
	case GL_ORDER:
	    GetMapOrder(alist->a0, alist->a2);
	    break;
	case GL_DOMAIN:
	    GetMapDomain(alist->a0, alist->a2);
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
}

static void
native_EvalCoord1(oobject_t list, oint32_t ac)
/* void gl.EvalCoord1(float64_t u); */
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
/* void gl.EvalCoord2(float64_t u, float64_t v); */
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
/* void gl.MapGrid1(int32_t un, float64_t u1, float64_t u2); */
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
/* void gl.MapGrid2(int32_t un, float64_t u1, float64_t u2,
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
/* void gl.EvalPoint1(int32_t i); */
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
/* void gl.EvalPoint2(int32_t i, int32_t j); */
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
/* void gl.EvalMesh1(uint32_t mode, int32_t i1, int32_t i2); */
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
/* void gl.EvalMesh2(uint32_t mode, int32_t i1, int32_t i2,
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

static void
native_Fog(oobject_t list, oint32_t ac)
/* void gl.Fog(uint32_t pname, float32_t param); */
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
/* void gl.Fogv(uint32_t pname, (int32_t|float32_t) param[4]); */
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
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, length);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glFogiv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glFogfv(alist->a0, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_FeedbackBuffer(oobject_t list, oint32_t ac)
/* void gl.FeedbackBuffer(uint32_t type, float32_t buffer[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_float32);
    glFeedbackBuffer(alist->a1->length, alist->a0, alist->a1->v.f32);
}

static void
native_PassThrough(oobject_t list, oint32_t ac)
/* void gl.PassThrough(float32_t token); */
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
/* void gl.SelectBuffer(uint32_t buffer[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    glSelectBuffer(alist->a0->length, alist->a0->v.u32);
}

static void
native_InitNames(oobject_t list, oint32_t ac)
/* void gl.InitNames(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glInitNames();
}

static void
native_LoadName(oobject_t list, oint32_t ac)
/* void gl.LoadName(uint32_t name); */
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
/* void gl.PushName(uint32_t name); */
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
/* void gl.PopName(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_void;
    glPopName();
}

static void
native_ActiveTexture(oobject_t list, oint32_t ac)
/* void gl.ActiveTexture(uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glActiveTexture(alist->a0);
}

static void
native_ClientActiveTexture(oobject_t list, oint32_t ac)
/* void gl.ClientActiveTexture(uint32_t texture); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glClientActiveTexture(alist->a0);
}

static void
native_MultiTexCoord1(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord1(uint32_t target, float64_t s); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_t			*alist;

    alist = (nat_u32_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord1d(alist->a0, alist->a1);
}

static void
native_MultiTexCoord1v(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord1v(uint32_t target,
			   (int16_t|int32_t|float32_t|float64_t) v[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 1);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glMultiTexCoord1sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_int32:
	    glMultiTexCoord1iv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glMultiTexCoord1fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glMultiTexCoord1dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_MultiTexCoord2(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord2(uint32_t target, float64_t s, float64_t t); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_t			*alist;

    alist = (nat_u32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord2d(alist->a0, alist->a1, alist->a2);
}

static void
native_MultiTexCoord2v(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord2v(uint32_t target,
			   (int16_t|int32_t|float32_t|float64_t) v[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 2);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glMultiTexCoord2sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_int32:
	    glMultiTexCoord2iv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glMultiTexCoord2fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glMultiTexCoord2dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_MultiTexCoord3(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord3(uint32_t target, float64_t s,
			  float64_t t, float64_t r); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord3d(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_MultiTexCoord3v(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord3v(uint32_t target,
			   (int16_t|int32_t|float32_t|float64_t) v[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 3);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glMultiTexCoord3sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_int32:
	    glMultiTexCoord3iv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glMultiTexCoord3fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glMultiTexCoord3dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_MultiTexCoord4(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord4(uint32_t target, float64_t s,
			  float64_t t, float64_t r, float64_t q); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glMultiTexCoord4d(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}

static void
native_MultiTexCoord4v(oobject_t list, oint32_t ac)
/* void gl.MultiTexCoord4v(uint32_t target,
			   (int16_t|int32_t|float32_t|float64_t) v[4]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 4);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glMultiTexCoord4sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_int32:
	    glMultiTexCoord4iv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glMultiTexCoord4fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glMultiTexCoord4dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_FogCoord(oobject_t list, oint32_t ac)
/* void gl.FogCoord(float64_t coord); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_f64_t				*alist;

    alist = (nat_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glFogCoordd(alist->a0);
}

static void
native_WindowPos2(oobject_t list, oint32_t ac)
/* void gl.WindowPos2(float64_t x, float64_t y); */
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
/* void gl.WindowPos2v((int16_t|int32_t|float32_t|float64_t) xy[2]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;

    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 2);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glWindowPos2sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glWindowPos2iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glWindowPos2fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glWindowPos2dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_WindowPos3(oobject_t list, oint32_t ac)
/* void gl.WindowPos3(float64_t x, float64_t y, float64_t z); */
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
/* void gl.WindowPos3v((int16_t|int32_t|float32_t|float64_t) xy[3]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_VECTOR(alist->a0);
    CHECK_LENGTH(alist->a0, 3);
    switch (otype(alist->a0)) {
	case t_vector|t_int16:
	    glWindowPos3sv(alist->a0->v.i16);
	    break;
	case t_vector|t_int32:
	    glWindowPos3iv(alist->a0->v.i32);
	    break;
	case t_vector|t_float32:
	    glWindowPos3fv(alist->a0->v.f32);
	    break;
	case t_vector|t_float64:
	    glWindowPos3dv(alist->a0->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_GenQueries(oobject_t list, oint32_t ac)
/* void gl.GenQueries(uint32_t ids[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    r0->t = t_void;
    glGenQueries(alist->a0->length, alist->a0->v.u32);
}

static void
native_DeleteQueries(oobject_t list, oint32_t ac)
/* void gl.DeleteQueries(uint32_t ids[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    r0->t = t_void;
    glDeleteQueries(alist->a0->length, alist->a0->v.u32);
}

static void
native_IsQuery(oobject_t list, oint32_t ac)
/* uint8_t gl.IsQuery(uint32_t id); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsQuery(alist->a0);
}

static void
native_BeginQuery(oobject_t list, oint32_t ac)
/* void gl.BeginQuery(uint32_t target, uint32_t id); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glBeginQuery(alist->a0, alist->a1);
}

static void
native_EndQuery(oobject_t list, oint32_t ac)
/* void gl.EndQuery(uint32_t target); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEndQuery(alist->a0);
}

static void
native_GetQueryv(oobject_t list, oint32_t ac)
/* void gl.GetQueryv(uint32_t target, uint32_t pname, int32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_uint32);
    CHECK_LENGTH(alist->a2, 1);
    r0->t = t_void;
    glGetQueryiv(alist->a0, alist->a1, alist->a2->v.i32);
}

static void
native_GetQueryObjectv(oobject_t list, oint32_t ac)
/* void gl.GetQueryObjectv(uint32_t target, uint32_t pname,
			   int32_t params[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    CHECK_LENGTH(alist->a2, 1);
    switch (otype(alist->a2)) {
	case t_vector|t_int32:
	    glGetQueryObjectiv(alist->a0, alist->a1, alist->a2->v.i32);
	    break;
	case t_vector|t_uint32:
	    glGetQueryObjectuiv(alist->a0, alist->a1, alist->a2->v.u32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    r0->t = t_void;
}

static void
native_BindBuffer(oobject_t list, oint32_t ac)
/* void gl.BindBuffer(uint32_t target, uint32_t buffer); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;

    alist = (nat_u32_u32_t *)list;
    check.nt = t_word;
    check.nv.w = alist->a1;
    okey_hashentry(&check);
    if ((entry = oget_hashentry(buffer_table, &check)) == null) {
	onew_object((oobject_t *)&thread_self->obj,
		    t_hashentry, sizeof(ohashentry_t));
	entry = thread_self->obj;
	entry->key = check.key;
	entry->nt = t_word;
	entry->nv.w = alist->a1;
	oput_hashentry(buffer_table, entry);
	entry->vt = t_void;
	onew_object(&entry->vv.o, t_word, BUFFER_VECTOR_SIZE);
    }

    r0 = &thread_self->r0;
    r0->t = t_void;
    glBindBuffer(alist->a0, alist->a1);
}

static void
native_GenBuffers(oobject_t list, oint32_t ac)
/* void gl.GenBuffers(uint32_t ids[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    r0->t = t_void;
    glGenBuffers(alist->a0->length, alist->a0->v.u32);
}

static void
native_DeleteBuffers(oobject_t list, oint32_t ac)
/* void gl.DeleteBuffers(uint32_t ids[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;
    oword_t				 offset;
    ohashentry_t			 check;
    ohashentry_t			*entry;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    check.nt = t_word;
    for (offset = 0; offset < alist->a0->length; offset++) {
	check.nv.w = alist->a0->v.u32[offset];
	okey_hashentry(&check);
	if ((entry = oget_hashentry(buffer_table, &check)))
	    orem_hashentry(buffer_table, entry);
    }
    r0->t = t_void;
    glDeleteBuffers(alist->a0->length, alist->a0->v.u32);
}

static void
native_IsBuffer(oobject_t list, oint32_t ac)
/* uint8_t gl.IsBuffer(uint32_t id); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsBuffer(alist->a0);
}

static void
native_BufferData(oobject_t list, oint32_t ac)
/* void gl.BufferData(uint32_t target, (int8_t|uint8_t|int16_t|uint16_t|int32_t|
					uint32_t|float32_t|float64_t) data[],
		      uint32_t usage); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 size;
    oword_t				 type;
    nat_u32_vec_u32_t			*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oint32_t				 buffer;
    obuffer_offset_t			*vector;

    alist = (nat_u32_vec_u32_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1)) {
	case t_vector|t_int8:
#if __WORDSIZE == 64
	    if (alist->a1->length > INT_MAX)
		ovm_raise(except_out_of_bounds);
#endif
	    type = GL_BYTE;
	    size = alist->a1->length;
	    break;
	case t_vector|t_uint8:
#if __WORDSIZE == 64
	    if (alist->a1->length > INT_MAX)
		ovm_raise(except_out_of_bounds);
#endif
	    type = GL_UNSIGNED_BYTE;
	    size = alist->a1->length;
	    break;
	case t_vector|t_int16:
	    if (alist->a1->length > INT_MAX >> 1)
		ovm_raise(except_out_of_bounds);
	    type = GL_SHORT;
	    size = alist->a1->length << 1;
	    break;
	case t_vector|t_uint16:
	    if (alist->a1->length > INT_MAX >> 1)
		ovm_raise(except_out_of_bounds);
	    type = GL_UNSIGNED_SHORT;
	    size = alist->a1->length << 1;
	    break;
	case t_vector|t_int32:
	    if (alist->a1->length > INT_MAX >> 2)
		ovm_raise(except_out_of_bounds);
	    type = GL_INT;
	    size = alist->a1->length << 2;
	    break;
	case t_vector|t_uint32:
	    if (alist->a1->length > INT_MAX >> 2)
		ovm_raise(except_out_of_bounds);
	    type = GL_UNSIGNED_INT;
	    size = alist->a1->length << 2;
	    break;
	case t_vector|t_float32:
	    if (alist->a1->length > INT_MAX >> 2)
		ovm_raise(except_out_of_bounds);
	    type = GL_FLOAT;
	    size = alist->a1->length << 2;
	    break;
	case t_vector|t_float64:
	    if (alist->a1->length > INT_MAX >> 3)
		ovm_raise(except_out_of_bounds);
	    type = GL_DOUBLE;
	    size = alist->a1->length << 3;
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    buffer = -1;
    if (unlikely(alist->a0 == GL_ELEMENT_ARRAY_BUFFER))
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer);
    else
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
    /* No buffer bound */
    if (!glIsBuffer(buffer))
	ovm_raise(except_invalid_argument);
    check.nt = t_word;
    check.nv.w = buffer;
    okey_hashentry(&check);
    /* Should never fail, unless buffer was bound not using the
     * gl interface what is not supported */
    if ((entry = oget_hashentry(buffer_table, &check)) == null)
	ovm_raise(except_invalid_argument);
    vector = entry->vv.o;
    switch (alist->a0) {
	case GL_ARRAY_BUFFER:
	    vector[ARRAY_BUFFER_OFFSET] = type;
	    vector[ARRAY_BUFFER_LENGTH] = alist->a1->length;
	    break;
	case GL_ELEMENT_ARRAY_BUFFER:
	    vector[ELEMENT_ARRAY_BUFFER_OFFSET] = type;
	    vector[ELEMENT_ARRAY_BUFFER_LENGTH] = alist->a1->length;
	    break;
	case GL_PIXEL_PACK_BUFFER:
	    vector[PIXEL_PACK_BUFFER_OFFSET] = type;
	    vector[PIXEL_PACK_BUFFER_LENGTH] = alist->a1->length;
	    break;
	case GL_PIXEL_UNPACK_BUFFER:
	    vector[PIXEL_UNPACK_BUFFER_OFFSET] = type;
	    vector[PIXEL_UNPACK_BUFFER_LENGTH] = alist->a1->length;
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    r0->t = t_void;
    glBufferData(alist->a0, size, alist->a1->v.obj, alist->a2);
}

static void
native_BufferSubData(oobject_t list, oint32_t ac)
/* void gl.BufferSubData(uint32_t target, word_t offset,
			 (int8_t|uint8_t|int16_t|uint16_t|int32_t|
			  uint32_t|float32_t|float64_t) data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 size;
    nat_u32_w_vec_t			*alist;

    alist = (nat_u32_w_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int8:	case t_vector|t_uint8:
	    size = alist->a2->length;
	    break;
	case t_vector|t_int16:	case t_vector|t_uint16:
	    size = alist->a2->length << 1;
	    if (alist->a1 & 1)		/* offset & 1 */
		ovm_raise(except_invalid_argument);
	    break;
	case t_vector|t_int32:	case t_vector|t_uint32:
	case t_vector|t_float32:
	    size = alist->a2->length << 2;
	    if (alist->a1 & 3)		/* offset & 3 */
		ovm_raise(except_invalid_argument);
	    break;
	case t_vector|t_float64:
	    size = alist->a2->length << 3;
	    if (alist->a1 & 7)		/* offset & 7 */
		ovm_raise(except_invalid_argument);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    r0->t = t_void;
    glBufferSubData(alist->a0, alist->a1, size, alist->a2->v.obj);
}

static void
native_GetBufferSubData(oobject_t list, oint32_t ac)
/* void gl.GetBufferSubData(uint32_t target, word_t offset,
			    (int8_t|uint8_t|int16_t|uint16_t|int32_t|
			     uint32_t|float32_t|float64_t) data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 size;
    nat_u32_w_vec_t			*alist;

    alist = (nat_u32_w_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_VECTOR(alist->a2);
    switch (otype(alist->a2)) {
	case t_vector|t_int8:	case t_vector|t_uint8:
	    size = alist->a2->length;
	    break;
	case t_vector|t_int16:	case t_vector|t_uint16:
	    size = alist->a2->length << 1;
	    if (alist->a1 & 1)		/* offset & 1 */
		ovm_raise(except_invalid_argument);
	    break;
	case t_vector|t_int32:	case t_vector|t_uint32:
	case t_vector|t_float32:
	    size = alist->a2->length << 2;
	    if (alist->a1 & 3)		/* offset & 3 */
		ovm_raise(except_invalid_argument);
	    break;
	case t_vector|t_float64:
	    size = alist->a2->length << 3;
	    if (alist->a1 & 7)		/* offset & 7 */
		ovm_raise(except_invalid_argument);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
    r0->t = t_void;
    glGetBufferSubData(alist->a0, alist->a1, size, alist->a2->v.obj);
}

static void
native_GetBufferParameterv(oobject_t list, oint32_t ac)
/* void gl.GetBufferParameterv(uint32_t target,
			       uint32_t value, int32_t data[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_uint32);
    CHECK_LENGTH(alist->a2, 1);
    r0->t = t_void;
    glGetBufferParameteriv(alist->a0, alist->a1, alist->a2->v.i32);
}

static void
native_BlendEquationSeparate(oobject_t list, oint32_t ac)
/* void gl.BlendEquationSeparate(uint32_t modeRGB, uint32_t modeAlpha); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glBlendEquationSeparate(alist->a0, alist->a1);
}

static void
native_DrawBuffers(oobject_t list, oint32_t ac)
/* void gl.DrawBuffers(uint32_t bufs[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_vec_t				*alist;

    alist = (nat_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a0);
    CHECK_TYPE(alist->a0, t_vector|t_uint32);
    glDrawBuffers(alist->a0->length, alist->a0->v.u32);
}

static void
native_StencilOpSeparate(oobject_t list, oint32_t ac)
/* void gl.StencilOpSeparate(uint32_t face, uint32_t sfail,
			     uint32_t dpfail, uint32_t dppass); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_u32_u32_t		*alist;

    alist = (nat_u32_u32_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilOpSeparate(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_StencilFuncSeparate(oobject_t list, oint32_t ac)
/* void gl.StencilFuncSeparate(uint32_t face, uint32_t func,
			       uint32_t ref, uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_u32_u32_t		*alist;

    alist = (nat_u32_u32_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilFuncSeparate(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_StencilMaskSeparate(oobject_t list, oint32_t ac)
/* void gl.StencilMaskSeparate(uint32_t face, uint32_t mask); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glStencilMaskSeparate(alist->a0, alist->a1);
}

static void
native_AttachShader(oobject_t list, oint32_t ac)
/* void gl.AttachShader(uint32_t program, uint32_t shader); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glAttachShader(alist->a0, alist->a1);
}

static void
native_BindAttribLocation(oobject_t list, oint32_t ac)
/* void gl.BindAttribLocation(uint32_t program,
			      uint32_t index, string_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_string);
    make_vec_text(alist->a2);
    glBindAttribLocation(alist->a0, alist->a1, alist->a2->v.obj);
}

static void
native_CompileShader(oobject_t list, oint32_t ac)
/* void gl.CompileShader(uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glCompileShader(alist->a0);
}

static void
native_CreateProgram(oobject_t list, oint32_t ac)
/* uint32_t gl.CreateProgram(); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glCreateProgram();
}

static void
native_CreateShader(oobject_t list, oint32_t ac)
/* uint32_t gl.CreateShader(uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glCreateShader(alist->a0);
}

static void
native_DeleteProgram(oobject_t list, oint32_t ac)
/* void gl.DeleteProgram(uint32_t program); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDeleteProgram(alist->a0);
}

static void
native_DeleteShader(oobject_t list, oint32_t ac)
/* void gl.DeleteShader(uint32_t shader); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDeleteShader(alist->a0);
}

static void
native_DetachShader(oobject_t list, oint32_t ac)
/* void gl.DetachShader(uint32_t program, uint32_t shader); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_t			*alist;

    alist = (nat_u32_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDetachShader(alist->a0, alist->a1);
}

static void
native_DisableVertexAttribArray(oobject_t list, oint32_t ac)
/* void gl.DisableVertexAttribArray(uint32_t index); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glDisableVertexAttribArray(alist->a0);
}

static void
native_EnableVertexAttribArray(oobject_t list, oint32_t ac)
/* void gl.EnableVertexAttribArray(uint32_t index); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glEnableVertexAttribArray(alist->a0);
}

static void
native_GetActiveAttrib(oobject_t list, oint32_t ac)
/* void gl.GetActiveAttrib(uint32_t program, uint32_t index,
			   int32_t size[1], uint32_t type[1],
			   string_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_vec_vec_t		*alist;
    oint32_t				 length;
    oint32_t				 size;
    ouint32_t				 type;
    char				 name[BUFSIZ];

    alist = (nat_u32_u32_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_int32);
    CHECK_LENGTH(alist->a2, 1);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint32);
    CHECK_LENGTH(alist->a3, 1);
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_string);
    glGetActiveAttrib(alist->a0, alist->a1,
		      BUFSIZ, &length, &size, &type, name);
    alist->a2->v.i32[0] = size;
    alist->a3->v.u32[0] = type;
    orenew_vector(alist->a4, length);
    memcpy(alist->a4->v.obj, name, length);
}

static void
native_GetActiveUniform(oobject_t list, oint32_t ac)
/* void gl.GetActiveUniform(uint32_t program, uint32_t index,
			    int32_t length[1], int32_t size[1],
			    uint32_t type[1], string_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_vec_vec_t		*alist;
    oint32_t				 length;
    oint32_t				 size;
    ouint32_t				 type;
    char				 name[BUFSIZ];

    alist = (nat_u32_u32_vec_vec_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_int32);
    CHECK_LENGTH(alist->a2, 1);
    CHECK_NULL(alist->a3);
    CHECK_TYPE(alist->a3, t_vector|t_uint32);
    CHECK_LENGTH(alist->a3, 1);
    CHECK_NULL(alist->a4);
    CHECK_TYPE(alist->a4, t_string);
    glGetActiveUniform(alist->a0, alist->a1,
		       BUFSIZ, &length, &size, &type, name);
    alist->a2->v.i32[0] = size;
    alist->a3->v.u32[0] = type;
    orenew_vector(alist->a4, length);
    memcpy(alist->a4->v.obj, name, length);
}

static void
native_GetAttachedShaders(oobject_t list, oint32_t ac)
/* void gl.GetAttachedShaders(uint32_t program, uint32_t shaders[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oint32_t				 count;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_vector|t_uint32);
    if (!glIsProgram(alist->a0))
	ovm_raise(except_invalid_argument);
    count = 0;
    glGetProgramiv(alist->a0, GL_ATTACHED_SHADERS, &count);
    orenew_vector(alist->a1, count);
    glGetAttachedShaders(alist->a0, count, null, alist->a1->v.u32);
}

static void
native_GetAttribLocation(oobject_t list, oint32_t ac)
/* int32_t gl.GetAttribLocation(uint32_t program, string_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    make_vec_text(alist->a1);
    r0->v.w = glGetAttribLocation(alist->a0, alist->a1->v.obj);
}

static void
native_GetProgramv(oobject_t list, oint32_t ac)
/* void gl.GetProgramv(uint32_t shader, uint32_t pname, int32_t params[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_int32);
    CHECK_LENGTH(alist->a2, 1);
    r0->t = t_void;
    switch (alist->a1) {
	case GL_DELETE_STATUS:
	case GL_LINK_STATUS:
	case GL_VALIDATE_STATUS:
	case GL_INFO_LOG_LENGTH:
	case GL_ATTACHED_SHADERS:
	case GL_ACTIVE_ATTRIBUTES:
	case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
	case GL_ACTIVE_UNIFORMS:
	case GL_ACTIVE_UNIFORM_MAX_LENGTH:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    glGetProgramiv(alist->a0, alist->a1, alist->a2->v.i32);
}

static void
native_GetProgramInfoLog(oobject_t list, oint32_t ac)
/* void gl.GetProgramInfoLog(uint32_t shader, string_t msg[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oint32_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    if (!glIsProgram(alist->a0))
	ovm_raise(except_invalid_argument);
    glGetProgramiv(alist->a0, GL_INFO_LOG_LENGTH, &length);
    orenew_vector(alist->a1, length - 1);
    glGetProgramInfoLog(alist->a0, length - 1, NULL, alist->a1->v.obj);
}

static void
native_GetShaderv(oobject_t list, oint32_t ac)
/* void gl.GetShaderv(uint32_t shader, uint32_t pname, int32_t params[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_u32_vec_t			*alist;

    alist = (nat_u32_u32_vec_t *)list;
    r0 = &thread_self->r0;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_int32);
    CHECK_LENGTH(alist->a2, 1);
    r0->t = t_void;
    switch (alist->a1) {
	case GL_SHADER_TYPE:
	case GL_DELETE_STATUS:
	case GL_COMPILE_STATUS:
	case GL_INFO_LOG_LENGTH:
	case GL_SHADER_SOURCE_LENGTH:
	    break;
	default:
	    ovm_raise(except_invalid_argument);
    }
    glGetShaderiv(alist->a0, alist->a1, alist->a2->v.i32);
}

static void
native_GetShaderInfoLog(oobject_t list, oint32_t ac)
/* void gl.GetShaderInfoLog(uint32_t shader, string_t msg[1]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oint32_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    if (!glIsShader(alist->a0))
	ovm_raise(except_invalid_argument);
    glGetShaderiv(alist->a0, GL_INFO_LOG_LENGTH, &length);
    orenew_vector(alist->a1, length - 1);
    glGetShaderInfoLog(alist->a0, length - 1, NULL, alist->a1->v.obj);
}

static void
native_GetShaderSource(oobject_t list, oint32_t ac)
/* void gl.GetShaderSource(uint32_t shader, string_t source); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oint32_t				 length;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    if (!glIsShader(alist->a0))
	ovm_raise(except_invalid_argument);
    length = 0;
    glGetShaderiv(alist->a0, GL_SHADER_SOURCE_LENGTH, &length);
    orenew_vector(alist->a1, length);
    glGetShaderSource(alist->a0, length, null, alist->a1->v.obj);
}

static void
native_GetUniformLocation(oobject_t list, oint32_t ac)
/* int32_t gl.GetUniformLocation(uint32_t program, string_t name); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    CHECK_NULL(alist->a1);
    CHECK_TYPE(alist->a1, t_string);
    make_vec_text(alist->a1);
    r0->v.w = glGetUniformLocation(alist->a0, alist->a1->v.obj);
}

static void
native_IsProgram(oobject_t list, oint32_t ac)
/* uint8_t gl.IsProgram(uint32_t shader); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsProgram(alist->a0);
}

static void
native_IsShader(oobject_t list, oint32_t ac)
/* uint8_t gl.IsShader(uint32_t shader); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = glIsShader(alist->a0);
}

static void
native_LinkProgram(oobject_t list, oint32_t ac)
/* uint32_t gl.LinkProgram(uint32_t program); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glLinkProgram(alist->a0);
}

static void
native_ShaderSource(oobject_t list, oint32_t ac)
/* void gl.ShaderSource(uint32_t shader, string_t string[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;
    oword_t				 offset;
    ovector_t				*string;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    if (alist->a1) {
	CHECK_VECTOR(alist->a1);
	offset = (alist->a1->length + 3) & -4;
	if (int_vector == null) {
	    onew_vector((oobject_t *)&int_vector, t_int32, offset);
	    onew_vector((oobject_t *)&str_vector, t_word, offset);
	}
	else if (offset > int_vector->length) {
	    orenew_vector(int_vector, offset);
	    orenew_vector(str_vector, offset);
	}
	for (offset = alist->a1->length - 1; offset >= 0; --offset) {
	    string = alist->a1->v.ptr[offset];
	    CHECK_NULL(string);
	    CHECK_TYPE(string, t_string);
	    int_vector->v.i32[offset] = string->length;
	    str_vector->v.w[offset] = (oword_t)string->v.obj;
	}
	glShaderSource(alist->a0, alist->a1->length,
		       (const GLchar **)str_vector->v.obj,
		       (const GLint *)int_vector->v.i32);
    }
}

static void
native_UseProgram(oobject_t list, oint32_t ac)
/* void gl.UseProgram(uint32_t type); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glUseProgram(alist->a0);
}

static void
native_Uniform1(oobject_t list, oint32_t ac)
/* void gl.Uniform1(int32_t location, float32_t v0); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f32_t			*alist;

    alist = (nat_i32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glUniform1f(alist->a0, alist->a1);
}

static void
native_Uniform2(oobject_t list, oint32_t ac)
/* void gl.Uniform2(int32_t location, float32_t v0, float32_t v1); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f32_f32_t			*alist;

    alist = (nat_i32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glUniform2f(alist->a0, alist->a1, alist->a2);
}

static void
native_Uniform3(oobject_t list, oint32_t ac)
/* void gl.Uniform3(int32_t location, float32_t v0,
		    float32_t v1, float32_t v2); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f32_f32_f32_t		*alist;

    alist = (nat_i32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glUniform3f(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_Uniform4(oobject_t list, oint32_t ac)
/* void gl.Uniform4(int32_t location, float32_t v0, float32_t v1,
		    float32_t v2, float32_t v3); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_f32_f32_f32_f32_t		*alist;

    alist = (nat_i32_f32_f32_f32_f32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glUniform4f(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}

static void
native_Uniform1v(oobject_t list, oint32_t ac)
/* void gl.Uniform1v(int32_t location, (int32_t|float32_t) value[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;

    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glUniform1iv(alist->a0, alist->a1->length, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glUniform1fv(alist->a0, alist->a1->length, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Uniform2v(oobject_t list, oint32_t ac)
/* void gl.Uniform2v(int32_t location, (int32_t|float32_t) value[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;

    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    if (alist->a1->length & 1)
	ovm_raise(except_invalid_argument);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glUniform2iv(alist->a0, alist->a1->length >> 1, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glUniform2fv(alist->a0, alist->a1->length >> 1, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Uniform3v(oobject_t list, oint32_t ac)
/* void gl.Uniform3v(int32_t location, (int32_t|float32_t) value[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;

    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    if (alist->a1->length % 3)
	ovm_raise(except_invalid_argument);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glUniform3iv(alist->a0, alist->a1->length / 3, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glUniform3fv(alist->a0, alist->a1->length / 3, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_Uniform4v(oobject_t list, oint32_t ac)
/* void gl.Uniform4v(int32_t location, (int32_t|float32_t) value[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_vec_t			*alist;

    alist = (nat_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    if (alist->a1->length & 3)
	ovm_raise(except_invalid_argument);
    switch (otype(alist->a1)) {
	case t_vector|t_int32:
	    glUniform4iv(alist->a0, alist->a1->length >> 2, alist->a1->v.i32);
	    break;
	case t_vector|t_float32:
	    glUniform4fv(alist->a0, alist->a1->length >> 2, alist->a1->v.f32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_UniformMatrix2v(oobject_t list, oint32_t ac)
/* void gl.UniformMatrix2v(int32_t location, uint8_t transpose, float32_t[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_u8_vec_t			*alist;

    alist = (nat_i32_u8_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    if (alist->a2->length & 3)
	ovm_raise(except_invalid_argument);
    glUniformMatrix2fv(alist->a0, alist->a2->length / 4,
		       alist->a1, alist->a2->v.f32);
}

static void
native_UniformMatrix3v(oobject_t list, oint32_t ac)
/* void gl.UniformMatrix3v(int32_t location, uint8_t transpose, float32_t[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_u8_vec_t			*alist;

    alist = (nat_i32_u8_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    if (alist->a2->length % 9)
	ovm_raise(except_invalid_argument);
    glUniformMatrix3fv(alist->a0, alist->a2->length / 9,
		       alist->a1, alist->a2->v.f32);
}

static void
native_UniformMatrix4v(oobject_t list, oint32_t ac)
/* void gl.UniformMatrix4v(int32_t location, uint8_t transpose, float32_t[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_i32_u8_vec_t			*alist;

    alist = (nat_i32_u8_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a2);
    CHECK_TYPE(alist->a2, t_vector|t_float32);
    if (alist->a2->length & 15)
	ovm_raise(except_invalid_argument);
    glUniformMatrix4fv(alist->a0, alist->a2->length / 16,
		       alist->a1, alist->a2->v.f32);
}

static void
native_ValidateProgram(oobject_t list, oint32_t ac)
/* void gl.ValidateProgram(uint32_t program); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_t				*alist;

    alist = (nat_u32_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glValidateProgram(alist->a0);
}

static void
native_VertexAttrib1(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib1(uint32_t index, float64_t x); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_t			*alist;

    alist = (nat_u32_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertexAttrib1d(alist->a0, alist->a1);
}

static void
native_VertexAttrib2(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib2(uint32_t index, float64_t x, float64_t y); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_t			*alist;

    alist = (nat_u32_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertexAttrib2d(alist->a0, alist->a1, alist->a2);
}

static void
native_VertexAttrib3(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib3(uint32_t index, float64_t x,
			 float64_t y, float64_t z); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertexAttrib3d(alist->a0, alist->a1, alist->a2, alist->a3);
}

static void
native_VertexAttrib4(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib4(uint32_t index, float64_t x, float64_t y,
			 float64_t z, float64_t w); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_f64_f64_f64_f64_t		*alist;

    alist = (nat_u32_f64_f64_f64_f64_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    glVertexAttrib4d(alist->a0, alist->a1, alist->a2, alist->a3, alist->a4);
}

static void
native_VertexAttrib1v(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib1v(uint32_t index, (int16_t|float32_t|float64_t) v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 1);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glVertexAttrib1sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_float32:
	    glVertexAttrib1fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertexAttrib1dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexAttrib2v(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib2v(uint32_t index, (int16_t|float32_t|float64_t) v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 2);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glVertexAttrib2sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_float32:
	    glVertexAttrib2fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertexAttrib2dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexAttrib3v(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib2v(uint32_t index, (int16_t|float32_t|float64_t) v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 3);
    switch (otype(alist->a1)) {
	case t_vector|t_int16:
	    glVertexAttrib3sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_float32:
	    glVertexAttrib3fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertexAttrib3dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexAttrib4v(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib4v(uint32_t index, (int8_t|uint8_t|int16_t|uint16_t|
					   int32_t|uint32_t|float32_t|float64_t)
			  v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 4);
    switch (otype(alist->a1)) {
	case t_vector|t_int8:
	    glVertexAttrib4bv(alist->a0, alist->a1->v.i8);
	    break;
	case t_vector|t_uint8:
	    glVertexAttrib4ubv(alist->a0, alist->a1->v.u8);
	    break;
	case t_vector|t_int16:
	    glVertexAttrib4sv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_uint16:
	    glVertexAttrib4usv(alist->a0, alist->a1->v.u16);
	    break;
	case t_vector|t_int32:
	    glVertexAttrib4iv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_uint32:
	    glVertexAttrib4uiv(alist->a0, alist->a1->v.u32);
	    break;
	case t_vector|t_float32:
	    glVertexAttrib4fv(alist->a0, alist->a1->v.f32);
	    break;
	case t_vector|t_float64:
	    glVertexAttrib4dv(alist->a0, alist->a1->v.f64);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexAttrib4Nv(oobject_t list, oint32_t ac)
/* void gl.VertexAttrib4Nv(uint32_t index, (int8_t|uint8_t|int16_t|uint16_t|
					    int32_t|uint32_t) v[]); */
{
    GET_THREAD_SELF()
    oregister_t				*r0;
    nat_u32_vec_t			*alist;

    alist = (nat_u32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;
    CHECK_NULL(alist->a1);
    CHECK_VECTOR(alist->a1);
    CHECK_LENGTH(alist->a1, 4);
    switch (otype(alist->a1)) {
	case t_vector|t_int8:
	    glVertexAttrib4Nbv(alist->a0, alist->a1->v.i8);
	    break;
	case t_vector|t_uint8:
	    glVertexAttrib4Nubv(alist->a0, alist->a1->v.u8);
	    break;
	case t_vector|t_int16:
	    glVertexAttrib4Nsv(alist->a0, alist->a1->v.i16);
	    break;
	case t_vector|t_uint16:
	    glVertexAttrib4Nusv(alist->a0, alist->a1->v.u16);
	    break;
	case t_vector|t_int32:
	    glVertexAttrib4Niv(alist->a0, alist->a1->v.i32);
	    break;
	case t_vector|t_uint32:
	    glVertexAttrib4Nuiv(alist->a0, alist->a1->v.u32);
	    break;
	default:
	    ovm_raise(except_type_mismatch);
    }
}

static void
native_VertexAttribPointer(oobject_t list, oint32_t ac)
/* void gl.VertexAttribPointer(uint32_t index, int32_t size, int32_t stride,
			       (int8_t|uint8_t|int16_t|uint16_t|int32_t|
				uint32_t|float32_t|float64_t)pointer[]); */
{

    GET_THREAD_SELF()
    oregister_t				*r0;
    oword_t				 type;
    nat_u32_i32_i32_vec_t		*alist;
    ohashentry_t			 check;
    ohashentry_t			*entry;
    oword_t				 offset;
    oword_t				 stride;
    oint32_t				 buffer;
    obuffer_offset_t			*vector;

    alist = (nat_u32_i32_i32_vec_t *)list;
    r0 = &thread_self->r0;
    r0->t = t_void;

    if (alist->a3 && (otype(alist->a3) & t_vector)) {
	switch (otype(alist->a3)) {
	    case t_vector|t_int8:
		type = GL_BYTE;
		stride = alist->a2;
		break;
	    case t_vector|t_uint8:
		type = GL_UNSIGNED_BYTE;
		stride = alist->a2;
		break;
	    case t_vector|t_int16:
		type = GL_SHORT;
		stride = alist->a2 << 1;
		break;
	    case t_vector|t_uint16:
		type = GL_UNSIGNED_SHORT;
		stride = alist->a2 << 1;
		break;
	    case t_vector|t_int32:
		type = GL_INT;
		stride = alist->a2 << 2;
		break;
	    case t_vector|t_uint32:
		type = GL_UNSIGNED_INT;
		stride = alist->a2 << 2;
		break;
	    case t_vector|t_float32:
		type = GL_FLOAT;
		stride = alist->a2 << 2;
		break;
	    case t_vector|t_float64:
		type = GL_DOUBLE;
		stride = alist->a2 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glVertexAttribPointer(alist->a0, alist->a1, type, 0,
			      stride, alist->a3->v.obj);
    }
    else {
	if (alist->a3 == null)
	    offset = 0;
	else if (otype(alist->a3) != t_word)
	    ovm_raise(except_type_mismatch);
	else
	    offset = *(oword_t *)alist->a3;
	if (offset < 0 || offset > (0x7fffffff >> 3))
	    ovm_raise(except_invalid_argument);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
	if (!glIsBuffer(buffer))
	    ovm_raise(except_invalid_argument);
	check.nt = t_word;
	check.nv.w = buffer;
	okey_hashentry(&check);
	/* should not happen */
	if ((entry = oget_hashentry(buffer_table, &check)) == null)
	    ovm_raise(except_invalid_argument);
	vector = entry->vv.o;
	switch ((type = vector[ARRAY_BUFFER_OFFSET])) {
	    case GL_BYTE:		case GL_UNSIGNED_BYTE:
		stride = alist->a2;
		break;
	    case GL_SHORT:		case GL_UNSIGNED_SHORT:
		offset <<= 1;
		stride = alist->a2 << 1;
		break;
	    case GL_INT:		case GL_UNSIGNED_INT:
	    case GL_FLOAT:
		offset <<= 2;
		stride = alist->a2 << 2;
		break;
	    case GL_DOUBLE:
		offset <<= 3;
		stride = alist->a2 << 3;
		break;
	    default:
		ovm_raise(except_type_mismatch);
	}
	glVertexAttribPointer(alist->a0, alist->a1, type, 0,
			      stride, (oobject_t)offset);
    }
}
