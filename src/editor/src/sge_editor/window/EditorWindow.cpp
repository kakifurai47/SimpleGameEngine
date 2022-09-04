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

	void EditorWindow::onNativeUIMouseEvent(UIMouseEvent& ev)  {
		using REG = CapturedRegion;
		using MET = UIMouseEventType;

		auto& io  = EditorGui::GetIO();

		if (ev.type == MET::Down) {
			m_capturedRegion = io.WantCaptureMouse ? REG::EditorGuiWindow : REG::MainWindow;
		}

		switch (m_capturedRegion) {
			case REG::MainWindow:	   		 Base::onNativeUIMouseEvent(ev); break;
			case REG::EditorGuiWindow: m_editorGuiHandle.onUIMouseEvent(ev); break;
		}

		if (ev.type == MET::Up) {
			m_capturedRegion = REG::None;
		}
	}
}

