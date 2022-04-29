#pragma once

#include "sge_render/backend/base/RenderFormat.h"

namespace sge
{
	enum class Vertex_SemanticType : u8 {
		None = 0,
		Position,
		Color,
		TexCoord,
		Normal,
		Tangent,
		Binormal,
	};


	enum class VertexType : u64 { None };

	struct VertexType_BitfieldUtil
	{
		using SemanticT = Vertex_SemanticType;
		using FormatT  = Render_FormatType;

		enum class Bitfield : u8 {
			None			 =  0,
			PosFormat		 =  0,  //8 bit
			ColorFormat		 =  8,  //8 bit
			UVFormat		 = 16,  //8 bit
			NormalFormat	 = 24,  //8 bit

			ColorCount		 = 32,  //2 bit
			UVCount			 = 34,  //8 bit
			NormalCount		 = 42,  //2 bit
			TangentCount	 = 44,  //2 bit
			BinormalCount	 = 46,  //2 bit
		};

		template<SemanticT SEM_TYPE> static constexpr  u64 set(FormatT fmt, size_t count) { return 0; }
		template<SemanticT SEM_TYPE> static constexpr void get(VertexType type, FormatT& fmt, size_t& count)  {}

#define SETBIT(SEM_TYPE, FMT_SHIFT, COUNT_SHIFT) \
		template<> static constexpr \
		u64 set<SEM_TYPE>(FormatT fmt, size_t count) { \
			return (u64)fmt << (u64)FMT_SHIFT | count << (u64)COUNT_SHIFT; \
		} \
//--------------
#define GETBIT(SEM_TYPE, FMT_SHIFT, FMT_MASK, COUNT_SHIFT, COUNT_MASK) \
		template<> static constexpr \
		void get<SEM_TYPE>(VertexType type, FormatT& fmt, size_t& count) { \
			fmt   = (FormatT)((u64)type >> (u64)FMT_SHIFT & FMT_MASK); \
			count = (u64)type >> (u64)COUNT_SHIFT & COUNT_MASK; \
		 } \
//--------------

#define CREATE_BIT_OP(SEM_TYPE, FMT_SHIFT, FMT_MASK, COUNT_SHIFT, COUNT_MASK) \
		SETBIT(SEM_TYPE, FMT_SHIFT, COUNT_SHIFT) \
		GETBIT(SEM_TYPE, FMT_SHIFT, FMT_MASK, COUNT_SHIFT, COUNT_MASK) \
//--------------

		CREATE_BIT_OP(SemanticT::Position,	 Bitfield::PosFormat,	 0xFF, Bitfield::None,		    0	);
		CREATE_BIT_OP(SemanticT::Color,		 Bitfield::ColorFormat,  0xFF, Bitfield::ColorCount,    0x3	);
		CREATE_BIT_OP(SemanticT::TexCoord,	 Bitfield::UVFormat,	 0xFF, Bitfield::UVCount,	    0xFF);
		CREATE_BIT_OP(SemanticT::Normal,	 Bitfield::NormalFormat, 0xFF, Bitfield::NormalCount,   0x3	);
		CREATE_BIT_OP(SemanticT::Tangent,	 Bitfield::NormalFormat, 0xFF, Bitfield::TangentCount,  0x3	);
		CREATE_BIT_OP(SemanticT::Binormal,	 Bitfield::NormalFormat, 0xFF, Bitfield::BinormalCount, 0x3	);

#undef CREATE_BIT_OP
#undef GETBIT
#undef SETBIT
	};


	class VertexLayout : public NonCopyable {
	public:
		using VtxBitUtil = VertexType_BitfieldUtil;
		using SemanticT	 = Vertex_SemanticType;
		using FormatT	 = Render_FormatType;
	
		struct Element {

			Element() = default;
			Element(FormatT format_, SemanticT semantic_, u8 semanticIdx_, u16 offset_)
				: format(format_)
				, semantic(semantic_)
				, semanticIdx(semanticIdx_)
				, offset(offset_)
			{
			}


			FormatT	  format		= FormatT::None;
			SemanticT semantic		= SemanticT::None;
			u8		  semanticIdx	= 0;
			u16		  offset		= 0;
		};
	
		VertexType	typeId = VertexType::None;
		size_t		stride = 0;
		Vector_<Element, 16> elements;

		template<class DESC>
		void addElement() {
			
			
		}


	};

	template<Vertex_SemanticType SMT_TYPE, Render_FormatType FMT_TYPE, size_t COUNT>
	struct Vertex_ElementDesc {
		static const Vertex_SemanticType semanticType = SMT_TYPE;
		static const Render_FormatType   formatType	  = FMT_TYPE;
		static const size_t				 count		  = COUNT;
	};

	template<class ...DESCs> 
	struct Vertex;

	template<class DESC>
	struct Vertex <DESC>
	{
		using TUtil = VertexType_BitfieldUtil;

		static const VertexType kType = 
			static_cast<VertexType>(TUtil::set<DESC::semanticType>(DESC::formatType, DESC::count));		

		static constexpr void onRegister(VertexLayout* layout) {
			layout->addElement<DESC>();
		}
	};

	template<class DESC1, class ...DESCs>
	struct Vertex<DESC1, DESCs...>
	{
		static const VertexType kType = static_cast<VertexType>(static_cast<u64>(Vertex<DESC1>   ::kType) + 
															    static_cast<u64>(Vertex<DESCs...>::kType));

		static constexpr void onRegister(VertexLayout* layout) {
			Vertex<DESC1>   ::onRegister(layout);
			Vertex<DESCs...>::onRegister(layout);
		}
	};


	struct Vertex_ElementDescLib {
		using FMT = Render_FormatType;
		using SMT = Vertex_SemanticType;

		using pos_f32x3_c1 = Vertex_ElementDesc<SMT::Position,	FMT::Floatb32x3, 1>;
		using tex_f32x2_c1 = Vertex_ElementDesc<SMT::TexCoord,	FMT::Floatb32x2, 3>;
		using col_c32x4_c1 = Vertex_ElementDesc<SMT::Color,		FMT::UNormb08x4, 1>;
	};


	struct VertexLib {
		using Desc	 = Vertex_ElementDescLib;

		using Pos    = Vertex<Desc::pos_f32x3_c1>;
		using PosTex = Vertex<Desc::pos_f32x3_c1, Desc::tex_f32x2_c1>;
	};
}