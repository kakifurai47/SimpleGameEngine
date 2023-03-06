#pragma once

#include "sge_core-config.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "../detect_platform/sge_detect_platform.h"

#if  _TARGET_EDITOR
	#define SGE_TARGET_EDITOR 1
#elif _TARGET_CLIENT
	#define SGE_TARGET_CLIENT 1
#elif _TARGET_SERVER
	#define SGE_TARGET_SERVER 1
#endif

#if SGE_OS_WINDOWS
	#define FD_SETSIZE 1024
	#define NOMINMAX 1
	#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
	#include <ws2tcpip.h> // struct sockaddr_in6
	#pragma comment(lib, "Ws2_32.lib")
	#include <Windows.h>
	#include <intsafe.h>
#else
	#include <sys/stat.h>
	#include <stdint.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/mman.h>	
	#include <unistd.h> // sleep
	#include <arpa/inet.h> // htons
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netdb.h> // struct addrinfo
	#include <netinet/in.h> // struct sockaddr_in

#endif

#if SGE_COMPILER_VC | SGE_OS_CYGWIN
	#include <intrin.h>
#else
	#include <x86intrin.h>
#endif

#include <cassert>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <atomic>

#include <nlohmann/json.hpp>

#include <EASTL/sort.h>

#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EASTL/string_view.h>
#include <EASTL/span.h>

#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector_map.h>
#include <EASTL/string_map.h>
#include <EASTL/unordered_map.h>

#include <EASTL/unique_ptr.h>

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
	template<class T> inline constexpr typename std::underlying_type<T>::type     enumInt(T  value) { return       static_cast<typename std::underlying_type<T>::type>(value);   }
	template<class T> inline constexpr typename std::underlying_type<T>::type& enumIntRef(T& value) { return *reinterpret_cast<typename std::underlying_type<T>::type*>(&value); }
	template<class T> inline constexpr typename std::underlying_type<T>::type const& enumIntRef(const T& value) { return *reinterpret_cast<const typename std::underlying_type<T>::type*>(&value); }

	template<class T> inline bool constexpr enumHas(const T& a, const T& b) { return static_cast<T>(enumInt(a) & enumInt(b)) != static_cast<T>(0); }

	template<class T> SGE_INLINE T* constCast(const T* v) { return const_cast<T*>(v); }
	template<class T> SGE_INLINE T& constCast(const T& v) { return const_cast<T&>(v); }

	template<class T> SGE_INLINE void swap(T& a, T& b) { T temp = a; a = b; b = temp; }

	using u8	= uint8_t;
	using u16	= uint16_t;
	using u32	= uint32_t;
	using u64	= uint64_t;
	using u128  = uint128_t;

	using i8	= int8_t;
	using i16	= int16_t;
	using i32	= int32_t;
	using i64	= int64_t;

	//using f16 = half;
	using f32	= float;
	using f64	= double;
	using f128	= long double;

	constexpr size_t char_bit() { return CHAR_BIT; }
	template <typename T> constexpr size_t bit_size(T) { return sizeof(T) * char_bit(); }
	template <typename T> constexpr size_t bit_size()  { return sizeof(T) * char_bit(); }

	template< class Obj, class Member > constexpr
	intptr_t memberOffset(Member Obj::* ptrToMember) {
		Obj* c = nullptr;
		Member* m = &(c->*ptrToMember);
		return reinterpret_cast<intptr_t>(m);
	}

	template<class T, size_t N> constexpr
	size_t array_size(const T(&)[N]) { return N; }

	using Json = nlohmann::json;

	template<class T> using UPtr = eastl::unique_ptr<T>;	

	template<class T> using Span = eastl::span<T>;
	using ByteSpan = Span<const u8>;

	template<class DST, class SRC> 
	inline Span<DST> spanCast(Span<SRC> src) {
		size_t sizeInBytes = src.size() * sizeof(SRC);
		return Span<DST>(reinterpret_cast<DST*>(src.data()), sizeInBytes / sizeof(DST));
	}

//	template<class T, size_t N, bool bEnableOverflow = true> using Vector_ = eastl::fixed_vector<T, N, bEnableOverflow>;
//
//	template<class T>			inline Span<T> makeSpan(Vector<T>&	 v)	  { return { v.data(), v.size() }; }
//	template<class T, size_t N> inline Span<T> makeSpan(Vector_<T, N>& v) { return { v.data(), v.size() }; }

	template<class T, size_t N, bool bEnableOverflow>
	struct Vector_Base {
		using Type = typename eastl::fixed_vector<T, N, bEnableOverflow>;
	};

	template<class T>
	struct Vector_Base<T, 0, true> {
		using Type = typename eastl::vector<T>;
	};

	template<class T, size_t N = 0, bool bEnableOverflow = true>
	class Vector : public Vector_Base<T, N, bEnableOverflow>::Type {
		using Base = typename Vector_Base<T, N, bEnableOverflow>::Type;
	public:
		using Base::begin;
		using Base::end;
		using Base::size;

		using ElementType = T;
		
		static const constexpr size_t kElementCount	  = N;
		static const constexpr bool	  kEnableOverflow = bEnableOverflow;

		void appendRange(const Span<T>& r) { Base::insert(end(), r.begin(), r.end()); }

		Span<      T> span()			{ return Span<      T>(begin(), end()); }
		Span<const T> span() const		{ return Span<const T>(begin(), end()); }
		
		operator Span<      T>()		{ return span(); }
		operator Span<const T>() const	{ return span(); }
		
		Span<      T> subspan(size_t offset, size_t count)			{ return Span<      T>(begin() + offset, count); }
		Span<const T> subspan(size_t offset, size_t count) const	{ return Span<const T>(begin() + offset, count); }
		
		Span<      T> subspan(size_t offset)		{ return subspan(offset, size() - offset); }
		Span<const T> subspan(size_t offset) const	{ return subspan(offset, size() - offset); }
	};

	template<class KEY, class VALUE> using Map				 = eastl::map<KEY, VALUE>;
	template<class KEY, class VALUE> using VectorMap		 = eastl::vector_map<KEY, VALUE>;
	template<class KEY, class VALUE> using UnorderedMap		 = eastl::unordered_map<KEY, VALUE>;
	template<class KEY, class VALUE> using UnorderedMultiMap = eastl::unordered_multimap<KEY, VALUE>;

	template<class T> using StrViewT = eastl::basic_string_view<T>;
	using StrViewA = StrViewT<char>;
	using StrViewW = StrViewT<wchar_t>;

	template<class T, size_t N, bool bEnableOverflow = true>
	struct StringT_Base {
		using type = typename eastl::fixed_string<T, N, bEnableOverflow>;
	};

	template<class T>
	struct StringT_Base<T, 0, true> {
		using type = typename eastl::basic_string<T>;
	};

	template<class T, size_t N, bool bEnableOverflow = true> // using FixedStringT = eastl::fixed_string<T, N, bEnableOverflow>;
	class StringT : public StringT_Base<T, N, bEnableOverflow>::type {
		using Base = typename StringT_Base<T, N, bEnableOverflow>::type;
	public:
		using ElementType = T;
		using Base::data;
		using Base::size;

		static const constexpr size_t kElementCount		= N;
		static const constexpr bool	  kEnableOverflow	= bEnableOverflow;

		StringT() = default;
		StringT(const T* begin, const T* end) : Base(begin, end) {}
		StringT(StrViewT<T> view) : Base(view.data(), view.size()) {}
		StringT(StringT&& str) : Base(std::move(str)) {}

		template<class R> void operator=(R&& r) { Base::operator=(SGE_FORWARD(r)); }

		void operator+=(StrViewT<T> v) { Base::append(v.begin(), v.end()); }
		template<class R> void operator+=(const R& r) { Base::operator+=(r); }

		StrViewT<T>	view() const { return StrViewT<T>(data(), size()); }
	};

	template<size_t N, bool bEnableOverflow = true> using StringA_ = StringT<char, N, bEnableOverflow>;
	template<size_t N, bool bEnableOverflow = true> using StringW_ = StringT<wchar_t, N, bEnableOverflow>;

	using TempStringA = StringA_<220>;
	using TempStringW = StringW_<220>;

	using StringA = StringA_<0>;
	using StringW = StringW_<0>;

	using StrView	= StrViewA;
	using String	= StringA;

	inline StrView StrView_make(ByteSpan s) {
		return StrView(reinterpret_cast<const char*>(s.data()), s.size());
	}

	inline ByteSpan ByteSpan_make(StrView v) {
		return ByteSpan(reinterpret_cast<const u8*>(v.data()), v.size());
	}

	template<size_t N> using String_ = StringA_<N>;
	using TempString = TempStringA;

	template<size_t N> struct CharBySize;
	template<> struct CharBySize<1> { using Type = char;	 };
	template<> struct CharBySize<2> { using Type = char16_t; };
	template<> struct CharBySize<4> { using Type = char32_t; };

	struct WCharUtil {
		using Char = typename CharBySize<sizeof(wchar_t)>::Type;
		Char    toChar (wchar_t c) { return static_cast<Char>(c);	 }
		wchar_t toWChar(Char    c) { return static_cast<wchar_t>(c); }
	};

	class SrcLoc {
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
		int	line = 0;
	};

	class NonCopyable
	{
	public:
		NonCopyable() = default;
	private:
		NonCopyable	  (const NonCopyable&) = delete;
		void operator=(const NonCopyable&) = delete;
	};

	class RefCountBase : public NonCopyable {
	public:
		std::atomic_int m_refCount  = 0;
		std::atomic_int m_weakCount = 0;
	};

	inline void sge_sleep(size_t seconds) {
	#if SGE_OS_WINDOWS
		::Sleep(seconds * 1000);
	#else
		::sleep(seconds);
	#endif
	}


	template<class T> inline void sge_destroy (T* p) { p->~T();			   }
	template<class T> inline void sge_release (T* p) { operator delete(p); }

	template<class T> inline void sge_delete  (T* p) { sge_destroy(p); sge_release(p); }


}