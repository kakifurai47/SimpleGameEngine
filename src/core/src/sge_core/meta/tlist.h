#include <sge_core/log/Log.h>

namespace sge  {
namespace meta {
	template<class... Ts> struct tlist {};
	using		  empty_tlist  = tlist <>;

	template<class... Ts> constexpr bool is_empty(tlist<Ts...>) noexcept { return false; }
						  constexpr bool is_empty(empty_tlist ) noexcept { return true;	 }

	template<class... Ts> constexpr size_t get_size(tlist<Ts...>) noexcept { return sizeof...(Ts); }

	template<class    T > void print_type (const T&)	 {  SGE_LOG("print_type: {}", typeid(T).name()); }
	template<class... Ts> void print_list (tlist<Ts...>) { (print_type(Ts{}), ...); }

	template<class... Ts, class	   ELE > constexpr auto push_back (tlist<Ts...>, ELE	) noexcept -> tlist<Ts..., ELE	  > { return {}; }
	template<class... Ts, class... ELEs> constexpr auto push_front(tlist<Ts...>, ELEs...) noexcept -> tlist<ELEs..., Ts...> { return {}; }		
	
	template<class H, class... Ts> constexpr auto front(tlist<H, Ts...>) noexcept -> H		   { return {}; }
								   constexpr auto front(empty_tlist	   ) noexcept -> null_type { return {}; }

	template<class... Ts> constexpr auto back(meta::tlist<Ts...>) noexcept -> typename decltype((type_identity<Ts>{}, ...))::type	{ return {}; }
						  constexpr auto back(empty_tlist		) noexcept -> null_type												{ return {}; }

	template<class H, class... Ts> constexpr auto pop_front(tlist<H, Ts...>) noexcept -> tlist<Ts...> { return {}; }
								   constexpr auto pop_front(empty_tlist    ) noexcept -> empty_tlist  { return {}; }

	template<size_t N>						 constexpr auto get(empty_tlist)	 noexcept -> null_type { return {}; }
	template<size_t N, class T, class... Ts> constexpr auto get(tlist<T, Ts...>) noexcept {
		if constexpr (N == 0) { return T{};						   }
		else				  { return get<N - 1>(tlist<Ts...>{}); }
	}

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
		using NewH = cond_t<COMPARE<ADD, H>::value, ADD, H>;
		using NewT = cond_t<COMPARE<ADD, H>::value, tlist<H, Ts...>,
													decltype(insert_sorted<COMPARE>(tlist<Ts...>{}, ADD{}))>;
		return push_front(NewT{}, NewH{});
	}

	//Insertion Sort
	template<template<class L, class R>class COMPARE> constexpr auto insertion_sort(empty_tlist) noexcept -> empty_tlist { return{}; }
	template<template<class L, class R>class COMPARE, class H, class... Ts>
	constexpr auto insertion_sort(tlist<H, Ts...>) noexcept {
		return insert_sorted<COMPARE>(insertion_sort<COMPARE>(tlist<Ts...>{}), H{});
	}

	//find
	template<template<class>class PRED> constexpr auto find(empty_tlist) noexcept -> null_type { return {}; }
	template<template<class>class PRED, class H, class... Ts>
	constexpr auto find(tlist<H, Ts...>) noexcept {
		if constexpr ( PRED<H>::value ) { return H{};						 }
		else							{ return find<PRED>(tlist<Ts...>{}); }
	}

	//find index
	template<template<class>class PRED, size_t IDX> constexpr auto find_idx(empty_tlist) noexcept -> null_value { return {}; }
	template<template<class>class PRED, size_t IDX = 0, class H, class... Ts>
	constexpr auto find_idx(tlist<H, Ts...>) noexcept {
		if constexpr ( PRED<H>::value ) { return vlist<IDX>{};							  }
		else							{ return find_idx<PRED, IDX + 1>(tlist<Ts...>{}); }
	}

	//filter
	template<template<class>class PRED> constexpr auto filter(empty_tlist) noexcept -> empty_tlist { return {}; }
	template<template<class>class PRED, class H, class... Ts>
	constexpr auto filter(tlist<H, Ts...>) noexcept {
		if constexpr (PRED<H>::value) { return push_front(filter<PRED>(tlist<Ts...>{}), H{}); }
		else						  { return filter<PRED>(tlist<Ts...>{});				  }
	}

	//expand
	template<size_t SIZE, class T>
	constexpr auto expand(T) {
		if	 constexpr (SIZE == 0) { return empty_tlist{}; }
		else					   { return push_front(expand<SIZE - 1>(T{}), T{}); }
	}

	//expand List
	template<class... Ts, auto... Vs>
	constexpr auto expandList(tlist<Ts...>, vlist<Vs...>)
		-> decltype (cocat(expand<Vs>(Ts{})...)) { return {}; }


	template<class LHS, class RHS> struct by_size_comp { static const bool value = sizeof(LHS) < sizeof(RHS); };
}
}


