#pragma once

#include "../base/NativeUIWindow_Base.h"
#if SGE_OS_WINDOWS

namespace sge {

	class NativeUIWindow_Win32 : public NativeUIWindow_Base 
	{
		using This = NativeUIWindow_Win32;
		using Base = NativeUIWindow_Base;
	public:
		using CreateDesc = Base::CreateDesc;

	public:
		HWND m_hwnd;

	protected:

		virtual void onCreate(CreateDesc& desc) override;
		virtual void onDestroy()				override;
		virtual void onPaintNeeded()			override;

		virtual void onResetCapture(NativeUIWindow_Base* win) override;

		virtual void onSetPos (const Vec2f& newPos ) override;
		virtual void onSetSize(const Vec2f& newSize) override;

	private:
		static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static BOOL    WINAPI s_monitorEnumPorc(HMONITOR hMonitor, HDC hdc, LPRECT lPrect, LPARAM lParam);
		
		SGE_INLINE static NativeUIWindow_Win32* s_getThis(HWND hwnd) {
			return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		LRESULT		_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void _updateMonitorInfos	  (HWND hwnd);


		UIEventModifier _getWin32Modifier();

		UIMouseEventButton m_capturedMouseButton = UIMouseEventButton::None;
	};
}

#endif