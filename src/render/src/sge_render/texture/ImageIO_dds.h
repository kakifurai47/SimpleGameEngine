#pragma once

#include <sge_core/graph/Color.h>

namespace sge {

	class  Image;
	struct ImageIO_dds : public NonCopyable {

		class Reader {
		public:
			void load(ByteSpan& src, Image& out, ColorType colorType = ColorType::None);

		private:

		};
	

	};


}