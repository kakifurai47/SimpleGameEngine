#pragma once

#include "sge_core/base/sge_base.h"

namespace sge
{
	template<class T>
	class Tuple2 {
	public:
		using ElementType = T;
		static const size_t kElementCount = 2;

		union {
			struct    { T x, y; };
			T data[kElementCount];
		};

		SGE_INLINE			T& operator[](int i)		 { return data[i]; }
		SGE_INLINE	const	T& operator[](int i) const	 { return data[i]; }

		SGE_INLINE Tuple2()				   = default;
		SGE_INLINE Tuple2(const Tuple2& v) = default;
		SGE_INLINE Tuple2(const T& x_, const T& y_) { set(x_, y_); }

		template<class U>
		explicit SGE_INLINE Tuple2(const Tuple2<U>& v) {
			set( static_cast<T>(v.x), static_cast<T>(v.y) );
		}	

		SGE_INLINE void set(const Tuple2<T>& v) { *this = v; }
		SGE_INLINE void set(const T& x_, const T& y_) {
			x = x_; y = y_;
		}

		SGE_INLINE void setAll(const T& v) { set(v, v); }
	};

	template<class T>
	class Tuple3 {
	public:
		using ElementType = T;
		static const size_t kElementCount = 3;

		union {
			struct { T x, y, z; };
			T data[kElementCount];
		};

		SGE_INLINE			T& operator[](int i)		 { return data[i]; }
		SGE_INLINE	const	T& operator[](int i) const	 { return data[i]; }

		SGE_INLINE Tuple3()				   = default;
		SGE_INLINE Tuple3(const Tuple3& v) = default;
		SGE_INLINE Tuple3(const T& x_, const T& y_, const T& z_) { set(x_, y_, z_); }

		template<class U>
		explicit SGE_INLINE Tuple3(const Tuple3<U>& v) {
			set( static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z) );
		}
				
		SGE_INLINE void set(const Tuple3<T>& v) { *this = v; }
		SGE_INLINE void set(const T& x_, const T& y_, const T& z_) {
			x = x_; y = y_; z = z_;
		}

		SGE_INLINE void setAll(const T& v) { set(v, v, v); }

	};

	template<class T>
	class Tuple4 {
	public:
		using ElementType = T;
		static const size_t kElementCount = 4;

		union {
			struct { T x, y, z, w; };
			T    data[kElementCount];
		};

		SGE_INLINE			T& operator[](int i)		 { return data[i]; }
		SGE_INLINE	const	T& operator[](int i) const	 { return data[i]; }

		SGE_INLINE Tuple4()				   = default;
		SGE_INLINE Tuple4(const Tuple4& v) = default;
		SGE_INLINE Tuple4(const T& x_, const T& y_, const T& z_, const T& w_) { set(x_, y_, z_, w_); }

		template<class U> 
		explicit SGE_INLINE Tuple4(const Tuple4<U>& v) {
			set( static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z), static_cast<T>(v.w) );
		}

		SGE_INLINE void set(const Tuple4<T>& v) { *this = v; }
		SGE_INLINE void set(const T& x_, const T& y_, const T& z_, const T& w_) {
			x = x_; y = y_; z = z_; w = w_;
		}

		SGE_INLINE void setAll(const T& v) { set(v, v, v, v); }
	};

	using Tuple2f = Tuple2<float>;
	using Tuple3f = Tuple3<float>;
	using Tuple4f = Tuple4<float>;

	using Tuple2d = Tuple2<double>;
	using Tuple3d = Tuple3<double>;
	using Tuple4d = Tuple4<double>;

	using Tuple2i = Tuple2<int>;
	using Tuple3i = Tuple3<int>;
	using Tuple4i = Tuple4<int>;
}