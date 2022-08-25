#pragma once

#include "../../Shader/Material.h"
#include "Shader_DX11.h"

namespace sge {	
	class RenderContext_DX11;

	class MaterialVertexStage_DX11 : public MaterialVertexStage {
		using This = MaterialVertexStage_DX11;
		using Base = MaterialVertexStage;
	public:
		This(ShaderStage* ss) : Base(ss) {}

		auto d3dShadStage() { return static_cast<Shader_DX11::VertexStage*>(m_shadStage); }

		void bind			(RenderContext_DX11* ctx, const VertexLayout* layout);
		void bindInputLayout(RenderContext_DX11* ctx, const VertexLayout* layout);

		void setConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
			dc->VSSetConstantBuffers1(bindPoint, 1, &d3dBuf, nullptr, nullptr); 
		}

		void _dxSetShaderResource(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DShaderResourceView* rv) {
			dc->VSSetShaderResources(bindPoint, 1, &rv);
		}

		void _dxSetSampler(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DSamplerState* ss) {
			dc->VSSetSamplers(bindPoint, 1, &ss);
		}


	private:
		VectorMap<const VertexLayout*, ComPtr<DX11_ID3DInputLayout>> m_inputLayoutMap;
	};

	class MaterialPixelStage_DX11 : public MaterialPixelStage {
		using This = MaterialPixelStage_DX11;
		using Base = MaterialPixelStage;
	public:
		This(ShaderStage* ss) : Base(ss) {}

		auto d3dShadStage() { return static_cast<Shader_DX11::PixelStage*>(m_shadStage); }

		void bind(RenderContext_DX11* ctx);

		void setConstBuffer(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DBuffer* d3dBuf) {
			dc->PSSetConstantBuffers1(bindPoint, 1, &d3dBuf, nullptr, nullptr);
		}

		void _dxSetShaderResource(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DShaderResourceView* rv) {
			dc->PSSetShaderResources(bindPoint, 1, &rv);
		}

		void _dxSetSampler(DX11_ID3DDeviceContext* dc, UINT bindPoint, DX11_ID3DSamplerState* ss) {
			dc->PSSetSamplers(bindPoint, 1, &ss);
		}

	};

	class MaterialPass_DX11 : public MaterialPass {
		using Base = MaterialPass;

		using VertexStage_DX11 = MaterialVertexStage_DX11;
		using  PixelStage_DX11 =  MaterialPixelStage_DX11;
	public:
		MaterialPass_DX11(Material* material, ShaderPass* shadPass);
		MaterialPass_DX11(MaterialPass_DX11&& other);

	protected:		
		virtual void onBind(RenderContext* ctx, const VertexLayout* layout) override;

		VertexStage_DX11  m_dx11VertexStage;
		PixelStage_DX11   m_dx11PixelStage;
	};

	class Material_DX11 : public Material {
	public:
		Shader_DX11* dx11Shad() { return static_cast<Shader_DX11*>(m_shader.ptr()); }
	protected:		
		virtual void onSetShader  (Span<ShaderPass*>  shadPasses) override;
		virtual void onResetPasses(Vector<Pass*, 1>&   outPasses) override;

		Vector<MaterialPass_DX11, 1> m_dx11Passes;
	};
}