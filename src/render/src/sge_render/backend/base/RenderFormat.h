#pragma once

#include <sge_core/base/sge_base.h>

namespace sge
{
	enum class Render_FmtComp_DataType : u8 { None = 0, Float, UInt, SInt, UNorm, SNorm };
	enum class Render_FmtComp_SizeType : u8 { b08 = 0, b16, b32, b64 };
	enum class Render_FmtComp_NumType  : u8 { x1 = 0, x2, x3, x4 };

	template<Render_FmtComp_SizeType CS> constexpr size_t get_comp_fmt_size();
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b08>() { return  8; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b16>() { return 16; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b32>() { return 32; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b64>() { return 64; }

	template<Render_FmtComp_NumType CS> constexpr size_t get_comp_fmt_num();
	template<> static constexpr size_t get_comp_fmt_num<Render_FmtComp_NumType::x1>()	 { return  1; }
	template<> static constexpr size_t get_comp_fmt_num<Render_FmtComp_NumType::x2>()	 { return  2; }
	template<> static constexpr size_t get_comp_fmt_num<Render_FmtComp_NumType::x3>()	 { return  3; }
	template<> static constexpr size_t get_comp_fmt_num<Render_FmtComp_NumType::x4>()	 { return  4; }

	//fmt bit : tttt_ss_cc
	static constexpr u8 _set_fmt_type_v(Render_FmtComp_DataType type,
										Render_FmtComp_SizeType size,
										Render_FmtComp_NumType  num) {
		return	static_cast<u8>(type) << 4 |
				static_cast<u8>(size) << 2 |
				static_cast<u8>(num);
	}

#define E(T, S, N) T##S##N \
		 = _set_fmt_type_v(Render_FmtComp_DataType::T,		\
						   Render_FmtComp_SizeType::##b##S, \
						   Render_FmtComp_NumType::N)		\
//-------------

	enum class Render_FormatType : u8 { //TO DO : delete b in front of comp size
		None = 0,

		E(Float, 08, x1), E(Float, 08, x2), E(Float, 08, x3), E(Float, 08, x4),
		E(Float, 16, x1), E(Float, 16, x2), E(Float, 16, x3), E(Float, 16, x4),
		E(Float, 32, x1), E(Float, 32, x2), E(Float, 32, x3), E(Float, 32, x4),
		E(Float, 64, x1), E(Float, 64, x2), E(Float, 64, x3), E(Float, 64, x4),

		E(UInt, 08, x1), E(UInt, 08, x2), E(UInt, 08, x3), E(UInt, 08, x4),
		E(UInt, 16, x1), E(UInt, 16, x2), E(UInt, 16, x3), E(UInt, 16, x4),
		E(UInt, 32, x1), E(UInt, 32, x2), E(UInt, 32, x3), E(UInt, 32, x4),
		E(UInt, 64, x1), E(UInt, 64, x2), E(UInt, 64, x3), E(UInt, 64, x4),

		E(SInt, 08, x1), E(SInt, 08, x2), E(SInt, 08, x3), E(SInt, 08, x4),
		E(SInt, 16, x1), E(SInt, 16, x2), E(SInt, 16, x3), E(SInt, 16, x4),
		E(SInt, 32, x1), E(SInt, 32, x2), E(SInt, 32, x3), E(SInt, 32, x4),
		E(SInt, 64, x1), E(SInt, 64, x2), E(SInt, 64, x3), E(SInt, 64, x4),

		E(UNorm, 08, x1), E(UNorm, 08, x2), E(UNorm, 08, x3), E(UNorm, 08, x4),
		E(UNorm, 16, x1), E(UNorm, 16, x2), E(UNorm, 16, x3), E(UNorm, 16, x4),
		E(UNorm, 32, x1), E(UNorm, 32, x2), E(UNorm, 32, x3), E(UNorm, 32, x4),
		E(UNorm, 64, x1), E(UNorm, 64, x2), E(UNorm, 64, x3), E(UNorm, 64, x4),

		E(SNorm, 08, x1), E(SNorm, 08, x2), E(SNorm, 08, x3), E(SNorm, 08, x4),
		E(SNorm, 16, x1), E(SNorm, 16, x2), E(SNorm, 16, x3), E(SNorm, 16, x4),
		E(SNorm, 32, x1), E(SNorm, 32, x2), E(SNorm, 32, x3), E(SNorm, 32, x4),
		E(SNorm, 64, x1), E(SNorm, 64, x2), E(SNorm, 64, x3), E(SNorm, 64, x4),
	};
#undef E

	
	template<Render_FormatType FMT_TYPE> struct Render_DataType;

	//maybe dynamic tuple to avoid this;
	template<> struct Render_DataType<Render_FormatType::Float32x1> { using type = f32; };
	template<> struct Render_DataType<Render_FormatType::Float32x2> { using type = Tuple2f; };
	template<> struct Render_DataType<Render_FormatType::Float32x3> { using type = Tuple3f; };
	template<> struct Render_DataType<Render_FormatType::Float32x4> { using type = Tuple4f; };

	template<> struct Render_DataType<Render_FormatType::Float64x1> { using type = f64; };
	template<> struct Render_DataType<Render_FormatType::Float64x2> { using type = Tuple2d; };
	template<> struct Render_DataType<Render_FormatType::Float64x3> { using type = Tuple3d; };
	template<> struct Render_DataType<Render_FormatType::Float64x4> { using type = Tuple4d; };

	template<> struct Render_DataType<Render_FormatType::SInt08x1> { using type = i8; };
	template<> struct Render_DataType<Render_FormatType::SInt16x1> { using type = i16; };
	template<> struct Render_DataType<Render_FormatType::SInt32x1> { using type = i32; };
	template<> struct Render_DataType<Render_FormatType::SInt64x1> { using type = i64; };

	template<> struct Render_DataType<Render_FormatType::UInt08x1> { using type = u8; };
	template<> struct Render_DataType<Render_FormatType::UInt16x1> { using type = u16; };
	template<> struct Render_DataType<Render_FormatType::UInt32x1> { using type = u32; };
	template<> struct Render_DataType<Render_FormatType::UInt64x1> { using type = u64; };

	template<> struct Render_DataType<Render_FormatType::UNorm08x4> { using type = Tuple4<u8>; };

	template<Render_FormatType FMT_TYPE>
	struct Render_FormatDesc : public NonCopyable {
	private:
		using CompDataType = Render_FmtComp_DataType;
		using CompSizeType = Render_FmtComp_SizeType;
		using CompNumType  = Render_FmtComp_NumType;
	public:
		static const CompDataType compDataType = static_cast<CompDataType>((static_cast<u8>(FMT_TYPE) >> 4	& 0xFu));
		static const CompSizeType compSizeType = static_cast<CompSizeType>((static_cast<u8>(FMT_TYPE) >> 2	& 0x3u));
		static const CompNumType  compNumType  = static_cast<CompNumType> ((static_cast<u8>(FMT_TYPE)		& 0x3u));

		static const size_t compSize = get_comp_fmt_size<compSizeType>();
		static const size_t comNum   = get_comp_fmt_num <compNumType> ();

		static constexpr size_t size = compSize * comNum;

		using CompatableType = Render_DataType<FMT_TYPE>;//ERRROR: when ::Type
	};
}

