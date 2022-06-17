#pragma once

namespace sge {


	class Lexer2 : public NonCopyable {
	public:
		enum class TokenType {
			None,
			Identifier,
			Separator,
			Operator,
			Number,
			String,

			Preprocessor,
			NewLine,
		};

		void reset(ByteSpan src_, StrView filename);
		void reset(StrView  src_, StrView filename);

		bool nextChar ();
		bool nextToken();

		void trimSpaces();
		void skipNewlineTokens();

		void expectOperator(StrView view);

		StrView getLastFewLines(size_t lineCount);


		bool isAlpha	(char c) { return isLowerCase(c) || isUpperCase(c); }
		bool isLowerCase(char c) { return c >= 'a' && c <= 'z'; }
		bool isUpperCase(char c) { return c >= 'A' && c <= 'Z'; }
		bool isDigit	(char c) { return c >= '0' && c <= '9'; }

	protected:
		struct Token {
			TokenType type = TokenType::None;
			String	  str;

			bool isNone			()			 const { return type == TokenType::None; }

			bool isIdentifier	()			 const { return type == TokenType::Identifier; }
			bool isIdentifier	(StrView s)	 const { return type == TokenType::Identifier && s == str; }

			bool isSeparator	()			 const { return type == TokenType::Separator; }

			bool isOperator		()			 const { return type == TokenType::Operator; }
			bool isOperator		(StrView s)	 const { return type == TokenType::Operator	 && s == str; }

			bool isNumber		()			 const { return type == TokenType::Number; }

			bool isString		()			 const { return type == TokenType::String; }
			bool isString		(StrView s)	 const { return type == TokenType::String && s == str; }

			bool isPreprocessor	()			 const { return type == TokenType::Preprocessor; }
			bool isNewLine		()			 const { return type == TokenType::NewLine;	}

			bool isEnd			()			 const { return type == TokenType::None && str == "\0"; }

			void setNone() {
				type = TokenType::None;
				str.clear();
			}
		};
		Token m_token;

		template<class... ARGs>
		void error(const ARGs&... args) {
			auto msg = Fmt(args...);
			_error(msg);
		}

		void errorUnexpectedChar();
		void errorUnexpectedToken();

		void _error(StrView msg);

		void _parseCommentBlock();
		void _parseCommentSingleLine();
		bool _parseIdentifier();
		bool _parseNumber();
		bool _parseString();

		String	m_filename;
		StrView m_source;

		const char* m_cur  = nullptr;
		char		m_ch   = 0;
		size_t		m_col  = 0;
		size_t		m_line = 0;
	};
}