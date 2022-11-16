
#include "EditorGuiHandle.h"

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>
#include <sge_render/shader/Material.h>

namespace sge
{
	struct EditorGuiHandle_InternalHelper //move this to EditorGui later
	{
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

		static inline NativeUIWindow*  getNativeUIWindow (ImGuiViewport* vp) { return reinterpret_cast<NativeUIWindow* >( vp->PlatformHandle   ); }
		static inline EditorGuiHandle* getEditorGuiHandle(ImGuiViewport* vp) { return reinterpret_cast<EditorGuiHandle*>( vp->PlatformUserData ); }
	};






	void EditorGuiHandle::create(CreateDesc& desc) 
	{
		using Helper = EditorGuiHandle_InternalHelper;

		m_mainWindow = desc.window;

		auto* renderer = Renderer::current();
		
		IMGUI_CHECKVERSION();
		EditorGui::CreateContext();

		ImGuiIO& io = EditorGui::GetIO(); (void)io;
		io.UserData = this;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

//		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
		    style.WindowRounding			  = 0.0f;
		    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		{//ImGui_ImplWin32_Init

			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
//			io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
//			io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)

			auto* mainVP = EditorGui::GetMainViewport();
			Helper::initViewport(mainVP, desc.window, this);

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				// Register platform interface (will be coupled with a renderer interface)
				ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

				platform_io.Platform_CreateWindow		= EditorGui_ImplWindow_CreateWindow;
				platform_io.Platform_DestroyWindow		= EditorGui_ImplWindow_DestroyWindow;
				platform_io.Platform_ShowWindow			= EditorGui_ImplWindow_ShowWindow;
				platform_io.Platform_SetWindowPos		= EditorGui_ImplWindow_SetWindowPos;
				platform_io.Platform_GetWindowPos		= EditorGui_ImplWindow_GetWindowPos;
				platform_io.Platform_SetWindowSize		= EditorGui_ImplWindow_SetWindowSize;
				platform_io.Platform_GetWindowSize		= EditorGui_ImplWindow_GetWindowSize;
				platform_io.Platform_SetWindowFocus		= EditorGui_ImplWindow_SetWindowFocus;
				platform_io.Platform_GetWindowFocus		= EditorGui_ImplWindow_GetWindowFocus;
				platform_io.Platform_GetWindowMinimized = EditorGui_ImplWindow_GetWindowMinimized;
				platform_io.Platform_SetWindowTitle		= EditorGui_ImplWindow_SetWindowTitle;
				platform_io.Platform_SetWindowAlpha		= EditorGui_ImplWindow_SetWindowAlpha;
				platform_io.Platform_UpdateWindow		= EditorGui_ImplWindow_UpdateWindow;
//				platform_io.Platform_GetWindowDpiScale	= EditorGui_ImplWindow_GetWindowDpiScale; // FIXME-DPI
//				platform_io.Platform_OnChangedViewport	= EditorGui_ImplWindow_OnChangedViewport; // FIXME-DPI

			}
		}

		{
			io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
			io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
		}

		{//ImGui_ImplDX11_Init

			{//Material
				auto shad  = renderer->createShader("Assets/Shaders/editorgui.shader");
				m_material = renderer->createMaterial();
				m_material->setShader(shad);
			}
			{//Layout
				//temporary -> needed to refector veretex;
				auto& pos = m_vertLayout.elements.emplace_back();
				pos.formatType  = RenderFormatType::Float32x2;
				pos.semantic    = VertexSemantic::POSITION;
				pos.offset      = 0;

				auto& uv  = m_vertLayout.elements.emplace_back();
				uv.formatType   = RenderFormatType::Float32x2;
				uv.semantic     = VertexSemantic::TEXCOORD0;
				uv.offset	    = 8;

				auto& col = m_vertLayout.elements.emplace_back();
				col.formatType  = RenderFormatType::UNorm08x4;
				col.semantic	= VertexSemantic::COLOR0;
				col.offset		= 16;

				m_vertLayout.stride = static_cast<size_t>( 8 + 8 + 4 );

				m_mesh.setVertexLayout(&m_vertLayout);
			}
			{//IndexFormat
				auto idxFormat = sizeof(ImDrawIdx) == 2 ? RenderFormatType::UInt16x1 :
														  RenderFormatType::UInt32x2;
				m_mesh.setIndexFormat(idxFormat);
			}

		}
	}

	EditorGuiHandle::~EditorGuiHandle()
	{
		auto* vp = EditorGui::GetMainViewport();
		vp->PlatformUserData      = nullptr;
		vp->PlatformHandle        = nullptr;

		EditorGui::DestroyContext();
	}

	void EditorGuiHandle::beginRender() {
		auto& io = EditorGui::GetIO();

		auto& rect     = m_mainWindow->clientRect();
		io.DisplaySize = {rect.size.x, rect.size.y};

		if (!m_fontsTex2D) {
			_createFontTexture();
		}

		EditorGui::NewFrame();
	}

	void EditorGuiHandle::render(RenderRequest& request)
	{
		EditorGui::Render();
		auto* dd = EditorGui::GetDrawData();

		if (dd->TotalVtxCount <= 0) {
			return;
		}

		bool resetVBuff = !m_mesh.vertexBuffer() || m_mesh.vertexCount() < dd->TotalVtxCount;
		bool resetIBuff = ! m_mesh.indexBuffer() ||  m_mesh.indexCount() < dd->TotalIdxCount;

		if(resetVBuff) m_mesh.resetVertexBuffer( static_cast<size_t>( dd->TotalVtxCount + 5000 ) );
		if(resetIBuff) m_mesh.resetIndexBuffer ( static_cast<size_t>( dd->TotalIdxCount + 10000) );

		{
			RenderGpuBuffer::MappedWriter idxWriter, vtxWriter;

			idxWriter.create(m_mesh.indexBuffer ());
			vtxWriter.create(m_mesh.vertexBuffer());
			m_mesh.setSubMeshCount(dd->CmdListsCount);

			auto subMeshes = m_mesh.subMeshes();

			size_t vtxOffset = 0;
			size_t idxOffset = 0;

			for (size_t i = 0; i < dd->CmdListsCount; i++)
			{
				auto* cmdList = dd->CmdLists[i];
				auto& subMesh = subMeshes[i];

				vtxWriter.write(Util::makeByteSpan(cmdList->VtxBuffer), vtxOffset * sizeof(ImDrawVert));
				idxWriter.write(Util::makeByteSpan(cmdList->IdxBuffer), idxOffset * sizeof(ImDrawIdx ));

				subMesh.setVertexCount(cmdList->VtxBuffer.Size);
				subMesh.setIndexCount (cmdList->IdxBuffer.Size);

				subMesh.setBaseVertex(vtxOffset);
				subMesh.setStartIndex(idxOffset);

				vtxOffset += cmdList->VtxBuffer.Size;
				idxOffset += cmdList->IdxBuffer.Size;
			}
		}

		{
			auto L = dd->DisplayPos.x;
			auto R = dd->DisplayPos.x + dd->DisplaySize.x;
			auto T = dd->DisplayPos.y;
			auto B = dd->DisplayPos.x + dd->DisplaySize.y;

			Mat4f mvp
			(
				{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f},
				{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f},
				{ 0.0f,         0.0f,           0.5f,       0.0f},
				{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f}
			);

			//change to cbuffer;
			m_material->setParam("ProjectionMatrix",  mvp);
			m_material->setParam("texture0", m_fontsTex2D);
		}

		request.drawMesh(SGE_LOC, m_mesh, m_material);

	}

	void EditorGuiHandle::setMonitorInfos(Span<MonitorInfo> infos) 
	{
		if (!isCreated()) { return; }

		auto& io = EditorGui::GetPlatformIO();
		auto& ms = io.Monitors;

		ms.clear();
		ms.reserve( static_cast<int>(infos.size()) );

		for (auto& i : infos) 
		{
			ImGuiPlatformMonitor o;
			Util::convert	(o, i);

			if (i.isPrimary) { ms.push_front(o); }
			else			 { ms.push_back (o); }
		}
	}


	void EditorGuiHandle::_createFontTexture()
	{
		auto& io = ImGui::GetIO();

		unsigned char* pixels = nullptr;
		int width	      = 0;
		int height	      = 0;
//		int bytesPerPixel = 0;

		io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
//		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);
		

		using Color = ColorRb;

		Texture2D_CreateDesc desc;

		desc.size	   = { width, height };
		desc.colorType = Color::kColorType;

		desc.imageToUpload.emplace();
		auto& img = desc.imageToUpload.value();
		img.create(Color::kColorType, width, height);
		img.copy  (ByteSpan(pixels, width * height));
//		img.copy  (ByteSpan(pixels, width * height * bytesPerPixel));

//		auto& ss  = desc.samplerState;
//		ss.filter = SamplerState::Filter::Linear;
//		ss.minLOD = 0.f;
//		ss.maxLOD = 0.f;

		m_fontsTex2D = Renderer::current()->createTexture2D(desc);
	}

	void EditorGuiHandle::EditorGui_ImplWindow_CreateWindow(ImGuiViewport* viewport)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		using T = MyWindow::CreateDesc::Type;

		auto* parentVP  = Helper::getParentViewport(viewport);
		if  (!parentVP) throw SGE_ERROR("creating window : w/o parent");

		auto* parentWin = Helper::getNativeUIWindow (parentVP);
		auto* handle    = Helper::getEditorGuiHandle(parentVP);

		if (!parentWin || !handle) { throw SGE_ERROR("creating window : w/o init"); }
		
		auto& newWin = handle->m_myWindows.emplace_back();
		newWin = new MyWindow();

		MyWindow::CreateDesc desc;

		desc.rect = { viewport->Pos.x,  viewport->Pos.y,
					  viewport->Size.x, viewport->Size.y };

		auto& flags = viewport->Flags;
		if (flags & ImGuiViewportFlags_NoDecoration)	desc.type		 = T::PopupWindow;
		if (flags & ImGuiViewportFlags_NoTaskBarIcon)	desc.type		 = T::ToolWindow;
		if (flags & ImGuiViewportFlags_TopMost)			desc.alwaysOnTop = true;

		desc.parent = parentWin;

		newWin->create(desc);
		Helper::initViewport(viewport, newWin, handle);
	}

	void EditorGuiHandle::EditorGui_ImplWindow_DestroyWindow(ImGuiViewport* viewport)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* handle = Helper::getEditorGuiHandle(viewport);
		auto* win    = Helper::getNativeUIWindow(viewport);

		if (!handle || !win) throw SGE_ERROR("destroying window w/o init");

		if (win->isCaptured()) {
			win->resetCapture(handle->m_mainWindow);
		}

		if (win != handle->m_mainWindow) 
		{
			auto* myWin  = static_cast<MyWindow*>(win);
			auto& myWins = handle->m_myWindows;
			myWins.erase_first_unsorted(myWin);
			myWin->destroy ();			
		}

		viewport->PlatformUserData = nullptr;
		viewport->PlatformHandle   = nullptr;
	}

	void EditorGuiHandle::EditorGui_ImplWindow_ShowWindow(ImGuiViewport* viewport)
	{
		SGE_DUMP_VAR("on show window");
	}



	void EditorGuiHandle::EditorGui_ImplWindow_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* win = Helper::getNativeUIWindow(viewport);
		if  (!win) throw SGE_ERROR("get window pos : w/o init");
		win->setPos( Util::toVec2f(pos) );
	}

	ImVec2 EditorGuiHandle::EditorGui_ImplWindow_GetWindowPos(ImGuiViewport* viewport) 
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* win = Helper::getNativeUIWindow(viewport);
		if  (!win) throw SGE_ERROR("get window pos : w/o init");
		return Util::toImVec2( win->pos() );

//		return {};
	}

	void EditorGuiHandle::EditorGui_ImplWindow_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* win = Helper::getNativeUIWindow(viewport);
		if  (!win) throw SGE_ERROR("get window pos : w/o init");
		win->setSize(Util::toVec2f(size));
	}

	ImVec2 EditorGuiHandle::EditorGui_ImplWindow_GetWindowSize(ImGuiViewport* viewport)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* win = Helper::getNativeUIWindow(viewport);
		if  (!win) throw SGE_ERROR("get window pos : w/o init");

		return Util::toImVec2( win->clientRect().size );
	}



	void EditorGuiHandle::EditorGui_ImplWindow_SetWindowFocus(ImGuiViewport* viewport)
	{
	//	SGE_DUMP_VAR("on set window focus");
	}

	bool EditorGuiHandle::EditorGui_ImplWindow_GetWindowFocus(ImGuiViewport* viewport)
	{
//		SGE_DUMP_VAR("on get window focus");
		return {};
	}

	bool EditorGuiHandle::EditorGui_ImplWindow_GetWindowMinimized(ImGuiViewport* viewport)
	{
//		auto* win = Util::getViewportWindow(viewport);
//		if  (!win) throw SGE_ERROR("get window pos : w/o init");
//		return win->isMinimized();

		return {};
	}



	void EditorGuiHandle::EditorGui_ImplWindow_SetWindowTitle(ImGuiViewport* viewport, const char* title)
	{
		using Helper = EditorGuiHandle_InternalHelper;

		auto* win = Helper::getNativeUIWindow(viewport);
		if  (!win) throw SGE_ERROR("get window pos : w/o init");

//		win->setTitle(title);

//		SGE_DUMP_VAR("on set window title");
	}

	void EditorGuiHandle::EditorGui_ImplWindow_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
	{
//		SGE_DUMP_VAR("on set window alpha");
	}

	void EditorGuiHandle::EditorGui_ImplWindow_UpdateWindow(ImGuiViewport* viewport)
	{
//		auto* handle = Util::getViewportHandle(viewport);
//		auto* win    = Util::getViewportWindow(viewport);

//		SGE_DUMP_VAR(handle->m_myWindows.size());
	}



	float EditorGuiHandle::EditorGui_ImplWindow_GetWindowDpiScale(ImGuiViewport* viewport)
	{
//		SGE_DUMP_VAR("get window dpi scale");
		return {};
	}

	void EditorGuiHandle::EditorGui_ImplWindow_OnChangedViewport(ImGuiViewport* viewport)
	{
//		SGE_DUMP_VAR("on changed viewport");
	}

	void EditorGuiHandle::EditorGui_ImplRenderer_CreateWindow(ImGuiViewport* viewport)
	{
//		auto* renderer = Renderer::current();

	}

}