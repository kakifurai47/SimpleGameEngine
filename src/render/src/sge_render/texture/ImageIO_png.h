#pragma once

#include <libpng16/png.h>
#include <sge_core/graph/Color.h>

namespace sge {

	class  Image;
	struct ImageIO_png : public NonCopyable {

		
		class Reader {
		public:
			~Reader();

			void load(ByteSpan& src, Image& outImg, ColorType expectType = ColorType::None);

			static void s_onRead(png_structp png_ptr, png_bytep dst, png_size_t len);
			void onRead(png_bytep dst, png_size_t len);

			void setReadFilter(ColorType outType, int inType, int inBit, bool inHasTransprancy);

		private:

			bool error_longjmp_restore_point();

			png_struct* m_png = nullptr;
			png_info*  m_info = nullptr;

			ByteSpan	m_data;
			const u8*	m_readPtr = nullptr;
		};
	};




}