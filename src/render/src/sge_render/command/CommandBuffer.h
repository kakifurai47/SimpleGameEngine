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

		template<typename CMD>
		CMD* addCmd() {
			auto s = sizeof(CMD);
			auto a = std::min<size_t>(32, Math::nextPow2(s));
			auto* cmd = new(m_alloc.allocate(s, a))CMD();
			m_cmds.push_back(cmd);

			return cmd;
		}

		void clear();

	protected:
		u8* m_buffer  = nullptr;
		LinearAllocator m_alloc;
		Vector<RenderCmdBase*> m_cmds;
	};
}