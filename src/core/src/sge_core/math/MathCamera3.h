#pragma once

#include "Quat4.h"
#include "Mat4.h"

namespace sge {

	template<class T>
	struct Camera3 {
		using Vec2	= Vec2<T>;
		using Vec3	= Vec3<T>;
		using Mat4	= Mat4<T>;
		using Quat4 = Quat4<T>;

		Camera3();

		void dolly	(const T& z);
		void pan	(const Vec2& v);
		void move	(const Vec3& v);
		void orbit	(const Vec2& v);

		Mat4 projMat();
		Mat4 viewMat();

		void setPos(const Vec3& pos) { m_pos = pos; }
		void setAim(const Vec3& aim) { m_aim = aim; }
		void setUp (const Vec3& up);

	private:
		float m_fov		= Math::radians(50.0f);
		float m_far		= 10000.0f;
		float m_near	= 1.0f;
		float m_asepct	= 16.0f/ 9;

		Vec3 m_pos {0,5,5};
		Vec3 m_aim {0,0,0};
		Vec3 m_up  {0,1,0};

		Mat4 m_view;
	};

	using Camera3f = Camera3<float>;
}