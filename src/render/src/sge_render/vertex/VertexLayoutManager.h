#pragma once 

#include "Vertex.h"

namespace sge {

	class VertexLayoutManager : public NonCopyable {
	public:
		static VertexLayoutManager* instance();

		VertexLayoutManager();

			  VertexLayout* _addLayout(const VertexType& type);
		const VertexLayout*  getLayout(const VertexType& type);

		template<class VERTEX>
		void registerLayout() {
			auto* layout = _addLayout(VERTEX::kType());
			layout->set(VERTEX{});
		}

	private:
		Map<VertexType, VertexLayout>  m_table;
	};



}