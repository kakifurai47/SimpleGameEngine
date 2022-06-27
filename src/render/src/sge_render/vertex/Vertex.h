#pragma once

#include "sge_render/backend/base/RenderFormat.h"


namespace sge {

	enum class VertexSemantic : u16;
	SGE_ENUM_ALL_OPERATOR(VertexSemantic)

	using VertexSemanticIndex = u8;

	enum class VertexSemanticType : u8 {
		None,
		POSITION,
		COLOR,
		TEXCOORD,
		NORMAL,
		TANGENT,
		BINORMAL,
	};

#define VertexSemanticType_ENUM_LIST(E) \
		E(None) \
		E(POSITION) \
		E(COLOR) \
		E(TEXCOORD) \
		E(NORMAL) \
		E(TANGENT) \
		E(BINORMAL) \
//-------
	SGE_ENUM_STR_UTIL(VertexSemanticType)


	struct VertexSemanticUtil {
		using Semantic = VertexSemantic;
		using Type  = VertexSemanticType;
		using Index = VertexSemanticIndex;

		static constexpr Semantic make(Type type, Index index) {
			return static_cast<Semantic>((enumInt(type) << 8) | index);
		};

		static constexpr u16 _make(Type type, Index index) {
			return static_cast<u16>(make(type, index));
		};

		static constexpr Type	getType (Semantic v) { return static_cast<Type>(enumInt(v) >> 8); }
		static constexpr Index	getIndex(Semantic v) { return static_cast<u8  >(enumInt(v)); }
	};

	enum class VertexSemantic : u16 {
		None = 0,

		POSITION	= VertexSemanticUtil::_make(VertexSemanticType::POSITION, 0),

		COLOR0		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 0),
		COLOR1		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 1),
		COLOR2		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 2),
		COLOR3		= VertexSemanticUtil::_make(VertexSemanticType::COLOR, 3),

		TEXCOORD0	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 0),
		TEXCOORD1	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 1),
		TEXCOORD2	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 2),
		TEXCOORD3	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 3),
		TEXCOORD4	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 4),
		TEXCOORD5	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 5),
		TEXCOORD6	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 6),
		TEXCOORD7	= VertexSemanticUtil::_make(VertexSemanticType::TEXCOORD, 7),

		NORMAL		= VertexSemanticUtil::_make(VertexSemanticType::NORMAL,   0),
		TANGENT		= VertexSemanticUtil::_make(VertexSemanticType::TANGENT,  0),
		BINORMAL	= VertexSemanticUtil::_make(VertexSemanticType::BINORMAL, 0),
	};

#define VertexSemantic_ENUM_LIST(E) \
	E(None)	\
	E(POSITION) \
	\
	E(COLOR0) \
	E(COLOR1) \
	E(COLOR2) \
	E(COLOR3) \
	\
	E(TEXCOORD0) \
	E(TEXCOORD1) \
	E(TEXCOORD2) \
	E(TEXCOORD3) \
	E(TEXCOORD4) \
	E(TEXCOORD5) \
	E(TEXCOORD6) \
	E(TEXCOORD7) \
	\
	E(NORMAL)   \
	E(TANGENT)  \
	E(BINORMAL) \
	//----
	SGE_ENUM_STR_UTIL(VertexSemantic)

	template<VertexSemanticType SMT_TYPE, class FMT, u8 CNT>
	struct VertexElmDesc {
	private:
		using Util =  RenderFormatTypeUtil;
	public:
		static constexpr auto semantic_t() noexcept { return SMT_TYPE;			}
		static constexpr auto format	() noexcept { return FMT{};				}
		static constexpr auto format_t	() noexcept { return Util::get<FMT>();	}
		static constexpr auto count		() noexcept { return CNT;		 		}
	};

	struct VertexType {
	private:
		using SmtType = VertexSemanticType;
		using FmtType = RenderFormatType;

		using Range = eastl::pair<u8, u8>;

		static constexpr Range _range(SmtType semanticType) {
			switch (semanticType) {
				case SmtType::POSITION:	return Range(0,  1);
				case SmtType::COLOR:	return Range(1,  5);
				case SmtType::TEXCOORD:	return Range(5,  13);
				case SmtType::NORMAL:	return Range(13, 14);
				case SmtType::TANGENT:	return Range(14, 15);
				case SmtType::BINORMAL:	return Range(15, 16);
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
			u64 data[2];
		};

		static VertexType None() { return {}; }


		template<class... Ts> static constexpr
		VertexType make(Ts&&... ts) {
			auto&& ret = _make(SGE_FORWARD(ts)...);
			ret.sort();
			return ret;
		}

		friend bool operator<(const VertexType& lhs, const VertexType& rhs) {
			if (lhs.data[1] != rhs.data[1]) { return lhs.data[1] < rhs.data[1]; }
			return lhs.data[0] < rhs.data[0];
		}

		friend bool operator ==(const VertexType& lhs, const VertexType& rhs) {
			return lhs.data[0] == rhs.data[0] &&
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
			return _add(SGE_FORWARD(temp), SGE_FORWARD(ts)...);
		}

		static constexpr VertexType _add(VertexType&& rhs) { return rhs; }

		static constexpr
		VertexType _add(VertexType&& rhs, SmtType smt, FmtType fmt, size_t count) {
			if (count <= 0) {
				return rhs;
			}
			auto range = _range(smt);
			auto start = range.first;
			auto end = range.second;

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
			Sort::InsertionSort(color);
			Sort::InsertionSort(texcoord);
		}
	};

	struct VertexLayout : public NonCopyable {
		using FormatType = RenderFormatType;
		using Semantic	 = VertexSemantic;
	public:
		struct Element {
			Semantic	 semantic	= Semantic::None;
			u16			 offset		= 0;
			FormatType	 formatType	= FormatType::None;
		};

		Vector_<Element, 16>	elements;
		VertexType				type;
		size_t					stride;

		template<class VERTEX> constexpr 
		void set(VERTEX) {
			type   = VERTEX::kType();
			stride = sizeof (VERTEX);
			_set(VERTEX::SlotList{});
		}

	private:
		template<class... SLOTs> constexpr 
		void _set(meta::tlist<SLOTs...>, u16 offset_ = 0)  {
			using slots = meta::tlist<SLOTs...>;
			if constexpr (meta::is_empty(slots{})) {
				return;
			}
			else {
				using pop = decltype( meta::front(slots{}) );
				auto& elm = elements.emplace_back();
				elm.formatType	= pop::format_t();
				elm.semantic	= pop::semantic();
				elm.offset		= offset_;
				_set(meta::pop_front(slots{}), offset_ + sizeof(decltype(pop::format())));
			}
		}
	};

	template<class FMT, VertexSemantic SMT>
	struct VertexSlot {
	public:
		static constexpr auto format  () noexcept { return FMT{}; }
		static constexpr auto format_t() noexcept { return RenderFormatTypeUtil::get<FMT>(); }
		static constexpr auto semantic() noexcept { return SMT;   }
	};

	template<class... DESCs>
	class VertexSlotUtil {
		using Util  = VertexSemanticUtil;

		using FT	= RenderFormatType;
		using ST	= VertexSemanticType;
		using SM	= VertexSemantic;

		template<class D1, class D2>
		struct DescComparator {
			static const bool value =
				(enumInt(D1::semantic_t()) << bit_size(FT{}) | enumInt(D1::format_t())) <
				(enumInt(D2::semantic_t()) << bit_size(FT{}) | enumInt(D2::format_t()));
		};

		template<class SLOT, SM SMT>
		struct IsSameSlot : meta::is_cond_t<SLOT::semantic() == SMT> {};

		template<class... DESCs> static constexpr
		auto _applyExpansion(meta::tlist<DESCs...>) noexcept
			-> decltype(meta::expandList(meta::tlist<DESCs...>{},
										 meta::vlist<DESCs::count()...>{})) {
			return {};
		}

		template<u8 SMT_IDX = 0, class DESC, class... DESCs> static constexpr
		auto _createSlot(meta::tlist<DESC, DESCs...>) noexcept {
			using rest = meta::tlist<DESCs...>;
			using add  = VertexSlot< decltype(DESC::format()), Util::make(DESC::semantic_t(), SMT_IDX) >;

			if constexpr (meta::is_empty(rest{})) {
				return meta::tlist<add>{};
			}
			else {
				using prev = decltype ( meta::front	(rest{}) );
				static constexpr u8 next_idx = prev::semantic_t() == DESC::semantic_t() ? SMT_IDX + 1 : 0;
				return meta::push_front(_createSlot<next_idx>(rest{}), add{});
			}
		}

		template<class... DESCs> static constexpr 
		auto _makeSlotList() noexcept {
			using	sorted	= decltype( meta::insertion_sort<DescComparator>( meta::tlist<DESCs...>{} ));
			return _createSlot( _applyExpansion(sorted{}) );
		}

		template<class... SLOTs> static constexpr
		auto _makeStorage(meta::tlist<SLOTs...>)  noexcept
			-> eastl::tuple< decltype(SLOTs::format())... > {
			return {};
		}
	public:
		using slotList	= decltype ( _makeSlotList<DESCs...>()  );
		using storage	= decltype ( _makeStorage(slotList {})	);
		
		template<ST SMT_TYPE, u8 IDX>
		struct Data {
			template<class DESC> using is_same = IsSameSlot<DESC, Util::make(SMT_TYPE, IDX)>;
			using index	= decltype( meta::find_idx<is_same>( slotList{} ));
		};
	};

	template<class... DESCs> 
	class Vertex {
		using ST		= VertexSemanticType;
		using SlotUtil  = VertexSlotUtil<DESCs...>;
	public:
		using SlotList	= typename SlotUtil::slotList;
		using Storage	= typename SlotUtil::storage;
	
		static constexpr VertexType kType() {
			return VertexType::make(meta::cocat(meta::vlist<DESCs::semantic_t(), DESCs::format_t(), DESCs::count()>{}...));
		}
		template<u8 SMT_IDX> constexpr inline decltype(auto) position() { return _data<ST::POSITION, SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) color	 () { return _data<ST::COLOR,	 SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) texcoord() { return _data<ST::TEXCOORD, SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) normal  () { return _data<ST::NORMAL,	 SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) tangent () { return _data<ST::TANGENT,  SMT_IDX>(); }
		template<u8 SMT_IDX> constexpr inline decltype(auto) binormal() { return _data<ST::BINORMAL, SMT_IDX>(); }
	
	private:
		template<ST SMT_TYPE, u8 SMT_IDX> constexpr inline
		decltype(auto) _data() {
			using index = typename SlotUtil::Data<SMT_TYPE, SMT_IDX>::index;
			if constexpr (meta::has_value(index{})) {
				return eastl::get< meta::get_value(index{}) >(m_storage);
			} else {
				static_assert(false, "invalid vertex idx");
			}
		}
		Storage m_storage;
	};

	struct VertexElmDescLib {
	private:
		using ST = VertexSemanticType;
	public:
		using Pos_Tup3f_C1	= VertexElmDesc<ST::POSITION,	Tuple3f, 1>;
		using Col_Col4b_C1	= VertexElmDesc<ST::COLOR,		Color4b, 1>;
		using Tex_Tup2f_C1	= VertexElmDesc<ST::TEXCOORD,	Tuple2f, 1>;

		using Tan_Tup3f_C1	 = VertexElmDesc<ST::TANGENT,	Tuple3f, 1>;
		using Norm_Tup3f_C1	 = VertexElmDesc<ST::NORMAL,	Tuple3f, 1>;
		using BNorm_tup3f_C1 = VertexElmDesc<ST::BINORMAL,	Tuple3f, 1>;
	};

	struct VertexLib {
	private:
		using DL = VertexElmDescLib;
	public:
		using Pos			= Vertex<DL::Pos_Tup3f_C1>;
		using PosCol		= Vertex<DL::Pos_Tup3f_C1, DL::Col_Col4b_C1>;
		using PosTex		= Vertex<DL::Pos_Tup3f_C1, DL::Tex_Tup2f_C1>;
		using PosColTex		= Vertex<DL::Pos_Tup3f_C1, DL::Col_Col4b_C1, DL::Tex_Tup2f_C1>;
		using PosColTexNorm = Vertex<DL::Pos_Tup3f_C1, DL::Col_Col4b_C1, DL::Tex_Tup2f_C1, DL::Norm_Tup3f_C1>;
	};
}
