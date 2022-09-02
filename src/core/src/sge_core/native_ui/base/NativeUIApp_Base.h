#pragma once

#include <sge_core/app/AppBase.h>

namespace sge {

	class NativeUIApp_Base : public AppBase {
	public:		
				void run	 ();
				void quit	 (int excitCode);
		virtual void willQuit() {};

	protected:
		virtual void onCreate	() {};
		virtual void onRun		() {};
		virtual void onQuit		() {};
		virtual void onUpdate	(float deltaTime) {};

		int m_excitCode   = 0;
		u32 m_tickElapsed = 0;
	};
}