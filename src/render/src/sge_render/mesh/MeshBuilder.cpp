#include "MeshBuilder.h"

#include <sge_render/backend/base/Renderer.h>


namespace sge {

	MeshBuilder::MeshBuilder(const CreateDesc& desc) {
		resetIndices (desc.indexCount,  desc.indexFormat);
		resetVertices(desc.vertexCount, desc.layout);
	}

	void MeshBuilder::clear() {
		resetIndices (0);
		resetVertices(0, nullptr);
	}

	void MeshBuilder::resetIndices(size_t indexCount, RenderFormatType indexFormat) {
		using FT = RenderFormatType;

		m_indexData.clear();

		auto rsltFormat = indexFormat;
		if (indexCount > 0) {
			if (rsltFormat == RenderFormatType::None) {
				rsltFormat = indexCount >= UINT16_MAX ? FT::UInt32x1 : FT::UInt16x1;
			}

			if (rsltFormat != FT::UInt16x1 &&
				rsltFormat != FT::UInt32x1) {
				throw SGE_ERROR("mesh builder : unsupported index format : [{}]", rsltFormat);
			}

			if(rsltFormat == FT::UInt16x1 && indexCount >= UINT16_MAX) {
				throw SGE_ERROR("mesh builder : invalid index foramt");
			}
		}

		m_indexCount  = indexCount;
		m_indexFormat = rsltFormat;
		m_indexData.resize(m_indexCount * indexStride());
	}

	void MeshBuilder::resetVertices(size_t vertexCount, const VertexLayout* vertexLayout) {
		m_vertexData.clear();
		
		m_layout	 = vertexLayout;
		m_vertexCount = vertexCount;
		m_vertexData.resize(m_vertexCount * vertexStride());
	}


	void MeshBuilder::createIndexResult(RenderFormatType& outFormat, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer)
	{
		if (m_indexCount < 0) {
			throw SGE_ERROR("mesh builder : creating empty buffer");
		}
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
		if (m_vertexCount < 0) {
			throw SGE_ERROR("mesh builder : creating empty buffer");
		}
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
			&m_vertexData, elm->formatType, elm->offset, vertexStride(), m_vertexCount
		};
	}
}