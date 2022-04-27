#pragma once

namespace sge 
{
	template<class T>
	class Color4 {
	public:
		union {
			struct {
				T r, g, b, a;
			};
			T data[4];
		};

		Color4() = default;

		Color4(T r_, T g_, T b_, T a_) : r(r_), g(g_), b(b_), a(a_)
		{
		}
	};

	using Color4f = Color4<float>;
	using Color4b = Color4<u8>;	
}
