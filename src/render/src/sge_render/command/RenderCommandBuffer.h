#pragma once
#include <sge_core/alllocators/LinearAllocator.h>
#include "RenderCommand.h"

namespace sge
{	
	class RenderCommandBuffer : public NonCopyable {
	public:
		static const u64 size = 4 * 1024 * 1024;

		RenderCommandBuffer ();
		~RenderCommandBuffer();

		Span<RenderCmd*> cmds() { return m_cmds; }

		RenderCmd_ClearFrameBuffers* clearFrameBuffers() {
			return addCmd<RenderCmd_ClearFrameBuffers>();
		}

		RenderCmd_SwapBuffers* swapBuffers() {
			return addCmd<RenderCmd_SwapBuffers>();
		}

		void drawMesh(const SrcLoc& debugLoc, Material& mat);


		template<typename CMD>
		CMD* addCmd() {
			auto s = sizeof(CMD);
			auto a = std::min<size_t>(32, Math::nextPow2(s));
			auto* cmd = new(m_alloc.allocate(s, a))CMD();
			m_cmds.emplace_back(cmd);
			return cmd;
		}

		void reset();

	protected:
		u8* m_buffer  = nullptr;
		LinearAllocator m_alloc;
		Vector<RenderCmd*> m_cmds;
	};
}