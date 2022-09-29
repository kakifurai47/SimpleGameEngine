
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

		m_hwnd = ::CreateWindowEx(0, clsName, clsName, WS_OVERLAPPEDWINDOW,
								  int(desc.rect.x),
								  int(desc.rect.y),
								  int(desc.rect.w),
								  int(desc.rect.h),
								  nullptr, nullptr, hInstance, this);

		if (!m_hwnd) {
			throw SGE_ERROR("CreateWindowEx");
		}
		::ShowWindow(m_hwnd, SW_NORMAL);
		_updateMonitorInfos(m_hwnd);
	}

	void NativeUIWindow_Win32::onDestroy() {
		::DestroyWindow(m_hwnd); 
	}

	void NativeUIWindow_Win32::onPaintNeeded() {
		::InvalidateRect(m_hwnd, nullptr, false);
	}

	void NativeUIWindow_Win32::onResetCapture(NativeUIWindow_Base* win)  {
		::ReleaseCapture();
		if (auto* w = static_cast<NativeUIWindow_Win32*>(win))
		{
			::SetCapture(w->m_hwnd);
		}
	}

	void NativeUIWindow_Win32::onSetPos(const Vec2f& newPos)
	{
		RECT rect =
		{
			(LONG)newPos.x, (LONG)newPos.y, (LONG)newPos.x, (LONG)newPos.y
		};

		::AdjustWindowRectEx(&rect, WS_CAPTION | WS_BORDER, FALSE, WS_CAPTION | WS_BORDER);
		::SetWindowPos(m_hwnd, nullptr, 0, 0, rect.left, rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	void NativeUIWindow_Win32::onSetSize(const Vec2f& newSize)
	{
		RECT rect =
		{
			(LONG)0, (LONG)0, (LONG)newSize.x, (LONG)newSize.y
		};
		
		::AdjustWindowRectEx(&rect, WS_CAPTION | WS_BORDER, FALSE, WS_CAPTION | WS_BORDER);
		::SetWindowPos(m_hwnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	}

	LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		switch (msg) {
			case WM_CREATE: {
				auto* createS = reinterpret_cast<CREATESTRUCT*>(lParam);
				auto* thisObj = reinterpret_cast<This*>(createS->lpCreateParams);
				thisObj->m_hwnd  = hwnd;
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)thisObj);
			}break;

			case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = ::BeginPaint(hwnd, &ps);
				if (auto* thisObj = s_getThis(hwnd)) {
					thisObj->onPaint();
				}
				::EndPaint(hwnd, &ps);
				return 0;
			}break;

			case WM_MOVE: {
				if (auto* thisObj = s_getThis(hwnd)) {
					Vec2f newPos = { (float)LOWORD(lParam), (float)HIWORD(lParam) };
					thisObj->onPositionChanged(newPos);
					return 0;
				}
			}break;

			case WM_SIZE: {
				if (auto* thisObj = s_getThis(hwnd)) {
					Rect2f newClientRect { 0,  0, (float)LOWORD(lParam), (float)HIWORD(lParam) };
					thisObj->onClientRectChanged(newClientRect, wParam == SIZE_MINIMIZED);
					return 0;
				}
			}break;

			case WM_DISPLAYCHANGE: {
				if (auto* thisObj = s_getThis (hwnd)) {
					thisObj->_updateMonitorInfos(hwnd);
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
					thisObj->m_hwnd = nullptr;
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

	BOOL WINAPI NativeUIWindow_Win32::s_monitorEnumPorc(HMONITOR hMonitor, HDC hdc, LPRECT lPrect, LPARAM lParam)
	{
		auto* infos = reinterpret_cast<Vector<MonitorInfo, 2>*>(lParam);
		if  (!infos) return FALSE;

		MONITORINFO info {};
		info.cbSize = sizeof(MONITORINFO);
		if (!::GetMonitorInfo(hMonitor, &info)) { return TRUE; }
		
		auto& newInfo		= infos->emplace_back();

		newInfo.rect		= Win32Util::toRect2f(info.rcMonitor);
		newInfo.workRect	= Win32Util::toRect2f(info.rcWork	);
		newInfo.isPrimary   = info.dwFlags & MONITORINFOF_PRIMARY;

		UINT xdpi = 96, ydpi = 96;
		xdpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
		ydpi = ::GetDeviceCaps(hdc, LOGPIXELSY);
		
		newInfo.dpiScale = xdpi / 96.0f;

		return TRUE;
	}

	void NativeUIWindow_Win32::_updateMonitorInfos(HWND hwnd) 
	{
		Vector<MonitorInfo, 2> infos;
		HDC hdc = ::GetDC(hwnd);
		::EnumDisplayMonitors(hdc, (LPCRECT)nullptr, s_monitorEnumPorc, (LPARAM)&infos);
		::ReleaseDC(hwnd, hdc);
		onMonitorsUpdated(infos);
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


		switch (HIWORD(wParam)) {
			case XBUTTON1: ev.button = Button::Button4; break;
			case XBUTTON2: ev.button = Button::Button5; break;
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
			case WM_MOUSEWHEEL:		{ ev.type = Type::Scroll;	ev.scroll.set(0,GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA); } break;
		#endif

		#if (_WIN32_WINNT >= 0x0600)
			// horizontal scroll wheel 
			case WM_MOUSEHWHEEL:	{ ev.type = Type::Scroll;	ev.scroll.set(GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA,0); } break;
		#endif

			default:
				return false;
		}

		{
			auto& b = m_capturedMouseButton;

			switch (ev.type) {
				case Type::Down: { if (b == Button::None) { b = ev.button; ::SetCapture(hwnd); onCaptureChanged(true) ; } } break;
				case Type::Up:   { if (b == ev.button) { b = Button::None; ::ReleaseCapture(); onCaptureChanged(false); } } break;
			}
		}

		ev.capturedButton = m_capturedMouseButton;
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
