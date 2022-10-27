#pragma once

#include "CRenderer.h"

namespace sge
{

	class RenderMesh;

	class CMeshRenderer : public CRenderer
	{
	public:
		SGE_TYPE_INFO(CMeshRenderer, CRenderer)

		void setMesh(RenderMesh* mesh) { m_mesh = mesh; }



	};









}