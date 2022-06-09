#pragma once

#include <sge_core/base/sge_base.h>

namespace sge {
	enum class RenderFormatType : u8 {
		None,
		Int08x1,	Int08x2,	Int08x3,	Int08x4,
		Int16x1,	Int16x2,	Int16x3,	Int16x4,
		Int32x1,	Int32x2,	Int32x3,	Int32x4,
		Int64x1,	Int64x2,	Int64x3,	Int64x4,

		UInt08x1,	UInt08x2,	UInt08x3,	UInt08x4,
		UInt16x1,	UInt16x2,	UInt16x3,	UInt16x4,
		UInt32x1,	UInt32x2,	UInt32x3,	UInt32x4,
		UInt64x1,	UInt64x2,	UInt64x3,	UInt64x4,

		Float16x1,	Float16x2,	Float16x3,	Float16x4,
		Float32x1,	Float32x2,	Float32x3,	Float32x4,
		Float64x1,	Float64x2,	Float64x3,	Float64x4,

		SNorm08x1,	SNorm08x2,	SNorm08x3,	SNorm08x4,
		SNorm16x1,	SNorm16x2,	SNorm16x3,	SNorm16x4,
		SNorm32x1,	SNorm32x2,	SNorm32x3,	SNorm32x4,

		UNorm08x1,	UNorm08x2,	UNorm08x3,	UNorm08x4,
		UNorm16x1,	UNorm16x2,	UNorm16x3,	UNorm16x4,
		UNorm32x1,	UNorm32x2,	UNorm32x3,	UNorm32x4,

		Texture1D,		Texture2D,		Texture3D,		TextureCube,
		Texture1DArray, Texture2DArray, Texture3DArray, TextureCubeArray,
	};

#define RenderFormatType_ENUM_LIST(E) \
	E(None) \
	\
	E(Int08x1)		E(Int08x2)		E(Int08x3)		E(Int08x4)		\
	E(Int16x1)		E(Int16x2)		E(Int16x3)		E(Int16x4)		\
	E(Int32x1)		E(Int32x2)		E(Int32x3)		E(Int32x4)		\
	E(Int64x1)		E(Int64x2)		E(Int64x3)		E(Int64x4)		\
	\
	E(UInt08x1)		E(UInt08x2)		E(UInt08x3)		E(UInt08x4)		\
	E(UInt16x1)		E(UInt16x2)		E(UInt16x3)		E(UInt16x4)		\
	E(UInt32x1)		E(UInt32x2)		E(UInt32x3)		E(UInt32x4)		\
	E(UInt64x1)		E(UInt64x2)		E(UInt64x3)		E(UInt64x4)		\
	\
	E(Float16x1)	E(Float16x2)	E(Float16x3)	E(Float16x4)	\
	E(Float32x1)	E(Float32x2)	E(Float32x3)	E(Float32x4)	\
	E(Float64x1)	E(Float64x2)	E(Float64x3)	E(Float64x4)	\
	\
	E(SNorm08x1)	E(SNorm08x2)	E(SNorm08x3)	E(SNorm08x4)	\
	E(SNorm16x1)	E(SNorm16x2)	E(SNorm16x3)	E(SNorm16x4)	\
	E(SNorm32x1)	E(SNorm32x2)	E(SNorm32x3)	E(SNorm32x4)	\
	\
	E(UNorm08x1)	E(UNorm08x2)	E(UNorm08x3)	E(UNorm08x4)	\
	E(UNorm16x1)	E(UNorm16x2)	E(UNorm16x3)	E(UNorm16x4)	\
	E(UNorm32x1)	E(UNorm32x2)	E(UNorm32x3)	E(UNorm32x4)	\
	\
	E(Texture1D)	  E(Texture2D)		E(Texture3D)	  E(TextureCube)		\
	E(Texture1DArray) E(Texture2DArray) E(Texture3DArray) E(TextureCubeArray)	\
//----




	SGE_ENUM_STR_UTIL(RenderFormatType)
	SGE_ENUM_ALL_OPERATOR(RenderFormatType)

	struct RenderFormatTypeUtil {
		RenderFormatTypeUtil() = delete;
		using Type = RenderFormatType;
		template<class T> static constexpr Type get();

		template<> static constexpr Type get<void>()	{ return Type::None;		}

		template<> static constexpr Type get<i8 >()		{ return Type::Int08x1;		}
		template<> static constexpr Type get<i16>()		{ return Type::Int16x1;		}
		template<> static constexpr Type get<i32>()		{ return Type::Int32x1;		}
		template<> static constexpr Type get<i64>()		{ return Type::Int64x1;		}

		template<> static constexpr Type get<u8 >()		{ return Type::UInt08x1;	}
		template<> static constexpr Type get<u16>()		{ return Type::UInt16x1;	}
		template<> static constexpr Type get<u32>()		{ return Type::UInt32x1;	}
		template<> static constexpr Type get<u64>()		{ return Type::UInt64x1;	}

		template<> static constexpr Type get<f32>()		{ return Type::Float32x1;	}
		template<> static constexpr Type get<f64>()		{ return Type::Float64x1;	}

		template<> static constexpr Type get<Tuple2f>()	{ return Type::Float32x2;	}
		template<> static constexpr Type get<Tuple2d>()	{ return Type::Float64x2;	}

		template<> static constexpr Type get<Tuple3f>()	{ return Type::Float32x3;	}
		template<> static constexpr Type get<Tuple3d>()	{ return Type::Float64x3;	}

		template<> static constexpr Type get<Tuple4f>()	{ return Type::Float32x4;	}
		template<> static constexpr Type get<Tuple4d>()	{ return Type::Float64x4;	}

		template<> static constexpr Type get<Color4b>()	{ return Type::UNorm08x4;	}
	};
}
