#pragma once

namespace sge  {
namespace meta {
	struct TypeTraitUtil {
	private:
		template<class T, class SE = meta::any_type, class = void> struct is_serdes : std::false_type {};
		template<class T, class SE>
		struct is_serdes<T, SE, std::void_t<decltype(onSerDes(std::declval<SE&>(), std::declval<T&>()))>> :
								std::true_type {};

		template<class T>			struct is_vector				: std::false_type {};
		template<class T>			struct is_vector<Vector<T>>		: std::true_type  {};
		template<class T, size_t N> struct is_vector<Vector_<T, N>> : std::true_type  {};

		template<class T>			struct is_string				: std::false_type {};
		template<class T, size_t N>	struct is_string<StringT<T, N>> : std::true_type  {};

	public:
		template<class T> bool static constexpr isArithmetic () { return std::is_arithmetic<T>::value; }
		template<class T> bool static constexpr isEnum		 () { return std::is_enum<T>::value;	   }
		template<class T> bool static constexpr isSerDes	 () { return is_serdes<T>::value;		   }
		template<class T> bool static constexpr isString	 () { return is_string<T>::value;		   }
		template<class T> bool static constexpr isVector	 () { return is_vector<T>::value;		   }
	};
}
}