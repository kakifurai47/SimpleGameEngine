
#include "NativeUIWindow_Win32.h"
#include "Win32Util.h"

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
		wc.hIcon			 = LoadIcon(hInstance, IDI_APPLICATION);
		wc.hCursor			 = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	 = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName		 = NULL;
		wc.lpszClassName	 = szWindowClass;
		wc.hIconSm			 = LoadIcon(hInstance, IDI_APPLICATION);
		
		WNDCLASSEX tmpWc;
		bool registered = (::GetClassInfoEx(hInstance, szWindowClass, &tmpWc) != 0);

		if (!registered) {
			if (!::RegisterClassEx(&wc)) {				
				throw SGE_ERROR("registerclassex");
			}
		}
		
		m_hwmd = ::CreateWindowEx(0,
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

		if (!m_hwmd) {
			throw SGE_ERROR("CreateWindowEx");
		}
		::ShowWindow(m_hwmd, SW_NORMAL);
	}

	void NativeUIWindow_Win32::onPaintNeeded() {
		::InvalidateRect(m_hwmd, nullptr, false);
	}

	LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		switch (msg) {
			case WM_CREATE: {
				auto* createS = reinterpret_cast<CREATESTRUCT*>(lParam);
				auto* thisObj = reinterpret_cast<This*>(createS->lpCreateParams);
				thisObj->m_hwmd = hwnd;
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
			}break;

			case WM_PAINT: {
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				if (auto* thisObj = s_getThis(hwnd)) {
					thisObj->onPaint();
				}
				EndPaint(hwnd, &ps);
				return 0;
			}break;

			case WM_SIZE: {
				
			}break;

			//case WM_ACTIVATE: {
			//	if (auto* thisObj = s_getThis(hwnd)) {
			//		u16 a = LOWORD(wParam);
			//		switch (a) {
			//			case WA_ACTIVE:
			//			case WA_CLICKACTIVE: SGE_LOG("activate");	   break; <= freeze;
			//			case WA_INACTIVE:	 SGE_LOG("inactivate");	   break;
			//		}
			//		return 0;
			//	}
			//}break;

			case WM_DESTROY: {
				if (auto* thisObj = s_getThis(hwnd)) {
					::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
					thisObj->m_hwmd = nullptr;
					sge_delete(thisObj);
				}
				return 0;
			}break;

			case WM_CLOSE: {
				if (auto* thisObj = s_getThis(hwnd)) {
					thisObj->onCloseButton();
					return 0;
				}
			}break;

			default: {
				if (auto* thisObj = s_getThis(hwnd)) {
					return thisObj->_handleNativeEvent(hwnd, msg, wParam, lParam);
				}
			}break;
		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	bool NativeUIWindow_Win32::_handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		UIMouseEvent ev;

		ev.modifier = _getWin32Modifier();

		using Button = UIMouseEventButton;
		using Type	 = UIMouseEventType;

		POINT curPos;
		::GetCursorPos(&curPos);
		::ScreenToClient(hwnd, &curPos);	

		Win32Util::convert(ev.pos, curPos);

		auto button = Button::None;
		switch (HIWORD(wParam)) {
			case XBUTTON1: button = Button::Button4; break;
			case XBUTTON2: button = Button::Button5; break;
		}

		switch (msg) {
			case WM_LBUTTONDOWN:	{ ev.button = Button::Left;		ev.type = Type::Down;	} break;
			case WM_RBUTTONDOWN:	{ ev.button = Button::Right;	ev.type = Type::Down;	} break;
			case WM_MBUTTONDOWN:	{ ev.button = Button::Middle;	ev.type = Type::Down;	} break;

			case WM_LBUTTONUP:		{ ev.button = Button::Left;		ev.type = Type::Up;	} break;
			case WM_RBUTTONUP:		{ ev.button = Button::Right;	ev.type = Type::Up;	} break;
			case WM_MBUTTONUP:		{ ev.button = Button::Middle;	ev.type = Type::Up;	} break;

			case WM_MOUSEMOVE:		{ ev.type = Type::Move;	} break;

		#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
			// vertical  scroll wheel 
			case WM_MOUSEWHEEL:		{ ev.type = Type::Scroll;	ev.scroll.set(0,GET_WHEEL_DELTA_WPARAM(wParam)); } break;
		#endif

		#if (_WIN32_WINNT >= 0x0600)
			// horizontal scroll wheel 
			case WM_MOUSEHWHEEL:	{ ev.type = Type::Scroll;	ev.scroll.set(GET_WHEEL_DELTA_WPARAM(wParam),0); } break;
		#endif

			default:
				return false;
		}
		
		switch (ev.type) {
			case Type::Down:		::SetCapture(hwnd); break;
			case Type::Up:			::ReleaseCapture(); break;
		}

		onNativeUIMouseEvent(ev);
		return true;
	}

	LRESULT NativeUIWindow_Win32::_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (_handleNativeUIMouseEvent(hwnd, msg, wParam, lParam)) return 0;
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	UIEventModifier NativeUIWindow_Win32::_getWin32Modifier() {
		auto o = UIEventModifier::None;
		if (::GetAsyncKeyState(VK_CONTROL))								     o |= UIEventModifier::Ctrl;
		if (::GetAsyncKeyState(VK_SHIFT	 ))								     o |= UIEventModifier::Shift;
		if (::GetAsyncKeyState(VK_MENU	 ))								     o |= UIEventModifier::Atl;
		if (::GetAsyncKeyState(VK_LWIN	 ) || ::GetAsyncKeyState(VK_RWIN))   o |= UIEventModifier::Cmd; 
		return o;
	}
}
#endif
