#include <sge_core/graph/Color.h>

#include "Image.h"

#include "ImageIO_dds.h"
#include "ImageIO_png.h"

namespace sge {

	void Image::clear() {
		m_info = {};
		m_pixels.clear();
	}

	void Image::load(StrView filename)
	{
		using SU = StringUtil;
		auto ext = FilePath::extension(filename);

		if (SU::ignoreCaseCompare(ext, "png") == 0) { return load_png(filename); }
		if (SU::ignoreCaseCompare(ext, "dds") == 0) { return load_dds(filename); }

		throw SGE_ERROR("loading unsupported image format: [{}]", ext);
	}

	template<class IMAGE_IO>
	void Image::_load(StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		_load<IMAGE_IO>(mm.span());
	}

	template<class IMAGE_IO>
	void Image::_load(ByteSpan data) {
		IMAGE_IO::Reader  r;
		r.load(data, *this);
	}

	void Image::create(ColorType type, int width, int height) 
	{
		m_info.colorType		= type;
		m_info.size				= { width, height };
		m_info.pixelSizeInBytes = ColorUtil::pixelSizeInBytes(type);
		m_info.mipmapCount		= 1;
		m_info.strideInBytes	= width * m_info.pixelSizeInBytes;

		try {
			m_pixels.clear();
			m_pixels.resize(m_info.strideInBytes * height);
		}
		catch (...) {
			clear();
			throw SGE_ERROR("fail create image");
		}
	}

	template void Image::_load<ImageIO_png>(StrView filename);
	template void Image::_load<ImageIO_dds>(StrView filename);

	template void Image::_load<ImageIO_png>(ByteSpan data);
	template void Image::_load<ImageIO_dds>(ByteSpan data);
}



