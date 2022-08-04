#include "MeshBuilder.h"

#include <sge_render/backend/base/Renderer.h>

namespace sge {

	MeshBuilder::MeshBuilder(const CreateDesc& desc) {
		resetIndices (desc.indexFormat, desc.indexCount);
		resetVertices(desc.layout,	   desc.vertexCount);
	}

	void MeshBuilder::clear() {
		resetIndices ();
		resetVertices();
	}

	void MeshBuilder::resetIndices(RenderFormatType indexFormat, size_t indexCount) {
		using FT = RenderFormatType;

		m_indexData.clear();

		auto rsltFmt = indexFormat;
		if (indexCount > 0) {
			if (rsltFmt == RenderFormatType::None) {
				rsltFmt = indexCount >= UINT16_MAX ? FT::UInt32x1 : FT::UInt16x1;
			}
			if (rsltFmt != FT::UInt32x1 && rsltFmt != FT::UInt16x1) {
				throw SGE_ERROR("mesh builder : unsupported index format : [{}]", rsltFmt);
			}
		}

		m_indexCount= indexCount;
		m_indexFormat  = rsltFmt;
		m_indexData.resize(m_indexCount * indexStride());
	}

	void MeshBuilder::resetVertices(const VertexLayout* vertexLayout , size_t vertexCount) {
		m_vertexData.clear();
		
		m_layout	 = vertexLayout;
		m_vertexCount = vertexCount;
		m_vertexData.resize(m_vertexCount * vertexStride());
	}


	void MeshBuilder::createIndexResult(RenderFormatType& outFormat, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer)
	{
		outFormat   = m_indexFormat;
		outCount	= m_indexCount;

		auto* r = Renderer::current();

		RenderGpuBuffer::CreateDesc desc {};
		desc.type		= RenderGpuBufferType::Index;
		desc.bufferSize = m_indexData.size();

		outBuffer = r->createGpuBuffer(desc);
		outBuffer->uploadToGpu( m_indexData);
	}

	void MeshBuilder::createVertexResult(const VertexLayout*& outLayout, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer)
	{
		outLayout = m_layout;
		outCount  = m_vertexCount;

		auto* r = Renderer::current();

		RenderGpuBuffer::CreateDesc	desc {};
		desc.type		 = RenderGpuBufferType::Vertex;
		desc.bufferSize  = m_vertexData.size();

		outBuffer = r->createGpuBuffer(desc);
		outBuffer->uploadToGpu(m_vertexData);
	}

	MeshBuilder::BuildArray MeshBuilder::vertAttr(VertexSemantic semantic) {
		if (!m_layout) {
			throw SGE_ERROR("mesh builder : absence of vertex layout");
		}
		auto* elm = m_layout->find(semantic);
		if  (!elm) {
			throw SGE_ERROR("mesh builder : access non existing vertex element");
		}
		return BuildArray {
			m_vertexData.data(), elm->formatType, elm->offset, vertexStride(), m_vertexCount
		};
	}
}