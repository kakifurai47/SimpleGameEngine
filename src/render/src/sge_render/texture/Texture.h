#pragma once

#include <sge_render/backend/base/RenderFormat.h>
#include "Image.h"

namespace sge {

#define TextureUsage_ENUM_LIST(E) \
	E(None,) \
	E(Default,) \
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

#define TextureCpuAccess_ENUM_LIST(E) \
	E(None,) \
	E(NoAccess,) \
	E(Read,) \
	E(Write,) \
	E(ReadWrite,) \
//------
	SGE_ENUM_CLASS(TextureCpuAccess, : u8)

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
		using Format	= RenderFormatType;
		using Usage		= TextureUsage;
		using CpuAccess = TextureCpuAccess;

		ColorType	colorType	= ColorType::RGBAb;
		Usage		usage		= Usage::Default;
		CpuAccess	cpuAcess	= CpuAccess::NoAccess;
		size_t		mipmapCount	= 1;
	};

	class Texture : public RefCountBase {
	public:
		using Usage		 = TextureUsage;
		using Format	 = RenderFormatType;
		using CpuAccess  = TextureCpuAccess;

		using CreateDesc = Texture_CreateDesc;

		ColorType	colorType	() const { return m_colorType;	 }
		Usage		usage		() const { return m_usage;		 }
		Format		format		() const { return m_format;		 }
		size_t		mipmapCount () const { return m_mipmapCount; }

	protected:

		Texture(CreateDesc& desc, Format format)
			: m_colorType(desc.colorType)
			, m_usage(desc.usage)
			, m_cpuAccess(desc.cpuAcess)
			, m_format(format)
			, m_mipmapCount(desc.mipmapCount)
		{
		}

		ColorType	m_colorType		= ColorType::None;
		Usage		m_usage			= Usage::None;
		CpuAccess	m_cpuAccess		= CpuAccess::NoAccess;
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
		struct Stock 
		{
			static Stock* instance() {
				SGE_ASSERT(s_instance != nullptr);
				return s_instance; 
			}

			void create();

			~Stock();

			SPtr<Texture2D>   blackTex;
			SPtr<Texture2D>   whiteTex;
			SPtr<Texture2D>     redTex;
			SPtr<Texture2D>   greenTex;
			SPtr<Texture2D>    blueTex;
			SPtr<Texture2D>	magentaTex;
			SPtr<Texture2D>	  errorTex;
			SPtr<Texture2D>	checkerTex;

		private:
			static Stock* s_instance;
		};

		using CreateDesc = Texture2D_CreateDesc;

		static Texture2D* black	 () { return Stock::instance()->  blackTex; };
		static Texture2D* white	 () { return Stock::instance()->  whiteTex; };
		static Texture2D* red	 () { return Stock::instance()->    redTex; };
		static Texture2D* green	 () { return Stock::instance()->  greenTex; };
		static Texture2D* blue	 () { return Stock::instance()->   blueTex; };
		static Texture2D* magenta() { return Stock::instance()->magentaTex; };
		static Texture2D* error	 () { return Stock::instance()->  errorTex; };
		static Texture2D* checker() { return Stock::instance()->checkerTex; };

		static SPtr<Texture2D> createSolidColorTex(const Color4b& col);

		void uploadToGpu(const Image& newImage);

	protected:

		virtual void onUploadToGpu(const Image& newImage) {};

		Texture2D(CreateDesc& desc);

		SamplerState m_samplerState;
		Vec2i		 m_size{ 0, 0 };
	};
}

