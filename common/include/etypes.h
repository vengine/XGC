/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef _ETYPES_H_
#define _ETYPES_H_
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

///#define _MSC_VER

#ifdef __MINGW32__
#include <stdbool.h>
#include <stdint.h>
#elif defined(__APPLE__)
/// nothing
#else
///typedef char bool;
#ifndef __cplusplus
#define bool char
#define true  1
#define false 0
#endif
///#include <windows.h>

#define __mingw_aligned_malloc _aligned_malloc
#define __mingw_aligned_free   _aligned_free
#endif

#define USE_MATERIAL_ID_ARRAY
#undef USE_MATERIAL_ID_ARRAY

typedef unsigned int euint32;
typedef unsigned long long euint64;
typedef long long esint64;

typedef char esint8;
///typedef char byte;
typedef unsigned char euint8;
typedef unsigned short euint16;
typedef unsigned char* euptr8;
///typedef euint* uptr;
typedef euint32* euptr32;
typedef euint64* euptr64;

typedef const char* EString;

typedef int esint32;
typedef short esint16;
#if BIT_WIDTH == 64
typedef euint64 euint;
typedef esint64 esint;
#else
typedef euint32 euint;
typedef esint32 esint;
#endif
typedef void* vptr;
typedef euint pptr;
typedef esint* sptr;
typedef euint* uptr;

#define _32BIT_PIXEL_SIZE_ 4
enum PixelSize
{
	Pixel8  = 8,
	Pixel9  = 9,
	Pixel10 = 10,
	Pixel11 = 11,
	Pixel12 = 12,
	Pixel13 = 13,
	Pixel14 = 14,
	Pixel15 = 15,
	Pixel16 = 16,
	Pixel17 = 17,
	Pixel18 = 18,
	Pixel20 = 20,
	Pixel22 = 22,
	Pixel24 = 24,
	Pixel26 = 26,
    Pixel30 = 30,
	Pixel40 = 40,
	Pixel60 = 60,
};

typedef euint ref_ptr;

typedef char* byte_ptr;
typedef enum _etype
{
    Sint8,
    Sint16,
    Sint32,
    Sint64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Vptr,
    String,
}etype;

typedef union
{
    esint8 sint8_var;
    esint16 sint16_var;
    esint32 sint32_var;
    esint64 sint64_var;
    euint8 uint8_var;
    euint16 uint16_var;
    euint32 uint32_var;
    euint64 uint64_var;
    vptr vptr_var;
    EString str_var;
} var;

typedef euint64 VID;

#define UINT32_EXCE 0xffffffff

typedef enum _param_type
{
	EmptyParam           = 0,
	Texture2D_Param      = 10,
	Texture1D_Param      = 11,
	Int_Param            = 12,
	TextureCube_Param    = 14,
	Float32_Param        = 2,
	Float32x2_Param      = 3,
	Float32x3_Param      = 4,
	Float32x4_Param      = 5,
	Matrix4x4_Param      = 6,
	Bool_Param           = 13,
} param_type;

#define FLOAT_TYPE            0x01000000
#define UBYTE_TYPE            0x02000000
#define UINT_TYPE             0x04000000

#define NUMBER_COMPONENT_MASK 0x00ffffff

typedef enum _element_type
{
    EmptyType    = 0,
    Float32_1    = FLOAT_TYPE | 0x00000001,
    Float32_2    = FLOAT_TYPE | 0x00000002,
    Float32_3    = FLOAT_TYPE | 0x00000003,
    Float32_4    = FLOAT_TYPE | 0x00000004,
	Float32_16   = FLOAT_TYPE | 0x000000016,
    Ubyte_1      = UBYTE_TYPE | 0x00000001,
    Ubyte_2      = UBYTE_TYPE | 0x00000002,
    Ubyte_3      = UBYTE_TYPE | 0x00000003,
    Ubyte_4      = UBYTE_TYPE | 0x00000004,
    Uint32_1     = UINT_TYPE  | 0x00000001,
    Uint32_2     = UINT_TYPE  | 0x00000002,
} element_type;
typedef enum _element_semantic
{
    EmptySemantic  = 0,
    Position       = 1,
    Normal         = 2,
    Color          = 3,
    Tangent        = 4,
    Binormal       = 5,
    TexCoord0      = 6,
	TexCoord1      = 7,
	TexCoord2      = 8,
	TexCoord3      = 9,
	SketchMatrix0  = 10,
	SketchMatrix1  = 11,
	MaxElementSemantics = 12,
} element_semantic;
typedef enum _shader_type
{
    VertexShader,
    PixelShader,
}shader_type;
enum PixelFormat
{
	RGBA8 =    0x1000,
	RGB8 =     0x2000,
	RGBA32F =  0x3000,
	RG32F =    0x3500,
	RG16 =     0x3600,
	RGB32F =   0x4000,
	RGBA16F =  0x5000,
	RGB16F =   0x6000,
	DEPTH32F = 0x7000,
	ALPHA8   = 0x8000,
	RGBA16 =   0x9000,
	RGB16 =    0x9900,
};
typedef euint16 float16;

typedef enum _shader_object_type
{
	Int_Obj             = 12,
	Float_Obj           = 2,
	Float2_Obj          = 3,
	Float3_Obj          = 4,
	Float4_Obj          = 5,
	Texture2D_Obj,
	TextureCube_Obj,
	Texture1D_Obj,
	Matrix4x4_Obj,
	Bool_Obj,
	Void_Obj            = 0,
}shader_object_type;

typedef enum _light_type
{
    DirectionType,
    SpotType,
    PointType,
} light_type;

typedef void* (*MALLOC)(euint);
typedef void (*MFREE)(void*);

typedef struct rw_buffer* RWBuffer;
/**
typedef struct _inter_rw_buffer
{
    bool (*read)(RWBuffer _self, euint* result, euint* read_size);
    bool (*write)(RWBuffer _self, const euint* from, const euint write_size);
} InterRWBuffer;
**/
typedef bool (*rw_buf_read)(RWBuffer _self, euint* result, euint* read_size);
typedef bool (*rw_buf_write)(RWBuffer _self, const euint* from, const euint write_size);
struct InterRWBuffer
{
	rw_buf_read read;
	rw_buf_write write;
    InterRWBuffer(rw_buf_read r, rw_buf_write w)
		: read(r)
		, write(w)
	{}
};

typedef enum _render_target
{
    RenderTarget0,
    RenderTarget1,
    RenderTarget2,
    RenderTarget3,
} render_target;
typedef enum _render_output
{
	ColorOutput,
	NormalSpecularOutput,
	MaterialIDOutput,
	VSMDepthOutput,
	PointVSMDepthOutput,
} render_output;
typedef enum _vertex_color_function
{
	Replace,
	Multiply,
	Discard,
} vertex_color_function;

struct render_pipe
{
    render_output out;
    render_target tgt;
	render_pipe()
		: out(ColorOutput)
		, tgt(RenderTarget0)
	{}
};

typedef enum _draw_mode
{
    Shaded,
    Wireframe,
} e_draw_mode;

typedef enum _mesh_mode
{
    Triangular = 0,
    Segment,
    MeshModeMax,
} e_mesh_mode;

///typedef class texture2d* Texture2D;

typedef struct _vertex_element
{
    element_type type;
    element_semantic semantic;
} vertex_element;
typedef struct _vertex_element VertexElement;
typedef struct _vertex_decl* VertexDecl;

typedef int (*KEY_COMPARE)(vptr, vptr);

typedef struct _vertex_decl
{
    euint num_elements;
    VertexElement* elements;
    const char* decl_string;
} vertex_decl;

typedef enum _input_event_type
{
    MouseMoveEvent,
    MouseButtonDownEvent,
    MouseButtonUpEvent,
    MouseAbsolutePositionEvent,
    KeyDownEvent,
    KeyUpEvent,
} input_event_type;

typedef enum _mouse_button_type
{
    LeftButton,
    MiddleButton,
    RightButton,
} mouse_button_type;

typedef struct _mouse_move_vector
{
    int x;
    int y;
} mouse_move_vector;
typedef struct _mouse_abs_pos
{
    int x;
    int y;
} s_mouse_abs_pos;

typedef union _mouse_event_info
{
    mouse_button_type mouse_button_info;
    mouse_move_vector mouse_move_info;
    s_mouse_abs_pos mouse_abs_pos;
} mouse_event_info;

typedef euint32 key_code;

typedef union _input_event_info
{
    mouse_event_info mouse_info;
    key_code key_info;
} input_event_info;

typedef struct _input_event
{
    input_event_type type;
    input_event_info info;
    euint64 time_stamp;
} input_event;

typedef struct _input_thread_param
{
    volatile bool is_thread_end;
    RWBuffer input_buffer;
    InterRWBuffer inter_input_buffer;
} input_thread_param;

typedef struct _input_buffer
{
    RWBuffer input_buffer;
    InterRWBuffer inter_input_buffer;
} input_buffer;

typedef struct _mesh_edge* MeshEdge;
typedef struct _mesh_triangle* MeshTriangle;
typedef MeshTriangle* MeshTriangleArray;

typedef enum _param_source
{
	EmptySrc = 0,
	VaryWorldPosition,
	VaryPosition,
	VaryTexCoord0,
	VaryTexCoord1,
	VaryNormal,
	VaryTangent,
	VaryBinormal,
	CameraPosition,
	CameraDirection,
	CameraPlaneNear,
	CameraPlaneFar,
	CameraPlaneWidth,
	CameraPlaneHeight,
	CameraProjectionMatrix,
	CameraWorldMatrix,
	InvertCameraProjectionMatrix,
	InvertCameraWorldMatrix,
	SketchPixelSize,

	LightInnerCos,
	LightOuterCos,

	LightPosition,
	LightDirection,
	LightColor,
	LightAtteCoef,
	LightNumEmitters,
	LightProjectionMatrix0,
	LightProjectionMatrix1,
	LightProjectionMatrix2,
	LightProjectionMatrix3,
	LightProjectionMatrix4,
	LightProjectionMatrix5,
	LightWorldMatrix0,
	LightWorldMatrix1,
	LightWorldMatrix2,
	LightWorldMatrix3,
	LightWorldMatrix4,
	LightWorldMatrix5,
	ShadowMap0,
	ShadowMap1,
	ShadowMap2,
	ShadowMap3,
	ShadowMap4,
	ShadowMap5,

	ShadowCubeMap,
	LightInfluence,

	ShadowDirection0,
	ShadowDirection1,
	ShadowDirection2,
	ShadowDirection3,
	ShadowDirection4,
	ShadowDirection5,

	RenderableWorldMatrix,
	ColorMap0,
	ColorMap1,
	ColorMap2,
	ColorMap3,
	NormalMap0,
	NormalMap1,
	NormalMap2,
	NormalMap3,
	Plaster,
	ColorSketch,
	NormalSketch,
	MaterialIDSketch,
	LightingSketch,
	DiffuseLightingSketch,
	SpecularLightingSketch,
	ColorSketchPixelSize,
	AmbiColor,
	CurrentMaterialID,

	///CurrentIntMaterialID,

	///PointLightPosition,
	///PointLightColor,
	///PointLightAtteCoef,
	MaxParamSource,

} param_source;

typedef enum _compare_operation
{
    EmptyCompare,
    Equal,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    NotEqual,
} compare_operation;

typedef struct _shader_node_base* ShaderNodeBase;
typedef const char* (*shader_node_compile_proc)(ShaderNodeBase);
typedef void (*shader_node_init_proc)(ShaderNodeBase);
typedef void (*shader_node_dest_proc)(ShaderNodeBase);
typedef ShaderNodeBase (*shader_node_clone_proc)(ShaderNodeBase);

typedef struct _string_buffer
{
    char* str;
    int size;
    int remainder;
} string_buffer;

#define string_buffer_new(s) char mbuf[s]; string_buffer str_buf = {mbuf, s, s - 1};
#define string_buffer_clear  { str_buf.str = mbuf; str_buf.remainder = str_buf.size - 1; }
#define sbuf_printf(fmt,...) { int offs = snprintf(str_buf.str, str_buf.remainder, fmt, ##__VA_ARGS__); \
                               str_buf.str += offs; \
                               str_buf.remainder -= offs; \
                               EAssert(str_buf.remainder > 0, "string buffer is too small"); }
#define get_string_buffer mbuf

#define STRING_BUFFER_SIZE 32 * 1024

struct _tree;
typedef struct _tree* Tree;

typedef struct _renderer_param_value
{
    vptr value;
} renderer_param_value;

typedef enum _value_type
{
    IntValue = 0,
    FloatValue,
    Float2Value,
    Float3Value,
    Float4Value,
    Matrix4x4Value,
    StringValue,
    BoolValue,
    Function,
    MaxValidValue,
    ValueArray,
    FuncArgValueArray,
    StructName,
    StructMemberName,
    UnknownValue,
} ValueType;

typedef enum _exce_id
{
	
///} ExceID;

///typedef enum _exce_id
///{
	Invalid_operation = 0,
	Symbol_is_defined_more_than_once = 1,

	MaterialNotExist = 2,
	RenderableWorldMatrixIsNull = 3,
	MemPoolIsCorrupted = 4,

	The_element_name_is_defined_more_than_once_in_struct,
	Unknown_data_type,
	Structure_is_defined_more_than_once,
	MaxExce,
} exce_id;

enum DerivedType
{
	Public,
	Protected,
	Private,
};

enum AnimationStatus
{
	NotExist,
	Playing,
	Paused,
	Stopped,
};

enum component
{
	CompX = 0,
	CompY,
	CompZ,
	CompW,
	CompR,
	CompG,
	CompB,
	CompA,
};

struct component_index
{
	component_index()
		: num_comps(0)
	{}
	euint num_comps;
	component comps[4];
	bool operator < (const component_index& index) const {
		if (num_comps < index.num_comps)
			return true;
		else if (num_comps > index.num_comps)
			return false;
		else {
			for (euint i = 0; i < num_comps; i++) {
				if (comps[i] < index.comps[i])
					return true;
				else if (comps[i] > index.comps[i])
					return false;
			}
			return false;
		}
	}
	bool operator > (const component_index& index) const {
		if (num_comps > index.num_comps)
			return true;
		else if (num_comps < index.num_comps)
			return false;
		else {
			for (euint i = 0; i < num_comps; i++) {
				if (comps[i] > index.comps[i])
					return true;
				else if (comps[i] < index.comps[i])
					return false;
			}
			return false;
		}
	}
};
#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */