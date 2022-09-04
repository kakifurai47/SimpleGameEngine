#pragma once

#include <sge_core/native_ui/NativeUI.h>
#include <sge_editor/gui/EditorGui.h>

namespace sge
{
	class EditorWindow : public NativeUIWindow 
	{
		using Base = NativeUIWindow;
	public:

		virtual void onCreate	   (CreateDesc& desc) override;
		virtual void onUIMouseEvent(UIMouseEvent& ev) override;

	protected:
		SPtr<RenderContext> m_renderContext;
		RenderRequest		m_renderRequest;
		EditorGuiHandle		m_editorGuiHandle;
	};
}