#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/graph/Color.h>

namespace sge 
{
	struct ImageIO_png;
	struct ImageIO_dds;

	struct ImageInfo 
	{
		ColorType	colorType					= ColorType::None;
		Vec2i		size						= {0, 0};
		size_t		pixelSizeInBytes() const	{ return ColorUtil::pixelSizeInBytes(colorType); }
		size_t		mipmapCount					= 1;
		size_t		strideInBytes				= 0;
	};

	class Image : public NonCopyable {
	public:
		using Info = ImageInfo;

		SGE_INLINE const Info&		info		() const { return m_info;			}
		SGE_INLINE const Vec2i		size		() const { return m_info.size;		}
		SGE_INLINE		 size_t		width		() const { return m_info.size.x;	}
		SGE_INLINE		 size_t		height		() const { return m_info.size.y;	}
		SGE_INLINE		 ColorType	colorType	() const { return m_info.colorType; }

		SGE_INLINE const void*	dataPtr		() const { return m_pixels.data();	}
		SGE_INLINE ByteSpan		pixelData	() const { return m_pixels;			}
		
		template<class COLOR> SGE_INLINE Span<const	COLOR> row	(size_t y) const	{ _checkType(COLOR::kColorType); return row_noCheck<const COLOR>(y); }
		template<class COLOR> SGE_INLINE Span<		COLOR> row	(size_t y)			{ _checkType(COLOR::kColorType); return row_noCheck<	  COLOR>(y); }

		template<class COLOR> SGE_INLINE Span<const COLOR> row_noCheck(size_t y) const { return Span<const COLOR>(reinterpret_cast<const COLOR*>(rowBytes(y).data()), m_info.size.x); }
		template<class COLOR> SGE_INLINE Span<		COLOR> row_noCheck(size_t y)	   { return Span<	   COLOR>(reinterpret_cast<		 COLOR*>(rowBytes(y).data()), m_info.size.x); }

		template<class COLOR> SGE_INLINE	  const	COLOR& pixel(size_t x, size_t y) const	{ return row<COLOR>(y)[x]; }
		template<class COLOR> SGE_INLINE	  		COLOR& pixel(size_t x, size_t y)		{ return row<COLOR>(y)[x]; }

		template<class COLOR> SGE_INLINE	  const	COLOR& pixel_noCheck(size_t x, size_t y) const	{ return row_noCheck<COLOR>(y)[x]; }
		template<class COLOR> SGE_INLINE			COLOR& pixel_noCheck(size_t x, size_t y)		{ return row_noCheck<COLOR>(y)[x]; }

		template<class COLOR> void fill(const COLOR& color);


		void clear();

		void setInfo  (ImageInfo& info) { m_info = info; }
		void setPixels(ByteSpan& pixel) {  }

		void load 	 (StrView filename);

		void load_png(StrView filename) { _load<ImageIO_png>(filename); }
		void load_dds(StrView filename) { _load<ImageIO_dds>(filename); }

		void load_png(ByteSpan data) { _load<ImageIO_png>(data); }
		void load_dds(ByteSpan data) { _load<ImageIO_dds>(data); }

		void create(ColorType type, size_t width, size_t height);

		void copy(const Image& src) {
			m_info	 = src.m_info;
			m_pixels = src.m_pixels;
		}

		void copy(ByteSpan src) {
			SGE_ASSERT	 (m_pixels.size()	 == src.size());
			memcpy(m_pixels.data(), src.data(), src.size());
		}

		SGE_INLINE Span<u8>		  rowBytes(size_t y)		{ return { &m_pixels[y * m_info.strideInBytes], m_info.strideInBytes }; }
		SGE_INLINE Span<const u8> rowBytes(size_t y) const	{ return { &m_pixels[y * m_info.strideInBytes], m_info.strideInBytes }; }

	private:
		template<class IMAGE_IO> void _load(StrView filename);
		template<class IMAGE_IO> void _load(ByteSpan	data);

		inline void _checkType(ColorType t) const 
		{
			if (t != m_info.colorType)
				throw SGE_ERROR("Image : Invalid Image Type");
		}

		 
		ColorType m_type = ColorType::None;

		Info	   m_info;
		Vector<u8> m_pixels;
	};

	template<class COLOR>
	void Image::fill(const COLOR& color) {
		_checkType(COLOR::kColorType);
		for (size_t y = 0; y < height(); y++) {
			for (auto& p : row_noCheck<COLOR>(y) ) {
				p = color;
			}
		}
	}



}