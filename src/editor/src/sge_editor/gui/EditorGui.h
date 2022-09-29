#pragma once

#include <imgui.h>
#include <sge_render/command/RenderRequest.h>


namespace sge {

//	namespace EditorGui = ImGui;

	namespace EditorGui
	{
		using namespace ImGui;
	};

	struct EditorGuiUtil
	{
		template<class T> static inline
		ByteSpan makeByteSpan(ImVector<T>& vec) {
			return spanCast<const u8, const T>(Span<const T>(vec.Data, vec.Size));
		}

		static inline
		int getGuiButton(UIMouseEventButton src) {
			switch (src) {
				case UIMouseEventButton::Left:		return ImGuiMouseButton_::ImGuiMouseButton_Left;
				case UIMouseEventButton::Right:		return ImGuiMouseButton_::ImGuiMouseButton_Right;
				case UIMouseEventButton::Middle:	return ImGuiMouseButton_::ImGuiMouseButton_Middle;

				default: throw SGE_ERROR("Unhandled mouse button pressed");
			}
		}

		static inline 
		void setWindowCreateDesc(NativeUIWindow::CreateDesc& desc, ImGuiViewportFlags flags) {
			using WinType = NativeUIWindow::CreateDesc::Type;

			desc.type = WinType::NormalWindow;

			if (flags & ImGuiViewportFlags_NoDecoration ) desc.type = WinType::PopupWindow;
			if (flags & ImGuiViewportFlags_NoTaskBarIcon) desc.type = WinType::ToolWindow;
			if (flags & ImGuiViewportFlags_TopMost)		  desc.alwaysOnTop = true;
		}

		static inline 
		NativeUIWindow* getNativeUIWindow(void* platformHandle) {
			return reinterpret_cast<NativeUIWindow*>(platformHandle);
		}

	};

	struct EditorGuiHandle_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class EditorGuiHandle : public NonCopyable
	{
	public:
		using CreateDesc = EditorGuiHandle_CreateDesc;
		using Util		 = EditorGuiUtil;

		class MyWindow : public NativeUIWindow
		{
		};

		~EditorGuiHandle();
		
		void create(CreateDesc& desc);

		void beginRender();
		void render(RenderRequest& request);

	private:		
		void EditorGui_ImplWindow_CreateWindow		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_DestroyWindow		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_ShowWindow		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_SetWindowPos		(ImGuiViewport* viewport, ImVec2 pos);
//		void EditorGui_ImplWindow_GetWindowPos		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_SetWindowSize		(ImGuiViewport* viewport, ImVec2 size);
//		void EditorGui_ImplWindow_GetWindowSize		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_SetWindowFocus	(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_GetWindowFocus	(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_GetWindowMinimized(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_SetWindowTitle	(ImGuiViewport* viewport, const char* title);
//		void EditorGui_ImplWindow_SetWindowAlpha	(ImGuiViewport* viewport, float alpha);
//		void EditorGui_ImplWindow_UpdateWindow		(ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_GetWindowDpiScale (ImGuiViewport* viewport);
//		void EditorGui_ImplWindow_OnChangedViewport (ImGuiViewport* viewport);
//
		void EditorGui_ImplRenderer_CreateWindow	(ImGuiViewport* viewport);
//		void EditorGui_ImplRenderer_DestroyWindow	(ImGuiViewport* viewport);
//		void EditorGui_ImplRenderer_SetWindowSize	(ImGuiViewport* viewport, ImVec2 size);
//		void EditorGui_ImplRenderer_RenderWindow	(ImGuiViewport* viewport, void*);
//		void EditorGui_ImplRenderer_SwapBuffers		(ImGuiViewport* viewport, void*);


		

	protected:
		RenderMesh		    m_mesh;
		VertexLayout	    m_vertLayout;
		SPtr<Material>	    m_material;
		SPtr<Texture2D>	    m_fontsTex2D;
						    
		NativeUIWindow*	    m_mainWindow = nullptr;
		Vector<MyWindow, 5> m_windows;
	};


}