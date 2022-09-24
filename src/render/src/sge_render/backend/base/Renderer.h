#pragma once

#include "Render_Common.h"
#include "RenderContext.h"

namespace sge
{
	enum class Render_ApiType {
#if SGE_RENDER_COMP_DX11
		DX11,
#endif
	};

	struct Renderer_CreateDesc   {
		Renderer_CreateDesc ();
		Render_ApiType apiType;
		bool multithread   : 1;
	};

	class Renderer : public NonCopyable {
	public:
		using CreateDesc = Renderer_CreateDesc;

		static Renderer* current() { return s_current; }

		static Renderer* create	(CreateDesc& desc);

				 Renderer();
		virtual ~Renderer();

		SPtr<Shader>			createShader	(StrView filename);
		SPtr<Material>			createMaterial	()									{ return onCreateMaterial ();			  }
		SPtr<Texture2D>			createTexture2D (Texture2D_CreateDesc&		 desc)	{ return onCreateTexture2D(desc);		  }	
		SPtr<RenderContext>		createContext	(RenderContext_CreateDesc&	 desc)	{ return onCreateContext  (desc);		  }
		SPtr<RenderGpuBuffer>	createGpuBuffer	(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc);		  }


	protected:
		virtual Material*		 onCreateMaterial () = 0;
		virtual Shader*			 onCreateShader	  (StrView	filename) = 0;
		virtual Texture2D*		 onCreateTexture2D(Texture2D_CreateDesc&	   desc) = 0;
		virtual RenderContext*	 onCreateContext  (RenderContext_CreateDesc&   desc) = 0;
		virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;

		static Renderer*	s_current;
		Map<u128, Shader*>  m_shaders;
		

	};
}