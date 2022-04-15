#pragma once



namespace sge {

	class NativeUIWindow_Base : public NonCopyable
	{
	public:
		void create();		
	protected:
		virtual void onCreate()  {};
		virtual void onClose()	 {};
		virtual void onDestroy() {};

		virtual void onPaint()	 {};
		


	};

}