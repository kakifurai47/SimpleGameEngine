
#include "EditorGui.h"

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>
#include <sge_render/shader/Material.h>

namespace sge {

	void EditorGuiHandle::create(CreateDesc& desc) 
	{
		m_mainWindow = desc.window;

		auto* renderer = Renderer::current();
		
		IMGUI_CHECKVERSION();
		EditorGui::CreateContext();

		ImGuiIO& io = EditorGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
		    style.WindowRounding			  = 0.0f;
		    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		{//ImGui_ImplWin32_Init

			// INT64 perf_frequency, perf_counter;
			// if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency)) return false;
			// if (!::QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter)) 	  return false;

			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
			io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
			io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)

			EditorGui::GetMainViewport()->PlatformHandle = desc.window;

//			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//			{
//				ImGui_ImplWin32_InitPlatformInterface();
//			}

		}

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

	EditorGuiHandle::~EditorGuiHandle() 
	{
		EditorGui::DestroyContext();
	}

	void EditorGuiHandle::beginRender() {
		auto& io = EditorGui::GetIO();

		auto& rect     = m_mainWindow->clientRect();
		io.DisplaySize = {rect.size.x, rect.size.y};

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

	void EditorGuiHandle::EditorGui_ImplWindow_CreateWindow(ImGuiViewport* viewport)
	{
		MyWindow::CreateDesc desc;
		Util::setWindowCreateDesc(desc, viewport->Flags);
		





	}

	void EditorGuiHandle::EditorGui_ImplRenderer_CreateWindow(ImGuiViewport* viewport)
	{
//		auto* renderer = Renderer::current();

	}

}