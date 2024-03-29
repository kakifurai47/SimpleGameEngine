#pragma once
#include <sge_render/backend/base/RenderFormat.h>
#include "ShaderInfo.h"

namespace sge {
	class ShaderStage : public NonCopyable {
	public:
		using Info = ShaderStageInfo;
		using Mask = ShaderStageMask;

		ShaderStage() = default;
		ShaderStage(ShaderStage&& other) noexcept;

		const Info*		info	() const { return &m_stageInfo; }
			  ByteSpan	byteCode() const { return m_byteCode;	}

	protected:
		Info	   m_stageInfo;
		Vector<u8>  m_byteCode;
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

		ShaderPass(Info* info, ShaderVertexStage& vtxStage,
							   ShaderPixelStage&  pxlStage);

		virtual ~ShaderPass() = default;

		ShaderVertexStage* vertexStage() { return m_vertexStage; }
		ShaderPixelStage*   pixelStage() { return  m_pixelStage; }

		const Info*				  info() { return m_passInfo;	 }

	protected:
		Info* m_passInfo = nullptr;

		ShaderVertexStage* m_vertexStage = nullptr;
		ShaderPixelStage*   m_pixelStage = nullptr;
	};

	class Shader : public Object {
	public:
		SGE_TYPE_INFO(Shader, Object)

		using Pass = ShaderPass;

		virtual ~Shader() = default;

		Shader(StrView filename);


		const ShaderInfo*	info  () const { return &m_info;	  }
			  Span<Pass*>	passes()	   { return m_shadPasses; }

	friend class ShaderPass;
	protected:

		ShaderInfo	m_info;
		String		m_filename;

		Vector<Pass*, 1> m_shadPasses;
	};
}
