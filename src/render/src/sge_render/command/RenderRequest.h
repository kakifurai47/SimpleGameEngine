#pragma once

#include "RenderCommand.h"

namespace sge
{

	class RenderRequest : public Object
	{
	public:

		Mat4f model			= Mat4f::s_identity();
		Mat4f view			= Mat4f::s_identity();
		Mat4f proj			= Mat4f::s_identity();

		Vec3f camera_pos	= Vec3f::s_zero();

		RenderCommandBuffer commandBuffer;
		
		void reset();

		void setMaterialCommonParams(Material* mtl);

		void drawMesh	(const SrcLoc& debugLoc, RenderMesh&    mesh,	 Material* mtl);
		void drawSubMesh(const SrcLoc& debugLoc, RenderSubMesh& subMesh, Material* mtl);

		RenderCommand_ClearFrameBuffers* clearFrameBuffers() {
			return commandBuffer.newCommand<RenderCommand_ClearFrameBuffers>();
		}

		RenderCommand_SwapBuffers* swapBuffers() {
			return commandBuffer.newCommand<RenderCommand_SwapBuffers>();
		}
	};




}