#pragma once

#include <sge_core/parser/Lexer2.h>
#include <sge_render/mesh/EditMesh.h>

namespace sge {

	class WavefrontObjLoader : public Lexer2 {
	public:
		void loadFile(EditMesh& outMesh, StrView filename);
		void loadMem (EditMesh& outMesh, StrView filename, ByteSpan src);

	private:
		void _parseObj	(EditMesh& outMesh);

		void _parse_v	();
		void _parse_vt	();
		void _parse_vn	();
		void _parse_f	(EditMesh& outMesh);
		
		template<class T> void _parse_number    (T&   out);

		u32 _addVertex(EditMesh& outMesh, int vi, int vt, int vn);

		Map<u128, u32>	m_indexTable;

		Vector<Tuple3f>	m_tmpPos;
		Vector<Tuple2f>	m_tmpUv;
		Vector<Tuple3f>	m_tmpNormal;
	};
}