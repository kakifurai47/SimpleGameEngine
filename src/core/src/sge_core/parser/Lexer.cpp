#include "Lexer.h"
#include <functional>

namespace sge {

	const char* Lexer::_type2char(Type type) {
		switch (type) {
		case Type::Identifier:		return "Identifier";
		case Type::Separator:		return "Separator";
		case Type::Operator:		return "Operator";
		case Type::Literal:			return "Literal";
		case Type::Command:			return "Command";
		default:					return "None";
		}
	}

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
		m_token.value = { &src[m_offset], 0 };
	}

	void Lexer::_advance(StrView src, size_t len, bool consume) {
		SGE_ASSERT(m_offset + len <= src.size());
		m_offset += len;
		if (consume) {
			m_token.value = { m_token.value.begin(), m_token.value.size() + len };
		}
		else {
			_resetToken(src);
		}
	}

	template<class FUNC>
	bool sge::Lexer::_findMatch(StrView src, Type type, bool consume, FUNC&& startPred, size_t searchLen) {
		if (m_offset + searchLen > src.size() ||
					   m_offset == src.size()) {
			return false;
		}
		if (startPred(src, m_offset)) {
			m_token.type = type;
			_advance(src, searchLen, consume);
			return true;
		}
		return false;
	}

	template<class FUNC>
	bool sge::Lexer::_scanUntil(StrView src, bool skip, FUNC&& endPred, size_t searchLen) {
		size_t count = 0;
		for (;;) {
			auto o = m_offset + count;
			if (o + searchLen > src.size() ||
						   o == src.size()) {
				return false;
			}
			if (endPred(src, o)) {
				_advance(src, count, true);
				
				if (skip) {
					m_offset += searchLen;
				}
				return true;
			}
			count++;
		}
		SGE_ASSERT(false);
		return false;
	}

	void Lexer::retrieve(Type& outType, String& outVal) {
		outType = m_token.type;
		outVal  = m_token.value;
	}

	bool Lexer::nextToken(StrView src) {
		if (m_offset >= src.size()) {
			return false;
		}
		_resetToken(src);
		{//White space
			auto whitespace = [](StrView s, size_t i) { return _match(s[i], " \n\t");  };
			if (_findMatch(src, Type::None, true, whitespace, 1)) { _scanUntil(src, false, std::not_fn(whitespace), 1); }

			if (m_offset >= src.size()) {
				return false;
			}
			_resetToken(src);
		}
		{//Command
			auto doubleSlash   = [](StrView s, size_t i) { return s[i] == '/' && s[i + 1] == '/';  };
			auto lineFeed	   = [](StrView s, size_t i) { return s[i] == '\n'; };
			auto slashAsterisk = [](StrView s, size_t i) { return s[i] == '/' && s[i + 1] == '*';  };
			auto asteriskSlash = [](StrView s, size_t i) { return s[i] == '*' && s[i + 1] == '/';  };

			if (_findMatch(src, Type::Command, false, doubleSlash,	  2)) { return _scanUntil(src, true, lineFeed,	    1); }
			if (_findMatch(src, Type::Command, false, slashAsterisk,  2)) { return _scanUntil(src, true, asteriskSlash, 2); }
		}
		{//Literal
			auto doubleQuote = [](StrView s, size_t i) { return s[i] == '\"';	};
			auto digit		 = [](StrView s, size_t i) { return _isDigit(s[i]); };
			auto digicont	 = [](StrView s, size_t i) { return _isDigit(s[i]) || _match(s[i], ",."); };
			if (_findMatch(src, Type::Literal, false, doubleQuote, 1)) { return _scanUntil(src, true,  doubleQuote, 1);			  };
			if (_findMatch(src, Type::Literal, true,  digit,	   1)) { return _scanUntil(src, false, std::not_fn(digicont), 1); };
		}
		{//Identifiers
			auto alphabet = [](StrView s, size_t i) { return _isAlphabetic(s[i]); };
			auto idenSymb = [](StrView s, size_t i) { char c = s[i];    return _isAlphabetic(c) || _isDigit(c) || c == '_';	     };
			if (_findMatch(src, Type::Identifier, true, alphabet, 1)) { return _scanUntil(src, false, std::not_fn(idenSymb), 1); };
		}
		{//Separator
			auto separator = [](StrView s, size_t i) { return _match(s[i], "[](){};");  };
			if (_findMatch(src, Type::Separator, true, separator, 1)) { return true;	}
		}
		{//Operator
			auto op = [](StrView s, size_t i) { return _match(s[i], "+-*/%=:."); };
			if(_findMatch(src, Type::Operator, true, op, 1)) { return true;		 }
		}

		SGE_ASSERT(false);
		//m_offset++;
		//SGE_LOG("Lexer Warning: Leftover : {}", static_cast<u64>(src[m_offset]));
		return true;
	}
}
