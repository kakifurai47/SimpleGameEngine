#pragma once

#include <sge_core/base/sge_base.h>

namespace sge
{
	enum class Render_FmtComp_DataType : u8 { None = 0, Float, UInt, SInt, UNorm, SNorm };
	enum class Render_FmtComp_SizeType : u8 { b08 = 0, b16, b32, b64 };
	enum class Render_FmtComp_NumType : u8 { x1 = 0, x2, x3, x4 };

	template<Render_FmtComp_SizeType CS> constexpr size_t get_comp_fmt_size()			 { return  0; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b08>() { return  8; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b16>() { return 16; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b32>() { return 32; }
	template<> static constexpr size_t get_comp_fmt_size<Render_FmtComp_SizeType::b64>() { return 64; }

	template<Render_FmtComp_NumType CS> constexpr size_t get_comp_fmt_num()				 { return  0; }
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
		 = _set_fmt_type_v(Render_FmtComp_DataType::T, \
						   Render_FmtComp_SizeType::S, \
						   Render_FmtComp_NumType::N) \
//-------------

	enum class Render_FormatType : u8 {
		None = 0,

		E(Float, b08, x1), E(Float, b08, x2), E(Float, b08, x3), E(Float, b08, x4),
		E(Float, b16, x1), E(Float, b16, x2), E(Float, b16, x3), E(Float, b16, x4),
		E(Float, b32, x1), E(Float, b32, x2), E(Float, b32, x3), E(Float, b32, x4),
		E(Float, b64, x1), E(Float, b64, x2), E(Float, b64, x3), E(Float, b64, x4),

		E(UInt, b08, x1), E(UInt, b08, x2), E(UInt, b08, x3), E(UInt, b08, x4),
		E(UInt, b16, x1), E(UInt, b16, x2), E(UInt, b16, x3), E(UInt, b16, x4),
		E(UInt, b32, x1), E(UInt, b32, x2), E(UInt, b32, x3), E(UInt, b32, x4),
		E(UInt, b64, x1), E(UInt, b64, x2), E(UInt, b64, x3), E(UInt, b64, x4),

		E(SInt, b08, x1), E(SInt, b08, x2), E(SInt, b08, x3), E(SInt, b08, x4),
		E(SInt, b16, x1), E(SInt, b16, x2), E(SInt, b16, x3), E(SInt, b16, x4),
		E(SInt, b32, x1), E(SInt, b32, x2), E(SInt, b32, x3), E(SInt, b32, x4),
		E(SInt, b64, x1), E(SInt, b64, x2), E(SInt, b64, x3), E(SInt, b64, x4),

		E(UNorm, b08, x1), E(UNorm, b08, x2), E(UNorm, b08, x3), E(UNorm, b08, x4),
		E(UNorm, b16, x1), E(UNorm, b16, x2), E(UNorm, b16, x3), E(UNorm, b16, x4),
		E(UNorm, b32, x1), E(UNorm, b32, x2), E(UNorm, b32, x3), E(UNorm, b32, x4),
		E(UNorm, b64, x1), E(UNorm, b64, x2), E(UNorm, b64, x3), E(UNorm, b64, x4),

		E(SNorm, b08, x1), E(SNorm, b08, x2), E(SNorm, b08, x3), E(SNorm, b08, x4),
		E(SNorm, b16, x1), E(SNorm, b16, x2), E(SNorm, b16, x3), E(SNorm, b16, x4),
		E(SNorm, b32, x1), E(SNorm, b32, x2), E(SNorm, b32, x3), E(SNorm, b32, x4),
		E(SNorm, b64, x1), E(SNorm, b64, x2), E(SNorm, b64, x3), E(SNorm, b64, x4),
	};
#undef E

	template<Render_FormatType FMT_TYPE>
	struct Render_FormatDesc : public NonCopyable {

		using CompDataType = Render_FmtComp_DataType;
		using CompSizeType = Render_FmtComp_SizeType;
		using CompNumType  = Render_FmtComp_NumType;

		static const CompDataType compDataType = static_cast<CompDataType>((static_cast<u8>(FMT_TYPE) >> 4	& 0xFu));
		static const CompSizeType compSizeType = static_cast<CompSizeType>((static_cast<u8>(FMT_TYPE) >> 2	& 0x3u));
		static const CompNumType  compNumType  = static_cast<CompNumType> ((static_cast<u8>(FMT_TYPE)		& 0x3u));

		static const size_t compSize = get_comp_fmt_size<compSize>();
		static const size_t comNum   = get_comp_fmt_size<compNum> ();

		static constexpr size_t size = compSize * comNum;
	};
}

