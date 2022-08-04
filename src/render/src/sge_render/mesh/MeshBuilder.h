#pragma once

#include <sge_render/vertex/Vertex.h>
#include <sge_render/mesh/DrawableUnit.h>

namespace sge {

	class MeshBuilder_BuildArray {
		using Util = RenderFormatTypeUtil;
	public:

		MeshBuilder_BuildArray() = default;
		MeshBuilder_BuildArray(u8* data, RenderFormatType format, size_t offset, size_t stride, size_t count)
			: m_data(data), m_count(count), m_format(format)
			, m_offset(offset), m_stride(stride)
		{
		}

		template<class T>
		void add(size_t index, const T& value) {
			_throwIfOutOfRange(index);
			_add(index, value);
		}

		template<class T>
		void copy(const Vector<T>& src) 
		{
			_throwIfOutOfRange(src.size() - 1);

			for (size_t i = 0; i < src.size(); i++) {
				_add(i, src[i]);
			}
		}
	private:
		void _throwIfOutOfRange(size_t  index) {
			if (index < 0 || index >= m_count) {
				throw SGE_ERROR("build array :  out of range");
			}
		}
		
		template<class S>
		void _add(size_t index, const S& value) 
		{
		#define ADD_CASE(CLSS_TYPE, ENUM_TYPE) \
			case ENUM_TYPE: { \
				if constexpr (std::is_convertible<S, CLSS_TYPE>::value) { \
					auto* p = m_data + m_offset + m_stride * index; \
					*reinterpret_cast<CLSS_TYPE*>(p) = static_cast<CLSS_TYPE>(value); \
				} \
				else { \
					throw SGE_ERROR("build array : no applicable conversion " \
									"from : [{}] to : [{}]", Util::get<S>(), m_format); \
				} \
			\
			} break; \
		//----

			switch (m_format) {
				SGE_RENDER_FMT_TRAIT_PAIRS(ADD_CASE)
				default: 
					throw SGE_ERROR("unhandled format");
			}
		#undef ADD_CASE
		}

		RenderFormatType m_format = RenderFormatType::None;
		
		size_t m_stride = 0;
		size_t m_offset = 0;
		size_t m_count	= 0;

		u8*	   m_data	= nullptr;
	};	

	struct MeshBuilder_CreateDesc 
	{
		const VertexLayout*   layout = nullptr;
		RenderFormatType indexFormat = RenderFormatType::None;

		size_t vertexCount = 0;
		size_t  indexCount = 0;
	};


	class MeshBuilder : public NonCopyable {
	public:
		using CreateDesc = MeshBuilder_CreateDesc;

		using BuildArray = MeshBuilder_BuildArray;
		using Util		 = VertexSemanticUtil;

		MeshBuilder(const CreateDesc& desc);

		SGE_INLINE size_t  indexStride() { return m_indexFormat == RenderFormatType::UInt32x1 ? 4 : 2; }
		SGE_INLINE size_t vertexStride() { return m_layout != nullptr ? m_layout->stride : 0; }

		SGE_INLINE size_t  indexCount() { return  m_indexCount; }
		SGE_INLINE size_t vertexCount() { return m_vertexCount; }

		SGE_INLINE RenderFormatType     indexFormat() const { return m_indexFormat; }
		SGE_INLINE const VertexLayout* vertexLayout() const { return m_layout; }

		SGE_INLINE ByteSpan  indexData() { return  m_indexData; }
		SGE_INLINE ByteSpan vertexData() { return m_vertexData; }

		BuildArray vertAttr(VertexSemantic	   semantic);
		BuildArray vertAttr(VertexSemanticType semanticType, u8 semanticIndex) { return vertAttr(Util::make(semanticType, semanticIndex));  }

		BuildArray idx		()				   { return {m_indexData.data(), m_indexFormat, 0, indexStride(), m_indexCount}; }
		BuildArray pos		(u8 semanticIndex) { return vertAttr(VertexSemanticType::POSITION, semanticIndex); }
		BuildArray uv		(u8 semanticIndex) { return vertAttr(VertexSemanticType::TEXCOORD, semanticIndex); }
		BuildArray color	(u8 semanticIndex) { return vertAttr(VertexSemanticType::COLOR,	   semanticIndex); }
		BuildArray normal	(u8 semanticIndex) { return vertAttr(VertexSemanticType::NORMAL,   semanticIndex); }
		BuildArray tangent	(u8 semanticIndex) { return vertAttr(VertexSemanticType::TANGENT,  semanticIndex); }
		BuildArray binormal	(u8 semanticIndex) { return vertAttr(VertexSemanticType::BINORMAL, semanticIndex); }

		void clear();

		void resetIndices (RenderFormatType     indexFormat = RenderFormatType::None, size_t count = 0);
		void resetVertices(const VertexLayout* vertexLayout = nullptr,				  size_t count = 0);

		void  createIndexResult(RenderFormatType&    outFormat, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer);
		void createVertexResult(const VertexLayout*& outLayout, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer);

	protected:
		const VertexLayout*   m_layout	= nullptr;
		RenderFormatType m_indexFormat	= RenderFormatType::None;

		size_t  m_indexCount = 0;
		size_t m_vertexCount = 0;

		Vector_<u8, 1024>  m_indexData;
		Vector_<u8, 1024> m_vertexData;
	};
}