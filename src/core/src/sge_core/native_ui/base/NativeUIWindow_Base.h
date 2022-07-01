#pragma once
#include <sge_core/input/UIEvent.h>

namespace sge {

	class NativeUIWindow_Base : public NonCopyable
	{
	public:
		void create()	   { onCreate();	  }
		void paintNeeded() { onPaintNeeded(); }

	protected:
		virtual void onCreate	   ()	{};
		virtual void onCloseButton ()	{};

		virtual void onPaint	  () {};
		virtual void onPaintNeeded() {};
		
		virtual void onUIMouseEvent		 (UIMouseEvent& ev) {};
		virtual void onNativeUIMouseEvent(UIMouseEvent& ev);

		UIMouseEventButton m_initButton			 = UIMouseEventButton::None;
		UIMouseEventButton m_pressedMouseButtons = UIMouseEventButton::None;
		Vec2f m_mousePos {0, 0};
		

	};

}