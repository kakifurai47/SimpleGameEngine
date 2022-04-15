#pragma once

#include "../base/sge_base.h"
#include "Fmt.h"

namespace sge {


	
	inline
	std::ostream& operator<<(std::ostream& os, const StrView& sv) {
		os.write(sv.data(), sv.size());
		return os;
	}

	



} // namespace
