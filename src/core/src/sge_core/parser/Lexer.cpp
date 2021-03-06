#include "Lexer.h"
#include <functional>

namespace sge {
	bool _match(char v, StrView match) {
		for (auto i : match) { if (i == v) return true; }
		return false;
	}

	bool _isAlphabetic(char v) {
		if (v >= 'a' && v <= 'z') return true;
		if (v >= 'A' && v <= 'Z') return true;
		return false;
	}

	bool _isDigit(char v) {
		if (v >= '0' && v <= '9') return true;
		return false;
	}

	void Lexer::_resetToken(StrView src) {
		m_tok.value = { &src[m_off], 0 };
	}

	void Lexer::_advance(StrView src, size_t len, bool consume) {
		SGE_ASSERT(m_off + len <= src.size());

		for (size_t i = 0; i < len; i++) { //<== temp
			if (src[m_off + i] != '\n') continue;
			m_line++;
		}

		m_off += len;
		if (consume) {
			m_tok.value = { m_tok.value.begin(), m_tok.value.size() + len };
		}
		else {
			_resetToken(src);
		}
	}

	template<class FUNC>
	bool sge::Lexer::_findMatch(StrView src, TokenType type, bool consume, FUNC&& startPred, size_t searchLen) {
		if (m_off + searchLen >= src.size() ||
					    m_off == src.size()) {
			return false;
		}
		if (startPred(src, m_off)) {
			m_tok.type = type;
			_advance(src, searchLen, consume);
			return true;
		}
		return false;
	}

	template<class FUNC>
	bool sge::Lexer::_scanUntil(StrView src, bool skip, FUNC&& endPred, size_t searchLen) {
		size_t count = 0;
		for (;;) {
			auto o = m_off + count;
			if (o + searchLen >= src.size()	||
					    m_off == src.size()) {
				return false;
			}
			if (endPred(src, o)) {
				_advance(src, count, true);
				
				if (skip) {
					m_off += searchLen;
				}

				return true;
			}
			count++;
		}
	}



	void Lexer::throwUnexpected() {
		throw SGE_ERROR( "unexpected token: type:[{}], value: [{}]", m_tok.type, m_tok.value);
	}

	void Lexer::_readString(String& out) {
		out = m_tok.value;
	}

	void Lexer::_readBool(bool& out) {
		if (m_tok.value.compare("true" )) { out = true;  return; }
		if (m_tok.value.compare("false")) { out = false; return; }
		throwUnexpected();
	}

	bool Lexer::tryNext(StrView src, TokenType type, StrView val) {
		auto tok = m_tok;
		auto off = m_off;
		next(src);

		if (checkToken(type, val)) 
			return true;
				
		m_tok = tok;
		m_off = off;
		return false;
	}

	StrView Lexer::getRemainSource() const {
		if (!m_src[m_off]) return {};
		return { &m_src[m_off], m_src.size() - m_off };
	}

	void Lexer::reset(StrView src) {
		if (src == nullptr) {
			throw SGE_ERROR("Lexer: reading nothing");
		}
		m_off = 0;
		m_line = 0;
		m_src = src;
		_resetToken(src);
	}

	bool Lexer::next(StrView src) {
		if (m_off >= src.size()) {
			return false;
		}
		_resetToken(src);

		for (;;) {
			{//White space
				auto whitespace = [](StrView s, size_t i) { return _match(s[i], " \n\t\r");  };
				if (_findMatch(src, TokenType::WhiteSpace, true, whitespace, 1)) { _scanUntil(src, false, std::not_fn(whitespace), 1); }

				if (m_off >= src.size()) {
					return false;
				}
				_resetToken(src);
			}

			auto parseCmd = false;
			{//Command				
				auto pound		   = [](StrView s, size_t i) { return s[i] == '#';	};
				auto doubleSlash   = [](StrView s, size_t i) { return s[i] == '/' && s[i + 1] == '/';  };
				auto lineFeed	   = [](StrView s, size_t i) { return s[i] == '\n';	};
				auto slashAsterisk = [](StrView s, size_t i) { return s[i] == '/' && s[i + 1] == '*';  };
				auto asteriskSlash = [](StrView s, size_t i) { return s[i] == '*' && s[i + 1] == '/';  };

				if (_findMatch(src, TokenType::Command, false, doubleSlash,	   2)) { _scanUntil(src, true, lineFeed,	  1); parseCmd = true; }
				if (_findMatch(src, TokenType::Command, false, slashAsterisk,  2)) { _scanUntil(src, true, asteriskSlash, 2); parseCmd = true; }
				if (_findMatch(src, TokenType::Command, false, pound,		   1)) { _scanUntil(src, true, lineFeed,	  1); parseCmd = true; }
			}
			if (!parseCmd) {
				break;
			}
		}
		{//Literal
			auto doubleQuote = [](StrView s, size_t i) { return s[i] == '\"';	};
			auto digit		 = [](StrView s, size_t i) { return _isDigit(s[i]); };
			auto digicont	 = [](StrView s, size_t i) { return _isDigit(s[i]) || _match(s[i], "."); };

			if (_findMatch(src, TokenType::Literal, false, doubleQuote, 1)) { return _scanUntil(src, true,  doubleQuote, 1);		   };
			if (_findMatch(src, TokenType::Literal, true,  digit,	    1)) { return _scanUntil(src, false, std::not_fn(digicont), 1); };
		}
		{//Identifiers
			auto alphabet = [](StrView s, size_t i) { return _isAlphabetic(s[i]); };
			auto idenSymb = [](StrView s, size_t i) { char c = s[i];  return _isAlphabetic(c) || _isDigit(c) || c == '_';	   };
			if (_findMatch(src, TokenType::Identifier, true, alphabet, 1)) { return _scanUntil(src, false, std::not_fn(idenSymb), 1); };
		}
		{//Separator
			auto separator = [](StrView s, size_t i) { return _match(s[i], "[](){};,");	};
			if (_findMatch(src, TokenType::Separator, true, separator, 1)) { return true;		}
		}
		{//Operator
			auto op = [](StrView s, size_t i) { return _match(s[i], "+-*/%=:."); };
			if(_findMatch(src, TokenType::Operator, true, op, 1)) { return true;		 }
		}

		SGE_ASSERT(false);
		//SGE_LOG("Lexer Warning: Leftover : {}", static_cast<u64>(src[m_offset]));
		//m_offset++;
		return true;
	}
}
