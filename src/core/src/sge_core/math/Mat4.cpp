#include "Mat4.h"

namespace sge {

	// explicit specialization to force VisualC check syntax in function body	

	#ifndef SGE_MATH_USE_SSE
	#error
	#elif SGE_MATH_USE_SSE
		template struct Mat4_Basic<float,  typename Mat4_SSE_Select<float >::Data>;
		template struct Mat4_Basic<double, typename Mat4_SSE_Select<double>::Data>;
	
	#else
		template struct Mat4_Basic<float>;
		template struct Mat4_Basic<double>;
	#endif
}