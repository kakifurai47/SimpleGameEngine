#pragma once

#include "sge_render/command/RenderCommand.h"

namespace sge  {

	struct RenderContext_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class CommandBuffer;	
	class RenderContext : public NonCopyable  {
	public:
		using CmdType	 = RenderCmdBase::CmdType;
		using CreateDesc = RenderContext_CreateDesc;

		static RenderContext* create(CreateDesc& desc);

		RenderContext(const CreateDesc& desc) {};
		virtual ~RenderContext() = default;

		void render(CommandBuffer& buffer);
	protected:
		
		virtual void onSetViewport(RenderCmd_SetViewport*	cmd) {}
		virtual void onClearBuffer(RenderCmd_Clear*			cmd) {}
		virtual void onDraw		  (RenderCmd_Draw*			cmd) {}
		virtual void onSwapBuffer () {}

		virtual void onBeginRender() {}
		virtual void onEndRender  () {}

		virtual void onTestDraw	  () {}
	};
	
}

