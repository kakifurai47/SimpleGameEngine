#pragma once

#include "sge_render/backend/base/RenderFormat.h"

namespace sge {

	enum class Vertex_SemanticType : u8 {
		None,
		Position,
		Color,
		Texcoord,
		Normal,
		Tangent,
		Binormal,
	};

	template<Vertex_SemanticType SMT, Render_FormatType	FMT, u8 CNT> 
	using Vertex_ElmDesc = meta::vlist<SMT, FMT, CNT>;

	struct VertexType;
	
	struct Vertex_Util {
	private:
		using ST = Vertex_SemanticType;
		using FT = Render_FormatType;

		template<ST	SMT_T, size_t SMT_IDX, FT FMT_T>
		using Slot = meta::vlist<SMT_T, SMT_IDX, FMT_T>;

		template<class D> static constexpr auto getSMT(D) noexcept { return meta::get<0>(D{}); }
		template<class D> static constexpr auto getFMT(D) noexcept { return meta::get<1>(D{}); }
		template<class D> static constexpr auto getCNT(D) noexcept { return meta::get<2>(D{}); }

		template<ST... SMT_TYPEs, size_t... SMT_IDXs, FT... FMT_TYPEs>
		static constexpr auto _makeVtxDataType(meta::tlist<meta::vlist<SMT_TYPEs, SMT_IDXs, FMT_TYPEs>...>) noexcept
			-> eastl::tuple<typename Render_DataType<FMT_TYPEs>::type...> {
			return {};
		}

		template<size_t COUNT = 0, size_t SMT_IDX = 0, class DESC_LIST>
		static constexpr auto _makeSlots(DESC_LIST descList) noexcept {
			if constexpr (meta::is_empty(descList)) {
				return meta::empty_tlist{};
			}
			else {
				using				front	   = decltype(meta::front		  (descList)			);
				static const bool   next	   =		    _moveNext		  (front{},		COUNT	);
				static const size_t nextCount  =		    _newCount		  (next,		COUNT	);
				using				nextStack  = decltype(  _newStack<next>	  (descList)			);
				static const size_t nextSmIdx  =		   _newSmtIdx<SMT_IDX>(nextStack{},	front{}	);

				return meta::push_front(_makeSlots<nextCount, nextSmIdx>(nextStack{}), 
										_slot<SMT_IDX>(front{}));
			}
		}
		
		template<ST D_SMT, FT D_FMT, size_t D_CNT>
		static constexpr bool _moveNext(Vertex_ElmDesc<D_SMT, D_FMT, D_CNT>, size_t COUNT) noexcept {
			if (COUNT + 1 >= D_CNT) { return true;  }
			else					{ return false; }
		}

		static constexpr size_t _newCount(bool moveNext, size_t count) noexcept { return moveNext ? 0 : count + 1;  }

		template<bool MOVE_NEXT,  class DESC_LIST>
		static constexpr auto _newStack(DESC_LIST descList) noexcept {
			if constexpr (MOVE_NEXT) { return meta::pop_front(descList); }
			else					 { return descList;					 }
		}

		template<size_t SMT_IDX, class DESC>
		static constexpr auto _newSmtIdx(meta::empty_tlist, DESC) noexcept { return 0; }

		template<size_t SMT_IDX, template<class...>class DESC_LIST, class... DESCs,
				 ST THIS_SMT, FT T_F, size_t T_C,
				 ST NEXT_SMT, FT N_F, size_t N_C>
		static constexpr auto _newSmtIdx(DESC_LIST<Vertex_ElmDesc<NEXT_SMT, N_F, N_C>, DESCs...>,
												   Vertex_ElmDesc<THIS_SMT, T_F, T_C>) noexcept {
			if constexpr (THIS_SMT == NEXT_SMT) { return SMT_IDX + 1;	}
			else								{ return 0;				}
		}

		template<size_t IDX, ST SMT, FT FMT, size_t CNT>
		static constexpr auto _slot(Vertex_ElmDesc<SMT, FMT, CNT>) noexcept
			-> Slot<SMT, IDX, FMT> {
			return {};
		}

	public:
		template<class D1, class D2> struct ElmDescComp {
			static const bool value = 
			( enumInt(getSMT(D1{})) << bit_size(FT{}) | enumInt(getFMT(D1{})) ) <
			( enumInt(getSMT(D2{})) << bit_size(FT{}) | enumInt(getFMT(D2{})) );
		};

		template<class... DESCs>  using SortedDESCs = decltype(meta::insertion_sort<ElmDescComp>(meta::tlist<DESCs...>{})  );

		template<class... DESCs>  static constexpr auto makeSlots() noexcept { return _makeSlots(SortedDESCs<DESCs...>{} ); }

		template<class SLOT_LIST> using VtxData = decltype(_makeVtxDataType(SLOT_LIST{}));
	};

	struct VertexType {
	private:
		using SmtType = Vertex_SemanticType;
		using FmtType = Render_FormatType;
	public:
		union {
			struct {
				FmtType position;
				FmtType color[4];
				FmtType texcoord[8];
				FmtType normal;
				FmtType tangent;
				FmtType binormal;
			};
			struct {
				FmtType formats[16];
			};
			u64 data[2]{};
		};

		//using fmtRange = std::tuple<size_t, size_t>;
		//
		//static constexpr fmtRange _getSlots(SmtType smt) {
		//	switch (smt) {
		//	case SmtType::Position: return fmtRange( 0,  1);
		//	case SmtType::Color:    return fmtRange( 1,  5);
		//	case SmtType::Texcoord: return fmtRange( 5, 13);
		//	case SmtType::Normal:   return fmtRange(13, 14);
		//	case SmtType::Tangent:  return fmtRange(14, 15);
		//	case SmtType::Binormal: return fmtRange(15, 16);
		//	}
		//	return fmtRange{};
		//}
	};

	class VertexLayout : public NonCopyable {
		using SmtType = Vertex_SemanticType;
		using FmtType = Render_FormatType;
	public:
		struct Element {
			FmtType format   = FmtType::None;
			SmtType semantic = SmtType::None;
			size_t  smtIdx	 = 0;
			size_t  offset	 = 0;
		};
		Vector_<Element, 16>	elements;
		VertexType				type;
		size_t					stride;
	};

	template<class... DESCs>
	class Vertex {	
		using ST = Vertex_SemanticType;
		using FT = Render_FormatType;
		using UT = Vertex_Util;
	public:
		using SlotList = decltype (		UT::makeSlots<DESCs...>()	);




		UT::VtxData<SlotList> data;
	};

	struct VertexElmDescLib {
	private:
		using F = Render_FormatType;
		using S = Vertex_SemanticType;
	public:
		using pos_f32x3_c1 = Vertex_ElmDesc<S::Position, F::Float32x3, 1>;
		using tex_f32x2_c1 = Vertex_ElmDesc<S::Texcoord, F::Float32x2, 1>;
		using tex_f32x3_c2 = Vertex_ElmDesc<S::Texcoord, F::Float32x3, 2>;
		using col_c32x4_c1 = Vertex_ElmDesc<S::Color,	 F::UNorm08x4, 4>;
	};

	struct VertexLib {
	private:
		using D = VertexElmDescLib;
	public:
		using Pos		= Vertex<D::pos_f32x3_c1>;
		using PosTex	= Vertex<D::pos_f32x3_c1, D::tex_f32x2_c1>;
		using PosCol	= Vertex<D::col_c32x4_c1, D::pos_f32x3_c1>;		
		using PosTexCol = Vertex<D::pos_f32x3_c1, D::tex_f32x2_c1, D::col_c32x4_c1, D::tex_f32x3_c2>;
	};
}