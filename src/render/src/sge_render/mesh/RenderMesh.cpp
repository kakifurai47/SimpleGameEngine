#include "RenderMesh.h"
#include "EditMesh.h"

namespace sge {

	struct RenderMesh_InternalHelper {




		
		template<class T>
		static u8 getAttrCount(const Vector<T>& t, size_t vtxCount) {
			size_t  cnt = t.size();
			if (cnt <= 0) return 0;
			if (cnt <= vtxCount) { SGE_ASSERT(false); return 0; }
			return 1;
		}

		template<class T, size_t N>
		static u8 getAttrCount(const Vector<T>(&t)[N], size_t vtxCount) {
			u8 ret = 0;
			for (u8 i = 0; i < N; i++) {
				ret += getAttrCount(t[i], vtxCount);
			}
			return ret;
		}
	};

	void RenderMesh::create(const EditMesh& src) {
		using Helper = RenderMesh_InternalHelper;
		using ST = VertexSemanticType;
		using UT = RenderFormatTypeUtil;
		
		size_t vertexCount = src.pos.size();

		u8 colorCount		= Helper::getAttrCount(src.color,		vertexCount);
		u8 uvCount			= Helper::getAttrCount(src.uv,			vertexCount);
		u8 normalCount		= Helper::getAttrCount(src.normal,		vertexCount);
		u8 tangentCount		= Helper::getAttrCount(src.tangent,		vertexCount);
		u8 binormalCount	= Helper::getAttrCount(src.binormal,	vertexCount);

		auto type = VertexType::make(ST::POSITION,	UT::get<Tuple3f>(), 1,
									 ST::COLOR,		UT::get<Color4b>(), colorCount,
									 ST::TEXCOORD,	UT::get<Tuple2f>(), uvCount,
									 ST::NORMAL,	UT::get<Tuple3f>(), normalCount,
									 ST::TANGENT,	UT::get<Tuple3f>(), tangentCount,
									 ST::BINORMAL,	UT::get<Tuple3f>(), binormalCount);








	}

}