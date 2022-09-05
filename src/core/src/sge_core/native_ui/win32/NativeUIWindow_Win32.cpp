
#include "NativeUIWindow_Win32.h"
#include "Win32Util.h"

#include <sge_core/string/UtfUtil.h>

#if SGE_OS_WINDOWS

namespace sge {


	void NativeUIWindow_Win32::onCreate(CreateDesc& desc)
	{
		Base::onCreate(desc);

		auto hInstance = ::GetModuleHandle(nullptr);		
		WNDCLASSEX wc = {};
		
		const wchar_t* clsName = L"MyWindow";
		
		wc.cbSize			 = sizeof(WNDCLASSEX);
		wc.style			 = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		 = s_wndProc;
		wc.cbClsExtra		 = 0;
		wc.cbWndExtra		 = 0;
		wc.hInstance		 = hInstance;
		wc.hIcon			 = LoadIcon(hInstance, IDI_APPLICATION);
		wc.hCursor			 = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	 = nullptr; //(HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName		 = nullptr;
		wc.lpszClassName	 = clsName;
		wc.hIconSm			 = LoadIcon(hInstance, IDI_APPLICATION);

		if (!desc.closeButton) {
			wc.style |= CS_NOCLOSE;
		}

		DWORD dwStyle {};
		DWORD dwExStyle = WS_EX_ACCEPTFILES;

		if (desc.alwaysOnTop) dwExStyle |= WS_EX_TOPMOST;

		switch (desc.type) {
			case CreateDesc::Type::ToolWindow:
			case CreateDesc::Type::NormalWindow: {
				dwStyle |= WS_OVERLAPPED | WS_SYSMENU;

				if (desc.closeButton) dwStyle |= WS_SYSMENU;
				if (desc.resizable  ) dwStyle |= WS_THICKFRAME;
				if (desc.titleBar   ) dwStyle |= WS_CAPTION;
				if (desc.minButton  ) dwStyle |= WS_MINIMIZEBOX;
				if (desc.maxButton  ) dwStyle |= WS_MAXIMIZEBOX;
			}break;

			case CreateDesc::Type::PopupWindow: {
				dwStyle |= WS_POPUP | WS_BORDER;
			}break;

			default: SGE_ASSERT(desc.type == CreateDesc::Type::None); break;
		}

		if (desc.type == CreateDesc::Type::ToolWindow) {
			dwExStyle |= WS_EX_TOOLWINDOW;
		}

		auto& rect = desc.rect;
		if (desc.centerToScreen) {
			auto screenSize = Vec2f((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));
			rect.pos = (screenSize - rect.size) / 2;
		}
		
		WNDCLASSEX tmpWc;
		bool registered = (::GetClassInfoEx(hInstance, clsName, &tmpWc) != 0);

		if (!registered) {
			if (!::RegisterClassEx(&wc)) {
				throw SGE_ERROR("registerclassex");
			}
		}

		m_hwmd = ::CreateWindowEx(0, clsName, clsName, WS_OVERLAPPEDWINDOW,
								  int(desc.rect.x),
								  int(desc.rect.y),
								  int(desc.rect.w),
								  int(desc.rect.h),
								  nullptr, nullptr, hInstance, this);

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

//			case WM_MOVE:
			case WM_SIZE: {
				if (auto* thisObj = s_getThis(hwnd)) {
					RECT clientRect;
					::GetClientRect(hwnd, &clientRect);
					Rect2f newClientRect = Win32Util::toRect2f(clientRect);
					thisObj->onClientRectChanged(newClientRect);
					return 0;
				}
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
