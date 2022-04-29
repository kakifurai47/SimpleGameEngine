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



	};

	struct BitUtil
	{
		template<class T, size_t shift> 
		static constexpr auto add(T t)
		{

		}



	};

	
}