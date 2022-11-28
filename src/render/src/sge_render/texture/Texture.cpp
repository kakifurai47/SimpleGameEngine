
#include "Texture.h"
#include <sge_render/backend/base/Renderer.h>

namespace sge {

	Texture2D::Texture2D(CreateDesc& desc) : Base(desc, Format::Texture2D)
		, m_samplerState(desc.samplerState)
		, m_size(desc.size)
	{
		if (desc.size.x == 0 || desc.size.y == 0) {
			throw SGE_ERROR("creating invalid texture size");
		}
	}

	Texture2D::Stock* Texture2D::Stock::s_instance = nullptr;

	void Texture2D::Stock::create()
	{
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;

		blackTex	= createSolidColorTex({255, 255, 255, 255});
		whiteTex	= createSolidColorTex({0,   0,   0,   255});
		redTex		= createSolidColorTex({255, 0,   0,   255});
		greenTex	= createSolidColorTex({0,   255, 0,   255});
		blueTex		= createSolidColorTex({0,   0,   255, 255});
	 	magentaTex	= createSolidColorTex({255, 0,   255, 255});
	 	errorTex	= createSolidColorTex({255, 0,   255, 255});

		{
			Texture2D_CreateDesc desc;

			desc.imageToUpload.emplace();
			auto& image 	= desc.imageToUpload.value();
			image.load("Assets/Textures/checkermap.png");

			desc.colorType	= image.colorType();
			desc.size		= image.size();

			checkerTex = Renderer::current()->createTexture2D(desc);
		}
	}

	Texture2D::Stock::~Stock() {
		SGE_ASSERT(s_instance == this);
		s_instance = nullptr;
	}


	SPtr<Texture2D> Texture2D::createSolidColorTex(const Color4b& col)
	{
		auto* r = Renderer::current();

		int w = 4;
		int h = 4;

		CreateDesc desc;

		desc.colorType	 = Color4b::kColorType;
		desc.mipmapCount = 1;
		desc.size		 = {w, h};

		desc.imageToUpload.emplace();
		auto& img = desc.imageToUpload.value();
		img.create(Color4b::kColorType, w,  h);
		img.fill(col);

		return r->createTexture2D(desc);
	}

	void Texture2D::uploadToGpu(const Image& newImage)
	{
		if (newImage.size() != m_size || newImage.colorType() != m_colorType) {
			throw SGE_ERROR("Texture2D : different size / colorType");
		}

		if (m_cpuAccess != CpuAccess::Write && m_cpuAccess != CpuAccess::ReadWrite)
		{
			throw SGE_ERROR("Texture2D : try to modifiy readonly texture");
		}

		if (m_usage != Usage::Dynamic && m_usage != Usage::Staging)
		{
			throw SGE_ERROR("Texture2D : try to modifiy readonly texture");
		}

		onUploadToGpu(newImage);
	}


}

