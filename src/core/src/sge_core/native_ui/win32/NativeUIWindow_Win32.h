#pragma once

#include "../base/NativeUIWindow_Base.h"

#if SGE_OS_WINDOWS

namespace sge {

	class NativeUIWindow_Win32 : public NativeUIWindow_Base 
	{
		using This = NativeUIWindow_Win32;
		using Base = NativeUIWindow_Base;

	public:
		HWND m_hmwd;

	protected:
		virtual void onCreate()  override;
		virtual void onDestroy() override;
	private:		
		static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		inline static NativeUIWindow_Win32* s_getThis(HWND hwnd)
		{
			return reinterpret_cast<This*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}
	};
}

#endif