#pragma once

#include "meta_common.h"

#include <sge_core/graph/Color.h>
#include <sge_core/math/Tuple.h>

#include <sge_core/math/Vec2.h>
#include <sge_core/math/Vec3.h>
#include <sge_core/math/Vec4.h>

#include <sge_core/math/Mat4.h>
#include <sge_core/math/Quat4.h>

namespace sge  {
namespace meta {

	template<class T, class SE = meta::any_type, class = void > struct is_serdes : std::false_type {};
	template<class T, class SE>
	struct is_serdes<T, SE, std::void_t<decltype(onSerDes(std::declval<SE&>(), std::declval<T&>()))>> :
							std::true_type {};
	
	template<class T>					struct is_vector					: std::false_type {};
	template<class E, size_t N, bool B>	struct is_vector<Vector<E, N, B>>	: std::true_type  {};

	template<class T>				    struct is_span						: std::false_type {};
	template<class T>				    struct is_span<Span<T>>				: std::true_type  {};
	
	template<class T>					struct is_string					: std::false_type {};
	template<class E, size_t N, bool B>	struct is_string<StringT<E, N, B>>	: std::true_type  {};

	template<class T> struct is_tuple			 : std::false_type {};
	template<class E> struct is_tuple<Tuple2<E>> : std::true_type  {};
	template<class E> struct is_tuple<Tuple3<E>> : std::true_type  {};
	template<class E> struct is_tuple<Tuple4<E>> : std::true_type  {};

	template<class T> struct is_vec			 : std::false_type {};
	template<class E> struct is_vec<Vec2<E>> : std::true_type  {};
	template<class E> struct is_vec<Vec3<E>> : std::true_type  {};
	template<class E> struct is_vec<Vec4<E>> : std::true_type  {};
	
	template<class T> struct is_mat			 : std::false_type {};
	template<class E> struct is_mat<Mat4<E>> : std::false_type {};
	
	template<class T> struct is_col					: std::false_type {};
	template<class E> struct is_col<ColorR   <E>>	: std::true_type  {};
	template<class E> struct is_col<ColorRG  <E>>	: std::true_type  {};
	template<class E> struct is_col<ColorRGB <E>>	: std::true_type  {};
	template<class E> struct is_col<ColorRGBA<E>>	: std::true_type  {};
	template<class E> struct is_col<ColorL	 <E>>	: std::true_type  {};
	template<class E> struct is_col<ColorLA	 <E>>	: std::true_type  {};

	template<class T> struct is_container : std::disjunction<is_vector<T>,
															 is_string<T>,
															 is_tuple <T>,
															 is_vec	  <T>,
															 is_mat	  <T>,
															 is_col	  <T>>::type {};


	template<class T> using enable_if_enum = std::enable_if<std::is_enum<T>::value>;
	
	template<class T> bool static constexpr isArithmetic () { return std::is_arithmetic<T>::value;	}
	template<class T> bool static constexpr isBool		 () { return std::is_same<T, bool>::value;	}
	template<class T> bool static constexpr isEnum		 () { return std::is_enum<T>::value;		}
	template<class T> bool static constexpr isSerDes	 () { return is_serdes	 <T>::value;		}
	template<class T> bool static constexpr isString	 () { return is_string	 <T>::value;		}
	template<class T> bool static constexpr isVector	 () { return is_vector	 <T>::value;		}
	template<class T> bool static constexpr isTuple		 () { return is_tuple	 <T>::value;		}
	template<class T> bool static constexpr isVec		 () { return is_vec		 <T>::value;		}
	template<class T> bool static constexpr isMatrix	 () { return is_mat		 <T>::value;		}
	template<class T> bool static constexpr isColor		 () { return is_col		 <T>::value;		}
	template<class T> bool static constexpr isContainer	 () { return is_container<T>::value;		}
	

}
}