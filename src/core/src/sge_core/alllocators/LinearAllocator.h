#pragma once

#include "sge_core/base/sge_base.h"

namespace sge
{
	class LinearAllocator : public NonCopyable
	{
	public:
		LinearAllocator(u8* buffer, int size)
			:m_buffer(buffer)
			,m_size(size)
			,m_offset(0)
		{
		}

		size_t offset() { return m_offset;	}
		size_t size()	{ return m_size;	}

		u8* allocate(size_t size, size_t align = 4);
		void free();

	private:
		u8*     m_buffer = nullptr;
		size_t  m_offset = 0;
		size_t	m_size   = 0;
	};

}

