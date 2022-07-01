#include "LinearAllocator.h"


namespace sge
{
	u8* LinearAllocator::allocate(size_t size, size_t align) {
		SGE_ASSERT(Math::isPow2(align));

		auto aligned = (m_offset + (align - 1)) & (~align + 1);

		if (aligned + size > m_size) {
			SGE_LOG("allocator is full");
			return nullptr;
		}
		m_offset = aligned + size;
		return m_buffer + aligned;
	}

	void LinearAllocator::free() {
		m_offset = 0;
	}
}
