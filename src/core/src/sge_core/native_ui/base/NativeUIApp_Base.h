#pragma once

#include <sge_core/app/AppBase.h>

namespace sge {

	class NativeUIApp_Base : public AppBase {
	public:		
		void run  ();
		void quit ();

	protected:
		virtual void onCreate() {};
		virtual void onRun()    {};
		virtual void onQuit()   {};

		int m_excitCode = 0;
	};

}