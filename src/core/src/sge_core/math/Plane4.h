#pragma once

#include "../math/Vec4.h"

namespace sge
{


	template<class T>
	struct Plane4
	{
		using ElementType = T;
		static const size_t kElementCount = 4;

		using Vec4 = Vec4<T>;
		using Vec3 = Vec3<T>;

		union {
			struct{T x, y, z, w;};
			T data[kElementCount];
		};

		SGE_INLINE Plane4() = default;
		SGE_INLINE Plane4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

		SGE_INLINE T    dot		(const Vec3& v) const { return (x * v.x) + (y * v.y) + (z * v.y) + w; }
		SGE_INLINE bool sameSide(const Vec3& v) const { return dot(v) > T(0); }
	};

	using Plane4f = Plane4<float>;

}