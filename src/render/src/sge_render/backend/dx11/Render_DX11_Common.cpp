#include "Render_DX11_Common.h"

namespace sge {
	VertexSemanticType DX11Util::parseDxSemanticName(StrView s) {
		VertexSemanticType ret;

		if (s == "SV_POSITION") {
			return VertexSemanticType::POSITION;
		}

		if (!enumTryParse(ret, s)) {
			throw SGE_ERROR("unknown Vertex Semantic Type {}", s);
		}
		return ret;
	}







}
