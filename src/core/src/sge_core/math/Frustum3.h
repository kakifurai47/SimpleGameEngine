#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/math/Plane4.h>
#include <sge_core/math/Mat4.h>
#include <sge_core/math/AABB3.h>

namespace sge
{


	template<class T>
	class Frustum3
	{
		using Plane4 = Plane4<T>;
		using AABB3  = AABB3<T>;
		using Vec3   = Vec3<T>;
		using Mat4   = Mat4<T>;
	public:
		
		class Planes : public NonCopyable
		{
		public:
			Planes() = default;
			Planes(const Mat4& mat) { reset(mat);  }

			Span<Plane4> data() { return m_planes; }

			void reset(const Mat4& mat);

		private:
			Vector<Plane4, 6> m_planes;
		};

		static bool s_isInBound(Planes& planes, const AABB3& aabb);
	};

	template<class T>
	void Frustum3<T>::Planes::reset(const Mat4& mat)
	{
		m_planes.clear();

		auto r0 = mat.row(0);
		auto r1 = mat.row(1);
		auto r2 = mat.row(2);
		auto r3 = mat.row(3);

		m_planes.emplace_back(r3 - r0);
		m_planes.emplace_back(r3 + r0);
		m_planes.emplace_back(r3 - r1);
		m_planes.emplace_back(r3 + r1);
		m_planes.emplace_back(r3 - r2);
		m_planes.emplace_back(r3 + r2);
	}

	template<class T>
	bool Frustum3<T>::s_isInBound(Planes& planes, const AABB3& aabb)
	{
		//https://iquilezles.org/articles/frustumcorrect/

		for (const auto& p : planes.data())
		{
			size_t out = 0;
			
			out += p.sameSide(Vec3(aabb.min.x, aabb.min.y, aabb.min.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.min.x, aabb.min.y, aabb.max.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.min.x, aabb.max.y, aabb.min.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.min.x, aabb.max.y, aabb.max.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.max.x, aabb.min.y, aabb.min.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.max.x, aabb.min.y, aabb.max.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.max.x, aabb.max.y, aabb.min.z)) ? 0 : 1;
			out += p.sameSide(Vec3(aabb.max.x, aabb.max.y, aabb.max.z)) ? 0 : 1;
			
			if (out == 8) { return false; }
		}

//		size_t out;
//
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].x > box.mMaxX) ? 1 : 0); if (out == 8) return false;
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].x < box.mMinX) ? 1 : 0); if (out == 8) return false;
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].y > box.mMaxY) ? 1 : 0); if (out == 8) return false;
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].y < box.mMinY) ? 1 : 0); if (out == 8) return false;
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].z > box.mMaxZ) ? 1 : 0); if (out == 8) return false;
//		out = 0; for (size_t i = 0; i < 8; i++) out += ((fru.mPoints[i].z < box.mMinZ) ? 1 : 0); if (out == 8) return false;

		return true;
	}

	using Frustum3f = Frustum3<float>;

}