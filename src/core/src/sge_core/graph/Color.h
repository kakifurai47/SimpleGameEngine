#pragma once

namespace sge {

#define ColorModel_ENUM_LIST(E) \
	E(None,) \
	E(R,) \
	E(RG,) \
	E(RGB,) \
	E(RGBA,) \
	E(L,)  /* Luminance */ \
	E(LA,) /* Luminance Alpha */ \
	E(HSV,) \
	E(BlockCompression,) \
//-------
	SGE_ENUM_CLASS(ColorModel, : u8)

#define ColorElementType_ENUM_LIST(E) \
	E(None,) \
	E(UNorm8,) \
	E(UNorm16,) \
	E(Float16,) \
	E(Float32,) \
	E(Float64,) \
//-------
	SGE_ENUM_CLASS(ColorElementType, : u8)

	template<class T> constexpr ColorElementType ColorElementType_get();
	template<> constexpr ColorElementType ColorElementType_get<u8 >() { return ColorElementType::UNorm8;  }
	template<> constexpr ColorElementType ColorElementType_get<u16>() { return ColorElementType::UNorm16; }
//	template<> constexpr ColorElementType ColorElementType_get<f16>() { return ColorElementType::Float16; }
	template<> constexpr ColorElementType ColorElementType_get<f32>() { return ColorElementType::Float32; }
	template<> constexpr ColorElementType ColorElementType_get<f64>() { return ColorElementType::Float64; }

#define ColorCompressType_ENUM_LIST(E) \
	E(None,) \
	E(BC1,)	\
	E(BC2,)	\
	E(BC3,)	\
	E(BC4,)	\
	E(BC5,)	\
	E(BC6h,) \
	E(BC7,) \
//-------
	SGE_ENUM_CLASS(ColorCompressType, : u8);

	enum class ColorType : u16;

constexpr ColorType ColorType_make(ColorModel model, ColorElementType elem) {
	return static_cast<ColorType>(
			  (static_cast<u32>(model) << 8)
			| (static_cast<u32>(elem))
			);
}

constexpr ColorType ColorType_make(ColorModel model, ColorCompressType compress) {
	return static_cast<ColorType>(
			  (static_cast<u32>(model) << 8)
			| (static_cast<u32>(compress))
			);
}

#define ColorType_ENUM_LIST(E) \
	E(None,) \
	E(Rb,		= (u16)ColorType_make(ColorModel::R,	ColorElementType::UNorm8 )) \
	E(Rs,		= (u16)ColorType_make(ColorModel::R,	ColorElementType::UNorm16)) \
	E(Rh,		= (u16)ColorType_make(ColorModel::R,	ColorElementType::Float16)) \
	E(Rf,		= (u16)ColorType_make(ColorModel::R,	ColorElementType::Float32)) \
	\
	E(Lb,		= (u16)ColorType_make(ColorModel::L,	ColorElementType::UNorm8 )) \
	E(Ls,		= (u16)ColorType_make(ColorModel::L,	ColorElementType::UNorm16)) \
	E(Lh,		= (u16)ColorType_make(ColorModel::L,	ColorElementType::Float16)) \
	E(Lf,		= (u16)ColorType_make(ColorModel::L,	ColorElementType::Float32)) \
	\
	E(LAb,		= (u16)ColorType_make(ColorModel::LA,	ColorElementType::UNorm8 )) \
	E(LAs,		= (u16)ColorType_make(ColorModel::LA,	ColorElementType::UNorm16)) \
	E(LAh,		= (u16)ColorType_make(ColorModel::LA,	ColorElementType::Float16)) \
	E(LAf,		= (u16)ColorType_make(ColorModel::LA,	ColorElementType::Float32)) \
	\
	E(RGb,		= (u16)ColorType_make(ColorModel::RG,	ColorElementType::UNorm8 )) \
	E(RGs,		= (u16)ColorType_make(ColorModel::RG,	ColorElementType::UNorm16)) \
	E(RGh,		= (u16)ColorType_make(ColorModel::RG,	ColorElementType::Float16)) \
	E(RGf,		= (u16)ColorType_make(ColorModel::RG,	ColorElementType::Float32)) \
	\
	E(RGBb,		= (u16)ColorType_make(ColorModel::RGB,	ColorElementType::UNorm8 )) \
	E(RGBs,		= (u16)ColorType_make(ColorModel::RGB,	ColorElementType::UNorm16)) \
	E(RGBh,		= (u16)ColorType_make(ColorModel::RGB,	ColorElementType::Float16)) \
	E(RGBf,		= (u16)ColorType_make(ColorModel::RGB,	ColorElementType::Float32)) \
	\
	E(RGBAb,	= (u16)ColorType_make(ColorModel::RGBA,	ColorElementType::UNorm8 )) \
	E(RGBAs,	= (u16)ColorType_make(ColorModel::RGBA,	ColorElementType::UNorm16)) \
	E(RGBAh,	= (u16)ColorType_make(ColorModel::RGBA,	ColorElementType::Float16)) \
	E(RGBAf,	= (u16)ColorType_make(ColorModel::RGBA,	ColorElementType::Float32)) \
	\
	E(BC1,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC1 )) \
	E(BC2,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC2 )) \
	E(BC3,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC3 )) \
	E(BC4,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC4 )) \
	E(BC5,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC5 )) \
	E(BC6h,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC6h)) \
	E(BC7,		= (u16)ColorType_make(ColorModel::BlockCompression, ColorCompressType::BC7 )) \
//----
	SGE_ENUM_CLASS(ColorType, : u16)

	template<class T>
	struct ColorR {
		using ElementType = T;

		static constexpr size_t		kElementCount = 1;
		static constexpr int		kAlphaBits = 0;
		static constexpr ColorType	kColorType = ColorType_make(ColorModel::R, ColorElementType_get<T>());

		union {
			struct	{ T r; };
			T data[kElementCount];
		};

		ColorR() = default;
		ColorR(const T& r_)
			: r(r_)
		{
		}
	};

	template<class T>
	struct ColorRG {
		using ElementType = T;
	
		static constexpr size_t		kElementCount = 2;
		static constexpr int		kAlphaBits = 0;
		static constexpr ColorType	kColorType = ColorType_make(ColorModel::RG, ColorElementType_get<T>());

		union {
			struct { T r, g; };
			T data[kElementCount];
		};

		ColorRG() = default;
		ColorRG(const T& r_, const T& g_)
			: r(r_), g(g_)
		{
		}
	};

	template<class T>
	struct ColorRGB {
		using ElementType = T;
	
		static constexpr size_t		kElementCount = 3;
		static constexpr int		kAlphaBits = 0;
		static constexpr ColorType	kColorType = ColorType_make(ColorModel::RGB, ColorElementType_get<T>());

		union {
			struct { T r, g, b; };
			T data[kElementCount];
		};

		ColorRGB() = default;
		ColorRGB(const T& r_, const T& g_, const T& b_)
			: r(r_), g(g_), b(b_)
		{
		}
	};

	template<class T>
	struct ColorRGBA {
		using ElementType = T;
	
		static constexpr size_t		kElementCount = 4;
		static constexpr int		kAlphaBits = sizeof(T) * 8;
		static constexpr ColorType	kColorType = ColorType_make(ColorModel::RGBA, ColorElementType_get<T>());

		union {
			struct { T r, g, b, a; };
			T	 data[kElementCount];
		};

		ColorRGBA() = default;
		ColorRGBA(const T& r_, const T& g_, const T& b_, const T& a_)
			: r(r_), g(g_), b(b_), a(a_)
		{
		}
	};

	template<class T>
	struct ColorL {
		using ElementType = T;
		static constexpr size_t		kElementCount = 1;
		static constexpr int		kAlphaBits	  = 0;
		static constexpr ColorType	kColorType	  = ColorType_make(ColorModel::L, ColorElementType_get<T>());
	
		union {
			struct { T l; };
			T data[kElementCount];
		};
	
		ColorL() = default;
		ColorL(const T& l_) : l(l_)
		{
		}
	};
	
	template<class T>
	struct ColorLA {
		using ElementType = T;
		static constexpr size_t		kElementCount = 2;
		static constexpr int		kAlphaBits	  = sizeof(T) * 8;
		static constexpr ColorType	kColorType	  = ColorType_make(ColorModel::LA, ColorElementType_get<T>());
	
		union {
			struct { T l, a; };
			T data[kElementCount];
		};
	
		ColorLA() = default;
		ColorLA(const T& l_, const T& a_)
			: l(l_), a(a_) 
		{
		}
	};


	
//	struct ColorBC1 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC1;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 1;
//		static constexpr int		kBytesPerPixelBlock = 8;
//	};
//	
//	struct ColorBC2 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC2;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 4;
//		static constexpr int		kBytesPerPixelBlock = 16;
//	};
//	
//	struct ColorBC3 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC3;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 8;
//		static constexpr int		kBytesPerPixelBlock = 16;
//	};
//
//	
//	struct ColorBC4 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC4;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 0;
//		static constexpr int		kBytesPerPixelBlock = 8;
//	};
//	
//	
//	struct ColorBC5 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC5;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 0;
//		static constexpr int		kBytesPerPixelBlock = 16;
//	};
//	
//	
//	struct ColorBC6h {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC3;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 0;
//		static constexpr int		kBytesPerPixelBlock = 16;
//	};
//	
//	struct ColorBC7 {
//		using ElementType = void;
//		static constexpr ColorType	kColorType			= ColorType::BC3;
//		static constexpr ColorModel	kColorModel			= ColorModel::BlockCompression;
//		static constexpr int		kAlphaBits			= 8;
//		static constexpr int		kBytesPerPixelBlock = 16;
//	};

	using ColorRGBAf = ColorRGBA<float>;
	using ColorRGBAb = ColorRGBA<u8>;
	using ColorRGBAs = ColorRGBA<u16>;

	using Color4f = ColorRGBAf;
	using Color4b = ColorRGBAb;

	using ColorRf = ColorR<float>;
	using ColorRb = ColorR<u8>;
	using ColorRs = ColorR<u16>;

	using ColorLf = ColorL<float>;
	using ColorLb = ColorL<u8>;
	using ColorLs = ColorL<u16>;
	
	using ColorLAf = ColorLA<float>;
	using ColorLAb = ColorLA<u8>;
	using ColorLAs = ColorLA<u16>;

	struct ColorUtil 
	{
		ColorUtil() = delete;

		static constexpr int pixelSizeInBytes  (ColorType type);
		static constexpr int bytesPerPixelBlock(ColorType type);

		static constexpr ColorElementType	elementType(ColorType t) { return static_cast<ColorElementType>( enumInt(t) & 0xff); }
		static constexpr ColorModel			colorModel (ColorType t) { return static_cast<ColorModel>((enumInt(t) >> 8) & 0xff); }

		static constexpr bool hasAlpha(ColorType t);
	};

	constexpr bool ColorUtil::hasAlpha(ColorType t) {
	auto model = colorModel(t);
	switch (model) {
		case ColorModel::RGBA: return true;
	}
	return false;
	}
	
	constexpr
	int ColorUtil::pixelSizeInBytes(ColorType t) {
		switch (t) {
			case ColorType::RGBAb : return sizeof(ColorRGBAb);
			case ColorType::RGBAs : return sizeof(ColorRGBAs);
			case ColorType::RGBAf : return sizeof(ColorRGBAf);

			case ColorType::Rf    : return sizeof(ColorRf);
			case ColorType::Rb    : return sizeof(ColorRb);
			case ColorType::Rs    : return sizeof(ColorRs);

			case ColorType::Lb    : return sizeof(ColorLb);
			case ColorType::Ls    : return sizeof(ColorLs);
			case ColorType::Lf    : return sizeof(ColorLf);
		}
	
		SGE_ASSERT(false);
		return 0;
	}

//	constexpr int ColorUtil::bytesPerPixelBlock(ColorType type) {
//		switch (type) {
//			#define E(T, ...) \
//				case ColorType::T: return Color##T::kBytesPerPixelBlock;
//			//----------
//				E(BC1)
//				E(BC2)
//				E(BC3)
//				E(BC4)
//				E(BC5)
//				E(BC6h)
//				E(BC7)
//			#undef E
//		}
//		return 0;
//	}
}
