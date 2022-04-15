#include "NativeUIWindow_Win32.h"
#include <sge_core/string/UtfUtil.h>

#if SGE_OS_WINDOWS

namespace sge {


	void NativeUIWindow_Win32::onCreate()
	{
		Base::onCreate();

		auto hInstance = ::GetModuleHandle(nullptr);
		
		WNDCLASSEX wc = {};
		
		const wchar_t* szWindowClass = L"MyWindow";
		
		wc.cbSize			 = sizeof(WNDCLASSEX);
		wc.style			 = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		 = s_wndProc;
		wc.cbClsExtra		 = 0;
		wc.cbWndExtra		 = 0;
		wc.hInstance		 = hInstance;
		wc.hIcon			 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
		wc.hCursor			 = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	 = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName		 = NULL;
		wc.lpszClassName	 = szWindowClass;
		wc.hIconSm			 = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
		
		
		::RegisterClassEx(&wc);
		
		m_hmwd = ::CreateWindowEx(0,
			szWindowClass,
			szWindowClass,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			hInstance,
			this
		);

		::ShowWindow  (m_hmwd, SW_NORMAL);
		::UpdateWindow(m_hmwd);
	}

	void NativeUIWindow_Win32::onDestroy()
	{
		Base::onDestroy();
		PostQuitMessage(0);
	}

	LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		switch (msg)
		{
			case WM_CREATE: {
				auto* createS = reinterpret_cast<CREATESTRUCT*>(lParam);
				auto* thisObj = reinterpret_cast<This*>(createS->lpCreateParams);
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
			}break;

			case WM_PAINT: {
				if (auto* thisObj = s_getThis(hwnd)) {
					thisObj->onPaint();
				}
			}break;

			case WM_SIZE: {

				SGE_LOG("On Resize");

			}break;

			case WM_DESTROY: {
				if (auto* thisObj = s_getThis(hwnd)) {
					::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
					thisObj->m_hmwd = nullptr;
					thisObj->onDestroy();
				}
				return 0;
			}break;

			case WM_CLOSE: {
				if (auto* thisObj = s_getThis(hwnd)) {
					thisObj->onClose();
				}
			}break;
		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

#endif