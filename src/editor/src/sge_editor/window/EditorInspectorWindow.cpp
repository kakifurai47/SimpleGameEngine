#include "EditorInspectorWindow.h"
#include <sge_editor/EditorContext.h>
#include <sge_engine/ecs/Component/CMeshRenderer.h>
#include <sge_render/mesh/EditMesh.h>
#include <sge_render/backend/base/Renderer.h>


namespace sge
{




	void EditorInspectorWindow::draw(Scene& scene, RenderRequest& request)
	{
		EditorGui::Window window("EntityWindow");

		auto* ctx = EditorContext::instance();
		auto& sel = ctx->editorSelection;

		auto selectId = sel.lastSelect();
		if  (selectId == EntityId::None)
		{
			return;
		}

		auto* select = scene.findEntityById(selectId);
		SGE_ASSERT(select);
		
		auto comps = select->components();

		EditorGui::Text("{} : id {} ", select->name(), enumInt(select->id()));
		EditorGui::ShowProperty (comps);

		if (EditorGui::Button("Add RenderMesh"))
		{
			auto r = select->addComponent<CMeshRenderer>();

			//----
			auto* renderer = Renderer::current();
			auto shad = renderer->createShader("Assets/Shaders/test.shader");
			auto mat  = renderer->createMaterial();
			mat->setShader(shad);
			mat->setParam("mainTex", Texture2D::checker());		

			EditMesh editMesh;
			editMesh.loadObj("Assets/Mesh/box.obj");
			auto* mesh = new RenderMesh();

			auto& colors = editMesh.color;
			colors.resize(editMesh.pos.size(), { 168, 255, 255, 255 });
			mesh->create (editMesh);

			r->setMesh(mesh);
			r->setMaterial(mat);

		}

	}

}