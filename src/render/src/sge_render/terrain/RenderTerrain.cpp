#include "RenderTerrain.h"


#include <sge_render/backend/base/Renderer.h>
#include <sge_render/mesh/MeshBuilder.h>
#include <sge_render/Vertex/VertexLayoutManager.h>


namespace sge 
{
	void RenderTerrain::createFromHeightMapFile(u8 maxLod, StrView heightMapFilename) {
		Image map;
		map.load	 (heightMapFilename);
		createFromHeightMap(maxLod, map);
	}

	void RenderTerrain::createFromHeightMap(u8 maxLod, const Image& heightMap) 
	{
		if (maxLod <= 0 || maxLod > 8) { throw SGE_ERROR("unaccepted lod"); }

		m_maxLod	 = maxLod;

		auto* renderer = Renderer::current();
		auto shader    = renderer->createShader("Assets/Shaders/terrain.shader");

		size_t n   = patchCellsPerRow();
		m_patchInstances.reserve(n * n);

		for (size_t y = 0; y < n; y++) {
			for (size_t x = 0; x < n; x++) {
				auto& p = m_patchInstances.emplace_back();
				p.create(this, shader.ptr());
			}
		}

		m_quadTree.create(this);

		_createVerts(VertexLib::Pos::kType());
		_createIndxs();	

		{
			Texture2D_CreateDesc desc;

			desc.imageToUpload.emplace();
			auto& iamge = desc.imageToUpload.value();
			iamge.copy(heightMap);

			desc.size		   = heightMap.size();
			desc.colorType	   = heightMap.colorType();

			desc.samplerState.filter = TextureFilter::Point;
			desc.samplerState.wrapU  = TextureWrap::Clamp;
			desc.samplerState.wrapV  = TextureWrap::Clamp;

			m_heightMap = renderer->createTexture2D(desc);
		}

	}



	void RenderTerrain::MyQuadTree::create(RenderTerrain* terrain)
	{
		m_terrain = terrain;

		auto maxLod = terrain->maxLod();

		m_startLevelIndices.resize(maxLod);
		m_cellsPerRows.resize(maxLod);

		m_nodes.reserve(MyQuadTree::treeSize(maxLod));
		
		for (u8 i = 0; i < maxLod; i++) 
		{
			u8 level = maxLod - 1 - i; //indexing tree from root to leaf, hence level in reverse order

			m_startLevelIndices[level] = MyQuadTree::treeSize(i);
			m_cellsPerRows[level]	   = Patch::cellsPerRow(i+1);

			auto n = m_cellsPerRows[level];

			for (u8 y = 0; y < n; y++) {
				for (u8 x = 0; x < n; x++) {
					
					auto& node = m_nodes.emplace_back();
					node.level = level;
					node.coord = {x,y};
				}
			}
		}
	}

	Vec3f RenderTerrain::MyQuadTree::Node::pos(MyQuadTree* tree) 
	{
		auto n = tree->m_cellsPerRows[level];
		auto p = (static_cast<Vec2f>(coord) * 2.f + 1.f) / (2.f * n) - 0.5f;
		return Vec3f( p.x, 0.0f, p.y ) * tree->m_cellsPerRows[0];
	}

	//no need to cal every frame -> cache
	float RenderTerrain::MyQuadTree::Node::scale(MyQuadTree* tree) {
		auto d = static_cast<float> ( tree->m_cellsPerRows[0]	  );
		auto s = static_cast<float> ( tree->m_cellsPerRows[level] );
		return d / s;
	}

	//here we assume max lod diff +- 1 with appropriate error matrix;
	int RenderTerrain::MyQuadTree::Node::searchLodDiff(MyQuadTree* tree, Vec2i dir) 
	{
		auto*  ngb = neigbour(tree, dir);
		if  ( !ngb || ngb->selected )  { return 0; }

		auto*  child = ngb->child(tree, 0);
		return child == nullptr ? 1 : child->selected ? -1 : 1;
	}

	void RenderTerrain::MyQuadTree::searchRenderNodes(const Vec3f& viewPos, Node* source) {

		if (!source->hasChild()) {
			source->selected = true;
			m_renderNodes.emplace_back(source);
			return;
		}		
		auto s = m_terrain->m_terrainSize;
		auto d = (source->pos(this) * s - viewPos).length();
		auto e = s * source->level / d;
		
		if (e < 0.25f) {
			source->selected = true;
			m_renderNodes.emplace_back(source);
			return;
		}

		for (u8 i = 0; i < 4; i++) {
			searchRenderNodes(viewPos, source->unsafeChild(this, i));
		}
	}


	void RenderTerrain::MyQuadTree::searchRenderNodes(const Vec3f& viewPos) 
	{
		for (auto* n : m_renderNodes) {
			n->selected = false;
		}
		m_renderNodes.clear();

		searchRenderNodes(viewPos, unsafeRoot());
	}

	void RenderTerrain::MyQuadTree::resetRenderNodes() {
		for (auto* n : m_renderNodes) {
			n->selected = false;
		}
		m_renderNodes.clear();
	}

	void RenderTerrain::patchInstance::create(Terrain* terrain, Shader* shad) {
		auto* renderer    = Renderer::current();
		m_material = renderer->createMaterial();
		m_material->setShader(shad);
		m_terrain = terrain;
	}


	void RenderTerrain::_createVerts(const VertexType& type)
	{
		MeshBuilder builder;

		auto n		 = patchVerticesPerRow();
		auto vtCount = n * n;

		auto* vtLayout = VertexLayoutManager::instance()->getLayout(type);
		if  (!vtLayout) {
			throw SGE_ERROR("terrain : missing vertex layout");
		}

		builder.resetVertices(vtCount, vtLayout);
		auto pos	= builder.pos(0);
		auto d		= 1.f  / (n - 1);

		for (int y = 0; y < n; y++) {
			for (int x = 0; x < n; x++) {
				pos.add(Tuple3f(x * d - 0.5f, 0, y * d - 0.5f));
			}
		}

		builder.createVertexResult(m_vertexLayout, m_vertexCount, m_vertexBuffer);
	}

	void RenderTerrain::_createIndxs()
	{
		MeshBuilder builder;

		m_patches.clear();
		m_patches.resize(Patch::permuntation);

		auto mask = Patch::StitchMask::None;

		for (auto& it : m_patches) {
			it.create(this, builder, m_maxLod, 0, mask);
			mask += 1;
		}
		
	}

	void RenderTerrain::Patch::create(const Terrain* terrain, MeshBuilder& builder, u8 maxLod, u8 lod, StitchMask mask) 
	{
		m_terrain		 = terrain;

		int rows		= Patch::cellsPerRow(maxLod-lod);
		int vertsPerRow	= terrain->patchVerticesPerRow();
		int idxCount	= rows * rows * 6;

		BuildArray idx;

		if (rows == 1) 
		{
			builder.resetIndices(idxCount);
			idx = builder.index();

			int x0 = 0;
			int x1 = vertsPerRow - 1;
			int y0 = 0;
			int y1 = vertsPerRow * (vertsPerRow - 1);

			idx.add(x0 + y0); idx.add(x1 + y1); idx.add(x1 + y0);
			idx.add(x0 + y0); idx.add(x0 + y1); idx.add(x1 + y1);
		}
		else
		{
			int n			= rows  / 2;
			int d			= n * 3;
			int maxLodToLod = 1  << lod;

			bool stitchMaskHasN = enumHas(mask, StitchMask::N); if(stitchMaskHasN) idxCount -= d;
			bool stitchMaskHasS = enumHas(mask, StitchMask::S); if(stitchMaskHasS) idxCount -= d;
			bool stitchMaskHasE = enumHas(mask, StitchMask::E); if(stitchMaskHasE) idxCount -= d;
			bool stitchMaskHasW = enumHas(mask, StitchMask::W); if(stitchMaskHasW) idxCount -= d;

			builder.resetIndices(idxCount);
			idx = builder.index();

			Vector<Vec2i, 256> sector0; //lod n
			Vector<Vec2i, 256> sector1; //lod n+1

			for (int y = 0; y < n; y++)
			{
				int lastRow = (y == n - 1) ? 1 : 0;
	
				for (int x = 0; x <= y; x++)
				{
					int odd = (x + y) % 2;
	
					// D-----E-----F
					// |     |     |
					// |     |     |
					// A-----B-----C
					// odd   even
	
					Vec2i v[3];
	
					v[0] = Vec2i( x,	 y			 ) * maxLodToLod;
					v[1] = Vec2i( x,	 y + 1		 ) * maxLodToLod;
					v[2] = Vec2i( x + 1, y + 1 - odd ) * maxLodToLod;
					//even : BEF, odd : ADB

					sector0.appendRange(v);

					if (lastRow) {
						v[1].x = (x - 1 + odd) * maxLodToLod;
						//even : BDF, odd : ADB
					}
					sector1.appendRange(v);
	
					if (x == y) break;
	
					v[0] = Vec2i( x + 1, y + 1	 ) * maxLodToLod;
					v[1] = Vec2i( x + 1, y		 ) * maxLodToLod;
					v[2] = Vec2i( x,	 y + odd ) * maxLodToLod;
					//even : FCB, odd : EBD
	
					sector0.appendRange(v);

					if ( !lastRow || !odd )
					{
						sector1.appendRange(v);
					}
				}
			}

			{
				auto& sector = stitchMaskHasN ?  sector1 : sector0;
				bool  skipS2 = stitchMaskHasN && rows == 2;

							_addToIndices(idx, sector, vertsPerRow, Vec2i( 1, 1), false); 
				if(!skipS2) _addToIndices(idx, sector, vertsPerRow, Vec2i(-1, 1), false); 
			}
			{
				auto& sector = stitchMaskHasS ?  sector1 : sector0;
				bool  skipS2 = stitchMaskHasS && rows == 2;

							_addToIndices(idx, sector, vertsPerRow, Vec2i(-1,-1), false); 
				if(!skipS2) _addToIndices(idx, sector, vertsPerRow, Vec2i( 1,-1), false); 
			}

			{
				auto& sector = stitchMaskHasE ?  sector1 : sector0;
				bool  skipS2 = stitchMaskHasE && rows == 2;

							_addToIndices(idx, sector, vertsPerRow, Vec2i( 1,-1),  true); 
				if(!skipS2) _addToIndices(idx, sector, vertsPerRow, Vec2i( 1, 1),  true); 
			}
			{
				auto& sector = stitchMaskHasW ?  sector1 : sector0;
				bool  skipS2 = stitchMaskHasW && rows == 2;

							_addToIndices(idx, sector, vertsPerRow, Vec2i(-1, 1),  true); 
				if(!skipS2) _addToIndices(idx, sector, vertsPerRow, Vec2i(-1,-1),  true); 
			}
		}

		SGE_ASSERT(idxCount == idx.size());

		builder.createIndexResult(m_indexFormat, m_indexCount, m_indexBuffer);
	}

	void RenderTerrain::Patch::_addToIndices(BuildArray& idx, Span<Vec2i> coords, int vertsPerRow, const Vec2i& dir, bool flipXY)
	{
		auto halfW   = vertsPerRow	 / 2;
		auto vtCount = coords.size() / 3;

		int rx  = dir.x < 0 ? 1 : 0;
		int ry  = dir.y < 0 ? 1 : 0;
		int rf  = flipXY	? 1 : 0;

		bool flipped = rx ^ ry ^ rf;

		for (size_t i = 0; i < vtCount; i++) 
		{
			struct Helper {
				static inline int _toIndex(int halfWidth, const Vec2i& coord, int vertsPerRow, const Vec2i& dir, bool flipXY)
				{
					auto c = flipXY ? coord.yx() : coord;
					c *= dir;
					c += halfWidth;
					return vertsPerRow * c.y + c.x;
				}
			};

			auto i0 = Helper::_toIndex( halfW, coords[i * 3 + 0], vertsPerRow, dir, flipXY );
			auto i1 = Helper::_toIndex( halfW, coords[i * 3 + 1], vertsPerRow, dir, flipXY );
			auto i2 = Helper::_toIndex( halfW, coords[i * 3 + 2], vertsPerRow, dir, flipXY );

			if (!flipped) {
				idx.add(i0); idx.add(i1); idx.add(i2);
			}
			else {
				idx.add(i1); idx.add(i0); idx.add(i2);
			}
		}
	}
	

	void RenderTerrain::patchInstance::render(RenderRequest& request, MyQuadTree::Node* node) {

		request.setMaterialCommonParams(m_material);

		auto mask = Patch::StitchMask::None;
		if( node->searchLodDiff(m_terrain->quadTree(), Vec2i( 0,  1)) > 0 ) mask |= Patch::StitchMask::N;
		if( node->searchLodDiff(m_terrain->quadTree(), Vec2i( 0, -1)) > 0 ) mask |= Patch::StitchMask::S;
		if( node->searchLodDiff(m_terrain->quadTree(), Vec2i( 1,  0)) > 0 ) mask |= Patch::StitchMask::E;
		if( node->searchLodDiff(m_terrain->quadTree(), Vec2i(-1,  0)) > 0 ) mask |= Patch::StitchMask::W;

		auto& buffer = request.commandBuffer;
		auto& patch  = m_terrain->patches()[enumInt(mask)];
		auto  passes = m_material->passes();

		for (size_t i = 0; i < passes.size(); i++)
		{
			auto* command = buffer.newCommand<RenderCommand_DrawCall>();

			auto l = node->level;
			auto s = node->scale(m_terrain->quadTree());
			auto p = node->pos  (m_terrain->quadTree());

			m_material->setParam("terrainHeightMap",  m_terrain->m_heightMap);
			m_material->setParam("terrainSize",	    m_terrain->m_terrainSize);
			m_material->setParam("terrainHeight", m_terrain->m_terrainHeight);
			m_material->setParam("patchLevel", l);
			m_material->setParam("patchPos",   p);
			m_material->setParam("patchSize",  s);

		#if _DEBUG
			command->debugLoc			= SGE_LOC;
		#endif
			command->primitive			= RenderPrimitiveType::Triangles;
			command->vertexCount		= m_terrain->vertexCount ();
			command->vertexLayout		= m_terrain->vertexLayout();
			command->vertexBuffer		= m_terrain->vertexBuffer();

			command->indexCount			= patch.indexCount ();
			command->indexFormat		= patch.indexFormat();
			command->indexBuffer		= patch.indexBuffer();

			command->material			= m_material.ptr();
			command->materialPassIndex	= i;
		}
	}

	void RenderTerrain::render(RenderRequest& request)
	{
		m_quadTree.searchRenderNodes(request.camera_pos);

		auto renderNodes = m_quadTree.renderNodes();

		for (size_t i = 0; i < renderNodes.size(); i++)
		{
			m_patchInstances[i].render(request, renderNodes[i]);
		}
	}
}
