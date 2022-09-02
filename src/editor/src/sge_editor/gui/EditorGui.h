#pragma once

#include <imgui.h>
#include <sge_render/command/RenderRequest.h>


namespace sge {

	namespace EditorGui = ImGui;

	struct EditorGuiHandle_CreateDesc {
		NativeUIWindow* window = nullptr;
	};

	class EditorGuiHandle : public Object  
	{
	public:
		using CreateDesc = EditorGuiHandle_CreateDesc;
//		using Vertex	 = VertexLib::Pos2DTexCol;

		~EditorGuiHandle();
		
		void createContext(CreateDesc& desc);

		void beginRender();
		void render(RenderRequest& request);


	protected:

		RenderMesh	    m_mesh;
		SPtr<Texture2D> m_fontsTex;
		SPtr<Material>  m_material;


		VertexLayout	m_layout;
	};


}