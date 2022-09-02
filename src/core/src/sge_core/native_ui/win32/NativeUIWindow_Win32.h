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


		HWND m_hwmd;

	protected:
		virtual void onCreate(CreateDesc& desc) override;
		virtual void onPaintNeeded()			override;
	private:
		static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		SGE_INLINE static NativeUIWindow_Win32* s_getThis(HWND hwnd) {
			return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		LRESULT		_handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		UIEventModifier _getWin32Modifier();
	};
}

#endif