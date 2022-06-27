#include "VertexLayoutManager.h"

namespace sge {

	VertexLayoutManager* VertexLayoutManager::instance() {
		static VertexLayoutManager s;
		return &s;
	}

	VertexLayoutManager::VertexLayoutManager() {
		using D = VertexLib;
		registerLayout<D::Pos>();
		registerLayout<D::PosCol>();
		registerLayout<D::PosTex>();
		registerLayout<D::PosColTex>();
		registerLayout<D::PosColTexNorm>();
	}


	VertexLayout* VertexLayoutManager::_addLayout(const VertexType& type) {
		auto* layout = getLayout(type);
		if (layout != nullptr) {
			throw SGE_ERROR("adding duplicate layout");
		}
		return &m_table[type];
	}

	const VertexLayout* VertexLayoutManager::getLayout(const VertexType& type) {
		if (type == VertexType::None())
			throw SGE_ERROR("finding empty type");

		auto it = m_table.find(type);
		if (it == m_table.end()) {
			return nullptr;
		}
		return &it->second;
	}
}