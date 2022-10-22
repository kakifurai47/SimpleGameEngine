#pragma once

#include <sge_render/command/RenderRequest.h>
#include "EditorGui.h"

namespace sge
{
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
		bool isCreated() {
			return EditorGui::GetCurrentContext() != nullptr;
		}

		void beginRender();
		void render(RenderRequest& request);

		void setMonitorInfos(Span<MonitorInfo> infos);


	private:

		void _createFontTexture();



//		static NativeUIWindow* s_getWindow(ImGuiViewport* vp) { return Util::toNativeUIWindow(vp->PlatformUserData); }

		static void	  EditorGui_ImplWindow_CreateWindow			(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_DestroyWindow		(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_ShowWindow			(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_SetWindowPos			(ImGuiViewport* viewport, ImVec2 pos);
		static ImVec2 EditorGui_ImplWindow_GetWindowPos			(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_SetWindowSize		(ImGuiViewport* viewport, ImVec2 size);
		static ImVec2 EditorGui_ImplWindow_GetWindowSize		(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_SetWindowFocus		(ImGuiViewport* viewport);
		static bool	  EditorGui_ImplWindow_GetWindowFocus		(ImGuiViewport* viewport);
		static bool	  EditorGui_ImplWindow_GetWindowMinimized	(ImGuiViewport* viewport);
		static void	  EditorGui_ImplWindow_SetWindowTitle		(ImGuiViewport* viewport, const char* title);
		static void	  EditorGui_ImplWindow_SetWindowAlpha		(ImGuiViewport* viewport, float alpha);
		static void	  EditorGui_ImplWindow_UpdateWindow			(ImGuiViewport* viewport);
		static float  EditorGui_ImplWindow_GetWindowDpiScale	(ImGuiViewport* viewport);
		static void   EditorGui_ImplWindow_OnChangedViewport	(ImGuiViewport* viewport);
//
		void EditorGui_ImplRenderer_CreateWindow	(ImGuiViewport* viewport);
//		void EditorGui_ImplRenderer_DestroyWindow	(ImGuiViewport* viewport);
//		void EditorGui_ImplRenderer_SetWindowSize	(ImGuiViewport* viewport, ImVec2 size);
//		void EditorGui_ImplRenderer_RenderWindow	(ImGuiViewport* viewport, void*);
//		void EditorGui_ImplRenderer_SwapBuffers		(ImGuiViewport* viewport, void*);


		

	protected:
		RenderMesh		m_mesh;
		VertexLayout	m_vertLayout;
		SPtr<Material>	m_material;
		SPtr<Texture2D>	m_fontsTex2D;
								
		NativeUIWindow*			m_mainWindow = nullptr;
		Vector<MyWindow*, 5>	m_myWindows;
	};







}