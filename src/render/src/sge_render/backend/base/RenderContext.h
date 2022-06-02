#pragma once

#include "sge_render/command/RenderCommand.h"
#include "sge_render/command/RenderCommandBuffer.h"

namespace sge  {

	struct RenderContext_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class RenderCommandBuffer;	
	class RenderContext : public Object  {
	public:
		using CmdType	 = RenderCmd::Type;
		using CreateDesc = RenderContext_CreateDesc;

		RenderContext(CreateDesc& desc) {};
		virtual ~RenderContext() = default;

		void beginRender() { onBeginRender(); }
		void endRneder()   { onEndRender();   }

		void commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }

		//void render(RenderCommandBuffer& buffer);
	protected:
		virtual void onBeginRender() {}
		virtual void onEndRender  () {}
		virtual void onCommit(RenderCommandBuffer& cmdBuf){}
		

		template<class IMPL>
		void _dispatch(IMPL* impl, RenderCommandBuffer& cmdBuf) {
			using Cmd = RenderCommandType;

		#define CMD_CASE(E) \
			case Cmd::E: { \
				auto* c = static_cast<RenderCmd_##E*>(cmd); \
				impl->onCmd_##E(*c); \
			} break; \
		//----

			for (auto* cmd : cmdBuf.cmds()) {
				switch (cmd->type()) {
					CMD_CASE(ClearFrameBuffers);
					CMD_CASE(SwapBuffers);
					CMD_CASE(DrawCall);
				default:
					throw SGE_ERROR("unhandled command");
				}
			}
		#undef CMD_CASE
		}
	};
	
}

