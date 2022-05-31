#pragma once

#ifdef SGE_RENDER_COMP_DX11
#include "../base/Render_Common.h"
#include "../../vertex/Vertex.h"
#include "../../shader/Shader.h"

#include <d3d11.h>
#include <d3d11_4.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment	(lib, "dxguid.lib")


namespace sge
{
	class Renderer_DX11;

	using DX11_IDXGIFactory						= IDXGIFactory1;
	using DX11_IDXGIDevice						= IDXGIDevice;
	using DX11_IDXGIAdapter						= IDXGIAdapter3;
	using DX11_IDXGISwapChain					= IDXGISwapChain;
	
	using DX11_ID3DDevice						= ID3D11Device1;
	using DX11_ID3DDeviceContext				= ID3D11DeviceContext4;
	using DX11_ID3DDebug						= ID3D11Debug;
	
	using DX11_ID3DTexture1D					= ID3D11Texture1D;
	using DX11_ID3DTexture2D					= ID3D11Texture2D;
	using DX11_ID3DTexture3D					= ID3D11Texture3D;
	
	using DX11_ID3DRenderTargetView				= ID3D11RenderTargetView;
	using DX11_ID3DDepthStencilView				= ID3D11DepthStencilView;
	
	using DX11_ID3DBuffer						= ID3D11Buffer;
	using DX11_ID3DBlob							= ID3DBlob;
	
	using DX11_ID3DVertexShader					= ID3D11VertexShader;
	using DX11_ID3DPixelShader					= ID3D11PixelShader;
	using DX11_ID3DInputLayout					= ID3D11InputLayout;
	
	using DX11_ID3DRasterizerState				= ID3D11RasterizerState;
	using DX11_ID3DDepthStencilState			= ID3D11DepthStencilState;
	using DX11_ID3DBlendState					= ID3D11BlendState;

	using DX11_ID3DShaderReflection				= ID3D11ShaderReflection;
	using DX11_ShaderReflectionConstantBuffer	= ID3D11ShaderReflectionConstantBuffer;
	using DX11_ShaderReflectionVariable			= ID3D11ShaderReflectionVariable;
	using DX11_ShaderReflectionType				= ID3D11ShaderReflectionType;

	struct DX11Util {

		DX11Util() = delete;

		inline static
		const char* DX11Util::getDxStageProfile(ShaderStage s) {
			switch (s) {
			case ShaderStage::Vertex:	return "vs_5_0";
			case ShaderStage::Pixel:	return "ps_5_0";
			default: return "";
			}
		}

		inline static 
		ShaderPropType getPropType(D3D_SHADER_VARIABLE_TYPE type,
								   size_t rows, size_t cols) 
		{
			using SRC = D3D_SHADER_VARIABLE_TYPE;
			using DST = ShaderPropType;

			switch (type) {
			case SRC::D3D10_SVT_INT: {
				if (rows == 1 && cols == 1) return DST::Int;
			}
			case SRC::D3D_SVT_FLOAT: {
				if (rows == 1 && cols == 1) return DST::Float;
				if (rows == 1 && cols == 2) return DST::Vec2f;
				if (rows == 1 && cols == 3) return DST::Vec3f;
				if (rows == 1 && cols == 4) return DST::Vec4f;
				if (rows == 4 && cols == 4) return DST::Matrix;
			}
			case SRC::D3D_SVT_DOUBLE: {
				if (rows == 1 && cols == 1) return DST::Double;
			}
			default: throw SGE_ERROR(
				"unsupported D3D_SHADER_VARIABLE_TYPE: {} rows: {} cols:{}",
				type, rows, cols);
			}
		}

		static ByteSpan toSpan(ID3DBlob* blob) {
			if (!blob) return {};
			return ByteSpan(reinterpret_cast<const u8*>(blob->GetBufferPointer()),
							static_cast<size_t>(blob->GetBufferSize()));
		}
		static StrView  toStrView(ID3DBlob* blob) { return StrView_make(toSpan(blob)); }

		static void throwIfError(HRESULT hr) {
			if (FAILED(hr)) {
				SGE_ERROR("HRESULT = {}", hr);
			}
		}

		static void throwIfError(HRESULT hr, ID3DBlob* errorMsg) {
			if (FAILED(hr)) {
				throw SGE_ERROR("HRESULT={}\n Error Message: {}", hr, toStrView(errorMsg));
			}
		}


	};



}

#endif