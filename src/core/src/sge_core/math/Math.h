#pragma once

#include "sge_core/base/sge_base.h"

namespace sge
{
	struct Math
	{
		inline static constexpr bool isPow2(u64 v) { return v && !(v & (v - 1)); }

		static constexpr u64 nextPow2(u64 v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			return ++v;
		}

		//Jenkins hash function
		static u32 hash(StrView src) {
			size_t i = 0;
			u32 ret  = 0;			

			for (auto i : src) {
				ret += src[i];
				ret += ret << 10;
				ret ^= ret >> 6;
			}
			ret += ret << 3;
			ret ^= ret >> 11;
			ret += ret << 15;
			return ret;
		}



	};
}