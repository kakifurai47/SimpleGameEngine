#pragma once

#include <EASTL/optional.h>

namespace sge  {
namespace meta {
	template<class T> struct type_identity { using type = T; };
	
	template<bool COND, class T = void> using enif_t = std::enable_if_t<COND, T>;

	struct any_type  {};

	struct null_type {};
	template<class T> constexpr bool is_null(T) noexcept { return std::is_same_v<null_type, T>; }

	template<class T> using opt = eastl::optional<T>;

}
}
