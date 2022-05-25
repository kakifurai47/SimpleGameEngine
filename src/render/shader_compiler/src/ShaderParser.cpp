#include "ShaderParser.h"

namespace sge {

	void ShaderParser::readFile(ShaderInfo& out, StrView fileName) {
		m_memMapFile.open(fileName);
		readMem(out, m_memMapFile, fileName);
	}

	void ShaderParser::readMem(ShaderInfo& out, ByteSpan data, StrView fileName) {
		_read(out, StrView_make(data));
	}

	void ShaderParser::_read(ShaderInfo& out, StrView src) {
		reset(src);
		ensureNext(src, TokenType::Idr, "Shader");

		if (tryNext(src, TokenType::Idr)) {
			retrieve(out.name);
		}
		ensureNext(src, TokenType::Sep, "{");
		while (next(src)) {

			if(tokCheck(TokenType::Idr, "Properties")) { _parsePropsBLock(out, src); continue; }
			if(tokCheck(TokenType::Idr, "Pass"))	   { _parsePassBlock (out, src); continue; }
			if(tokCheck(TokenType::Idr, "}"))		   { break; }
			throwUnexpected();
		}
	}

	void ShaderParser::_parsePropAtt(ShaderInfo::Property& out, StrView src) {
		retrieveNext(src, TokenType::Idr, out.attTag);

		if (tryNext		(src, TokenType::Opr, "=")) {
			retrieveNext(src, TokenType::Lit, out.attDefval);
		}
		ensureNext(src, TokenType::Sep, "]");
	}

	void ShaderParser::_parseDefVal(String& out, StrView src) {
		if (tryNext(src, TokenType::Sep, "{")) {
			while (next(src)) {
				if (tokCheck(TokenType::Lit))	   { out.append(m_tok.value.data()); continue; }
				if (tokCheck(TokenType::Sep, ",")) { out.append(m_tok.value.data()); continue; }
				if (tokCheck(TokenType::Sep, "}")) { return; }
			}
		}
		retrieveNext(src, TokenType::Lit, out);
	}

	void ShaderParser::_parsePropsBLock(ShaderInfo& out, StrView src) {
		ensureNext(src, TokenType::Sep, "{");

		while (next(src)) {
			auto& prop = out.properties.emplace_back();
			if (tokCheck(TokenType::Sep, "[")) {
				_parsePropAtt(prop, src);
			}
			if (tokCheck(TokenType::Idr)) {
				retrieve(prop.dataType);
				retrieveNext(src, TokenType::Idr, prop.name);
				if ( tryNext(src, TokenType::Opr, "=")) {
					_parseDefVal(prop.defval, src);
					//Do data type validation here.
				}
			}
			if (tokCheck(TokenType::Sep, "}")) break;
			throwUnexpected();
		}
	}

	void ShaderParser::_parsePassBlock(ShaderInfo& out, StrView src) {
		ensureNext(src, TokenType::Sep, "{");
		auto& pass = out.passes.emplace_back();
		while (next(src)) {
			if (tokCheck(TokenType::Idr, "VsFunc")) { retrieveNext(src, TokenType::Idr, pass.vsFunc); continue; }
			if (tokCheck(TokenType::Idr, "PsFunc")) { retrieveNext(src, TokenType::Idr, pass.psFunc); continue; }
			if (tokCheck(TokenType::Sep, "}"))		{ break; }
			throwUnexpected();			
		}
	}
}