#pragma once
#include "sge_core/base/sge_base.h"

namespace sge {

	class Lexer : public NonCopyable {
	public:
		enum class TokenType : u8 {
			None, Idr, Sep, Opr, Lit, Cmd,
		};

		constexpr const char* Lexer::_type2char(TokenType type) {
			switch (type) {
			case TokenType::Idr:	return "Identifier";
			case TokenType::Sep:	return "Separator";
			case TokenType::Opr:	return "Operator";
			case TokenType::Lit:	return "Literal";
			case TokenType::Cmd:	return "Command";
			default:				return "None";
			}
		}

		bool reset(StrView src);

		bool next	   (StrView src);
		bool tryNext   (StrView src, TokenType type, StrView val = nullptr);
		void ensureNext(StrView src, TokenType type, StrView val = nullptr);

		void retrieveNext(StrView& src, TokenType type, String& out);
		void retrieve	 (String& out);

		void throwUnexpected();

		inline
		bool Lexer::tokCheck(TokenType type, StrView val = nullptr) {
			return val == nullptr ? type == m_tok.type :
									type == m_tok.type && val.compare(m_tok.value) == 0;
		}

	protected:
		struct Token {
			TokenType  type = TokenType::None;
			StrView	  value = {};
		};
		Token m_tok;

	private:
		template<class FUNC> bool _findMatch(StrView src, TokenType type, bool consume, FUNC&& startPred, size_t searchLen);
		template<class FUNC> bool _scanUntil(StrView src,				  bool skip,	FUNC&& endPred,   size_t searchLen);

		void _resetToken(StrView src);
		void _advance	(StrView src, size_t len, bool consume);

		size_t  m_off = 0;
	};


}