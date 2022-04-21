#pragma once



namespace sge
{
	class  RenderContext;
	struct RenderContext_CreateDesc;

	enum class Render_ApiType {
#if SGE_RENDER_COMP_DX11
		DX11,
#endif
	};

	struct Renderer_CreateDesc
	{
		Renderer_CreateDesc();
		Render_ApiType	 type;
		bool multithread  : 1;
	};


	class Renderer : public NonCopyable
	{
	public:
		using CreateDesc = Renderer_CreateDesc;

		static Renderer* current() { return s_current; }

		static Renderer* create(CreateDesc& desc);
		
		virtual RenderContext* onCreateRenderContext(RenderContext_CreateDesc& desc) = 0;

	protected:
		static Renderer* s_current;
	};
}