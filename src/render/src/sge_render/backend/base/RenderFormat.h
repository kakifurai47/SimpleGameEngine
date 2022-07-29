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


	struct RenderFormatTypeUtil {
		RenderFormatTypeUtil() = delete;
		using Type = RenderFormatType;
		template<class T> static constexpr Type get();

		template<> static constexpr Type get<void>()		{ return Type::None;		}

		template<> static constexpr Type get<i8 >()			{ return Type::Int08x1;		}
		template<> static constexpr Type get<i16>()			{ return Type::Int16x1;		}
		template<> static constexpr Type get<i32>()			{ return Type::Int32x1;		}
		template<> static constexpr Type get<i64>()			{ return Type::Int64x1;		}

		template<> static constexpr Type get<u8 >()			{ return Type::UInt08x1;	}
		template<> static constexpr Type get<u16>()			{ return Type::UInt16x1;	}
		template<> static constexpr Type get<u32>()			{ return Type::UInt32x1;	}
		template<> static constexpr Type get<u64>()			{ return Type::UInt64x1;	}

		template<> static constexpr Type get<f32>()			{ return Type::Float32x1;	}
		template<> static constexpr Type get<f64>()			{ return Type::Float64x1;	}

		template<> static constexpr Type get<Tuple2f>()		{ return Type::Float32x2;	}
		template<> static constexpr Type get<Tuple2d>()		{ return Type::Float64x2;	}

		template<> static constexpr Type get<Tuple3f>()		{ return Type::Float32x3;	}
		template<> static constexpr Type get<Tuple3d>()		{ return Type::Float64x3;	}

		template<> static constexpr Type get<Tuple4f>()		{ return Type::Float32x4;	}
		template<> static constexpr Type get<Tuple4d>()		{ return Type::Float64x4;	}

		template<> static constexpr Type get<Color4b>()		{ return Type::UNorm08x4;	}

		template<> static constexpr Type get<Mat4f>()		{ return Type::Float32_4x4;	}

		template<> static constexpr Type get<Texture2D>()	{ return Type::Texture2D;	}
	};
}
