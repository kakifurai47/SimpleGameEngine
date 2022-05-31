#pragma once

#include <sge_render.h>
#include <sge_render/backend/dx11/Render_DX11_Common.h>

namespace sge {

	class ShaderCompiler_DX11 : public NonCopyable {
		using Util = DX11Util;
	public:
		void compile(StrView outPath, ShaderStage stage, StrView srcFileName, StrView entryFunc);
		void reflect(StrView outPath, ShaderStage stage, ShaderInfo& outInfo);
	};

}
