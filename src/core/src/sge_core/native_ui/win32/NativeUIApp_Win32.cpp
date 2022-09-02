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

		MSG msg {};

		for (;;) {
			if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
			{
				if (msg.message == WM_QUIT) {
					break;
				}
				::TranslateMessage(&msg);
				::DispatchMessage (&msg);
			}
			else {
				auto  tc = ::GetTickCount();
				float dt = (tc - m_tickElapsed) / 1000.f;

				onUpdate(dt);
				m_tickElapsed = tc;
			}
		}

		willQuit();
	}

	void NativeUIApp_Win32::onQuit() {
		Base::onQuit();
		::PostQuitMessage(m_excitCode);
	}

}

#endif
