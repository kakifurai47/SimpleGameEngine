#pragma once

#include <sge_core/native_ui/NativeUI.h>
#include <sge_editor/gui/EditorGui.h>

namespace sge
{
	#define UIEditorCapturedRegion_ENUM_LIST(E) \
		E(None,) \
		E(MainWindow,) \
		E(EditorGuiWindow,) \
	//----

	SGE_ENUM_CLASS(UIEditorCapturedRegion, : u8)

	class EditorWindow : public NativeUIWindow 
	{
		using Base = NativeUIWindow;
	public:
		using CapturedRegion = UIEditorCapturedRegion;

		virtual void onCreate			 (CreateDesc& desc) override;
		virtual void onNativeUIMouseEvent(UIMouseEvent& ev) override;

	protected:
		SPtr<RenderContext> m_renderContext;
		RenderRequest		m_renderRequest;
		EditorGuiHandle		m_editorGuiHandle;

		CapturedRegion		m_capturedRegion = CapturedRegion::None;
	};
}