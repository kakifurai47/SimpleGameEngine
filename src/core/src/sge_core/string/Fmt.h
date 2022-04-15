#pragma once

#include <fmt/format.h>

namespace sge {
	template<class Fmt_Str, class ... Args> inline
	void FmtTo(TempString& out, Fmt_Str&& s, Args&& ... args)
	{
		fmt::format_to(std::back_inserter(out), s, args ...);
	}
}
