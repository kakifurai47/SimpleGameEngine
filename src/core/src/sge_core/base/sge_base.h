#pragma once

#include "../detect_platform/sge_detect_platform.h"

#if SGE_OS_WINDOWS
#define NOMINMAX 1

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> // struct sockaddr_in
#endif

#include <cassert>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdint>

#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EASTL/string_view.h>
#include <EASTL/span.h>

#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/string_map.h>

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>

#include "sge_macro.h"

//==== EASTL ====

#if !EASTL_DLL // If building a regular library and not building EASTL as a DLL...
	// It is expected that the application define the following
	// versions of operator new for the application. Either that or the
	// user needs to override the implementation of the allocator class.
inline void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) { return malloc(size); }
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
#if SGE_OS_WINDOWS
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}
#endif


namespace sge
{
	template<class T> using UPtr = eastl::unique_ptr<T>;

	//template<class T, class... Args> inline UPtr<T> MakeUPtr(Args&& ... args) { return eastl::make_unique<T>(args ...); }

	using TempString = eastl::fixed_string<char, 220, true>;
	using StrView    = eastl::basic_string_view<char>;
	using String     = eastl::basic_string<char>;

	class NonCopyable
	{
	public:
		NonCopyable() = default;
	private:
		NonCopyable(NonCopyable&&) = delete;

		NonCopyable(const NonCopyable&)	= delete;
		void operator=(const NonCopyable&) = delete;		
	};

	template<class T> inline void sge_delete(T* p) { delete p; }
}