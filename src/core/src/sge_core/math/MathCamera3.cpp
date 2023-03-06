#include "MathCamera3.h"

namespace sge {

	template<class T>
	Camera3<T>::Camera3() {
		setUp(m_up);
	}

	template<class T>
	void Camera3<T>::setUp(const Vec3& up) {
		auto d =  m_aim  - m_pos;
		auto right = up.cross(d);

		m_up = d.cross(right).normalize();
	}

	template<class T>
	void Camera3<T>::dolly(const T& z) {
		auto u   = m_pos - m_aim;
		auto d   = u.length();
		auto dir = u.normalize();
		d += z;
		d = Math::max(d, static_cast<T>(0.001));
		m_pos = m_aim + dir * d;
	}

	template<class T>
	void Camera3<T>::pan(const Vec2& v) {
		(void)v;
	}

	template<class T>
	void Camera3<T>::move(const Vec3& v) {
		auto u     = m_aim - m_pos;
		auto dir   = u.normalize();
		auto right = m_up.cross(dir);
		
		auto t =  right * v.x + m_up * v.y + dir * v.z;
		m_pos += t;
		m_aim += t;
	}

	template<class T>
	void Camera3<T>::orbit(const Vec2& v) {
		auto u     = m_pos - m_aim;
		auto right = m_up.cross(u).normalize();
		
		auto q = Quat4::s_angleAxis(v.y, right) * Quat4::s_eulerY(v.x);
		u		= q *  u;
		m_pos   = m_aim + u;
		m_up	= q *  m_up;
	}

	template<class T>
	Mat4<T> Camera3<T>::projMat() {
		T aspect = m_viewport.h != 0 ? m_viewport.w / m_viewport.h : T(0);
		return Mat4::s_perspective(m_fov, aspect, m_near, m_far);
	}

	template<class T>
	Mat4<T> Camera3<T>::viewMat() {
		return Mat4::s_lookAt(m_pos, m_aim, m_up);		
	}

	template struct Camera3<float>;
}