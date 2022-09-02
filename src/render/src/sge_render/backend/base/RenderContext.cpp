#include "RenderContext.h"
#include "Renderer.h"

namespace sge {

	void RenderContext::setFrameBufferSize(const Vec2f newSize)
	{
		if (newSize == m_frameBufferSize) {
			return;
		}

		m_frameBufferSize = newSize;


	}

}

