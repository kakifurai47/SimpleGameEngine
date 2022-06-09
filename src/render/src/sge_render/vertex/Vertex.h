#pragma once

#include "sge_render/backend/base/RenderFormat.h"


namespace sge {

	enum class VertexSemanticType : u8 {
		None,
		Position,
		Color,
		Texcoord,
		Normal,
		Tangent,
		Binormal,
	};

#define VertexSemanticType_ENUM_LIST(E) \
		E(None) \
		E(Position) \
		E(Color) \
		E(Texcoord) \
		E(Normal) \
		E(Tangent) \
		E(Binormal) \
//-------
	SGE_ENUM_STR_UTIL(VertexSemanticType)

	template<VertexSemanticType SMT_TYPE, class FMT, u8 CNT>
	struct VertexElmDesc {
	private:
		using Util =  RenderFormatTypeUtil;
	public:
		static constexpr auto format_t	() noexcept { return Util::get<FMT>();	}
		static constexpr auto semantic_t() noexcept { return SMT_TYPE;			}
		static constexpr auto count		() noexcept { return CNT;		 		}
		static constexpr auto format	() noexcept { return FMT{};				}
	};

	struct VertexType {
	private:
		using SmtType = VertexSemanticType;
		using FmtType = RenderFormatType;

		using Range = eastl::pair<u8, u8>;

		static constexpr Range _range(SmtType semanticType) {
			switch (semanticType) {
				case SmtType::Position:	return Range(0,  1);
				case SmtType::Color:	return Range(1,  5);
				case SmtType::Texcoord:	return Range(5,  13);
				case SmtType::Normal:	return Range(13, 14);
				case SmtType::Tangent:	return Range(14, 15);
				case SmtType::Binormal:	return Range(15, 16);
				default: SGE_ASSERT(false); return {};
			}
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
			ret.sort();
			return ret;
		}

		bool operator() (const VertexType& lhs, const VertexType& rhs) {
			return	lhs.data[0] == rhs.data[0] &&
					lhs.data[1] == rhs.data[1];
		}
	
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
			auto range = _range(smt);
			auto start = range.first;
			auto end   = range.second;

			for (size_t i = start; i < end; i++) {
				if (rhs.formats[i] == FmtType::None) {
					break;
				}
				start++;
			}
			SGE_ASSERT(start + count <= end);
			for (size_t i = 0, slotIdx = start; i < count; i++, slotIdx++) {
				if (slotIdx >= end) {
					return rhs;
				}
				rhs.formats[slotIdx] = fmt;
			}
			return rhs;
		}

		template<class... Ts> static constexpr
		decltype(auto) _add(VertexType&& rhs, SmtType smt, FmtType fmt, size_t cnt, Ts&&... ts) {
			return _add(SGE_FORWARD(_add(SGE_FORWARD(rhs), smt, fmt, cnt)), SGE_FORWARD(ts)...);
		}

		void sort() {
			InsertionSort(color);
			InsertionSort(texcoord);
		}
	};

	struct VertexLayout : public NonCopyable {
		using SmtType = VertexSemanticType;
		using FmtType = RenderFormatType;
	public:
		struct Element {
			FmtType	fmtType	= FmtType::None;
			SmtType	smtType	= SmtType::None;
			u8		smtIdx	= 0;
			u16		offset	= 0;
		};

		Vector_<Element, 16>	elements;
		VertexType				type;
		size_t					stride;

		template<class VERTEX> constexpr 
		void set(VERTEX) {
			type	 = VERTEX::kType();
			stride	 = sizeof (VERTEX);
			_set(VERTEX::DescList{});
		}

	private:
		template<class... DESCs> constexpr 
		auto _set(meta::tlist<DESCs...>, u16 offset_ = 0)  {
			using descs = meta::tlist<DESCs...>;
			if constexpr (meta::is_empty(descs{})) {
				return;
			}
			else {
				using pop = decltype( meta::front(descs{}) );
				auto& elm = elements.emplace_back();
				elm.fmtType =	pop::format_t();
				elm.smtType =	pop::semantic_t();
				elm.smtIdx	=	pop::count();
				elm.offset	=	offset_;
				_set(meta::pop_front(descs{}), offset_ + sizeof(decltype(pop::format())));
			}
		}
	};

	template<class... DESCs>
	class VertexSlots {
		using FT	= RenderFormatType;
		using ST	= VertexSemanticType;

		template<class D1, class D2>
		struct Comparator {
			static const bool value =
				(enumInt(D1::semantic_t()) << bit_size(FT{}) | enumInt(D1::format_t())) <
				(enumInt(D2::semantic_t()) << bit_size(FT{}) | enumInt(D2::format_t()));
		};

		template<class D1, ST SMT_TYPE, u8 SMT_IDX>
		struct IsSameDescription : meta::is_cond_t<D1::semantic_t() == SMT_TYPE &&
												   D1::count()	    == SMT_IDX> {};

		template<class... DESCs> static constexpr
		auto _applyExpansion(meta::tlist<DESCs...>) noexcept
			-> decltype(meta::expandList(meta::tlist<DESCs...>{},
										 meta::vlist<DESCs::count()...>{})) {
			return {};
		}

		template<u8 SMT_IDX = 0, class DESC, class... DESCs> static constexpr
		auto _setIndex(meta::tlist<DESC, DESCs...>) noexcept {
			using rest = meta::tlist<DESCs...>;
			if constexpr (meta::is_empty(rest{})) {
				return meta::tlist<VertexElmDesc<DESC::semantic_t(), decltype(DESC::format()), SMT_IDX>>{};
			}
			else {
				using prev = decltype ( meta::front	(rest{}) );
				static constexpr u8 smt_idx = prev::semantic_t() == DESC::semantic_t() ? SMT_IDX + 1 : 0;
				return meta::push_front(_setIndex<smt_idx>(rest{}),
										VertexElmDesc<DESC::semantic_t(), decltype(DESC::format()), smt_idx> {});
			}
		}

		template<class... DESCs> static constexpr 
		auto _makeDescList() noexcept {
			using	sorted	  = decltype( meta::insertion_sort<Comparator>( meta::tlist<DESCs...>{} ));
			return _setIndex( _applyExpansion(sorted{}) );
		}

		template<class... DESCs> static constexpr
		auto _makeStorage(meta::tlist<DESCs...>)  noexcept 
			-> eastl::tuple< decltype(DESCs::format())... > {
			return {};
		}
	public:
		using descList  = decltype ( _makeDescList<DESCs...>()  );
		using storage	= decltype ( _makeStorage(descList {})	);
		
		template<ST SMT_TYPE, u8 IDX>
		struct Data {
			template<class DESC> using is_same = IsSameDescription<DESC, SMT_TYPE, IDX>;
			using index	= decltype( meta::find_idx<is_same>( descList{} ));
		};
	};

	template<class... DESCs> 
	class Vertex {
		using ST		= VertexSemanticType;
		using Slot		= VertexSlots<DESCs...>;
	public:
		using DescList	= typename Slot::descList;
		using Storage	= typename Slot::storage;
	
		static const VertexType kType() {
			return VertexType::make(meta::cocat( meta::vlist<DESCs::semantic_t(), DESCs::format_t(), DESCs::count()>{}...));
		}
		template<u8 SMT_IDX> constexpr inline decltype(auto) position() { return _data<ST::Position, SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) color	 () { return _data<ST::Color,	 SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) texcoord() { return _data<ST::Texcoord, SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) normal  () { return _data<ST::Normal,	 SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) tangent () { return _data<ST::Tangent,  SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) binormal() { return _data<ST::Binormal, SMT_IDX>(); }
	
	private:
		template<ST SMT_TYPE, u8 SMT_IDX> constexpr inline
		decltype(auto) _data() {
			using index = typename Slot::Data<SMT_TYPE, SMT_IDX>::index;
			if constexpr (meta::has_value(index{})) {
				static_assert(false, "invalid vertex idx");
			} else {
				return eastl::get< meta::get_value(index{}) >(m_storage);
			}
		}
		Storage m_storage;
	};

	struct VertexElmDescLib {
	private:
		using ST = VertexSemanticType;
	public:
		using pos_tup3f_c1 = VertexElmDesc<ST::Position, Tuple3f, 1>;
		using col_col4b_c1 = VertexElmDesc<ST::Color,	 Color4b, 1>;
		using tex_tup2f_c1 = VertexElmDesc<ST::Texcoord, Tuple2f, 1>;

		using nrm_tup3f_c1 = VertexElmDesc<ST::Normal,   Tuple3f, 1>;
		using tan_tup3f_c1 = VertexElmDesc<ST::Tangent,  Tuple3f, 1>;
		using bnm_tup3f_c1 = VertexElmDesc<ST::Binormal, Tuple3f, 1>;
	};

	struct VertexLib {
	private:
		using DL = VertexElmDescLib;
	public:
		using Pos		= Vertex<DL::pos_tup3f_c1>;
		using PosCol	= Vertex<DL::pos_tup3f_c1, DL::col_col4b_c1>;
		using PosTex	= Vertex<DL::pos_tup3f_c1, DL::tex_tup2f_c1>;
		using PosColTex = Vertex<DL::pos_tup3f_c1, DL::col_col4b_c1, DL::tex_tup2f_c1>;
	};
}
