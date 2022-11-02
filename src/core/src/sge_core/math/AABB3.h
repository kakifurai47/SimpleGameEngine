#pragma once

namespace sge
{

	template<class T>
	struct AABB3 {
		Vec3<T> min;
		Vec3<T> max;
	};

	using AABB3f = AABB3<float>;
}

