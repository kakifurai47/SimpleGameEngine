

namespace sge  {
namespace meta {

	struct null_type {};
	template<class T> constexpr bool is_null(T) noexcept { return std::is_same_v<null_type, T>; }

	template<class... Ts> struct tlist {};
	using		  empty_tlist  = tlist <>;

	template<class... Ts> constexpr bool is_empty(tlist<Ts...>) noexcept { return false; }
						  constexpr bool is_empty(empty_tlist ) noexcept { return true;	 }

	template<class    T > void print_type (T)			 {  SGE_LOG("print_type: {}", typeid(T).name()); }
	template<class... Ts> void print_list (tlist<Ts...>) { (print_type(Ts{}), ...); }

	template<class... Ts, class	   ELE > constexpr auto push_back (tlist<Ts...>, ELE	) noexcept -> tlist<Ts..., ELE	  > { return {}; }
	template<class... Ts, class... ELEs> constexpr auto push_front(tlist<Ts...>, ELEs...) noexcept -> tlist<ELEs..., Ts...> { return {}; }		
	
	template<class H, class... Ts> constexpr auto front(tlist<H, Ts...>) noexcept -> H		   { return {}; }
								   constexpr auto front(empty_tlist	   ) noexcept -> null_type { return {}; }

	template<class... Ts> constexpr auto back(meta::tlist<Ts...>) noexcept -> typename decltype((type_identity<Ts>{}, ...))::type	{ return {}; }
						  constexpr auto back(empty_tlist		) noexcept -> null_type												{ return {}; }

	template<class H, class... Ts> constexpr auto pop_front(tlist<H, Ts...>) noexcept -> tlist<Ts...> { return {}; }
								   constexpr auto pop_front(empty_tlist    ) noexcept -> empty_tlist  { return {}; }

	//Concatenation
	template<class... T1s, class... Ts> constexpr auto cocat(tlist<T1s...>, Ts...) noexcept -> tlist<T1s...> { return {}; }
	template<class... T1s, class... T2s, class... Ts>
	constexpr auto cocat(tlist<T1s...>, tlist<T2s...>,	 Ts	 ...)  noexcept
		->decltype(cocat(tlist<T1s...,		  T2s...>{}, Ts{}...) ) { 
		return{}; 
	}

	//Insert Sorted
	template<template<class L, class R>class COMPARE, class ADD>
	constexpr auto insert_sorted(empty_tlist,   ADD   ) noexcept
		-> decltype(push_front  (empty_tlist{}, ADD{})) {
		return {};
	}
	template<template<class L, class R>class COMPARE, class H, class ...Ts, class ADD>
	constexpr auto insert_sorted(tlist<H, Ts...>, ADD) noexcept {
		using NewH = std::conditional_t<COMPARE<ADD, H>::value, ADD, H>;
		using NewT = std::conditional_t<COMPARE<ADD, H>::value, tlist<H, Ts...>,
																decltype(insert_sorted<COMPARE>(tlist<Ts...>{}, ADD{}))>;
		return push_front(NewT{}, NewH{});
	}

	//Insertion Sort
	template<template<class L, class R>class COMPARE> constexpr auto insertion_sort(empty_tlist) noexcept -> empty_tlist { return{}; }
	template<template<class L, class R>class COMPARE, class H, class... Ts>
	constexpr auto insertion_sort(tlist<H, Ts...>) noexcept {
		return insert_sorted<COMPARE>(insertion_sort<COMPARE>(tlist<Ts...>{}), H{});
	}


	template<class LHS, class RHS> struct by_size_comp { static const bool value = sizeof(LHS) < sizeof(RHS); };
}
}


