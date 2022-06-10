#pragma once

namespace sge {


	struct BitUtil {
		BitUtil() = delete;

		template<class T>	constexpr	static bool	hasAny(T  value, T bits) { return (value & bits) != T(0); }
		template<class T>	constexpr	static int	count1(T  value);

	};

	template<class T> constexpr
	int BitUtil::count1(T value) {
		int result = 0;
		int n = bit_size(value);
		T m {1};
		for (int i = 0; i < n; i++) {
			if (hasAny(value, m)) result++;
			m <<= 1;
		}
		return result;
	}

}
