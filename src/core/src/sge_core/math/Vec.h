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

	class Vec3f {
	public:
		union {
			struct {
				float x, y, z;
			};
			float data[3];
		};

		Vec3f() = default;

		Vec3f(float x_, float y_, float z_)
			: x(x_)
			, y(y_)
			, z(z_)
		{
		}
	};




}