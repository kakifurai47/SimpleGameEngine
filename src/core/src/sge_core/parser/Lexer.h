#pragma once

#include <sge_core/meta/type_trait.h>

namespace sge {

	#define Lexer_TokenType_ENUM_LIST(E) \
		E(None,) \
		E(WhiteSpace,) \
		E(Identifier,) \
		E(Separator,) \
		E(Operator,) \
		E(Literal,) \
		E(Command,) \
//--------

	SGE_ENUM_CLASS(Lexer_TokenType, : u8)

	class Lexer : public NonCopyable {
	public:
		using TokenType = Lexer_TokenType;

		size_t line() const	{ return m_line; }
		StrView getRemainSource() const;

		void reset(StrView src);

		void expect		(TokenType type, StrView val = nullptr) { if (!checkToken(type, val)) throwUnexpected(); }

		bool next		(StrView src);
		bool tryNext	(StrView src, TokenType type, StrView val = nullptr);

		void throwUnexpected();

		inline
		bool checkToken(TokenType type, StrView val = nullptr) {
			return val == nullptr ? type == m_tok.type :
									type == m_tok.type && val.compare(m_tok.value) == 0;
		}

	protected:
		struct Token {
			TokenType  type = TokenType::None;
			StrView	  value = {};
		};
		Token m_tok;

		inline bool isIdentifier(StrView value)			  { return checkToken(TokenType::Identifier, value); }
		inline bool isSeparator (StrView value)			  { return checkToken(TokenType::Separator,  value); }
		inline bool isLiteral	(StrView value = nullptr) { return checkToken(TokenType::Literal,	 value); }

		template<class T> void readIdentifier(T& out) { expect(TokenType::Identifier); _read(out); }
		template<class T> void readLiteral	 (T& out) { expect(TokenType::Literal);    _read(out); }

		template<class T> 
		constexpr void _read(T& out) {
			if		constexpr (meta::isString<T>()) { _readString(out); }
			else if constexpr (meta::isBool  <T>()) { _readBool  (out); }
			else if constexpr (meta::isEnum  <T>()) { _readEnum  (out); }
			else {
				SGE_ASSERT(false); //unexpected type
			}
		}

							 void _readString(String& out);
							 void _readBool	 (bool&   out);
		template<class ENUM> void _readEnum	 (ENUM&   out) { if (!enumTryParse(out, m_tok.value)) { throwUnexpected(); }
		}

	private:
		template<class FUNC> bool _findMatch(StrView src, TokenType type, bool consume, FUNC&& startPred, size_t searchLen);
		template<class FUNC> bool _scanUntil(StrView src,				  bool skip,	FUNC&& endPred,   size_t searchLen);

		void _resetToken(StrView src);
		void _advance	(StrView src, size_t len, bool consume);

		StrView m_src;
		size_t  m_off  = 0;
		size_t  m_line = 0;
	};
}

