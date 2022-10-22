
#include "EditorGui.h"

#include <sge_render/vertex/VertexLayoutManager.h>
#include <sge_render/backend/base/Renderer.h>
#include <sge_render/shader/Material.h>

namespace sge {

	void EditorGui::CreateContext()
	{
		ImGui::CreateContext();

//		auto* vp = EditorGui::GetMainViewport();
//
//		SGE_ASSERT(vp->PlatformUserData == nullptr);
//		SGE_ASSERT(vp->PlatformHandle   == nullptr);
//
//		vp->PlatformUserData = handle;
//		vp->PlatformHandle	 = mainWindow;
	}

}