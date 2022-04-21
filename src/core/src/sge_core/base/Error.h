#pragma once

#include "../string/String.h"
#include "../string/Fmt.h"

#define SGE_ERROR(...) Error(SGE_SRC_LOC, Fmt(__VA_ARGS__))

namespace sge {

	class IError : public std::exception {
	public:

	};

	class Error : public IError {
	public:
		Error() = default;
		Error(const SrcLoc& loc, StrView msg)
			: m_loc(loc),
			  m_msg(msg)
		{
		}
	//private:
		SrcLoc	   m_loc;
		TempString m_msg;
	};


} // namespace