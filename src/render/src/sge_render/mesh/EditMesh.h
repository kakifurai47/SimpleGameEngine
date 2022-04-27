#pragma once

namespace sge 
{
	class EditMesh : public NonCopyable
	{
	public:
		const int kMaxUv = 8;

		Vector<Vec2f> Uv;


		void loadObj();

	};
}

