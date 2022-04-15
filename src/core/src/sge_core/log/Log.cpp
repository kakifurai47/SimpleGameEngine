#include <sge_core/log/Log.h>
#include <iostream>

namespace sge {
	Log _log;

	void Log::onWrite(StrView str)
	{
		std::cout << str << "\n";
	}

} // namespace