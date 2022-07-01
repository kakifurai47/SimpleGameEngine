#pragma once
#include "Shader.h"
#include "ShaderManager.h"
#include "../buffer/RenderGpuBuffer.h"

namespace sge {

	class RenderContext;
	class Material;

	struct VertexLayout;

	struct Material_ValuePtr {
		using Type = RenderFormatType;

		Material_ValuePtr() = default;

		template<class V>
		Material_ValuePtr(const V& v) 
			: type(RenderFormatTypeUtil::get<V>())
			, data(&v)
			, size(sizeof(V))
		{
		}

		template<class V>
		const V& asValue() const {
			if (type != RenderFormatTypeUtil::get<V>())
				throw SGE_ERROR("invalid type");
			return *reinterpret_cast<const V*>(data);
		}

		const void* data = nullptr;
		size_t		size = 0;
		Type		type = Type::None;
	};

	class MaterialStage : public NonCopyable {
	public:
		MaterialStage(ShaderStage* ss);
		MaterialStage(MaterialStage&& other);

		struct ConstBuffer {
			using Info = ShaderStageInfo::ConstBuffer;
			using VarInfo = ShaderStageInfo::Variable;

			Vector<u8>				cpuBuffer;
			SPtr<RenderGpuBuffer>	gpuBuffer;

			const Info* info() { return m_info; }
			void create(const Info& info);
			void uploadToGpu();

			void setParam(StrView name, const Material_ValuePtr& valPtr);
		private:

			template<class T>
			void _setParam(const VarInfo* varInfo, const T& value);

			const Info* m_info	= nullptr;
			bool		m_dirty = false;
		};

		void setParam(StrView name, const Material_ValuePtr& valPtr);

	protected:
		ShaderStage* m_shadStage  = nullptr;
		Vector_<ConstBuffer, 4> m_constBufs;
	};

	class MaterialVertexStage : public MaterialStage  {
	public:
		using ShaderStage = ShaderVertexStage;
		MaterialVertexStage(ShaderVertexStage* ss) : MaterialStage(ss) {}
	};
	class MaterialPixelStage  : public MaterialStage {
	public:
		using ShaderStage = ShaderPixelStage;
		MaterialPixelStage (ShaderPixelStage* ss) : MaterialStage(ss) {}
	};

	class MaterialPass : public NonCopyable {
	public:
		using VertexStage	= MaterialVertexStage;
		using PixelStage	= MaterialPixelStage;

		MaterialPass::MaterialPass(Material* material, ShaderPass* shadPass,
								VertexStage& vtxStage, PixelStage& pxlStage);

		virtual ~MaterialPass() = default;

		void bind(RenderContext* ctx, const VertexLayout* layout) { onBind(ctx, layout); }
		void setParam(StrView name, const Material_ValuePtr& valPtr);
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

		virtual ~Material() = default;

		Span<Pass*> passes() { return m_matPasses; }

		void setShader(Shader* shad);
		void setParam (StrView name, const Material_ValuePtr& valPtr);

	protected:
		virtual void onSetShader  (Span<ShaderPass*>  shadPasses) = 0;
		virtual void onResetPasses(Vector_<Pass*, 1>&  outPasses) = 0;	
		
		SPtr<Shader> m_shader;
		Vector_<Pass*, 1> m_matPasses;
	};
}
