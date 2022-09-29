#include "ShaderCompiler_DX11.h"
#include <sge_core/serializer/json/JsonUtil.h>

namespace sge {

	void ShaderCompiler_DX11::compile(StrView outPath, ShaderStageMask stage,
									  StrView srcFileName, StrView entryFunc) 
	{
		TempStringA entryPt = entryFunc;

		MemMapFile mm;
		mm.open(srcFileName);

		auto hlsl = mm.span();

		UINT flags1 = 0;
		UINT flags2 = 0;

#if _DEBUG
		//flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr {};

		ComPtr<ID3DBlob>	bytecode;
		ComPtr<ID3DBlob>	errorMsg;

		auto profile = Util::getDxStageProfile(stage);

		hr = D3DCompile2(	hlsl.data(), hlsl.size(), mm.filename().c_str(),
							nullptr, nullptr,
							entryPt.c_str(),
							profile,
							flags1, flags2, 0, nullptr, 0,
							bytecode.ptrForInit(),
							errorMsg.ptrForInit());
		Util::throwIfError(hr, errorMsg);

		Directory::create(outPath);

		auto bytecodeSpan = Util::toSpan(bytecode);
		auto outFilename  = Fmt("{}/{}.bin", outPath, profile);
		File::writeFileIfChanged(outFilename, bytecodeSpan, false);

		_reflect(outFilename, bytecodeSpan, stage, profile);
	}

	void ShaderCompiler_DX11::_reflect(StrView outFilename, ByteSpan bytecode,
									   ShaderStageMask stage, StrView profile)
	{
		HRESULT hr {};
		ComPtr<DX11_ID3DShaderReflection> reflect;
		hr = D3DReflect(bytecode.data(), bytecode.size(), IID_PPV_ARGS(reflect.ptrForInit()));
		Util::throwIfError(hr);

		D3D11_SHADER_DESC desc {};
		hr = reflect->GetDesc(&desc);
		Util::throwIfError(hr);

		ShaderStageInfo outInfo;
		outInfo.profile = profile;
		outInfo.stage	= stage;

		{
			_reflect_inputs			(outInfo, reflect, desc);
			_reflect_constBuffers	(outInfo, reflect, desc);
			_reflect_textures		(outInfo, reflect, desc);
			_reflect_samplers		(outInfo, reflect, desc);
		}

		auto jsonFilename = Fmt("{}.json", outFilename);
		JsonUtil::writeFileIfChanged(jsonFilename, outInfo, false);
	}

	void ShaderCompiler_DX11::_reflect_inputs(ShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc)
	{
		HRESULT hr {};
		outInfo.inputs.reserve(desc.InputParameters);

		for (UINT i = 0; i < desc.InputParameters; i++) {
			auto& outInputs = outInfo.inputs.emplace_back();

			D3D11_SIGNATURE_PARAMETER_DESC paramDesc {};
			hr = reflect->GetInputParameterDesc(i, &paramDesc);
			Util::throwIfError(hr);

			VertexSemanticType semanticType;
			semanticType = Util::parseDxSemanticName(paramDesc.SemanticName);
			
			outInputs.semantic = VertexSemanticUtil::make(semanticType, static_cast<VertexSemanticIndex>(paramDesc.SemanticIndex));

			TempString formatType;

			switch (paramDesc.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32:		formatType.append("UInt08");	break;
				case D3D_REGISTER_COMPONENT_SINT32:		formatType.append("Int32");		break;
				case D3D_REGISTER_COMPONENT_FLOAT32:	formatType.append("Float32");	break;
				default: throw SGE_ERROR("unspported component type");
			}

			auto compCount = BitUtil::count1(paramDesc.Mask);
			if (compCount < 1 || compCount > 4) {
				throw SGE_ERROR("invalid componenet Count");
			}

			FmtTo(formatType, "x{}", compCount);

			if (!enumTryParse(outInputs.formatType, formatType)) {
				throw SGE_ERROR("cannot parse Render FormatType enum");
			}
		}
	}

	void ShaderCompiler_DX11::_reflect_constBuffers(ShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc)
	{
		HRESULT hr {};
		outInfo.constBuffers.reserve(desc.BoundResources);

		for (UINT i = 0; i < desc.BoundResources; i++) {
			D3D11_SHADER_INPUT_BIND_DESC resDesc {};
			hr = reflect->GetResourceBindingDesc(i, &resDesc);

			if(resDesc.Type != D3D_SIT_CBUFFER) continue;

			auto& outCB = outInfo.constBuffers.emplace_back();

			D3D11_SHADER_BUFFER_DESC bufDesc {};
			auto* cb = reflect->GetConstantBufferByName(resDesc.Name);
			hr = cb->GetDesc(&bufDesc);
			Util::throwIfError(hr);

			outCB.name		= bufDesc.Name;
			outCB.bindPoint = static_cast<i16>(resDesc.BindPoint);
			outCB.bindCount = static_cast<i16>(resDesc.BindCount);
			outCB.dataSize	= bufDesc.Size;

			{
				outCB.variables.reserve(bufDesc.Variables);
				for (UINT j = 0; j < bufDesc.Variables; j++) {
					auto* cbv = cb->GetVariableByIndex(j);
					D3D11_SHADER_VARIABLE_DESC varDesc {};
					hr = cbv->GetDesc(&varDesc);
					Util::throwIfError(hr);

					D3D11_SHADER_TYPE_DESC varType {};
					hr = cbv->GetType()->GetDesc(&varType);
					Util::throwIfError(hr);

					if (0 == (varDesc.uFlags & D3D_SVF_USED)) continue;

					auto& outVar  = outCB.variables.emplace_back();
					outVar.name   = varDesc.Name;
					outVar.offset = varDesc.StartOffset;
					
					TempString  formatType;
					switch (varType.Type) {
					case D3D_SVT_BOOL:		formatType.append("Bool");		break;
					case D3D_SVT_INT:		formatType.append("Int32");		break;
					case D3D_SVT_UINT8:		formatType.append("UInt08");	break;
					case D3D_SVT_UINT:		formatType.append("UInt16");	break;
					case D3D_SVT_FLOAT:		formatType.append("Float32");	break;
					case D3D_SVT_DOUBLE:	formatType.append("Float64");	break;
						default: throw SGE_ERROR("unsupported type");
					}
					switch (varType.Class) {
					
					case D3D_SVC_SCALAR:		 FmtTo(formatType, "x{}",				   varType.Columns); break;
					case D3D_SVC_VECTOR:		 FmtTo(formatType, "x{}",				   varType.Columns); break;
					case D3D_SVC_MATRIX_COLUMNS: FmtTo(formatType, "_{}x{}", varType.Rows, varType.Columns); break;
					case D3D_SVC_MATRIX_ROWS:	 FmtTo(formatType, "_{}x{}", varType.Rows, varType.Columns); break;
					default: throw SGE_ERROR("unsupported class");
					}

					if (!enumTryParse(outVar.formatType, formatType)) {
						throw SGE_ERROR("cannot parse format type");
					}

					if (outVar.formatType == FormatType::None) {
						throw SGE_ERROR("format type is None");
					}
				}
			}
		}
	}

	void ShaderCompiler_DX11::_reflect_textures(ShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
		HRESULT hr {};
		outInfo.textures.reserve(desc.BoundResources);

		for (UINT i = 0; i < desc.BoundResources; i++) {
			D3D11_SHADER_INPUT_BIND_DESC resDesc {};
			hr = reflect->GetResourceBindingDesc(i, &resDesc);

			if (resDesc.Type != D3D_SIT_TEXTURE) {
				continue;
			}

			auto& outTex = outInfo.textures.emplace_back();
			outTex.name = resDesc.Name;
			outTex.bindPoint = static_cast<i16>(resDesc.BindPoint);
			outTex.bindCount = static_cast<i16>(resDesc.BindCount);
			
			switch (resDesc.Dimension) {
				using RFT = RenderFormatType;				
				case D3D_SRV_DIMENSION_TEXTURE1D		: outTex.formatType = RFT::Texture1D;			break;
				case D3D_SRV_DIMENSION_TEXTURE2D		: outTex.formatType = RFT::Texture2D;			break;
				case D3D_SRV_DIMENSION_TEXTURE3D		: outTex.formatType = RFT::Texture3D;			break;
				case D3D_SRV_DIMENSION_TEXTURECUBE		: outTex.formatType = RFT::TextureCube;			break;

				case D3D_SRV_DIMENSION_TEXTURE1DARRAY	: outTex.formatType = RFT::Texture1DArray;		break;
				case D3D_SRV_DIMENSION_TEXTURE2DARRAY	: outTex.formatType = RFT::Texture2DArray;		break;
				case D3D_SRV_DIMENSION_TEXTURECUBEARRAY	: outTex.formatType = RFT::TextureCubeArray;	break;			

				case D3D_SRV_DIMENSION_TEXTURE2DMS		: outTex.formatType = RFT::Texture2DMS;			break;
				case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY	: outTex.formatType = RFT::Texture2DMSArray;	break;

				default: throw SGE_ERROR("unsupported texture dimension");
			}
		}
	}

	void ShaderCompiler_DX11::_reflect_samplers(ShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
		HRESULT hr {};
		outInfo.samplers.reserve(desc.BoundResources);

		for (UINT i = 0; i < desc.BoundResources; i++) {
			D3D11_SHADER_INPUT_BIND_DESC resDesc {};
			hr = reflect->GetResourceBindingDesc(i, &resDesc);

			if (resDesc.Type != D3D_SIT_SAMPLER) {
				continue;
			}

			auto& outSampler = outInfo.samplers.emplace_back();
			outSampler.name		 = resDesc.Name;
			outSampler.bindPoint = static_cast<i16>(resDesc.BindPoint);
			outSampler.bindCount = static_cast<i16>(resDesc.BindCount);
		}
	}
}

