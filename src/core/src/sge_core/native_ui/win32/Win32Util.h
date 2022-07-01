#pragma once

namespace sge {

	struct Win32Util {
		Win32Util() = delete;

		static void convert(Vec2f& out, const POINT& point) {
			out.x = static_cast<float>(point.x);
			out.y = static_cast<float>(point.y);
		}
	};





}