#pragma once

#include "../string/String.h"
#include "../string/Fmt.h"

//#define SGE_ERROR(...) 

namespace sge {

	class IError : public std::exception {
	public:

	};

	class Error : public IError {

	};


} // namespace