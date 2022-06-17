#pragma once

#include <EASTL/optional.h>

namespace sge {
namespace meta {
	template<class T> struct type_identity { using type = T; };

	template<bool COND, class T = void> using enableIf = std::enable_if_t<COND, T>;


	struct any_type   {};
	struct null_type  {};
	struct null_value {};

	template<class T> constexpr bool is_null  (T) noexcept { return  std::is_same_v<null_type,  T>; }
	template<class T> constexpr bool has_value(T) noexcept { return !std::is_same_v<null_value, T>; }

	template<class T> using opt	= eastl::optional<T>;
	using nullopt = eastl::nullopt_t;


	template<bool B, class T, class F>  using cond   = std::conditional<B, T, F>;
	template<bool B, class T, class F>  using cond_t = typename cond<B, T, F>::type;
	
	template<bool B> using is_cond   = cond<B, std::true_type, std::false_type>;
	template<bool B> using is_cond_t = typename is_cond<B>::type;
}
}
