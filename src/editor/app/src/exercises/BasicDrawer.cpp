
#include "BasicDrawer.h"

namespace sge 
{
	void BasicDrawer::create(const Vec2i& canvasSize)
	{
		auto* renderer = Renderer::current();

		int w = canvasSize.x;
		int h = canvasSize.y;

		Texture2D::CreateDesc t2dDesc;

		t2dDesc.colorType	= Color4b::kColorType;
		t2dDesc.mipmapCount = 1;
		t2dDesc.size		= canvasSize;
		t2dDesc.cpuAcess	= Texture2D::CpuAccess::Write;
		t2dDesc.usage		= Texture2D::Usage::Dynamic;

		auto& ss  = t2dDesc.samplerState;
		ss.filter = TextureFilter::Point;

		m_tex = renderer->createTexture2D(t2dDesc);

		m_canvas.create(Color4b::kColorType, w, h);
		m_canvas.fill  (Color4b{ 255, 0, 0, 255 });

		auto shad = renderer->createShader("Assets/Shaders/fullscreen.shader");
		m_fullScreenMat = renderer->createMaterial();
		m_fullScreenMat->setShader(shad);
		m_fullScreenMat->setParam("mainTex", m_tex);
	}

	void BasicDrawer::render(RenderRequest& request)
	{
		m_tex->uploadToGpu(m_canvas);
		request.drawFullScreenTriangle(SGE_LOC, m_fullScreenMat);

		if (m_drawDebug)
		{
			VertexLib::PosCol pts[2];

			auto w = m_canvas.width ();
			auto h = m_canvas.height();

			auto x0 = -1 + 2.0f * ((float)m_startPoint.x + 0.5f) / w;
			auto y0 = -1 + 2.0f * ((float)m_startPoint.y + 0.5f) / h;

			auto x1 = -1 + 2.0f * ((float)m_endPoint.x + 0.5f) / w;
			auto y1 = -1 + 2.0f * ((float)m_endPoint.y + 0.5f) / h;

			pts[0].position<0>() = Vec3f{ x0, y0, 0.0f };
			pts[1].position<0>() = Vec3f{ x1, y1, 0.0f };

			pts[0].color<0>() = Color4b{ 255,0,255,255 };
			pts[1].color<0>() = Color4b{ 255,0,255,255 };

			u16 indicies[] = { 0, 1 };

			request.drawLines(pts, indicies);
		}
	}

	void BasicDrawer::simpleScanLne(Vec2i p0, Vec2i p1)
	{
		m_canvas.fill(m_backgroundCol);

		m_startPoint = p0;
		m_endPoint   = p1;

		auto col = m_lineCol;

		auto h = m_canvas.height();

		if (p0 == p1) {
			m_canvas.pixel<Color4b>(p0.x, h - 1 - p0.y) = col;
			return;
		}

		auto d  = p1 - p0;

		if (Math::abs(d.x) >= Math::abs(d.y)) 
		{
			if (d.x < 0) {
				swap(p0, p1);
				d *= -1;
			}

			auto m = static_cast<float>(d.y) / d.x;
			auto c = p0.y - m * p0.x;

			for (int xi = p0.x; xi <= p1.x; xi++) 
			{
				auto x = xi;
				auto y = h - 1 - Math::roundToInt(m * xi + c);

				m_canvas.pixel<Color4b>(x, y) = col;
			}
		}
		else {
			if (d.y < 0) {
				swap(p0, p1);
				d *= -1;
			} 

			auto m = static_cast<float>(d.x) / d.y;
			auto c = p0.x - m * p0.y;

			for (int yi = p0.y; yi <= p1.y; yi++) 
			{
				auto x = Math::roundToInt(m * yi + c);
				auto y = h - 1 - yi;

				m_canvas.pixel<Color4b>(x, y) = col;
			}
		}
	}

	void BasicDrawer::midpointScanLine(Vec2i p0, Vec2i p1)
	{
		m_canvas.fill(m_backgroundCol);

		auto col = m_lineCol;

		m_startPoint = p0;
		m_endPoint   = p1;

		auto h = m_canvas.height();
		auto d = p1 - p0;

		if (Math::abs(d.x) >= Math::abs(d.y))
		{
			if (d.x < 0) 
			{
				swap(p0, p1);
				d *= -1;
			}

			int x = p0.x;
			int y = p0.y;

			int signY = (p1.y - p0.y) > 0 ? 1 : -1;

			int D	  = 2 * signY * d.y - d.x;
			int incX  = 2 * signY * d.y;
			int incXY = 2 * (signY * d.y - d.x);

			for (;; ) {
				m_canvas.pixel<Color4b>(x, h - 1 - y) = col;

				if (x == p1.x) break;

				if (D <= 0) 
				{
					D += incX;
					x++;
				}
				else {
					D += incXY;
					y += signY;
					x++;
				}
			}
		}
		else 
		{
			if (d.y < 0)
			{
				swap(p0, p1);
				d *= -1;
			}

			int x = p0.x;
			int y = p0.y;

			int signX = (p1.x - p0.x) > 0 ? 1 : -1;

			int D	  = 2 * signX * d.x - d.y;
			int incY  = 2 * signX * d.x;
			int incXY = 2 * (signX * d.x - d.y);

			for (;; )
			{
				m_canvas.pixel<Color4b>(x, h - 1 - y) = col;

				if (y == p1.y) break;

				if (D <= 0) 
				{
					D += incY;
					y++;
				}
				else {
					D += incXY;
					x += signX;
					y++;
				}
			}
		}
	}

	



}
