#pragma once

#include "sge_core/base/sge_base.h"

#if SGE_OS_WINDOWS

#include "Win32/NativeUIApp_Win32.h"
#include "Win32/NativeUIWindow_Win32.h"

namespace sge {
	using NativeUIWindow_Impl = NativeUIWindow_Win32;
	using NativeUIApp_Impl	  = NativeUIApp_Win32;
}

#else
#error "unsupported platform"

#endif


namespace sge {

	class NativeUIWindow : public NativeUIWindow_Impl
	{
	};
	
	class NativeUIApp : public NativeUIApp_Impl
	{
	};

}