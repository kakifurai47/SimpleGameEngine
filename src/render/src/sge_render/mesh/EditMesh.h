#pragma once

namespace sge 
{
	class EditMesh : public NonCopyable
	{
	public:
		static const u8 kUvCountMax = 4;

		Vector<u32> indicies;
		
		Vector<Tuple3f> pos;
		Vector<Tuple2f> uv[kUvCountMax];
		Vector<Color4b> color;

		Vector<Tuple3f> normal;
		Vector<Tuple3f> tangent;
		Vector<Tuple3f> binormal;

		void loadObj(StrView filename);
		void clear();
	};
}

