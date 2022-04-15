#pragma once

#include <sge_core/base/sge_base.h>
#include <sge_core/string/Fmt.h>
#include <sge_core/string/String.h>

#define SGE_LOG(...) do { sge::_log.write(__VA_ARGS__); } while(false)

namespace sge {

	class Log : public NonCopyable 
	{
	public:
		template<class Fmt_Str, class ... Args> inline
		void write(Fmt_Str&& str, Args&& ... args)
		{
			TempString temp;
			FmtTo(temp, str, args ...);
			onWrite(temp);
		}

		void onWrite(StrView str);
	};

	extern Log _log;

} //namespace


