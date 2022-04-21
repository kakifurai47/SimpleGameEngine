#pragma once

#if SGE_OS_WINDOWS
	#define SGE_RENDER_COMP_DX11	1
	#define SGE_RENDER_COMP_OPENGL  1
#endif

#if SGE_OS_IOS
	#define SGE_RENDER_COMP_METAL   1
	#define SGE_RENDER_COMP_OPENGL  1
#endif
