#pragma once

namespace sge
{

	template<class T>
	struct AABB3 {
		Vec3<T> minimumPoint;
		Vec3<T> maximumPoint;
	};

	using AABB3f = AABB3<float>;
}

