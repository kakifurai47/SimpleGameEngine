#pragma once

namespace sge 
{
	class Color4f {
	public:
		union {
			struct {
				float r, g, b, a;
			};
			float data[4];
		};

		Color4f() = default;

		Color4f(float r_, float g_, float b_, float a_)
			: r(r_), g(g_), b(b_), a(a_)
		{
		}
	};

	
}
