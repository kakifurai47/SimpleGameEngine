#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/input/UIEvent.h>

namespace sge {

	class NativeUIWindow;

	struct NativeUIWindow_Base_CreateDesc 
	{	
		enum class Type {
			None,
			NormalWindow,
			ToolWindow,
			PopupWindow,
		};

		NativeUIWindow_Base_CreateDesc()
			: titleBar(true)
			, isMainWindow(false)
			, visible(true)
			, resizable(true)
			, closeButton(true)
			, minButton(true)
			, maxButton(true)
			, centerToScreen(true)
			, alwaysOnTop(false)
		{
		}

		Type   type = Type::NormalWindow;
		Rect2f rect {10,10,640,480};

		NativeUIWindow* parent = nullptr;

		bool		titleBar		: 1;
		bool		isMainWindow	: 1;
		bool		visible			: 1;
		bool		resizable		: 1;
		bool		closeButton		: 1;
		bool		minButton		: 1;
		bool		maxButton		: 1;
		bool		centerToScreen	: 1;
		bool		alwaysOnTop		: 1;
	};

	struct MonitorInfo {
		bool isPrimary = false;
		Rect2f rect     {};
		Rect2f workRect {};
		float  dpiScale = 0.0f;
	};

	class NativeUIWindow_Base : public NonCopyable {
		using This = NativeUIWindow_Base;
	public:
		using CreateDesc = NativeUIWindow_Base_CreateDesc;
		

		void create		(CreateDesc& desc)	{ onCreate(desc);	}
		void destroy()						{ onDestroy();		}

		void update(float deltaTime)       { onUpdate(deltaTime); }
		void paintNeeded()				   { onPaintNeeded();	  }

		void resetCapture(This* win)	   { onResetCapture(win); }
		
		Span<MonitorInfo> monitorInfos()		{ return m_monitorInfos; }

		const Vec2f&	  pos		  () const  { return m_winPos;		}
		const Rect2f&	  clientRect  () const	{ return m_clientRect;  }
		bool			  isMinimized () const	{ return m_isMinimized; }
		bool			  isCaptured  () const	{ return m_isCaptured;  }

		void setPos (const Vec2f& newPos)  { onSetPos(newPos);   }
		void setSize(const Vec2f& newSize) { onSetSize(newSize); }


	protected:
		virtual void onCreate	   (CreateDesc& desc) {};
		virtual void onDestroy	   ()	{};
		virtual void onCloseButton ()	{};


		virtual void onPaint	  () {};
		virtual void onPaintNeeded() {};

		virtual void onUpdate(float deltaTime) {};


		virtual void onResetCapture		 (This* win) {};
		virtual void onCaptureChanged	 (bool  isCaptured_) { m_isCaptured = isCaptured_; }
		
		virtual void onUIMouseEvent		 (UIMouseEvent& ev) {};
		virtual void onNativeUIMouseEvent(UIMouseEvent& ev);

		virtual void onPositionChanged	 (const Vec2f& pos) { m_winPos = pos; }

		virtual void onClientRectChanged (const Rect2f& rc, bool isMinimized_) {
			m_clientRect  = rc;
			m_isMinimized = isMinimized_; 
		}

		virtual void onMonitorsUpdated  (Span<MonitorInfo> infos) { m_monitorInfos.appendRange(infos);  }

		virtual void onSetPos (const Vec2f& newPos ) {}
		virtual void onSetSize(const Vec2f& newSize) {}

	private:

		UIMouseEventButton m_pressedMouseButtons = UIMouseEventButton::None;

		bool   m_isMinimized = false;
		bool   m_isCaptured  = false;

		Vec2f  m_mousePos   {};
		Rect2f m_clientRect {};
		Vec2f  m_winPos     {};

		Vector<MonitorInfo, 2> m_monitorInfos;
	};

}