#pragma once

#include "Render_Common.h"

namespace sge
{
	class  RenderContext;
	struct RenderContext_CreateDesc;

	class  RenderGpuBuffer;
	struct RenderGpuBuffer_CreateDesc;

	class Material;
	class Shader;

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

		Material*		 createMaterial ()									{ return onCreateMaterial();			}
		Shader*			 createShader	(StrView shadFilename)				{ return onCreateShader(shadFilename);	}
		RenderContext*	 createContext  (RenderContext_CreateDesc&	 desc)	{ return onCreateContext  (desc);		}
		RenderGpuBuffer* createGpuBuffer(RenderGpuBuffer_CreateDesc& desc)	{ return onCreateGpuBuffer(desc);		}

	protected:
		virtual Material*		 onCreateMaterial () = 0;
		virtual Shader*			 onCreateShader	  (StrView shadFilename) = 0;
		virtual RenderContext*	 onCreateContext  (RenderContext_CreateDesc&   desc) = 0;
		virtual RenderGpuBuffer* onCreateGpuBuffer(RenderGpuBuffer_CreateDesc& desc) = 0;

		static Renderer*	s_current;
	};
}