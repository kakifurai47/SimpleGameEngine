#pragma once

namespace sge
{
	struct Math
	{
		inline static constexpr bool isPow2(u_int v) { return v && !(v & (v - 1)); }

		static constexpr u_int nextPow2(u_int v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;			
			return ++v;
		}
	};
}