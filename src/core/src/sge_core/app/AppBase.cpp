#include "AppBase.h"

namespace sge {

#if SGE_OS_WINDOWS
	String AppBase::getExecutableFileName() {
		wchar_t temp[MAX_PATH + 1];
		if (!::GetModuleFileName(nullptr, temp, MAX_PATH)) {
			throw SGE_ERROR("");
		}
		String o = UtfUtil::toString(temp);
		return o;
	}
#endif
}