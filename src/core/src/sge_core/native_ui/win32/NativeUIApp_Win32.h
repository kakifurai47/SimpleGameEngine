#pragma once

#include "../base/NativeUIApp_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

	class NativeUIApp_Win32 : public NativeUIApp_Base {
		using Base = NativeUIApp_Base;

	protected:
		virtual void onCreate() override;
		virtual void onRun() override;


	};

}

#endif