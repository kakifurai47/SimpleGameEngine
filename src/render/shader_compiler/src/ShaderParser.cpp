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

		next(src);
		expect(TokenType::Identifier, "Shader");
	
		if (tryNext (src, TokenType::Identifier)) {
			//retrieve(out.name);
		}

		next(src);
		expect(TokenType::Separator, "{");

		while (next(src)) {
			if (isIdentifier("Properties"))  { next(src); _readProperties(out, src); continue; }
			if (isIdentifier("Pass"))		 { next(src); _readPass		 (out, src); continue; }
			if (isSeparator("}"))			 { break; }
			throwUnexpected();
		}
	}
	
	void ShaderParser::_readPass(ShaderInfo& out, StrView src) {
		expect(TokenType::Separator, "{");
		auto& pass = out.passes.emplace_back();
		while (next(src)) 
		{
			if (isIdentifier("Cull"))		{ next(src); readIdentifier(pass.renderState.cullType);				continue; }

			if (isIdentifier("BlendRGB"))	{ next(src); _readBlendFunc(pass.renderState.blend.rgbFunc, src);	continue; }
			if (isIdentifier("BlendAlpha")) { next(src); _readBlendFunc(pass.renderState.blend.rgbFunc, src);	continue; }
			
			if (isIdentifier("DepthTest"))	{ next(src); readIdentifier(pass.renderState.depthTest.op);			continue; }
			if (isIdentifier("DepthWrite")) { next(src); readIdentifier(pass.renderState.depthTest.writeMask);	continue; }

			if (isIdentifier("VsFunc"))		{ next(src); readIdentifier(pass.vsFunc);							continue; }
			if (isIdentifier("PsFunc"))		{ next(src); readIdentifier(pass.psFunc);							continue; }
			if (isSeparator ("}"))			{ break; }

			throwUnexpected();
		}
	}
	
	void ShaderParser::_readBlendFunc(BlendFunc& out, StrView src) {
		readIdentifier(out.op);
		next(src); readIdentifier(out.srcFactor);
		next(src); readIdentifier(out.dstFactor);
	}

	void ShaderParser::_readProperties(ShaderInfo& out, StrView src) {
		expect(TokenType::Separator, "{");
		for (; ; ) {
			if (tryNext(src, TokenType::Separator, "}")) break;
			
			auto& p = out.props.emplace_back();

			if (tryNext(src, TokenType::Separator, "[")) {
				next(src);
				_readAttribute(p.attribute, src); 
			}

			next(src); readIdentifier(p.propType);
			next(src); readIdentifier(p.name);

			if (tryNext(src, TokenType::Operator, "=")) {
				next(src);
				_readPropDefaultValue(p.defaultValue, src);
			}
		}
	}
	
	void ShaderParser::_readAttribute(Attribute& out, StrView src) {
		readIdentifier(out.tag);
		if (tryNext(src, TokenType::Operator, "=")) {
			next(src);
			readLiteral(out.defaultValue);
		}
		next(src);
		expect(TokenType::Separator, "]");
	}
	
	void ShaderParser::_readPropDefaultValue(String& out, StrView src) {
		if (auto isArrayType = isSeparator("{")) {
			while (next(src)) {
				if (isLiteral())		{ out += m_tok.value; continue; }
				if (isSeparator(","))	{ out += m_tok.value; continue; }
				if (isSeparator("}"))	{ return; }
			}			
		}
		readLiteral(out);
	}
}