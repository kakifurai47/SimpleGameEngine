#pragma once

#include <imgui.h>
#include <sge_render/command/RenderRequest.h>


namespace sge {

//	namespace EditorGui = ImGui;

	namespace EditorGui
	{
		using namespace ImGui;
	};

	class  EditorGuiHandle;
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
		ImGuiViewport* getParentViewport(ImGuiViewport* viewport) {
			if (!viewport || viewport->ParentViewportId == 0) return nullptr;
			return EditorGui::FindViewportByID( viewport->ParentViewportId );
		}

		static inline void initViewport(ImGuiViewport* vp, NativeUIWindow* win, EditorGuiHandle* hdle) 
		{
			vp->PlatformUserData      = hdle;
			vp->PlatformHandle        = win;
			vp->PlatformRequestResize = false;
		}

		static inline NativeUIWindow*  getViewportWindow(ImGuiViewport* vp) { return reinterpret_cast<NativeUIWindow* >( vp->PlatformHandle   ); }
		static inline EditorGuiHandle* getViewportHandle(ImGuiViewport* vp) { return reinterpret_cast<EditorGuiHandle*>( vp->PlatformUserData ); }


		static inline ImVec2 toImVec2(const Vec2f & i) { return ImVec2( i.x, i.y ); }
		static inline Vec2f  toVec2f (const ImVec2& i) { return Vec2f ( i.x, i.y ); }

		static inline void   convert(::ImGuiPlatformMonitor& o, const MonitorInfo& i)
		{
			o.MainPos	= toImVec2(i.rect.pos	  );
			o.MainSize	= toImVec2(i.rect.size	  );
			o.WorkPos	= toImVec2(i.workRect.pos );
			o.WorkSize	= toImVec2(i.workRect.size);
			o.DpiScale	= i.dpiScale;
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
		bool isCreated() {
			return EditorGui::GetCurrentContext() != nullptr;
		}

		void beginRender();
		void render(RenderRequest& request);

		void setMonitorInfos(Span<MonitorInfo> infos);


	private:		
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