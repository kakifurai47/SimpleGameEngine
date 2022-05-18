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

		template<class D1, class D2> struct ElmDescComp {
			static const bool value =
				(enumInt(getSMT(D1{})) << bit_size(FT{}) | enumInt(getFMT(D1{}))) <
				(enumInt(getSMT(D2{})) << bit_size(FT{}) | enumInt(getFMT(D2{})));
		};

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

		template<ST SMT_TYPE, size_t SMT_IDX, size_t RET_IDX>
		static constexpr auto _getSlotIdx(meta::empty_tlist) noexcept -> meta::null_type { return {}; }

		template<ST SEARCH_SMT_TYPE, size_t SEARCH_SMT_IDX,
				 size_t RET_IDX = 0,
				 ST	SMT_T, size_t SMT_IDX, FT FMT_T,
				 class... SLOTs>
		static constexpr auto _getSlotIdx(meta::tlist<Slot<SMT_T, SMT_IDX, FMT_T>, SLOTs...>) noexcept {			
			if constexpr( SMT_T	  == SEARCH_SMT_TYPE && 
						  SMT_IDX == SEARCH_SMT_IDX) //TODO: constant get<>();
				 { return RET_IDX;																			  }
			else { return _getSlotIdx<SEARCH_SMT_TYPE, SEARCH_SMT_IDX, RET_IDX + 1>(meta::tlist<SLOTs...>{}); }
		}
	public:
		template<class... DESCs>  using SortedDESCs = decltype(meta::insertion_sort<ElmDescComp>(meta::tlist<DESCs...>{}));
		template<class... DESCs>  static constexpr auto makeSlots() noexcept { return _makeSlots(SortedDESCs<DESCs...>{});}

		template<ST SMT_TYPE, size_t SMT_IDX, class SLOT_LIST> static constexpr
		auto getSlotIdx(SLOT_LIST) noexcept { return _getSlotIdx<SMT_TYPE, SMT_IDX> (SLOT_LIST{}); }

		template<class  SLOT_LIST> using VtxData = decltype(		_makeVtxDataType(SLOT_LIST{})  );
	};

	struct VertexType {
	private:
		using SmtType = Vertex_SemanticType;
		using FmtType = Render_FormatType;

		using Range = eastl::pair<size_t, size_t>;

		//cannot get member offset at compile time, workaround?
		static constexpr Range _getRange(SmtType smt) { 
			switch (smt) {
			case SmtType::Position: return Range( 0,  1);
			case SmtType::Color:    return Range( 1,  5);
			case SmtType::Texcoord: return Range( 5, 13);
			case SmtType::Normal:   return Range(13, 14);
			case SmtType::Tangent:  return Range(14, 15);
			case SmtType::Binormal: return Range(15, 16);
			}
			return Range{};
		}

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

		template<class... Ts> static constexpr
		VertexType make(Ts&&... ts) {
			auto&& ret = _make(SGE_FORWARD(ts)...);
			//TODO : add optional sorting here
			return ret;
		}

		//TODO : add comparator here for map

	private:
		template<auto... Vs> static constexpr
		VertexType _make(meta::vlist<Vs...>&&) {
			return _make(SGE_FORWARD(Vs)...);
		}

		template<class... Ts> static constexpr
		VertexType _make(SmtType smt, FmtType fmt, size_t cnt, Ts&&... ts) {
			auto&& temp = _add(SGE_FORWARD(VertexType{}), smt, fmt, cnt);
			return _add(SGE_FORWARD(temp), SGE_FORWARD(ts)...		   );
		}

		static constexpr VertexType _add(VertexType&& rhs) { return rhs; }

		static constexpr
		VertexType _add(VertexType&& rhs, SmtType smt, FmtType fmt, size_t count) {
			auto range = _getRange(smt);
			auto start = range.first;
			auto end   = range.second;

			for (size_t i = start; i < end; i++) {
				if (rhs.formats[i] == FmtType::None) {
					break;
				}
				start++;
			}
			//TODO : static assert if intended add desc exceed allowance
			//	   : warning at run-time.
			for (size_t i = 0, slotIdx = start; i < count; i++, slotIdx++) {
				if (slotIdx >= end) {
					return rhs;
				}
				rhs.formats[slotIdx] = fmt;
			}
			return rhs;
		}

		template<class ...Ts> static constexpr
		decltype(auto) _add(VertexType&& rhs, SmtType smt, FmtType fmt, size_t cnt, Ts&&... ts) {
			return _add(SGE_FORWARD(_add(SGE_FORWARD(rhs), smt, fmt, cnt)), SGE_FORWARD(ts) ...);
		}
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

		//TODO : fill layout here

	};

	template<class... DESCs>
	class Vertex {
		using ST = Vertex_SemanticType;
		using FT = Render_FormatType;
		using UT = Vertex_Util;

		using SlotList = decltype (UT::makeSlots<DESCs...>());
		UT::VtxData<SlotList> m_data;

		template<ST SMT_TYPE, size_t SMT_IDX> static constexpr decltype(auto) idx	   ()  noexcept { return UT::getSlotIdx<SMT_TYPE, SMT_IDX>(SlotList{});	}
		template<ST SMT_TYPE, size_t SMT_IDX> static constexpr decltype(auto) valid	   ()  noexcept { return !meta::is_null(idx<SMT_TYPE, SMT_IDX>());		}
		template<ST SMT_TYPE, size_t SMT_IDX>		 constexpr decltype(auto) slotData ()  noexcept { return eastl::get<idx<SMT_TYPE, SMT_IDX>()>(m_data);	}
		template<ST SMT_TYPE, size_t SMT_IDX> using							  slotData_t = decltype (		 eastl::get<idx<SMT_TYPE, SMT_IDX>()>(m_data)	);

		template<ST SMT_TYPE, size_t SMT_IDX>
		constexpr meta::enif_t<valid<SMT_TYPE, SMT_IDX>(), slotData_t<SMT_TYPE, SMT_IDX>>
		getSlotData() {
			return slotData<SMT_TYPE, SMT_IDX>();
		}
	public:
		static constexpr VertexType kType() { return VertexType::make(meta::cocat(DESCs{}...)); } //work around? w/o cocat

		template<size_t SMT_IDX> constexpr decltype(auto) position	() { return getSlotData<ST::Position,	SMT_IDX>(); }
		template<size_t SMT_IDX> constexpr decltype(auto) color		() { return getSlotData<ST::Color,		SMT_IDX>(); }
		template<size_t SMT_IDX> constexpr decltype(auto) texcoord	() { return getSlotData<ST::Texcoord,	SMT_IDX>(); }
		template<size_t SMT_IDX> constexpr decltype(auto) normal	() { return getSlotData<ST::Normal,		SMT_IDX>(); }
		template<size_t SMT_IDX> constexpr decltype(auto) tangent	() { return getSlotData<ST::Tangent,	SMT_IDX>(); }
		template<size_t SMT_IDX> constexpr decltype(auto) binormal	() { return getSlotData<ST::Binormal,	SMT_IDX>(); }
	};

	struct VertexElmDescLib {
	private:
		using FT = Render_FormatType;
		using ST = Vertex_SemanticType;
	public:
		using pos_f32x3_c1 = Vertex_ElmDesc<ST::Position, FT::Float32x3, 1>;
		using tex_f32x2_c1 = Vertex_ElmDesc<ST::Texcoord, FT::Float32x2, 1>;
		using col_c32x4_c1 = Vertex_ElmDesc<ST::Color,	  FT::UNorm08x4, 1>;
	};

	struct VertexLib {
	private:
		using DL = VertexElmDescLib;
	public:
		using Pos		= Vertex<DL::pos_f32x3_c1>;
		using PosTex	= Vertex<DL::pos_f32x3_c1, DL::tex_f32x2_c1>;
		using PosCol	= Vertex<DL::col_c32x4_c1, DL::pos_f32x3_c1>;
		using PosTexCol = Vertex<DL::pos_f32x3_c1, DL::tex_f32x2_c1, DL::col_c32x4_c1>;
	};
}