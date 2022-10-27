#pragma once

namespace sge
{
	namespace Math
	{
		SGE_INLINE constexpr bool isPow2(u64 v) { return v && !(v & (v - 1)); }

		constexpr u64 nextPow2(u64 v) {
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			return ++v;
		}

		template<class T> SGE_INLINE constexpr T max(const T& a, const T& b) { return a > b ? a : b; }
		template<class T> SGE_INLINE constexpr T min(const T& a, const T& b) { return a < b ? a : b; }

		template<class T> constexpr T clamp		(const T& v, const T& a, const T& b) { return max(a, min(v, b)); }
		template<class T> constexpr T clamp01	(const T& v)						 { return clamp(v, 0, 1);	 }
		template<class T> constexpr T saturate	(const T& v)						 { return clamp01(v);		 }

		template < class T > constexpr T	inf		()				{ return std::numeric_limits<T>::infinity(); }
		template < class T > constexpr bool	isInf	( const T& v )	{ return std::numeric_limits<T>::has_infinity && v == inf<T>(); }



		template<class T> SGE_INLINE constexpr T byteToK(const T& v) { return v / 1024; }
		template<class T> SGE_INLINE constexpr T byteToM(const T& v) { return v / (1024 * 1024); }
		template<class T> SGE_INLINE constexpr T byteToG(const T& v) { return v / (1024 * 1024 * 1024); }

		struct _Helper {
			template<class T>
			static constexpr T alignTo_uint(T n, T a) {
				static_assert(std::is_unsigned<T>::value, "");
				T r = n % a;
				return r ? (n + a - r) : n;
				//	return (n + a - 1) / a * a;
			}
		};

		SGE_INLINE constexpr size_t alignTo(size_t n, size_t a) { return _Helper::alignTo_uint(n, a); }

#if 0
#pragma mark ------ Trigonometry ------
#endif
		template< class T> constexpr T	PI()	{ return static_cast<T>(3.14159265358979323846); }
	
		template<class T> constexpr T	twoPI	()		{ return PI<T>() * 2; }
		template<class T> constexpr T	halfPI	()		{ return PI<T>() * 0.5; }
	
		template<class T> constexpr T	radians	(T deg) { return deg * (PI<T>() / static_cast<T>(180)); }
		template<class T> constexpr T	degrees	(T rad) { return rad * (static_cast<T>(180) / PI<T>()); }
	
	
		SGE_INLINE float  sin (float  rad) { return ::sinf(rad); }
		SGE_INLINE double sin (double rad) { return ::sin (rad); }

		SGE_INLINE float  cos (float  rad) { return ::cosf(rad); }
		SGE_INLINE double cos (double rad) { return ::cos (rad); }

		SGE_INLINE float  tan (float  rad) { return ::tanf(rad); }
		SGE_INLINE double tan (double rad) { return ::tan (rad); }

		SGE_INLINE float  asin(float    x) { return ::asin(x);   }
		SGE_INLINE double asin(double   x) { return ::asin(x);   }

		SGE_INLINE float  acos(float    x) { return ::acos(x);   }
		SGE_INLINE double acos(double   x) { return ::acos(x);   }

		SGE_INLINE float  atan2(float   y, float x) { return ::atan2(y, x); }
		SGE_INLINE double atan2(double  y, float x) { return ::atan2(y, x); }



	
#if SGE_OS_MACOSX
		SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::__sincosf(rad, &out_sin, &out_cos); }
		SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::__sincos (rad, &out_sin, &out_cos); }
#elif SGE_OS_LINUX
		SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::sincosf(rad, &out_sin, &out_cos); }
		SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::sincos (rad, &out_sin, &out_cos); }
#else
		SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
		SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
#endif



		//template<class T> SGE_INLINE constexpr T abs(const T& v) { T mask = v >> bit_size<T>() - 1; return (v ^ mask) - mask; }
		template<class T> SGE_INLINE constexpr T abs(const T& v) { return v < 0 ? -v : v; }

		template<class T> constexpr T		epsilon();
		template<>		  constexpr int		epsilon<int >() { return 0;			   }
		template<>		  constexpr f32		epsilon<f32 >() { return FLT_EPSILON;  }
		template<>		  constexpr f64		epsilon<f64 >() { return DBL_EPSILON;  }
		template<>		  constexpr f128	epsilon<f128>() { return LDBL_EPSILON; }
	
		template<class T, class EP = T> SGE_INLINE constexpr bool equals (const T& a, const T& b, const EP& ep = epsilon<T>()) { return abs(a-b) <= ep; }
		template<class T, class EP = T> SGE_INLINE constexpr bool equals0(const T& a,             const EP& ep = epsilon<T>()) { return abs( a ) <= ep; }

		SGE_INLINE float	sqrt(float  n) { return std::sqrt(n); }
		SGE_INLINE double	sqrt(double n) { return std::sqrt(n); }
		SGE_INLINE int		sqrt(int    n) { return static_cast<int>(std::sqrt(static_cast<double>(n))); }
	};
}