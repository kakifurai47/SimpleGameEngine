#pragma once

namespace sge  {
namespace Sort {
	template<class T>
	void InsertionSort(T& t) {
		eastl::insertion_sort(eastl::begin(t), eastl::end(t));
	}

	template<class T, size_t N>
	void InsertionSort(T(&t)[N]) {
		eastl::insertion_sort(eastl::begin(t), eastl::end(t));
	}
}
}
