#include "EditorWindow.h"
#include <sge_render/backend/base/Renderer.h>

namespace sge
{
	void EditorWindow::onCreate(CreateDesc& desc_)
	{
		Base::onCreate(desc_);
		auto* renderer = Renderer::current();
		{
			RenderContext::CreateDesc desc;
			desc.window = this;
			m_renderContext = renderer->createContext(desc);
		}
		{
			EditorGuiHandle::CreateDesc desc;
			desc.window = this;
			m_editorGuiHandle.create(desc);
		}
	}

	void EditorWindow::onNativeUIMouseEvent(UIMouseEvent& ev)
	{
		using EVT  = UIMouseEventType;
		auto& io = EditorGui::GetIO();

		io.AddMousePosEvent(ev.pos.x, ev.pos.y);
		switch (ev.type) {
			case EVT::Down:		io.AddMouseButtonEvent(Util::getGuiButton(ev.button), true ); break;
			case EVT::Up:		io.AddMouseButtonEvent(Util::getGuiButton(ev.button), false); break;
			case EVT::Scroll:	io.AddMouseWheelEvent(ev.scroll.x, ev.scroll.y);			  break;
		}


		if (!io.WantCaptureMouse) { Base::onNativeUIMouseEvent(ev); }
	}
}
