#pragma once

namespace sge {

	struct Win32Util {
		Win32Util() = delete;

		static void convert(Rect2f& o, const ::RECT& i)
		{
			o.x = static_cast<float>(i.left);
			o.y = static_cast<float>(i.top);
			o.w = static_cast<float>(i.right  - i.left);
			o.h = static_cast<float>(i.bottom - i.top );
		}

		static void convert(::RECT& o, const Rect2f& i)
		{
			o.left   = static_cast<LONG>(i.x);
			o.top    = static_cast<LONG>(i.y);
			o.right  = static_cast<LONG>(i.x + i.w);
			o.bottom = static_cast<LONG>(i.y + i.h);
		}


		static void convert(Vec2f& out, const POINT& point) {
			out.x = static_cast<float>(point.x);
			out.y = static_cast<float>(point.y);
		}

		static Vec2f  toVec2f(const ::POINT& i) { Vec2f  o; convert(o, i); return o; }
		static Rect2f toRect2f(const ::RECT& i) { Rect2f o; convert(o, i); return o; }
	};





}