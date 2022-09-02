
#include "EditorGui.h"

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>
#include <sge_render/shader/Material.h>

#include <imgui_impl_win32.h>

namespace sge {

	struct EditorGui_InternalHelper
	{
		template<class T> static
		inline ByteSpan makeByteSpan(ImVector<T>& vec) {
			return spanCast<const u8,const T>( Span<const T>( vec.Data, vec.Size ) );
		}

		static inline int getButton(UIMouseEventButton src) {
			switch (src) {
				case UIMouseEventButton::Left:		return 0;
				case UIMouseEventButton::Right:		return 1;
				case UIMouseEventButton::Middle:	return 2;

				default: throw SGE_ERROR("Unhandled mouse button pressed");
			}
		}
	};

	void EditorGuiHandle::createContext(CreateDesc& desc)
	{
		IMGUI_CHECKVERSION();
		EditorGui::CreateContext();

	    ImGuiIO& io = ImGui::GetIO(); (void)io;
	    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable  Gamepad Controls

		EditorGui::StyleColorsDark();

	#if SGE_OS_WINDOWS
		auto* win = static_cast<NativeUIWindow_Win32*>(desc.window);
		ImGui_ImplWin32_Init(win->m_hwmd);
	#else

	#endif
		auto* renderer = Renderer::current();

		{			
			auto shad  = renderer->createShader("Assets/Shaders/editorgui.shader");
			m_material = renderer->createMaterial();
			m_material->setShader(shad);
		}
		{
			//temporary -> needed to refector veretex;
			auto& pos = m_layout.elements.emplace_back();
			pos.formatType = RenderFormatType::Float32x2;
			pos.semantic   = VertexSemantic::POSITION;
			pos.offset     = 0;

			auto& uv  = m_layout.elements.emplace_back();
			uv.formatType  = RenderFormatType::Float32x2;
			uv.semantic    = VertexSemantic::TEXCOORD0;
			uv.offset	   = 8;

			auto& col = m_layout.elements.emplace_back();
			col.formatType  = RenderFormatType::UNorm08x4;
			col.semantic	= VertexSemantic::COLOR0;
			col.offset		= 16;

			m_layout.stride = static_cast<size_t>( 8 + 8 + 4 );

			m_mesh.setVertexLayout(&m_layout);
		}
		{
			auto idxFormat = sizeof(ImDrawIdx) == 2 ? RenderFormatType::UInt16x1 :
													  RenderFormatType::UInt32x2;
			m_mesh.setIndexFormat(idxFormat);
		}

		{
			unsigned char* pixels = nullptr;
			int width = 0, height = 0, bytesPerPixel = 0;

			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

			auto s = static_cast<size_t>(width * height * bytesPerPixel);

			SamplerState ss;
			ss.filter = SamplerState::Filter::Linear;
			ss.minLOD = 0.f;
			ss.maxLOD = 0.f;
			
			Texture2D::CreateDesc t2dDesc;
			t2dDesc.colorType		= ColorType::RGBAb;
			t2dDesc.samplerState	= ss;
			t2dDesc.mipmapCount		= 1;
			t2dDesc.size			= {width, height};
			t2dDesc.imageToUpload.emplace();

			auto& img = t2dDesc.imageToUpload.value();
			img.create(ColorType::RGBAb, width,  height );
			img.copy(Span<u8>(pixels, s));

			m_fontsTex = renderer->createTexture2D(t2dDesc);
		}

		m_material->setParam("texture0", m_fontsTex);
	}

	EditorGuiHandle::~EditorGuiHandle() 
	{
	#if SGE_OS_WINDOWS
		ImGui_ImplWin32_Shutdown ();
	#else
	#endif

		EditorGui::DestroyContext();
	}

	void EditorGuiHandle::render(RenderRequest& request) 
	{
		EditorGui::Render();

		auto* dd = EditorGui::GetDrawData();

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
				using Helper = EditorGui_InternalHelper;

				auto* cmdList = dd->CmdLists[i];
				auto& subMesh = subMeshes[i];

				vtxWriter.write(Helper::makeByteSpan(cmdList->VtxBuffer), vtxOffset * sizeof(ImDrawVert));
				idxWriter.write(Helper::makeByteSpan(cmdList->IdxBuffer), idxOffset * sizeof(ImDrawIdx ));

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
				Vec4f( 2.0f/(R-L),   0.0f,           0.0f,       0.0f),
				Vec4f( 0.0f,         2.0f/(T-B),     0.0f,       0.0f),
				Vec4f( 0.0f,         0.0f,           0.5f,       0.0f),
				Vec4f( (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f)
			);

			m_material->setParam("ProjectionMatrix", mvp);
			//change to cbuffer
		}
		{	
		}

		request.drawMesh(SGE_LOC, m_mesh, m_material);
	}

	void EditorGuiHandle::onUIMouseEvent(UIMouseEvent& ev) {
		using Helper = EditorGui_InternalHelper;
		auto& io = EditorGui::GetIO();

		switch (ev.type) {
			case UIMouseEventType::Down: io.AddMouseButtonEvent(Helper::getButton(ev.button), true );
			case UIMouseEventType::Up:   io.AddMouseButtonEvent(Helper::getButton(ev.button), false);
		}
	}

	void EditorGuiHandle::beginRender()
	{
	#if SGE_OS_WINDOWS		
		ImGui_ImplWin32_NewFrame();
	#else

	#endif
		EditorGui::NewFrame();
	}
}