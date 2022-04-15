

#pragma once

#include <sge_core/base/sge_base.h>

#if SGE_OS_WINDOWS
#define SGE_RENDER_COMP_DX11	1
#define SGE_RENDER_COMP_OPENGL  1
#endif

#if SGE_OS_IOS
#define SGE_RENDER_COMP_OPENGL  1
#define SGE_RENDER_COMP_METAL   1
#endif

enum class RenderAPI
{
#if SGE_RENDER_COMP_DX11
	dx11,
#endif
#if SGE_RENDER_COMP_OPENGL
	opengl,
#endif
#if SGE_RENDER_COMP_METAL
	metal,
#endif
};

#ifdef SGE_RENDER_COMP_DX11
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#endif