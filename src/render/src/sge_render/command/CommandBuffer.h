#pragma once
#include <sge_core/alllocators/LinearAllocator.h>
#include "RenderCommand.h"

namespace sge
{	
	class CommandBuffer : public NonCopyable
	{
	public:
		static const u64 size = 4 * 1024 * 1024;

		CommandBuffer ();
		~CommandBuffer();

		Span<RenderCmdBase*> cmds() { return m_cmds; }

		template<typename T>
		T* addCmd() {
			auto s = sizeof(T);
			auto a = std::min<size_t>(32, Math::nextPow2(s));
			auto* c = new(m_alloc.allocate(s, a))T();
			m_cmds.push_back(c);

			return c;
		}

		void clear();

	protected:
		u8* m_buffer  = nullptr;
		LinearAllocator m_alloc;
		Vector<RenderCmdBase*> m_cmds;
	};
}