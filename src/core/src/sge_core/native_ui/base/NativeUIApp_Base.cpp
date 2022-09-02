#include "NativeUIApp_Base.h"
#include "NativeUIWindow_Base.h"

namespace sge {

	void NativeUIApp_Base::run() {
		onCreate();
		onRun();
	}

	void NativeUIApp_Base::quit(int exitCode) {
		m_excitCode = exitCode;
		onQuit();
	}

}