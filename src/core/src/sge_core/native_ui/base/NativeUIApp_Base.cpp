#include "NativeUIApp_Base.h"
#include "NativeUIWindow_Base.h"

namespace sge {

	void NativeUIApp_Base::run() {
		onCreate();
		onRun();
	}

	void NativeUIApp_Base::quit() {
		onQuit();
	}

}