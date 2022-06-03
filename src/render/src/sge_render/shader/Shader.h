#pragma once
#include <sge_render/backend/base/RenderFormat.h>
#include "ShaderInfo.h"


namespace sge {
	class Shader : public NonCopyable {
	public:
		void create(StrView filename);

		const ShaderInfo& info() const { return m_info; }

	protected:
		virtual void onCreate(StrView compiledPath) = 0;	
		ShaderInfo m_info;
	};
}

 