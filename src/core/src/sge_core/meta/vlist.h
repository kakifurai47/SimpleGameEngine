

namespace sge  {
namespace meta {
	template<auto... Vs> struct vlist {};

	template<size_t N, auto V, auto... Vs> constexpr decltype(auto) get(vlist<V, Vs...>) noexcept {
		if constexpr (N == 0)	{ return V;							 }
		else					{ return get<N - 1>(vlist<Vs...>{}); }
	}

	template<auto... Vs> void print_list(vlist<Vs...>) { (print_value<Vs>(), ...); }
	template<auto	 V > void print_value() {
		if constexpr (std::is_enum<decltype(V)>())	{ SGE_LOG("print_value: {}", static_cast<std::underlying_type_t<decltype(V)>>(V)); }
		else										{ SGE_LOG("print_value: {}", V); }
	}

	template<auto... V1s, auto... V2s> constexpr auto cocat(vlist<V1s...>, vlist<V2s...>)  noexcept ->vlist<V1s..., V2s...> { return{}; }
	template<auto... V1s, auto... V2s, class... Ts>
	constexpr auto cocat(vlist<V1s...>, vlist<V2s...>,  Ts  ...)  noexcept
		->decltype(cocat(vlist<V1s...,		  V2s...>{},Ts{}...)) {
		return{};
	}


}
}
