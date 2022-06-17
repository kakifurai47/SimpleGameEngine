#include "WavefrontObjLoader.h"
#include <sge_core/base/sge_base.h>

namespace sge {
	void WavefrontObjLoader::loadFile(EditMesh& outMesh, StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(outMesh, filename, mm);
	}

	void WavefrontObjLoader::loadMem(EditMesh& outMesh, StrView filename, ByteSpan src) {
		outMesh.clear();
		reset(src, filename);
		_parseObj(outMesh);

	}

	void WavefrontObjLoader::_parseObj(EditMesh& outMesh) {
		size_t call_v = 0;
		size_t call_f = 0;

		for(;;) {
			skipNewlineTokens();
			if (m_token.isEnd())			{ return; }

			if (m_token.isIdentifier("v" )) { _parse_v ();		  continue; }
			if (m_token.isIdentifier("vt")) { _parse_vt();		  continue; }
			if (m_token.isIdentifier("vn")) { _parse_vn();		  continue; }
			if (m_token.isIdentifier("f" )) { _parse_f (outMesh); continue; }
			return errorUnexpectedToken();
		}
	}

	template<class T>
	void WavefrontObjLoader::_parse_number(T& out) {
		bool isNegative = false;
		if (m_token.isOperator("-")) {
			isNegative = true;
			nextToken();
		}
		if (!m_token.isNumber() || !StringUtil::tryParse(m_token.str, out)) {
			error("invalid number", m_token.str);
		}
		if (isNegative) out *= -1;
	}

	void WavefrontObjLoader::_parse_v() {
		Tuple4f value { 0,0,0,1 };

		for (size_t i = 0; i < 4; i++) {
			nextToken();
			if (m_token.isNewLine()) {
				if (i == 3) break;
				errorUnexpectedToken();
			}
			_parse_number(value[i]);
		}
		auto  w	  = value.w;
		auto& pos = m_tmpPos.emplace_back();
		pos = { value.x / w, value.y / w , value.z / w };
	}

	void WavefrontObjLoader::_parse_vt() {
		Tuple2f value{};
		for (size_t i = 0; i < 2; i++) {
			nextToken();
			_parse_number(value[i]);
		}
		m_tmpUv.emplace_back(value);
	}

	void WavefrontObjLoader::_parse_vn() {
		Tuple3f value{};
		for (size_t i = 0; i < 3; i++) {
			nextToken();
			_parse_number(value[i]);
		}
		m_tmpNormal.emplace_back(value);
	}

	u32 WavefrontObjLoader::_addVertex(EditMesh& outMesh, int vi_, int vt_, int vn_) {
		if (vi_ == 0) {
			_error("vertex w/o position");
		}

		bool hasUv	   = vt_ != 0;
		bool hasNormal = vn_ != 0;

		if (outMesh.pos.size() >= 0) {
			if(!hasUv	  && outMesh.uv[0].size()  > 0) _error("incoherent vertex layout : missing vn");
			if(!hasNormal && outMesh.normal.size() > 0) _error("incoherent vertex layout : missing vt");
		}

		u32 vi = vi_ < 0 ? vi_ + static_cast<u32>(	   m_tmpPos.size()	) : vi_ - 1;
		u32 vt = vt_ < 0 ? vt_ + static_cast<u32>(		m_tmpUv.size()	) : vt_ - 1;
		u32 vn = vn_ < 0 ? vn_ + static_cast<u32>(	m_tmpNormal.size()	) : vn_ - 1;

		u128 hash { 0,	vi + 1, vt + 1, vn + 1 };

		auto it = m_indexTable.find(hash);
		if (it != m_indexTable.end()) {
			return it->second;
		}

		size_t c = outMesh.pos.size();

		if (c >= std::numeric_limits<u32>::max()) {
			_error("exceeded vertex count");
		}
		auto& idx = m_indexTable[hash];
		idx = static_cast<u32>(c);

		if (vi >= m_tmpPos.size())  _error("invalid vi");
		outMesh.pos.emplace_back(m_tmpPos[vi]);
		
		if (hasUv) {
			if (vt >= m_tmpUv.size()) _error("invalid uv");
			outMesh.uv[0].emplace_back(m_tmpUv[vt]);
		}

		if (hasNormal) {
			if (vn >= m_tmpNormal.size()) _error("invalid normal");
			outMesh.normal.emplace_back(m_tmpNormal[vn]);
		}
		return idx;
	}

	void WavefrontObjLoader::_parse_f(EditMesh& outMesh) {
		nextToken();
		Vector_<int, 64> face_idx;

		for (;;) {
			if (m_token.isNewLine() || m_token.isEnd()) {
				break;
			}
			int vi = 0, vt = 0, vn = 0;
			for (;;) {
				_parse_number(vi); nextToken(); if (!m_token.isOperator("/")) { break; } else{ nextToken(); }
				_parse_number(vt); nextToken(); if (!m_token.isOperator("/")) { break; } else{ nextToken(); }
				_parse_number(vn); nextToken(); break;
			}
			auto idx = _addVertex(outMesh, vi, vt, vn);
			face_idx.emplace_back(idx);
		}

		if (face_idx.size() < 3) {
			_error("face size < 3");
		}

		for (size_t i = 0; i < face_idx.size(); i++) {
			if (i >= 3) {
				outMesh.indicies.emplace_back(	static_cast<u32>(face_idx[0])	);
				outMesh.indicies.emplace_back(	static_cast<u32>(face_idx[0 + i - 1])	);
			}
			outMesh.indicies.emplace_back(face_idx[i]);
		}
	}
}
