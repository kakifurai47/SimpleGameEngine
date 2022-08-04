#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/graph/Color.h>

namespace sge {

class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;

#define RenderFormatType_ENUM_LIST(E) \
	E(None,) \
	\
	E(Int08x1,)		E(Int08x2,)		E(Int08x3,)		E(Int08x4,)		\
	E(Int16x1,)		E(Int16x2,)		E(Int16x3,)		E(Int16x4,)		\
	E(Int32x1,)		E(Int32x2,)		E(Int32x3,)		E(Int32x4,)		\
	E(Int64x1,)		E(Int64x2,)		E(Int64x3,)		E(Int64x4,)		\
	\
	E(UInt08x1,)	E(UInt08x2,)	E(UInt08x3,)	E(UInt08x4,)	\
	E(UInt16x1,)	E(UInt16x2,)	E(UInt16x3,)	E(UInt16x4,)	\
	E(UInt32x1,)	E(UInt32x2,)	E(UInt32x3,)	E(UInt32x4,)	\
	E(UInt64x1,)	E(UInt64x2,)	E(UInt64x3,)	E(UInt64x4,)	\
	\
	E(Float16x1,)	E(Float16x2,)	E(Float16x3,)	E(Float16x4,)	\
	E(Float32x1,)	E(Float32x2,)	E(Float32x3,)	E(Float32x4,)	\
	E(Float64x1,)	E(Float64x2,)	E(Float64x3,)	E(Float64x4,)	\
	\
	E(SNorm08x1,)	E(SNorm08x2,)	E(SNorm08x3,)	E(SNorm08x4,)	\
	E(SNorm16x1,)	E(SNorm16x2,)	E(SNorm16x3,)	E(SNorm16x4,)	\
	E(SNorm32x1,)	E(SNorm32x2,)	E(SNorm32x3,)	E(SNorm32x4,)	\
	\
	E(UNorm08x1,)	E(UNorm08x2,)	E(UNorm08x3,)	E(UNorm08x4,)	\
	E(UNorm16x1,)	E(UNorm16x2,)	E(UNorm16x3,)	E(UNorm16x4,)	\
	E(UNorm32x1,)	E(UNorm32x2,)	E(UNorm32x3,)	E(UNorm32x4,)	\
	\
	E(Float32_4x4,)	\
	\
	E(Texture1D,)			E(Texture2D,)			E(Texture3D,)		E(TextureCube,)			\
	E(Texture1DArray,)		E(Texture2DArray,)		E(Texture3DArray,)	E(TextureCubeArray,)	\
	\
	E(Texture1DMS,)			E(Texture2DMS,)			E(Texture3DMS,)			\
	E(Texture1DMSArray,)	E(Texture2DMSArray,)	E(Texture3DMSArray,)	\
//----
	SGE_ENUM_CLASS(RenderFormatType, : u8)



#define SGE_RENDER_FMT_TRAIT_PAIRS(E) \
	E(i8,			RenderFormatType::Int08x1)		\
	E(i16,			RenderFormatType::Int16x1)		\
	E(i32,			RenderFormatType::Int32x1)		\
	E(i64,			RenderFormatType::Int64x1)		\
	\
	E(u8 ,			RenderFormatType::UInt08x1)		\
	E(u16,			RenderFormatType::UInt16x1)		\
	E(u32,			RenderFormatType::UInt32x1)		\
	E(u64,			RenderFormatType::UInt64x1)		\
	\
	E(f32,			RenderFormatType::Float32x1)	\
	E(f64,			RenderFormatType::Float64x1)	\
	\
	E(Tuple2f,		RenderFormatType::Float32x2)	\
	E(Tuple2d,		RenderFormatType::Float64x2)	\
	\
	E(Tuple3f,		RenderFormatType::Float32x3)	\
	E(Tuple3d,		RenderFormatType::Float64x3)	\
	\
	E(Tuple4f,		RenderFormatType::Float32x4)	\
	E(Tuple4d,		RenderFormatType::Float64x4)	\
	\
	E(Color4b,		RenderFormatType::UNorm08x4)	\
	\
	E(Mat4f,		RenderFormatType::Float32_4x4)	\
	\
	E(Texture2D,	RenderFormatType::Texture2D)	\
//----

	struct RenderFormatTypeUtil 
	{
		RenderFormatTypeUtil() = delete;
		using Type  =  RenderFormatType;
		template<class T> static constexpr Type get();

#define SGE_RENDER_FMT_GET_ENUM(CLSS_TYPE, ENUM_TYPE) \
		template<> static constexpr Type get<CLSS_TYPE>() { return ENUM_TYPE; } \
//----
		SGE_RENDER_FMT_TRAIT_PAIRS(SGE_RENDER_FMT_GET_ENUM)

#undef  SGE_RENDER_FMT_GET_ENUM
	};
}
