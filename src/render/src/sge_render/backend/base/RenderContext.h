#pragma once

#include "sge_render/command/RenderCommand.h"

namespace sge  {

	struct RenderContext_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class RenderCommandBuffer;	
	class RenderContext : public Object  {
	public:
		using CmdType	 = RenderCommand::Type;
		using CreateDesc = RenderContext_CreateDesc;

		RenderContext(CreateDesc& desc) {};
		virtual ~RenderContext() = default;

		void beginRender() { onBeginRender(); }
		void endRender()   { onEndRender();   }

		void setFrameBufferSize(const Vec2f newSize);

		void commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }

	protected:
		virtual void onBeginRender() {}
		virtual void onEndRender  () {}
		virtual void onCommit	   (RenderCommandBuffer& cmdBuf){}
		virtual void onSetFrameBufferSize(const Vec2f& newSize) {};

		template<class IMPL>
		void _dispatch(IMPL* impl, RenderCommandBuffer& cmdBuf) {
			using Cmd = RenderCommandType;

		#define CMD_CASE(E) \
			case Cmd::E: { \
				auto* c = static_cast<RenderCommand_##E*>(cmd); \
				impl->onCommand_##E(*c); \
			} break; \
		//----

			for (auto* cmd : cmdBuf.commands()) {
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


		Vec2f m_frameBufferSize {0,0};
	};
	
}

