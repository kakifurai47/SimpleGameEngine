#include "EditorWindow.h"

#include <sge_render/backend/base/Renderer.h>

#include <imgui_impl_win32.h>

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


	void EditorWindow::onUIMouseEvent(UIMouseEvent& ev) 
	{
		Base::onUIMouseEvent(ev);
		m_editorGuiHandle.onUIMouseEvent(ev);
	}

}

