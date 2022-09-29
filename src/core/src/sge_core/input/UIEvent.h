#pragma once

namespace sge {

	enum class UIMouseEventType {
		None,
		Move,
		Down,
		Up,
		Scroll,
	};

	enum class UIMouseEventButton {
		None,
		Left	= 1 << 0,
		Middle	= 1 << 1,
		Right	= 1 << 2,
		Button4 = 1 << 3,
		Button5 = 1 << 4,
	};
	SGE_ENUM_ALL_OPERATOR(UIMouseEventButton)

	enum class UIEventModifier {
		None,
		Shift	= 1 << 0,
		Ctrl	= 1 << 1,
		Atl		= 1 << 2,
		Cmd		= 1 << 3,
		Fn		= 2 << 4,
	};
	SGE_ENUM_ALL_OPERATOR(UIEventModifier)


	struct UIMouseEvent {
		using Type		 = UIMouseEventType;
		using Modifier	 = UIEventModifier;
		using Button	 = UIMouseEventButton;

		void reset() { *this = {}; }

		bool isUp()		  const	{ return type == Type::Up; }
		bool isDown()	  const	{ return type == Type::Down; }
		bool isMove()	  const	{ return type == Type::Move; }
		bool isScroll()	  const	{ return type == Type::Scroll; }
		bool isDragging() const { return type == Type::Move && pressedButtons != Button::None; }

		Button		capturedButton	  {};

		Type		type			  {};
		Button		button			  {};
		Button		pressedButtons	  {};
		Modifier	modifier		  {};

		Vec2f pos		{0, 0};
		Vec2f deltaPos	{0, 0};
		Vec2f scroll	{0, 0};
	};

}