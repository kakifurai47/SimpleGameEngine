#include "NativeUIApp_Win32.h"
#include "../NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

	void NativeUIApp_Win32::onCreate() {
		Base::onCreate();		
	}

	void NativeUIApp_Win32::onRun()
	{
		Base::onRun();

		MSG msg;

		while (::GetMessage(&msg, nullptr, 0, 0)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		Base::quit();
	}

	void NativeUIApp_Win32::onQuit() {
		Base::onQuit();
		::PostQuitMessage(m_excitCode);
	}

}

#endif
