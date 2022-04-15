#pragma once

namespace sge {

	class NativeUIApp_Base : public NonCopyable {
	public:		
		void run  ();
		void quit ();

	protected:
		virtual void onCreate() {};
		virtual void onRun()    {};
		virtual void onQuit()   {};
	};

}