#pragma once

#include "Render_Common.h"

namespace sge
{
	class  RenderContext;
	struct RenderContext_CreateDesc;

	class Material;

	enum class Render_ApiType {
#if SGE_RENDER_COMP_DX11
		DX11,
#endif
	};

	struct Renderer_CreateDesc  {
		Renderer_CreateDesc();
		Render_ApiType	 apiType;
		bool multithread	 : 1;
	};

	class Renderer : public NonCopyable {
	public:
		using CreateDesc = Renderer_CreateDesc;

		static Renderer* current() { return s_current; }

		static Renderer* create	(CreateDesc& desc);

				 Renderer();
		virtual ~Renderer();

		RenderContext* createContext (RenderContext_CreateDesc& desc) { return onCreateContext (desc); }
		Material*	   createMaterial()								  { return onCreateMaterial();	   }

	protected:
		virtual RenderContext* onCreateContext (RenderContext_CreateDesc& desc) = 0;
		virtual Material*	   onCreateMaterial() = 0;

	protected:
		static Renderer* s_current;
	};
}