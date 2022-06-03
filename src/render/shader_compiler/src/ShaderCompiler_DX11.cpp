#include "ShaderCompiler_DX11.h"

namespace sge {

	void ShaderCompiler_DX11::compile(StrView outPath,	   ShaderStage stage,
									  StrView srcFileName, StrView entryFunc) {
		TempStringA entryPt = entryFunc;

		MemMapFile memmap;
		memmap.open(srcFileName);

		auto hlsl = memmap.span();

		UINT flags1 = 0;
		UINT flags2 = 0;

#if _DEBUG
		//flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr {};

		ComPtr<ID3DBlob>	byteCode;
		ComPtr<ID3DBlob>	errorMsg;

		auto profile = Util::getDxStageProfile(stage);

		hr = D3DCompile2(hlsl.data(), hlsl.size(), memmap.filename().c_str(),
						 nullptr, nullptr,
						 entryPt.c_str(),
						 profile,
						 flags1, flags2, 0, nullptr, 0,
						 byteCode.ptrForInit(),
						 errorMsg.ptrForInit());

		Util::throwIfError(hr, errorMsg);

		Directory::create(outPath);

		auto outFilename = Fmt("{}/{}.bin", outPath, profile);
		File::writeFile(outFilename, Util::toSpan(byteCode), false);
	}

	void ShaderCompiler_DX11::reflect(StrView outPath, ShaderStage stage, ShaderInfo& outInfo) {
		auto profile	 = Util::getDxStageProfile(stage);
		auto outFilename = Fmt("{}/{}.bin", outPath, profile);

		MemMapFile memmap;
		memmap.open(outFilename);
		auto byteCode = memmap.span();

		HRESULT hr{};
		ComPtr<DX11_ID3DShaderReflection> shadReflect;
		hr = D3DReflect(byteCode.data(),
						byteCode.size(),
						IID_ID3D11ShaderReflection,
						reinterpret_cast<void**>(shadReflect.ptrForInit()));
		Util::throwIfError(hr);

		D3D11_SHADER_DESC shadDesc{};
		hr = shadReflect->GetDesc(&shadDesc);
		Util::throwIfError(hr);

		for (size_t i = 0; i < shadDesc.ConstantBuffers; i++) {
			DX11_ShaderReflectionConstantBuffer* bufReflect = shadReflect->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC	 bufDesc {};
			D3D11_SHADER_INPUT_BIND_DESC bindDesc{};

			bufReflect->GetDesc(&bufDesc);
			hr = shadReflect->GetResourceBindingDesc(i, &bindDesc);
			Util::throwIfError(hr);

			auto& constBufInfo	 = outInfo.constBufInfos.emplace_back();
			constBufInfo.size	 = bufDesc.Size;
			constBufInfo.slotIdx = bindDesc.BindPoint;

			for (size_t j = 0; j < bufDesc.Variables; j++) {
				DX11_ShaderReflectionVariable*	varReflect	= bufReflect->GetVariableByIndex(j);
				DX11_ShaderReflectionType*		typeReflect = varReflect->GetType();

				D3D11_SHADER_TYPE_DESC		typeDesc{};
				D3D11_SHADER_VARIABLE_DESC	 varDesc{};

				varReflect->GetDesc(&varDesc);
				typeReflect->GetDesc(&typeDesc);

				ShaderInfo::Prop* prop = nullptr;

				for (auto& p : outInfo.properties) {
					if (p.name != varDesc.Name) continue;
					prop = &p;
					break;
				}
				if (!prop) {
					 prop = &outInfo.properties.emplace_back();
					 prop->name = varDesc.Name;
					 prop->type = Util::getPropType(typeDesc.Type,
								  typeDesc.Rows, typeDesc.Columns);
				}
				prop->isDefined = true;
				prop->offset	= varDesc.StartOffset;
				prop->slotIdx	= bindDesc.BindPoint;
			}
		}
	}
}

