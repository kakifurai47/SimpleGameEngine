#pragma once

#include <sge_core/parser/Lexer.h>
#include <sge_render/shader/ShaderInfo.h>

namespace sge {

	class ShaderParser : public Lexer {
		using BlendFunc = RenderState::Blend::Function;
	public:
		void readFile(ShaderInfo& out, StrView fileName);
		void readMem (ShaderInfo& out, ByteSpan data, StrView fileName);
	private:
		using Attribute = ShaderInfo::Attr;
	
		void _readShad	(ShaderInfo& out, StrView src);

		void _readProperties		(ShaderInfo& out, StrView src);
		void _readAttribute			(Attribute&	 out, StrView src);
		void _readPropDefaultValue	(String&	 out, StrView src);

		void _readPass		(ShaderInfo& out, StrView src);
		void _readBlendFunc (BlendFunc&  out, StrView src);

		MemMapFile m_memMapFile;
	};
}