
#include "EditorGui.h"

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>
#include <sge_render/shader/Material.h>

#include <imgui_impl_win32.h>


#include <imgui_impl_dx11.h>
#include <sge_render/backend/dx11/Renderer_DX11.h>

namespace sge {

	void EditorGuiHandle::create(CreateDesc& desc) 
	{
		auto* renderer = Renderer::current();
		
		IMGUI_CHECKVERSION();
		EditorGui::CreateContext();

		ImGuiIO& io = EditorGui::GetIO(); (void)io;
//		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

//		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
//
//		ImGuiStyle& style = ImGui::GetStyle();
//		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
//		{
//		    style.WindowRounding			  = 0.0f;
//		    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//		}



	#if SGE_OS_WINDOWS
		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
		ImGui_ImplWin32_Init(win->m_hwmd);
	#else
	#endif

	#if 0 //For dx11 impl checking
		{
			auto* rd11 = Renderer_DX11::current();
			ImGui_ImplDX11_Init(rd11->d3dDevice(), rd11->d3dDeviceContext());
		}
	#endif

		{//Material
			auto shad  = renderer->createShader("Assets/Shaders/editorgui.shader");
			m_material = renderer->createMaterial();
			m_material->setShader(shad);
		}
		{//Layout
			//temporary -> needed to refector veretex;
			auto& pos = m_vertLayout.elements.emplace_back();
			pos.formatType = RenderFormatType::Float32x2;
			pos.semantic   = VertexSemantic::POSITION;
			pos.offset     = 0;

			auto& uv  = m_vertLayout.elements.emplace_back();
			uv.formatType  = RenderFormatType::Float32x2;
			uv.semantic    = VertexSemantic::TEXCOORD0;
			uv.offset	   = 8;

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

		{//Texture
			unsigned char* pixels = nullptr;
			int width = 0, height = 0, bytesPerPixel = 0;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

			Texture2D::CreateDesc t2dDesc;
			t2dDesc.colorType		= ColorType::RGBAb;
			t2dDesc.mipmapCount		= 1;
			t2dDesc.size			= {width, height};

			{
				auto imageSize = static_cast<size_t>(width * height * bytesPerPixel);
				t2dDesc.imageToUpload.emplace();
				auto& img  =  t2dDesc.imageToUpload.value();
				img.create(ColorType::RGBAb, width, height);
				img.copy  ( Span<u8>( pixels, imageSize ) );
			}
			{
				auto& ss  = t2dDesc.samplerState;
				ss.filter = SamplerState::Filter::Linear;
				ss.minLOD = 0.f;
				ss.maxLOD = 0.f;
			}

			m_fontsTex2D = renderer->createTexture2D(t2dDesc);
			m_material->setParam("texture0", m_fontsTex2D);
		}
	}

	EditorGuiHandle::~EditorGuiHandle() {
	#if SGE_OS_WINDOWS
		ImGui_ImplWin32_Shutdown();
	#else
	#endif
		EditorGui::DestroyContext();
	}

	void EditorGuiHandle::beginRender() 
	{
	#if SGE_OS_WINDOWS
		ImGui_ImplWin32_NewFrame();
	#else
	#endif

	#if 0
		ImGui_ImplDX11_NewFrame();
	#endif

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
			m_material->setParam("ProjectionMatrix", mvp);
		}

		request.drawMesh(SGE_LOC, m_mesh, m_material);

	}

	void EditorGuiHandle::onUIMouseEvent(UIMouseEvent& ev) {
		using T = UIMouseEventType;

		auto& io = EditorGui::GetIO();

		switch (ev.type) {
			case T::Down:	io.AddMouseButtonEvent(Util::getGuiButton(ev.button), true ); break;
			case T::Up:		io.AddMouseButtonEvent(Util::getGuiButton(ev.button), false); break;

			case T::Scroll: {
				auto s = ev.scroll * 0.01f;
				io.AddMouseWheelEvent(s.x , s.y);
			}break;


		}
	}
}