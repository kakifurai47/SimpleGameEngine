#include <sge_core/parser/Lexer.h>
#include <sge_render/shader/ShaderInfo.h>

namespace sge {

	class ShaderParser : public Lexer {
	public:
		void readFile(ShaderInfo& out, StrView fileName);
		void readMem (ShaderInfo& out, ByteSpan data, StrView fileName);
	private:
		void _read(ShaderInfo& out, StrView src);
		
		void _parseDefVal	  (String&				 out, StrView src);
		void _parsePropsBLock (ShaderInfo&			 out, StrView src);
		void _parsePropAtt	  (ShaderInfo::Property& out, StrView src);
		void _parsePassBlock  (ShaderInfo&			 out, StrView src);

		MemMapFile m_memMapFile;
	};
}