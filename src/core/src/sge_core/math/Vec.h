#pragma once

namespace sge {

	class Vec2f {
	public:
		union {
			struct {
				float x, y;
			};
			float data[2];
		};

		Vec2f() = default;

		Vec2f(float x_, float y_)
			:x(x_)
			, y(y_)
		{
		}
	};

}