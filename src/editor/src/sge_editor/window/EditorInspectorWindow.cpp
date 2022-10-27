#include "EditorInspectorWindow.h"
#include <sge_editor/EditorContext.h>
#include <sge_engine/ecs/Component/CMeshRenderer.h>

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
			select->addComponent<CMeshRenderer>();
		}

	}

}