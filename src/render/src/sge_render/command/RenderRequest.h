#pragma once

#include "RenderCommand.h"

namespace sge
{

	class RenderRequest : public Object
	{
		struct Command {
			using Clear = RenderCommand_ClearFrameBuffers;
			using Swap  = RenderCommand_SwapBuffers;
			using Draw  = RenderCommand_DrawCall;
		};

	public:

		Mat4f model			= Mat4f::s_identity();
		Mat4f view			= Mat4f::s_identity();
		Mat4f proj			= Mat4f::s_identity();

		Vec3f camera_pos	= Vec3f::s_zero();

		RenderCommandBuffer commandBuffer;

		SPtr<Material> lineMaterial;
		
		void reset(RenderContext* ctx);

		//TODO : separate cbuffer
		void setMaterialCommonParams(Material* mtl);

		void drawMesh	(const SrcLoc& debugLoc, RenderMesh&    mesh,	 Material* mtl);
		void drawSubMesh(const SrcLoc& debugLoc, RenderSubMesh& subMesh, Material* mtl);

		void drawFullScreenTriangle(const SrcLoc& debugLoc, Material* mtl);
		void drawLines(Span<VertexLib::PosCol> points, Span<u16> indicies);
		void drawWiredCube(const Vec3f& center, const Vec3f& size, const Color4b& color);


		RenderCommand_ClearFrameBuffers*	clearFrameBuffers() { return commandBuffer.newCommand<Command::Clear>(); }
		RenderCommand_SwapBuffers*			swapBuffers()		{ return commandBuffer.newCommand<Command::Swap>();  }
		RenderCommand_DrawCall*				addDrawCall()		{ return commandBuffer.newCommand<Command::Draw>();  }

		void commit();

	private:
		struct InlineDraw {
			Vector<u8> vertexData;
			Vector<u8> indexData;

			SPtr<RenderGpuBuffer> vertexBuffer;
			SPtr<RenderGpuBuffer> indexBuffer;

			void reset();
			void uploadToGpu();

		private:
			void _uploadToGpu(SPtr<RenderGpuBuffer>& buf, const Vector<u8>& data);
		};

		InlineDraw m_inlineDraw;

		RenderContext* m_renderContext = nullptr;
	};




}