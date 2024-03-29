#pragma once

#include "Vec4_Basic.h"

namespace sge {

	template<class T, class M>
	struct Vec4_SSE_Data {
	public:
		using RegisterType = M;
		using ElementType  = T;
		static const size_t kElementCont = 4;

		union {
			struct { T x, y, z, w; };
			T data[kElementCont];
			M _m;
		};

		SGE_INLINE			T& operator[](int i)		{ return data[i]; }
		SGE_INLINE	const	T& operator[](int i) const	{ return data[i]; }

		SGE_INLINE Vec4_SSE_Data() = default;
		SGE_INLINE Vec4_SSE_Data(const T & x_, const T & y_, const T & z_, const T& w_) {
			static_assert(sizeof(Vec4_SSE_Data) == sizeof(T) * 4, "");
			set(x_, y_, z_, w_);
		}
		SGE_INLINE Vec4_SSE_Data(const Tuple4<T>&v) {
			set(v.x, v.y, v.z, v.w);
		}
		SGE_INLINE void set(const Tuple4<T>&v) {
			set(v.x, v.y, v.z, v.w);
		}
		SGE_INLINE void set(const T & x_, const T & y_, const T & z_, const T& w_) {
			x = x_; y = y_; z = z_; w = w_;
		}
	};

	using Vec4f_SSE_Data = Vec4_SSE_Data<float,  __m128 >;
	using Vec4d_SSE_Data = Vec4_SSE_Data<double, __m256d>;

	template<class T> struct Vec4_SSE_Select  { using Data = Vec4_Basic_Data<T>; };
	template<> struct Vec4_SSE_Select<float > { using Data = Vec4f_SSE_Data;	 };
	template<> struct Vec4_SSE_Select<double> { using Data = Vec4d_SSE_Data;	 };
	
	template<class T> using Vec4_SSE = Vec4_Basic<T, typename Vec4_SSE_Select<T>::Data>;
	
	using Vec4f_SSE = Vec4_SSE<float >;
	using Vec4d_SSE = Vec4_SSE<double>;

	SGE_INLINE Vec4f_SSE Vec4_SSE_make(__m128  m) { Vec4f_SSE o; o._m = m; return o; }
	SGE_INLINE Vec4d_SSE Vec4_SSE_make(__m256d m) { Vec4d_SSE o; o._m = m; return o; }
	
	#if defined(SGE_CPU_ENDIAN_LITTLE)	
	template<> SGE_INLINE void Vec4f_SSE::set(const Tuple4<float >& v) { _m =    _mm_set_ps(v.w, v.z, v.y, v.x); }
	template<> SGE_INLINE void Vec4d_SSE::set(const Tuple4<double>& v) { _m = _mm256_set_pd(v.w, v.z, v.y, v.x); }
	
	#elif defined(SGE_CPU_ENDIAN_BIG)
	template<> SGE_INLINE void Vec4f_SSE::set(const Tuple4<float >& v) { _m =    _mm_set_ps(v.x, v.y, v.z, v.w); }
	template<> SGE_INLINE void Vec4d_SSE::set(const Tuple4<double>& v) { _m = _mm256_set_pd(v.x, v.y, v.z, v.w); }	
	#else
		#error
	#endif
	
	template<> SGE_INLINE void Vec4f_SSE::setAll(const float  & s) { _m =    _mm_set1_ps(s); }
	template<> SGE_INLINE void Vec4d_SSE::setAll(const double & s) { _m = _mm256_set1_pd(s); }
	
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator+ (const Vec4& r) const { return Vec4_SSE_make(   _mm_add_ps(_m, r._m)); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator- (const Vec4& r) const { return Vec4_SSE_make(   _mm_sub_ps(_m, r._m)); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator* (const Vec4& r) const { return Vec4_SSE_make(   _mm_mul_ps(_m, r._m)); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator/ (const Vec4& r) const { return Vec4_SSE_make(   _mm_div_ps(_m, r._m)); }
														 
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator+ (const Vec4& r) const { return Vec4_SSE_make(_mm256_add_pd(_m, r._m)); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator- (const Vec4& r) const { return Vec4_SSE_make(_mm256_sub_pd(_m, r._m)); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator* (const Vec4& r) const { return Vec4_SSE_make(_mm256_mul_pd(_m, r._m)); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator/ (const Vec4& r) const { return Vec4_SSE_make(_mm256_div_pd(_m, r._m)); }
														 
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator+ (const float & s) const { return Vec4_SSE_make(   _mm_add_ps(_m,    _mm_set1_ps(s))); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator- (const float & s) const { return Vec4_SSE_make(   _mm_sub_ps(_m,    _mm_set1_ps(s))); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator* (const float & s) const { return Vec4_SSE_make(   _mm_mul_ps(_m,    _mm_set1_ps(s))); }
	template<> SGE_INLINE Vec4f_SSE Vec4f_SSE::operator/ (const float & s) const { return Vec4_SSE_make(   _mm_div_ps(_m,    _mm_set1_ps(s))); }
														 
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator+ (const double& s) const { return Vec4_SSE_make(_mm256_add_pd(_m, _mm256_set1_pd(s))); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator- (const double& s) const { return Vec4_SSE_make(_mm256_sub_pd(_m, _mm256_set1_pd(s))); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator* (const double& s) const { return Vec4_SSE_make(_mm256_mul_pd(_m, _mm256_set1_pd(s))); }
	template<> SGE_INLINE Vec4d_SSE Vec4d_SSE::operator/ (const double& s) const { return Vec4_SSE_make(_mm256_div_pd(_m, _mm256_set1_pd(s))); }

}