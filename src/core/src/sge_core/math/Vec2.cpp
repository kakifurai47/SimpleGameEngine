#include "Vec2.h"

namespace sge {

#ifndef SGE_MATH_USE_SSE
#error
#elif SGE_MATH_USE_SSE
	template struct Vec2_Basic<float,  typename Vec2_SSE_Select<float >::Data>;
	template struct Vec2_Basic<double, typename Vec2_SSE_Select<double>::Data>;

#else
	template struct Vec2_Basic<float >;
	template struct Vec2_Basic<double>;
#endif

}