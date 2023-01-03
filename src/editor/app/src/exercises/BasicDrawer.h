#pragma once

namespace sge
{
	class BasicDrawer : public NonCopyable
	{

	public:
		void create(const Vec2i& canvasSize);
		void render(RenderRequest&  request);


		void simpleScanLne   (Vec2i p0, Vec2i p1);
		void midpointScanLine(Vec2i p0, Vec2i p1);

	private:
		SPtr<Material>  m_fullScreenMat;
		SPtr<Texture2D> m_tex;
		Image		    m_canvas;

		Color4b		   m_backgroundCol = Color4b{   0,   0, 0, 255 };
		Color4b		   m_lineCol	   = Color4b{ 255, 255, 0, 255 };

		bool		   m_drawDebug = true;

		Vec2i m_startPoint;
		Vec2i m_endPoint;
	};





}