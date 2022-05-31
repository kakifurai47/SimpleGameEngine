#pragma once



namespace sge {

	class NativeUIWindow_Base : public NonCopyable
	{
	public:
		void create();
		void paintNeeded() { onPaintNeeded(); }

	protected:
		virtual void onCreate()  {};
		virtual void onClose()	 {};
		virtual void onDestroy() {};

		virtual void onPaint()	 {};
		
		virtual void onPaintNeeded() {};


	};

}