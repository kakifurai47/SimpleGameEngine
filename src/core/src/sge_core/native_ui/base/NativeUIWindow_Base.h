#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/input/UIEvent.h>

namespace sge {

	struct NativeUIWindow_Base_CreateDesc 
	{
		Rect2f rect {10,10,640,480};
	};


	class NativeUIWindow_Base : public NonCopyable {
	public:
		using CreateDesc = NativeUIWindow_Base_CreateDesc;

		void create		(CreateDesc& desc) { onCreate(desc);  }

		void update(float deltaTime)       { onUpdate(deltaTime); }
		void paintNeeded()				   { onPaintNeeded();	  }

		const Rect2f& clientRect() { return m_clientRect; }

	protected:
		virtual void onCreate	   (CreateDesc& desc) {};
		virtual void onCloseButton ()	{};

		virtual void onPaint	  () {};
		virtual void onPaintNeeded() {};

		virtual void onUpdate(float deltaTime) {};
		
		virtual void onUIMouseEvent		 (UIMouseEvent& ev) {};
		virtual void onNativeUIMouseEvent(UIMouseEvent& ev);

		virtual void onClientRectChanged(const Rect2f& rc) { m_clientRect = rc; }

		UIMouseEventButton m_firstPressedMouseButton = UIMouseEventButton::None;
		UIMouseEventButton m_pressedMouseButtons	 = UIMouseEventButton::None;
		Vec2f m_mousePos {0, 0};
		
		Rect2f m_clientRect {0,0,0,0};

	};

}