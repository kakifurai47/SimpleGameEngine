
#include "Texture.h"

namespace sge {

	Texture2D::Texture2D(CreateDesc& desc) : Base(desc, Format::Texture2D)
		, m_samplerState(desc.samplerState)
		, m_size(desc.size)
	{
		if (desc.size.x == 0 || desc.size.y == 0) {
			throw SGE_ERROR("creating invalid size texture");
		}
	}

}

