#pragma once
#include <sge_render/backend/base/RenderFormat.h>
#include "ShaderInfo.h"

namespace sge {

	class Shader : public NonCopyable {
	using RenderType = Render_FormatType;
	public:
		//struct Prop {
		//	RenderType	type = RenderType::None;
		//	size_t		slotIdx = 0;
		//	size_t		offset	= 0;
		//	String		name	 {};
		//};
		//
		//Vector_<Prop, 8> properties;
	};
}

