#include "ImageIO_png.h"
#include "Image.h"

namespace sge {
		

	ImageIO_png::Reader::~Reader() {
		if (m_info) {
			png_destroy_info_struct(m_png, &m_info);
			m_info = nullptr;
		}
		if (m_png) {
			png_destroy_read_struct(&m_png, nullptr, nullptr);
			m_png = nullptr;
		}
	}

	void ImageIO_png::Reader::s_onRead(png_structp png_ptr, png_bytep dst, png_size_t length) {
		auto* reader = reinterpret_cast<Reader*>(png_get_io_ptr( png_ptr ));
		if ( !reader ) {
			throw SGE_ERROR("png error : read data");
		}
		reader->onRead(dst, length);
	}

	void ImageIO_png::Reader::onRead(png_bytep dst, png_size_t len) {
		size_t n = len;
		if (m_readPtr + n > m_data.end()) {
			throw SGE_ERROR("png error : read data out of range");
		}
		memcpy(dst, m_readPtr, n);
		m_readPtr += n;
	}

	void ImageIO_png::Reader::setReadFilter(ColorType outColorType, int inColorType, int inBit, bool inHasTransprancy) {
		auto outElementType =  ColorUtil::elementType(outColorType);
		if  (outElementType == ColorElementType::None) {
			throw SGE_ERROR("png error : unsupported format");
		}
		//8 <-> 16
		switch (outElementType) {
			case ColorElementType::UNorm8: {
				switch (inBit) {
					case 8:  break;
					case 16: png_set_scale_16(m_png); break;
					default: throw SGE_ERROR("png error : unsupported format");
				}
			}break;

			case ColorElementType::UNorm16: {
				switch (inBit) {
					case 8:  png_set_expand_16(m_png); break;
					case 16: break;
					default: throw SGE_ERROR("png error : unsupported format");
				}
			#if SGE_CPU_ENDIAN_LITTLE
				 png_set_swap(m_png);
			#endif
			}break;
			default: throw SGE_ERROR("png error : unsupported format");
		}

		//alpha
		bool inHasAlpha = inColorType & PNG_COLOR_MASK_ALPHA;
		if (ColorUtil::hasAlpha(outColorType)) 
		{
			if ( inHasTransprancy) { png_set_tRNS_to_alpha(m_png); }
			if (!inHasTransprancy || !inHasAlpha) {
				png_set_filler(m_png, 0xffff, PNG_FILLER_AFTER);
			}
		}
		else if(inHasAlpha) {
			png_set_strip_alpha(m_png);
		}

		//rgb <-> greyScale <-> palette
		bool inIsColorMask	 =  inColorType & PNG_COLOR_MASK_COLOR  ;
		bool inIsPalette	 =  inColorType & PNG_COLOR_MASK_PALETTE;
		bool inIsGrayScale	 = !inIsColorMask;

		auto	outColorModel = ColorUtil::colorModel(outColorType);
		switch (outColorModel) 
		{
			case ColorModel::RGB: {
				if (inIsPalette || inIsGrayScale) png_set_gray_to_rgb(m_png);
				if (inIsPalette && inHasTransprancy) {
					throw SGE_ERROR("png error : unsupported format");
				}
			} break;
			case ColorModel::RGBA: {
				if (inIsGrayScale) png_set_gray_to_rgb   (m_png);
				if (inIsPalette)   png_set_palette_to_rgb(m_png);
			}break;

			case ColorModel::L :
			case ColorModel::LA: {
				if (inIsColorMask) png_set_rgb_to_gray_fixed(m_png, -1, -1, -1);
			}break;

			default: throw SGE_ERROR("png error : unsupported format");
		}

	}

	bool ImageIO_png::Reader::error_longjmp_restore_point() {
		// !!! call this function before any libpng C-function that might longjmp()
		// to avoid any C++ destructor or exception try/catch block happen in between
		
		#if SGE_COMPILER_VC
		#pragma warning(push) 
		#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
		#endif
		
		// longjmp() to here from pngReportError()
		return setjmp(png_jmpbuf(m_png)) != 0;
		
		#if SGE_COMPILER_VC
		#pragma warning(pop) 
		#endif
	}

	void ImageIO_png::Reader::load(ByteSpan& src, Image& outImg, ColorType expectType)
	{
		m_data	  = src;
		m_readPtr = src.data();

		SGE_ASSERT(m_png == nullptr);

		if (src.size() < 8 || png_sig_cmp(src.data(), 0, 8)) {
			throw SGE_ERROR("png error : not png");
		}

		m_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (!m_png) {
			throw SGE_ERROR("png error : read struct");//out of memory
		}

		m_info = png_create_info_struct(m_png);
		if(!m_info) {			
			throw SGE_ERROR("png error: info struct");//out of memory
		}

		png_set_read_fn(m_png, this, &s_onRead);

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error : read header");
		}

		png_read_info(m_png, m_info);

		png_uint_32 inWidth  {};
		png_uint_32 inHeight {};
		
		int	 inBit				{};
		int	 inColorType		{};
		int	 inInterlace		{};
		bool inHasTransprancy	{};

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error : decompress");
		}
		png_get_IHDR(m_png, m_info, &inWidth, &inHeight, &inBit, &inColorType, &inInterlace, nullptr, nullptr);

		auto outColorType = expectType;
		if  (outColorType == ColorType::None)
		{
			auto outColorModel		 = ColorModel::None;
			auto outColorElementType = ColorElementType::None;

			switch (inColorType) {
				case PNG_COLOR_TYPE_RGB:			outColorModel = ColorModel::RGBA;	break;
				case PNG_COLOR_TYPE_RGB_ALPHA:		outColorModel = ColorModel::RGBA;	break;
				case PNG_COLOR_TYPE_GRAY:			outColorModel = ColorModel::L;		break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:		outColorModel = ColorModel::LA;		break;
				case PNG_COLOR_TYPE_PALETTE:		outColorModel = ColorModel::RGBA;	break;

				default: throw SGE_ERROR("png error : unsupported format");
			}

			switch (inBit) {
				case 8:		outColorElementType = ColorElementType::UNorm8;		break;
				case 16:	outColorElementType = ColorElementType::UNorm16;	break;

				default: throw SGE_ERROR("png error : unsupported format");
			}

			{
				if (error_longjmp_restore_point()) {
					throw SGE_ERROR("png error : read header color type");
				}

				if (png_get_valid(m_png, m_info, PNG_INFO_tRNS)) {
					inHasTransprancy = true;
				}
			}

			outColorType = ColorType_make(outColorModel, outColorElementType);
		}

		if (outColorType == ColorType::None) {
			throw SGE_ERROR("png error : unsupported output format");
		}

		setReadFilter(outColorType, inColorType, inBit, inHasTransprancy);


		int width  = inWidth ;
		int height = inHeight;

		if (width <= 0 || height <= 0) {
			throw SGE_ERROR("png error : width <= 0 or height <= 0");
		}

		outImg.create(outColorType, width, height);

		Vector<png_byte*, 2048> rows;
		rows.reserve(height);

		for (int y = 0; y < height; y++) {
			rows.emplace_back( static_cast<png_byte*>(outImg.rowBytes(y).data()) );			
		}

		png_set_rows(m_png, m_info, rows.data());

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error : read pixels");
		}
		png_read_image(m_png, rows.data());
	}

}

