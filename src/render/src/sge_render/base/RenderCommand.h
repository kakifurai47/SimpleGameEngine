#pragma once

namespace sge
{
	class RenderCmdBase : public NonCopyable
	{
	public:
		enum class CmdType : u16 {
			None,
			SetViewport,
			Clear,
			Draw,
		};

		RenderCmdBase(CmdType type_)
			: type(type_)
		{
		}

		CmdType type;		
	};

#define RenderCmd_Construct(T) \
	public: \
		RenderCmd_##T() \
			: RenderCmdBase(CmdType::T) \
	{ \
	} \
	//------------

	class RenderCmd_SetViewport : public RenderCmdBase {
		RenderCmd_Construct(SetViewport)
	public:

		int test_setVP = 0;

	};

	class RenderCmd_Clear : public RenderCmdBase {
		RenderCmd_Construct(Clear)
	public:
		Color4f color;
	};

	class RenderCmd_Draw : public RenderCmdBase {
		RenderCmd_Construct(Draw)
	public:

		int buffer = 0;
		int shader = 0;
		int vertexCnt = 0;
		int primitiveType = 0;
		int layout = 0;
	};
}
