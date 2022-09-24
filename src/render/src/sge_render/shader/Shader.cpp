#include <sge_core/serializer/json/JsonUtil.h>
#include "Shader.h"

namespace sge {

	ShaderStage::ShaderStage(ShaderStage&& other) noexcept
		: m_stageInfo(std::move(other.m_stageInfo))
		, m_byteCode (std::move( other.m_byteCode))
	{
	}

	ShaderPass::ShaderPass(Info* info, ShaderVertexStage& vtxStage,
									   ShaderPixelStage&  pxlStage)
		: m_passInfo(info), m_vertexStage(&vtxStage)
						  , m_pixelStage (&pxlStage)
	{
	}

	Shader::Shader(StrView filename)
		: m_filename(filename)
	{
		auto importPath = ProjectSettings::instance()->importedPath();

		auto compiledPath = Fmt("{}/{}", importPath, filename);
		auto infoFilename = Fmt("{}/info.json",  compiledPath);

		JsonUtil::readFile(infoFilename, m_info);
	}
}
