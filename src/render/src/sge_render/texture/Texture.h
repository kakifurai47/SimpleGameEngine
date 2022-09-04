#pragma once

#include <sge_render/backend/base/RenderFormat.h>
#include "Image.h"

namespace sge {

#define TextureUsage_ENUM_LIST(E) \
	E(None,) \
	E(Staging,) \
	E(Dynamic,)	\
	E(Immutable,) \
//------
	SGE_ENUM_CLASS(TextureUsage, : u8);

#define TextureWrap_ENUM_LIST(E) \
	E(None,) \
	E(Repeat,) \
	E(Clamp,) \
	E(Mirror,) \
	E(MirrorOnce,) \
//------	
	SGE_ENUM_CLASS(TextureWrap, : u8)


#define TextureFilter_ENUM_LIST(E) \
	E(None,)  \
	E(Point,)  \
	E(Linear,)  \
	E(Bilinear,) \
	E(Trilinear,) \
	E(Anisotropic,)\
//------
	SGE_ENUM_CLASS(TextureFilter, : u8)

	struct SamplerState {
	public:
		using Filter = TextureFilter;
		using Wrap	 = TextureWrap;
		
		Filter	filter = Filter::Bilinear;
		Wrap	wrapU  = Wrap::Repeat;
		Wrap	wrapV  = Wrap::Repeat;
		Wrap	wrapW  = Wrap::Repeat;

		float	minLOD = 0;
		float	maxLOD = Math::inf<float>();
	};
	
	struct Texture_CreateDesc {	
		using Format = RenderFormatType;
		using Usage	 = TextureUsage;

		ColorType	colorType	= ColorType::RGBAb;
		Usage		usage		= Usage::None;
		size_t		mipmapCount	= 1;
	};

	class Texture : public RefCountBase {
		using Usage	 = TextureUsage;
		using Format = RenderFormatType;
	public:
		using CreateDesc = Texture_CreateDesc;

		ColorType	colorType	() { return m_colorType;	}
		Usage		usage		() { return m_usage;		}
		Format		format		() { return m_format;		}
		size_t		mipmapCount () { return m_mipmapCount;	}

	protected:
		Texture(CreateDesc& desc, Format format)
			: m_colorType(desc.colorType)
			, m_usage(desc.usage)
			, m_format(format)
			, m_mipmapCount(desc.mipmapCount)
		{
		}

		ColorType	m_colorType		= ColorType::None;
		Usage		m_usage			= Usage::None;
		Format		m_format		= Format::None;
		size_t		m_mipmapCount	= 1;
	};

	struct Texture2D_CreateDesc : public Texture_CreateDesc {
		Opt<Image>	 imageToUpload;
		SamplerState samplerState;
		Vec2i		 size{ 0, 0 };
	};


	class Texture2D : public Texture {
		using Base   = Texture;
		using Format = RenderFormatType;
	public:
		using CreateDesc = Texture2D_CreateDesc;
		
	protected:
		Texture2D(CreateDesc& desc);

		SamplerState m_samplerState;
		Vec2i		 m_size{ 0, 0 };
	};
}

