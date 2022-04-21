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

	template<class T> using StrViewT = eastl::basic_string_view<T>;
	using StrViewA = StrViewT<char>;
	using StrViewW = StrViewT<wchar_t>;
	
	template<class T, size_t N, bool bEnableOverflow = true>
	class StringT : public eastl::fixed_string<T, N, bEnableOverflow> {
		using Base = eastl::fixed_string<T, N, bEnableOverflow>;
	public:
		StringT() = default;
		StringT(StrViewT<T> view) : Base(view.data(), view.size()) {}
		StringT(StringT&&	str)  : Base(std::move(str)) {}
	
		template<class R> void operator=(R&& r) { Base::operator=(SGE_FORWARD(r)); }
	};

	using StringA = eastl::basic_string<char>;
	using StringW = eastl::basic_string<wchar_t>;
	
	template<size_t N, bool bEnableOverflow = true> using StringA_ = StringT<char, N, bEnableOverflow>;
	template<size_t N, bool bEnableOverflow = true> using StringW_ = StringT<wchar_t, N, bEnableOverflow>;
	
	using TempStringA = StringA_<220>;
	using TempStringW = StringW_<220>;
	
	using StrView	= StrViewA;
	using String	= StringA;
	
	template<size_t N> using String_ = StringA_<N>;
	using TempString = TempStringA;

	class SrcLoc
	{
	public:
		SrcLoc() = default;
		SrcLoc(const char* file_, int line_, const char* func_)
			: file(file_),
			  line(line_),
			  func(func_)
		{
		}
		const char* file = "";
		const char* func = "";
		const int	line = 0;
	};

	template<class T>
	class ComPtr : NonCopyable
	{
	public:
		ComPtr() = default;
		ComPtr(const ComPtr& r) { reset(r.m_p);   }
		~ComPtr() noexcept      { reset(nullptr); }

		T* operator->() noexcept		{ return m_p; }
		operator T* ()  noexcept		{ return m_p; }

				T* ptr() noexcept		{ return m_p; }
		const	T* ptr() const noexcept { return m_p; }

		void reset(T* p) {
			if (p == m_p) return;
			if (m_p) {
				m_p->Release();
				m_p  = nullptr;
			}
			m_p = p;
			if (m_p) {
				m_p->AddRef();
			}
		}

		T** ptrForInit() noexcept { reset(nullptr); return &m_p; }
		T*  detach() { T* o = m_p;  m_p = nullptr;  return o;	 }

	private:
		T* m_p = nullptr;		
	};

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