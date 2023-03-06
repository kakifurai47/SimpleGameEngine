#include "Vec4.h"

namespace sge {

	// explicit specialization to force VisualC check syntax in function body

#ifndef SGE_MATH_USE_SSE
#error
#elif SGE_MATH_USE_SSE
	template struct Vec4_Basic<float,  typename Vec4_SSE_Select<float >::Data>;
	template struct Vec4_Basic<double, typename Vec4_SSE_Select<double>::Data>;

#else
	template struct Vec4_Basic<float >;
	template struct Vec4_Basic<double>;
#endif



}