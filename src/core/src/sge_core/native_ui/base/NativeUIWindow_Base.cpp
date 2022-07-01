#include "NativeUIWindow_Base.h"

namespace sge {
	void NativeUIWindow_Base::onNativeUIMouseEvent(UIMouseEvent& ev) {
		using Button = UIMouseEventButton;
		using Type	 = UIMouseEventType;

		switch (ev.type) {
			case Type::Down: {
				if (m_pressedMouseButtons == Button::None) {
					m_initButton = ev.button;
				}
				BitUtil::set(m_pressedMouseButtons, ev.button); 
			} break;

			case Type::Up: {
				BitUtil::unset(m_pressedMouseButtons, ev.button); 
			} break;
		}


		ev.initButton	  = m_initButton;
		ev.pressedButtons = m_pressedMouseButtons;



		ev.deltaPos = ev.pos - m_mousePos;
		m_mousePos  = ev.pos;

		onUIMouseEvent(ev);
	}

}