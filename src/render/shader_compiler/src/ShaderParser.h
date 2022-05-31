#pragma once

#include <sge_core/parser/Lexer.h>
#include <sge_render/shader/ShaderInfo.h>

namespace sge {

	class ShaderParser : public Lexer {
	public:
		void readFile(ShaderInfo& out, StrView fileName);
		void readMem (ShaderInfo& out, ByteSpan data, StrView fileName);
	private:
		using Attribute = ShaderInfo::Attr;
	
		void _readShad (ShaderInfo& out, StrView src);
		
		void _readPass (ShaderInfo& out, StrView src);
		void _readProps(ShaderInfo& out, StrView src);
	
		void _readAtt	(Attribute&	 out, StrView src);
		void _readDefVal(String&	 out, StrView src);
	
		MemMapFile m_memMapFile;
	};
}