#pragma once

namespace sge
{
	enum class Vertex_SemanticType : u8 {
		None	 = 0,
		Position = 1,
		Color	 = 2,
		Normal	 = 3,
		Binormal = 4,
		Tangent	 = 5,
		TexCoord = 6,
	};

	enum class Vertex_Semantic : u16
	{
		None,
		Position,

		Color0,
		Color1,
		Color2,
		Color3,

		TexCoord0,
		TexCoord1,
		TexCoord2,
		TexCoord3,
		TexCoord4,
		TexCoord5,
		TexCoord6,
		TexCoord7,

		Normal,
		Binormal,
		Tangent,
	};


	enum class VFormat_DataType : u8 {
		None  = 0,
		Float = 1,
		UInt  = 2,
		SInt  = 3,
		UNorm = 4,
		SNorm = 5,
	};

	enum class VFormat_SizeType : u8 {
		b00 = 0,
		b08 = 1,
		b16 = 2,
		b32 = 3,
		b64 = 4,
	};

	enum class VFormat_CompCount : u8 {
		x1 = 0,
		x2 = 1,
		x3 = 2,
		x4 = 3,
	};

	//fmt bit : dddd_ss_cc	
	template<VFormat_DataType Dtype, VFormat_SizeType Size, VFormat_CompCount Cnt>
	static constexpr u8 Vertex_toFmt_t() {
		return static_cast<u8>(Dtype) << 4 |
			   static_cast<u8>(Size)  << 2 |
			   static_cast<u8>(Cnt);
	}

	enum class Vertex_FormatType : u8;
	struct VFormt_Util {

		using FmtT   = Vertex_FormatType;
		using DataT  = VFormat_DataType ;
		using SizeT  = VFormat_SizeType ;
		using CountT = VFormat_CompCount;

		static constexpr DataT  getData_t (FmtT t) { return static_cast<DataT> (static_cast<u8>(t) >> 4 & 0xF); }
		static constexpr SizeT  getSize_t (FmtT t) { return static_cast<SizeT> (static_cast<u8>(t) >> 2 & 0x3); }
		static constexpr CountT getCount_t(FmtT t) { return static_cast<CountT>(static_cast<u8>(t)	    & 0x3); }
	};

#define E(T, SIZE, CNT) T##SIZE##CNT \
	 = Vertex_toFmt_t<VFormat_DataType::T, VFormat_SizeType::SIZE, VFormat_CompCount::CNT>()\

	enum class Vertex_FormatType : u8 {
		None,
		E(Float, b08, x1), E(Float, b08, x2), E(Float, b08, x3), E(Float, b08, x4),
		E(Float, b16, x1), E(Float, b16, x2), E(Float, b16, x3), E(Float, b16, x4),
		E(Float, b32, x1), E(Float, b32, x2), E(Float, b32, x3), E(Float, b32, x4),
		E(Float, b64, x1), E(Float, b64, x2), E(Float, b64, x3), E(Float, b64, x4),
					   
		E(UInt,  b08, x1), E(UInt,  b08, x2), E(UInt,  b08, x3), E(UInt,  b08, x4),
		E(UInt,  b16, x1), E(UInt,  b16, x2), E(UInt,  b16, x3), E(UInt,  b16, x4),
		E(UInt,  b32, x1), E(UInt,  b32, x2), E(UInt,  b32, x3), E(UInt,  b32, x4),
		E(UInt,  b64, x1), E(UInt,  b64, x2), E(UInt,  b64, x3), E(UInt,  b64, x4),
				 		
		E(SInt,  b08, x1), E(SInt,  b08, x2), E(SInt,  b08, x3), E(SInt,  b08, x4),
		E(SInt,  b16, x1), E(SInt,  b16, x2), E(SInt,  b16, x3), E(SInt,  b16, x4),
		E(SInt,  b32, x1), E(SInt,  b32, x2), E(SInt,  b32, x3), E(SInt,  b32, x4),
		E(SInt,  b64, x1), E(SInt,  b64, x2), E(SInt,  b64, x3), E(SInt,  b64, x4),
					   
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

	class VertexLayout : public NonCopyable
	{
	public:
		using SemanticT	 = Vertex_SemanticType;
		using FormatT	 = Vertex_FormatType;

		struct Element {
			FormatT	  format		= FormatT::None;
			SemanticT semantic		= SemanticT::None;
			u8		  semanticIdx	= 0;
			u16		  offset		= 0;
		};

		VertexLayout() 
			: typeId(0, 0) , stride(0)
		{
		}

		u128   typeId;
		size_t stride;
		Vector_<Element, 16> elements;

		template<int X>
		void create(/*X&& x, Y&& y, ARGS&& ...args*/)
		{
			SGE_DUMP_VAR(X);
			//SGE_DUMP_VAR(Y);			
		}


		template<int X, int ...ARGS>
		void create(/*X&& x, Y&& y, ARGS&& ...args*/)
		{
			create<X>();
			//create<ARGS...>();


			//create(SGE_FORWARD(args)...);
		}

		//template<template<class> class H, class S>
		//void create_02(H<S> value)
		//{
		//	
		//}

		//template<int a>
		//void create_03()
		//{

		//}
	};

	enum class VertexType : u64 { None };

	

	
	
	

	



}