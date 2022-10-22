
#include "EditorHierachyWindow.h"
#include <sge_editor/EditorContext.h>

namespace sge
{

	void EditorHierachyWindow::draw(Scene& scene, RenderRequest& request)
	{
		EditorGui::Window window("HierachyWindow");

		TempString tmp;
		auto entities = scene.entities();

		for (auto* e : entities)
		{
			if (e->transform()->hasParent())
			{
				continue;
			}

			_drawEntity(tmp, e);
		}
	}

	void EditorHierachyWindow::_drawEntity(TempString& temp, Entity* entity)
	{
		temp.clear();
		FmtTo(temp, "{} id ({})", entity->name(), enumInt( entity->id() ));

		auto  childs = entity->transform()->childs();

		auto* ctx	 = EditorContext::instance();
		auto& sel	 = ctx->editorSelection;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		if (sel.has(entity->id()))
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (childs.size() <= 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		EditorGui::TreeNode node(temp, enumInt(entity->id()), flags);

		if (EditorGui::IsItemClicked())
		{
			sel.select(entity->id());
		}

		if (node.isOpen()) {
			for (auto* transform : childs)
			{
				_drawEntity(temp, transform->entity());
			}
		}
	}
}