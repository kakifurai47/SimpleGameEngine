#pragma once
#include "Shader.h"
#include "../buffer/RenderGpuBuffer.h"
#include "../texture/Texture.h"

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
		const V&  asValue() const {
			if (type != RenderFormatTypeUtil::get<V>()) {
				throw SGE_ERROR("invalid type");
			}
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

		struct TexParam {
			using FormatType = ShaderStageInfo::FormatType;
			using Info		 = ShaderStageInfo::Texture;

			void create(const Info& info) { m_info = &info; }

			Texture* getUpdatedTexture();

			StrView		name	  () const { return m_info->name;		}
			int			bindPoint () const { return m_info->bindPoint;	}
			FormatType  formatType() const { return m_info->formatType; }

			void setTexParam(Texture* tex);
		private:

			SPtr<Texture>		m_tex;
			const   Info*		m_info = nullptr;
		};

		const ShaderStageInfo* info() const { return m_shadStage->info(); }

		Span<ConstBuffer> constBufs()		{ return m_constBufs; }
		Span<TexParam>	  texParams()		{ return m_texParams; }
		
		void setParam	(StrView name, const Material_ValuePtr& valPtr);
		void setTexParam(StrView name, Texture* tex);

	protected:
		ShaderStage* m_shadStage  = nullptr;
		Vector<ConstBuffer, 4> m_constBufs;
		Vector<TexParam,	4> m_texParams;
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

		void bind(RenderContext* ctx, const VertexLayout* layout)   { onBind(ctx, layout); }

		const ShaderInfo::Pass* info() { return m_shadPass ? m_shadPass->info() : nullptr; }

		void setParam	(StrView name, const Material_ValuePtr& valPtr);
		void setTexParam(StrView name, Texture* tex);
	protected:
		virtual void onBind(RenderContext* ctx, const VertexLayout* layout) = 0;
		
		ShaderPass*  m_shadPass = nullptr;
		Material*	 m_material = nullptr;

		VertexStage* m_vertexStage = nullptr;
		PixelStage*  m_pixelStage  = nullptr;
	};

	class Material : public Object {
	public:
		SGE_TYPE_INFO(Material, Object);

		using Pass = MaterialPass;

		virtual ~Material() = default;

		Span<Pass*> passes() { return m_matPasses; }

		Pass* getPass(size_t index);

		void setShader(Shader* shad);

		void setParam(StrView name, const float&	 value) { _setParam   (name, value); }
		void setParam(StrView name, const Tuple2f&   value) { _setParam   (name, value); }
		void setParam(StrView name, const Tuple3f&   value) { _setParam   (name, value); }
		void setParam(StrView name, const Tuple4f&	 value) { _setParam   (name, value); }
		void setParam(StrView name, const Mat4f&	 value) { _setParam   (name, value); }
		void setParam(StrView name,		  Texture2D* value) { _setTexParam(name, value); }

	protected:
		void _setParam	 (StrView name, const Material_ValuePtr&  valPtr);
		void _setTexParam(StrView name,	Texture* tex);

		virtual void onSetShader  (Span<ShaderPass*>  shadPasses) = 0;
		virtual void onResetPasses(Vector<Pass*, 1>&   outPasses) = 0;	
		
		SPtr<Shader>	 m_shader;
		Vector<Pass*, 1> m_matPasses;
	};
}
