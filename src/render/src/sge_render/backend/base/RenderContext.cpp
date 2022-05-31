#include "RenderContext.h"
#include "Renderer.h"
#include "sge_render/command/CommandBuffer.h"

namespace sge {

	RenderContext* RenderContext::create(CreateDesc& desc) {
		auto* rdr = Renderer::current();
		if (!rdr) return nullptr;
		return rdr->onCreateRenderContext(desc);
	}

	void RenderContext::render(CommandBuffer& buffer) {
		onBeginRender();
		auto cmds = buffer.cmds();

		for (auto* c : cmds) {
			if (!c) continue;

			switch (c->type) {
			case CmdType::SetViewport:	onSetViewport(static_cast<RenderCmd_SetViewport*>(c)); break;
			case CmdType::Clear:		onClearBuffer(static_cast<RenderCmd_Clear*>(c));	   break;
			case CmdType::Draw:			onDraw		 (static_cast<RenderCmd_Draw*>(c));		   break;
		    case CmdType::SwapBuffer:	onSwapBuffer ();									   break;
			case CmdType::None:																   break;
			default:																		   break;
			}
		}
		onSwapBuffer();
		onEndRender();
		buffer.clear();
	}

}

