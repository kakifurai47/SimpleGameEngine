#include "Lexer2.h"

namespace sge {

	void Lexer2::reset(ByteSpan src_, StrView filename) {
		reset(StrView_make(src_), filename);
	}

	void Lexer2::reset(StrView src_, StrView filename) {
		m_filename = filename;
		m_source = src_;
		
		m_cur  = m_source.data();
		m_ch   = 0;
		m_col  = 0;
		m_line = 1;

		nextChar();
		nextToken();
	}

	bool Lexer2::nextChar() {
		m_ch = 0;

		if (!m_cur) return false;
		if (m_cur >= m_source.end()) return false;
		
		m_ch = *m_cur;
		m_cur++;
		m_col++;

		if (m_ch == '\n') {
			m_line++;
			m_col = 0;
		}
		return true;
	}

	bool Lexer2::nextToken() {
		m_token.setNone();
		
		for (;;) {
			trimSpaces();
			if (!m_ch) return false;

			if (m_ch == '#') {
				_parseCommentSingleLine();
				continue;
			}

			if (m_ch == '_' || isAlpha(m_ch)) {
				_parseIdentifier();
				return true;
			}

			if (isDigit(m_ch)) return _parseNumber();
			if (m_ch == '\"')  return _parseString();

			if (m_ch == '\n') {
				m_token.type = TokenType::NewLine;
				m_token.str += "<newline>";
				nextChar();
				return true;
			}

			if (m_ch == '/') {
				nextChar();

				if (m_ch == '*') {
					_parseCommentBlock();
					continue;
				}
				if (m_ch == '/') {
					_parseCommentSingleLine();
					continue;
				}

				m_token.type = TokenType::Operator;
				m_token.str = '/';
				return true;
			}

			m_token.type = TokenType::Operator;
			m_token.str += m_ch;
			nextChar();
			return true;
		}
		return false;
	}

	void Lexer2::trimSpaces() {
		for(;;) {
			if (m_ch == ' ' || m_ch == '\t' || m_ch == '\r') {
				nextChar();
			}
			else {
				break;
			}
		}
	}

	void Lexer2::skipNewlineTokens() {
		while (m_token.isNewLine()) {
			nextToken();
		}
	}

	void Lexer2::expectOperator(StrView view) {
		if (!m_token.isOperator(view)) errorUnexpectedToken();
		nextToken();
	}

	void Lexer2::errorUnexpectedChar() {
		error("Unexpected character [{}]", m_ch);
	}

	void Lexer2::errorUnexpectedToken() {
		error("Unexpected token [{}]", m_token.str);
	}

	StrView Lexer2::getLastFewLines(size_t lineCount) {
		if (!m_cur) return {};

		auto n = lineCount;

		auto* start = m_source.begin();
		auto* end   = m_source.end();

		auto* pStart = m_cur;
		auto* pEnd   = m_cur;

		for (;;) {
			pEnd++;
			if (pEnd >= end || *pEnd == '\n') {
				break;
			}
		}		
		for (; pStart >= start; pStart--) {
			if (*pStart == '\n') {
				n--;
			}
			if (n <= 0) break;
		}
		pStart++;
		return StrView(pStart, pEnd - pStart);
	}


	void Lexer2::_error(StrView msg) {
		TempString tmp = msg;
		FmtTo(tmp, "\n{}\n", getLastFewLines(3)); 
		{
			TempString lastLine = getLastFewLines(1);
			size_t i = 0;
			for (auto& c : lastLine) {
				if (i >= m_col - 1) break;
				if (c == ' ')  { tmp += '-';		i++; continue; }
				if (c == '\t') { tmp += "--------"; i++; continue; }
				tmp += '-';
				i++;
			}
			tmp += "^\n";
		}
		FmtTo(tmp, "\nfilename	= [{}]\ntoken		= [{}]\nline:col	= [{}:{}]\n", m_filename, m_token.str, m_line, m_col);
		throw SGE_ERROR("{}", tmp);
	}

	void Lexer2::_parseCommentBlock() {
		nextChar();
		for (;;) {
			if (!m_ch) return;
			if (m_ch == '*') {
				nextChar();
				if (m_ch == '/') {
					nextChar();
					return;
				}
			}
			else {
				nextChar();
			}
		}
	}

	void Lexer2::_parseCommentSingleLine() {
		nextChar();
		for (;;) {
			if (!m_ch) return;
			if  (m_ch == '\n') {
				nextChar();
				return;
			}
			else {
				nextChar();
			}
		}
	}

	bool Lexer2::_parseIdentifier() {
		m_token.type = TokenType::Identifier;

		m_token.str += m_ch;
		nextChar();

		while (m_ch) {
			if (m_ch == '_' || isAlpha(m_ch) || isDigit(m_ch)) {
				m_token.str += m_ch;
				nextChar();
			}
			else {
				break;
			}
		}
		return true;
	}

	bool Lexer2::_parseNumber() {
		m_token.type = TokenType::Number;
		m_token.str += m_ch;
		nextChar();

		bool hasExponent = false;
		bool hasDot = false;

		while (m_ch) {
			if (m_ch == '.') {
				if (hasDot || hasExponent) {
					errorUnexpectedChar();
				}
				hasDot = true;
				m_token.str += m_ch;
				nextChar();
			}
			else if (isDigit(m_ch)) {
				m_token.str += m_ch;
				nextChar();
			}
			else if (m_ch == 'e') {
				if (hasExponent) {
					errorUnexpectedChar();
				}
				hasExponent  = true;
				m_token.str += m_ch;
				nextChar();

				if (m_ch == '-') {
					m_token.str += m_ch;
					nextChar();
				}

				if (!isDigit(m_ch)) {
					errorUnexpectedChar();
				}
				else {
					m_token.str += m_ch;
					nextChar();
				}
			}
			else {
				break;
			}
		}
		return true;
	}

	bool Lexer2::_parseString() {
		m_token.type = TokenType::String;

		for (;;) {
			nextChar();
			if (m_ch == '\\') {
				nextChar();
				switch (m_ch) {
				case '\\':
				case '/':
				case '"':
					m_token.str += m_ch;
					break;
				case 'b': m_token.str += '\b'; break;
				case 'f': m_token.str += '\f'; break;
				case 'n': m_token.str += '\n'; break;
				case 'r': m_token.str += '\r'; break;
				case 't': m_token.str += '\t'; break;
				default: 
					error("unknown escape character [{}]", m_ch);
				}
			}
			else if (m_ch == '\"') {
				nextChar();
				break;
			}
			else {
				m_token.str += m_ch;
			}
		}
		return true;
	}

}