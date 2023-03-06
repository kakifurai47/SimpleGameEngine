#include "Vec3.h"

namespace sge {

	// explicit specialization to force VisualC check syntax in function body

#ifndef SGE_MATH_USE_SSE
#error
#elif SGE_MATH_USE_SSE
	template struct Vec3_Basic<float,  typename Vec3_SSE_Select<float >::Data>;
	template struct Vec3_Basic<double, typename Vec3_SSE_Select<double>::Data>;

#else
	template struct Vec3_Basic<float >;
	template struct Vec3_Basic<double>;
#endif

}