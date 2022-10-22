#pragma once

#include "EditorWindow.h"


namespace sge
{
	class EditorHierachyWindow : public EditorWindow
	{
	public:
		virtual void draw(Scene& scene, RenderRequest& request) override;

	private:
		void _drawEntity(TempString& temp, Entity* entity);
		
	};



}