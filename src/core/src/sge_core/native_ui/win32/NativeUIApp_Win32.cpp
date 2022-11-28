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

//		for (;;) {
//			if (::PeekMessage(&m_win32_msg, nullptr, 0, 0, PM_REMOVE))
//			{
//				if (m_win32_msg.message == WM_QUIT) {
//					break;
//				}
//				::TranslateMessage(&m_win32_msg);
//				::DispatchMessage (&m_win32_msg);
//			}
//			else {
//				auto  tc = ::GetTickCount();
//				float dt = (tc - m_tickElapsed) / 1000.f;
//
//				onUpdate(dt);
//				m_tickElapsed = tc;
//			}
//		}

		while (::GetMessage(&m_win32_msg, NULL, 0, 0))
		{
			::TranslateMessage(&m_win32_msg);
			::DispatchMessage (&m_win32_msg);
		}


		willQuit();
	}

	void NativeUIApp_Win32::onQuit() {
		Base::onQuit();
		::PostQuitMessage(m_excitCode);
	}

}

#endif
