#pragma once

#include "EditorSelection.h"

namespace sge
{
	class EditorContext : public NonCopyable
	{
	public:

		static EditorContext* instance() { return s_instance; }

		static EditorContext* createContext();
		static void			 destoryContext();

		EntitySelection editorSelection;


	private:
		static EditorContext* s_instance;

	};


}