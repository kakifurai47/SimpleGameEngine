#pragma once

#include "../window/EditorWindow.h"

namespace sge
{
	class Scene;
	class RenderRequest;
	
	class EditorControlTab : public EditorWindow
	{
	public:
		void draw(Scene& scene, RenderRequest& req);


	};
}