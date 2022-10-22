#pragma once

#include <sge_editor/gui/EditorGuiHandle.h>
#include <sge_engine/Scene.h>

namespace sge
{
	class EditorWindow : public NativeUIWindow 
	{
	public:
		virtual void draw(Scene& scene, RenderRequest& request) {}



	};
}