#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/graph/Color.h>

namespace sge 
{
	struct ImageIO_png;
	struct ImageIO_dds;

	struct ImageInfo 
	{
		ColorType colorType			 = ColorType::None;
		Vec2i	  size				 = {0, 0};
		size_t	  pixelSizeInBytes   = 0;
		size_t	  mipmapCount		 = 1;
		size_t	  strideInBytes		 = 0;
	};

	class Image : public NonCopyable {
	public:
		using Info = ImageInfo;

		SGE_INLINE const Info& info  () const { return m_info; }
		SGE_INLINE const Vec2i size	 () const { return m_info.size;	   }
		SGE_INLINE		 int   width () const { return m_info.size.x;  }
		SGE_INLINE		 int   height() const { return m_info.size.y;  }

		SGE_INLINE ColorType colorType() const { return m_info.colorType; }

		
		SGE_INLINE const void* dataPtr() const { return m_pixels.data(); }

		void clear();

		void setInfo  (ImageInfo& info) { m_info = info; }
		void setPixels(ByteSpan& pixel) {  }

		void load 	 (StrView filename);

		void load_png(StrView filename) { _load<ImageIO_png>(filename); }
		void load_dds(StrView filename) { _load<ImageIO_dds>(filename); }

		void load_png(ByteSpan data) { _load<ImageIO_png>(data); }
		void load_dds(ByteSpan data) { _load<ImageIO_dds>(data); }

		void create(ColorType type, int width, int height);

		void copy(const Image& src) {
			m_info	 = src.m_info;
			m_pixels = src.m_pixels;
		}

		void copy(ByteSpan src) {
			SGE_ASSERT(m_pixels.size() == src.size());
			memcpy(m_pixels.data(), src.data(), src.size());
		}


		SGE_INLINE Span<u8>		  rowBytes(int y)		{ return { &m_pixels[y * m_info.strideInBytes], m_info.strideInBytes }; }
		SGE_INLINE Span<const u8> rowBytes(int y) const { return { &m_pixels[y * m_info.strideInBytes], m_info.strideInBytes }; }

	private:
		template<class IMAGE_IO> void _load(StrView filename);
		template<class IMAGE_IO> void _load(ByteSpan	data);

		ColorType m_type = ColorType::None;

		Info	   m_info;
		Vector<u8> m_pixels;
	};




}