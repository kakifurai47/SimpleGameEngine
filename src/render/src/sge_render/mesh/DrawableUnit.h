#pragma once

#include <sge_render/backend/base/Render_Common.h>
#include <sge_render/buffer/RenderGpuBuffer.h>
#include <sge_render/vertex/Vertex.h>

namespace sge 
{
	class DrawableUnit {
	public:
		SGE_INLINE RenderPrimitiveType	  primitive() const { return m_primitive;	 }

		SGE_INLINE RenderFormatType     indexFormat() const { return m_indexFormat;  }
		SGE_INLINE const VertexLayout* vertexLayout() const { return m_vertexLayout; }

		SGE_INLINE RenderGpuBuffer*		indexBuffer()		{ return m_indexBuffer;  }
		SGE_INLINE RenderGpuBuffer*	   vertexBuffer()		{ return m_vertexBuffer; }
		
		SGE_INLINE size_t  indexCount() const { return m_indexCount;  }
		SGE_INLINE size_t vertexCount() const { return m_vertexCount; }

		//void setPrimtive(RenderPrimitiveType primitive) { m_primitive = primitive; }
		//
		//void setVertices(const VertexLayout*  vtxLayout, SPtr<RenderGpuBuffer> vtxbuff, size_t count);
		//void setIndices (  RenderFormatType	  idxFormat, SPtr<RenderGpuBuffer> idxBuff, size_t count);

	protected:
		RenderPrimitiveType	m_primitive		= RenderPrimitiveType::None;
		
		const VertexLayout* m_vertexLayout	= nullptr;
		RenderFormatType	m_indexFormat	= RenderFormatType::None;

		SPtr<RenderGpuBuffer>  m_indexBuffer;
		SPtr<RenderGpuBuffer> m_vertexBuffer;

		size_t  m_indexCount	= 0;
		size_t m_vertexCount	= 0;
	};
}