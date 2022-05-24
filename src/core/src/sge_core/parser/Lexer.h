#include "sge_core/base/sge_base.h"

namespace sge {

	class Lexer : public NonCopyable {
	public:
		enum class Type : u8 {
			None,
			Identifier,
			Separator,
			Operator,
			Literal,
			Command,
		};

		bool nextToken(StrView src);
		void retrieve (Type& outType, String &outVal);

		static const char* _type2char(Type type);

	private:
		struct Token {
			Type    type  = Type::None;
			StrView value = {};
		};

		template<class FUNC> bool _findMatch(StrView src, Type type, bool consume, FUNC&& startPred, size_t searchLen);
		template<class FUNC> bool _scanUntil(StrView src,			 bool skip,	   FUNC&& endPred,   size_t searchLen);

		void _resetToken(StrView src);
		void _advance	(StrView src, size_t len, bool consume);

		Token  m_token = {};
		size_t m_offset = 0;
	};


}