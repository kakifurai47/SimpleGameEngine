#pragma once

#include "sge_core/base/sge_base.h"

namespace sge
{
	template<class T>
	class Tuple2 {
	public:
		union {
			struct {
				T x, y;
			};
			T data[2];
		};

		Tuple2() = default;

		Tuple2(T x_, T y_) : x(x_), y(y_)
		{
		}
	};

	template<class T>
	class Tuple3 {
	public:
		union {
			struct {
				T x, y, z;
			};
			T data[3];
		};

		Tuple3() = default;

		Tuple3(T x_, T y_, T z_) : x(x_), y(y_), z(z_)
		{
		}
	};

	template<class T>
	class Tuple4 {
	public:
		union {
			struct {
				T x, y, z, w;
			};
			T data[4];
		};

		Tuple4() = default;

		Tuple4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_)
		{
		}
	};

	using Tuple2f = Tuple2<float>;
	using Tuple3f = Tuple3<float>;
	using Tuple4f = Tuple4<float>;

	using Tuple2d = Tuple2<double>;
	using Tuple3d = Tuple3<double>;
	using Tuple4d = Tuple4<double>;
}