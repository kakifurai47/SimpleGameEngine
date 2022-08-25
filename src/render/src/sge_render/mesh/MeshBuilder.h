#pragma once

#include <sge_render/texture/Texture.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/mesh/DrawableUnit.h>

namespace sge {

	using MeshBuilder_Buffer = Vector<u8, 1024>;

	class MeshBuilder_BuildArray {
		using Util	 = RenderFormatTypeUtil;
	public:
		using Buffer = MeshBuilder_Buffer;

		MeshBuilder_BuildArray() = default;
		MeshBuilder_BuildArray(Buffer* vector, RenderFormatType format, size_t offset, size_t stride, size_t capacity)
			: m_buffer(vector), m_capacity(capacity), m_format(format)
			, m_offset(offset), m_stride(stride)
		{
		}

		SGE_INLINE size_t size		() { return m_size; }
		SGE_INLINE size_t capacity	() { return m_capacity; }

		template<class T>
		void assign(size_t index, const T& value) {
			_throwIfOutOfRange(index);
			_assign(index, value);
		}

		template<class T>
		void copy(const Vector<T>& src) 
		{
			_throwIfOutOfRange(src.size() - 1);

			for (size_t i = 0; i < src.size(); i++) {
				_assign(i, src[i]);
			}
		}

		template<class T>
		void add(const T&  value) { assign(m_size, value); m_size++; }

	private:
		void _throwIfOutOfRange(size_t  index) {
			if (index < 0 || index >= m_capacity) {
				throw SGE_ERROR("build array :  out of range");
			}
		}
		
		template<class S>
		void _assign(size_t index, const S& value) 
		{
		#define ADD_CASE(CLSS_TYPE, ENUM_TYPE) \
			case ENUM_TYPE: { \
				if constexpr (std::is_convertible<S, CLSS_TYPE>::value) { \
					auto* p = m_buffer->data() + m_offset + m_stride * index; \
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

		Buffer* m_buffer	= nullptr;
		size_t	m_size		= 0;
		size_t	m_capacity	= 0;
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
		using Buffer	 = MeshBuilder_Buffer;

		MeshBuilder() = default;
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

		BuildArray index	()				   { return BuildArray{&m_indexData, m_indexFormat, 0, indexStride(), m_indexCount}; }
		BuildArray pos		(u8 semanticIndex) { return vertAttr(VertexSemanticType::POSITION, semanticIndex); }
		BuildArray uv		(u8 semanticIndex) { return vertAttr(VertexSemanticType::TEXCOORD, semanticIndex); }
		BuildArray color	(u8 semanticIndex) { return vertAttr(VertexSemanticType::COLOR,	   semanticIndex); }
		BuildArray normal	(u8 semanticIndex) { return vertAttr(VertexSemanticType::NORMAL,   semanticIndex); }
		BuildArray tangent	(u8 semanticIndex) { return vertAttr(VertexSemanticType::TANGENT,  semanticIndex); }
		BuildArray binormal	(u8 semanticIndex) { return vertAttr(VertexSemanticType::BINORMAL, semanticIndex); }

		void clear();

		void resetIndices (size_t count, RenderFormatType indexFormat = RenderFormatType::None); //warning : do not use the BuildArray after reset get the new one
		void resetVertices(size_t count, const VertexLayout* vertexLayout);						 //warning : do not use the BuildArray after reset get the new one

		void  createIndexResult(RenderFormatType&    outFormat, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer);
		void createVertexResult(const VertexLayout*& outLayout, size_t& outCount, SPtr<RenderGpuBuffer>& outBuffer);

	protected:
		const VertexLayout*   m_layout	= nullptr;
		RenderFormatType m_indexFormat	= RenderFormatType::None;

		size_t  m_indexCount = 0;
		size_t m_vertexCount = 0;

		Buffer  m_indexData;
		Buffer m_vertexData;
	};
}