#include "ShaderParser.h"

namespace sge {

	void ShaderParser::readFile(ShaderInfo& out, StrView fileName) {
		m_memMapFile.open(fileName);
		readMem(out, m_memMapFile, fileName);
	}
	
	void ShaderParser::readMem(ShaderInfo& out, ByteSpan data, StrView fileName) {
		_readShad(out, StrView_make(data));
	}
	
	void ShaderParser::_readShad(ShaderInfo& out, StrView src) {
		reset(src);
		ensureNext(src, TokenType::Idr, "Shader");
	
		if (tryNext (src, TokenType::Idr)) {
			//retrieve(out.name);
		}
		ensureNext(src, TokenType::Sep, "{");
		while (next(src)) {
		
			if(tokCheck(TokenType::Idr, "Properties")) { _readProps(out, src); continue; }
			if(tokCheck(TokenType::Idr, "Pass"))	   { _readPass (out, src); continue; }
			if(tokCheck(TokenType::Sep, "}"))		   { break; }
			throwUnexpected();
		}
	}
	
	void ShaderParser::_readPass(ShaderInfo& out, StrView src) {
		ensureNext(src, TokenType::Sep, "{");
		auto& pass = out.passes.emplace_back();
		while (next(src)) {
			if (tokCheck(TokenType::Idr, "VsFunc")) { rtrvNext(src, TokenType::Idr, pass.vsFunc); continue; }
			if (tokCheck(TokenType::Idr, "PsFunc")) { rtrvNext(src, TokenType::Idr, pass.psFunc); continue; }
			if (tokCheck(TokenType::Sep, "}"))		{ break; }
			throwUnexpected();
		}
	}
	
	void ShaderParser::_readProps(ShaderInfo& out, StrView src) {
		ensureNext(src, TokenType::Sep, "{");
		for (; ; ) {
			if (tryNext(src, TokenType::Sep, "}")) break;
			
			auto& p = out.props.emplace_back();
			if (tryNext(src, TokenType::Sep, "["))
				_readAtt(p.attribute, src);
			
			ensureNext(src, TokenType::Idr);
			if (!enumTryParse(p.propType, m_tok.value))
				throwUnexpected();
			
			rtrvNext(src, TokenType::Idr, p.name);
	
			if (tryNext(src, TokenType::Opr, "="))
				_readDefVal(p.defaultValue, src);
		}
	}
	
	void ShaderParser::_readAtt(Attribute& out, StrView src) {
		rtrvNext(src, TokenType::Idr, out.tag);
	
		if (tryNext (src, TokenType::Opr, "=")) {
			rtrvNext(src, TokenType::Lit, out.defaultValue);
		}
		ensureNext(src, TokenType::Sep, "]");
	}
	
	void ShaderParser::_readDefVal(String& out, StrView src) {
		if (tryNext(src, TokenType::Sep, "{")) {
			while (next(src)) {
				if (tokCheck(TokenType::Lit))	   { out += m_tok.value; continue; }
				if (tokCheck(TokenType::Sep, ",")) { out += m_tok.value; continue; }
				if (tokCheck(TokenType::Sep, "}")) { return; }
			}
		}
		rtrvNext(src, TokenType::Lit, out);
	}
}