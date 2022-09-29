#pragma once

#include <sge_core/native_ui/NativeUI.h>
#include <sge_editor/gui/EditorGui.h>
#include <sge_render/backend/base/RenderContext.h>

namespace sge
{



	class EditorWindow : public NativeUIWindow 
	{
		using Base = NativeUIWindow;
		using Util = EditorGuiHandle::Util;
	
	protected:
		virtual void onCreate			 (CreateDesc&		 desc) override;
		virtual void onNativeUIMouseEvent(UIMouseEvent&		   ev) override;
		virtual void onMonitorsUpdated   (Span<MonitorInfo> infos) override;

		




		SPtr<RenderContext> m_renderContext;
		RenderRequest		m_renderRequest;
		EditorGuiHandle		m_editorGuiHandle;
	};
}