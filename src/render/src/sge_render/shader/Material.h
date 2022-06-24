#pragma once
#include "Shader.h"
#include "ShaderManager.h"
#include "../buffer/RenderGpuBuffer.h"

namespace sge {

	class RenderContext;
	class VertexLayout;
	class Material;

	class MaterialStage : public NonCopyable {
	public:
		MaterialStage::MaterialStage(ShaderStage* ss);

		struct ConstBuffer {
			using Info = ShaderStageInfo::ConstBuffer;

			Vector<u8>				cpuBuffer;
			SPtr<RenderGpuBuffer>	gpuBuffer;

			const Info* info() { return m_info; }
			void create(const Info& info);

		private:
			const Info* m_info	= nullptr;
			bool		m_dirty = false;
		};
	protected:
		ShaderStage* m_shadStage  = nullptr;
		Vector_<ConstBuffer, 4> m_constBufs;
	};

	class MaterialVertexStage : public MaterialStage  {
	public:
		using ShaderStage = ShaderVertexStage;
		MaterialVertexStage(ShaderVertexStage* ss) : MaterialStage(ss)
		{
		}
	};
	class MaterialPixelStage  : public MaterialStage {
	public:
		using ShaderStage = ShaderPixelStage;
		MaterialPixelStage (ShaderPixelStage* ss) : MaterialStage(ss) 
		{
		}
	};

	class MaterialPass : public RefCountBase {
	public:
		using VertexStage	= MaterialVertexStage;
		using PixelStage	= MaterialPixelStage;

		MaterialPass::MaterialPass(Material* material, ShaderPass* shadPass,
								VertexStage& vtxStage, PixelStage& pxlStage)
			: m_material(material), m_shadPass(shadPass)
			, m_vertexStage(&vtxStage)
			, m_pixelStage (&pxlStage)
		{
		}

		void bind(RenderContext* ctx, const VertexLayout* layout) { onBind(ctx, layout); }
	protected:
		virtual void onBind(RenderContext* ctx, const VertexLayout* layout) = 0;
		
		ShaderPass*  m_shadPass = nullptr;
		Material*	 m_material = nullptr;

		VertexStage* m_vertexStage = nullptr;
		PixelStage*  m_pixelStage  = nullptr;
	};

	class Material : public RefCountBase {
	public:
		using Pass = MaterialPass;

		Span<UPtr<Pass>> passes() { return m_matPasses; }
		void setShader (Shader* shad);
	protected:
		virtual Pass* onCreatePass(ShaderPass* pass) = 0;
		
		SPtr<Shader> m_shader;
		Vector_<UPtr<Pass>, 1> m_matPasses;
	
	private:
		template<class T>
		void _setParam(T&& value) {

		}

	};
}
