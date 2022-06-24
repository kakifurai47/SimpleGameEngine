#pragma once
#include <sge_render/backend/base/RenderFormat.h>
#include "ShaderInfo.h"

namespace sge {
	class ShaderStage : public NonCopyable {
	public:
		using Info = ShaderStageInfo;
		using Mask = ShaderStageMask;

		const Info* info() const { return &m_stageInfo; }
	protected:
		Info	   m_stageInfo;
		Vector<u8> m_byteCode;
	};

	class ShaderVertexStage  : public ShaderStage {
	public:
		static constexpr Mask kStageMask() { return Mask::Vertex; }
	};
	class ShaderPixelStage	 : public ShaderStage  {
	public:
		static constexpr Mask kStageMask() { return Mask::Pixel;  }
	};

	class ShaderPass : public NonCopyable {
	public:
		using Info			= ShaderInfo::Pass;
		using StageMask		= ShaderStageMask;

		ShaderPass(Info& info, ShaderVertexStage& vtxStage,
							   ShaderPixelStage&  pxlStage);

		virtual ~ShaderPass() = default;

		ShaderVertexStage* vertexStage() { return m_vertexStage; }
		ShaderPixelStage*   pixelStage() { return  m_pixelStage; }
	protected:
		Info* m_passInfo = nullptr;

		ShaderVertexStage* m_vertexStage = nullptr;
		ShaderPixelStage*   m_pixelStage = nullptr;
	};

	class Shader : public RefCountBase {
	public:
		using Pass = ShaderPass;

		virtual ~Shader() = default;

		const ShaderInfo*		info  () const { return &m_info;	  }
			  Span<UPtr<Pass>>	passes()	   { return m_shadPasses; }

		void create(StrView filename);

	friend class ShaderPass;
	protected:
		virtual void onCreate(StrView compiledPath) = 0;

		ShaderInfo	m_info;
		String		m_filename;

		Vector_<UPtr<Pass>, 1> m_shadPasses;
	};
}
