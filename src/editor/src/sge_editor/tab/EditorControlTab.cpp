#include "EditorControlTab.h"

#include <sge_editor/gui/EditorGui.h>
#include <sge_editor/EditorContext.h>
#include <sge_engine/Scene.h>

namespace sge
{
	void EditorControlTab::draw(Scene& scene, RenderRequest& req)
	{
		EditorGui::Window window("ControlTab");
		{
			auto add = EditorGui::Button("Add Entity");
			if  (add) {
				auto* e = scene.addEntity();
				e->setName("entity");

				auto selectId = EditorContext::instance()->editorSelection.lastSelect();
				if  (selectId != EntityId::None)
				{
					if( auto* selected = scene.findEntityById(selectId) ) 
					{
						selected->transform()->addChild(e->transform());
					}
				}
			}
		}
	}
	
}


