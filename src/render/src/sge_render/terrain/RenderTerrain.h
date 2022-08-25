#pragma once

#include <sge_render/command/RenderRequest.h>

namespace sge {

#define RenderTerrain_PatchStitchMask_ENUM_LIST(E_) \
	E_(None, = 0) \
	E_(N,	 = 1 << 0) \
	E_(S,	 = 1 << 1) \
	E_(E,	 = 1 << 2) \
	E_(W,	 = 1 << 3) \
	\
//-----
	SGE_ENUM_CLASS(RenderTerrain_PatchStitchMask,  : u8)
	SGE_ENUM_ALL_OPERATOR(RenderTerrain_PatchStitchMask)

	class MeshBuilder;
	class MeshBuilder_BuildArray;

	class RenderTerrain : public NonCopyable {
	public:
		using Terrain		 = RenderTerrain;

		class MyQuadTree {
		public:
			struct Node {
				Vec2b	coord      {0 ,0};
				u8		level        = 0 ;
				bool    selected = false ;

				bool  isRoot	()	{ return level == 0 && coord.equals0(); }
				bool  hasChild  ()  { return level > 0; }
				
				SGE_INLINE Node* unsafeParent	(MyQuadTree* tree)				{ return tree->unsafeNode(level + 1, coord.x >> 1,				coord.y >> 1			); }
				SGE_INLINE Node* unsafeChild	(MyQuadTree* tree, u8  index)	{ return tree->unsafeNode(level - 1, coord.x * 2 + index % 2,	coord.y * 2 + index / 2	); }
				SGE_INLINE Node* unsafeNeigbour	(MyQuadTree* tree, Vec2i dir)	{ return tree->unsafeNode(level,	 coord.x + dir.x,			coord.y + dir.y			); }

				SGE_INLINE Node* parent			(MyQuadTree* tree)				{ return tree->node		 (level + 1, coord.x >> 1,				coord.y >> 1			); }
				SGE_INLINE Node* child			(MyQuadTree* tree, u8  index)	{ return tree->node		 (level - 1, coord.x * 2 + index % 2,	coord.y * 2 + index / 2	); }
				SGE_INLINE Node* neigbour		(MyQuadTree* tree, Vec2i dir)	{ return tree->node		 (level,	 coord.x + dir.x,			coord.y + dir.y			); }

				int searchLodDiff(MyQuadTree* tree, Vec2i dir);

				Vec3f pos	(MyQuadTree* tree);
				float scale	(MyQuadTree* tree);
			};

			static SGE_INLINE u16 treeSize(u8 maxLod) { //return 0 + 1 + ... + 2 ^(2 * n);
				return ( (1 << 2 * maxLod) - 1 ) / 3; 
			}

			SGE_INLINE bool  checkInRange(size_t i)						{ return i >= 0 && i < m_nodes.size(); }

			SGE_INLINE u8	 maxLod		 ()								{ return m_terrain->maxLod(); }
			SGE_INLINE int   atlasId	 (int level, int cx, int cy)	{ return m_startLevelIndices[level] + m_cellsPerRows[level] * cy + cx; }
										 
			SGE_INLINE Node* unsafeRoot	 ()								{ return &m_nodes[0]; }
			SGE_INLINE Node* unsafeNode	 (int level, int cx, int cy)	{ return &m_nodes[atlasId(level, cx, cy)]; }

			SGE_INLINE Node* root		 ()								{ return node(0,0,0); }
			SGE_INLINE Node* node		 (int level, int cx, int cy)
			{
				if (level < 0 || level >= maxLod()) {
					return nullptr;
				}
				auto i = atlasId(level, cx, cy);
				return checkInRange(i) ? &m_nodes[atlasId(level, cx, cy)] : nullptr; 
			}

			void create(RenderTerrain* terrain);

			void searchRenderNodes(const Vec3f& viewPos);
			void searchRenderNodes(const Vec3f& viewPos, Node* source);

			void resetRenderNodes  ();

			Span<Node>  nodes	   () { return m_nodes;		  }
			Span<Node*> renderNodes() { return m_renderNodes; }
		
		protected:

			RenderTerrain* m_terrain = nullptr;
			Vector<Node>   m_nodes;
			Vector<u16, 7> m_startLevelIndices;
			Vector<u16, 7> m_cellsPerRows;
			Vector<Node*>  m_renderNodes;
		};

		class Patch  {
			using BuildArray	  = MeshBuilder_BuildArray;
		public:
			using StitchMask	  = RenderTerrain_PatchStitchMask;

			static constexpr u8 permuntation = 16;

			SGE_INLINE static u16		cellsPerRow(u8 maxLod) { return 1 << (maxLod - 1); }

			SGE_INLINE RenderFormatType	indexFormat	() { return m_indexFormat;	}
			SGE_INLINE size_t			indexCount	() { return m_indexCount;	}
			SGE_INLINE RenderGpuBuffer* indexBuffer () { return m_indexBuffer;  }

			void create(const Terrain* terrain, MeshBuilder& builder, u8 maxLod, u8 lod, StitchMask mask);

		protected:

			void _addToIndices(BuildArray& idx, Span<Vec2i> coords, int vertsPerRow, const Vec2i& dir, bool flipXY);

			const Terrain*		  m_terrain		 = nullptr;

			RenderFormatType	  m_indexFormat  = RenderFormatType::None;
			size_t				  m_indexCount	 = 0;

			SPtr<RenderGpuBuffer> m_indexBuffer;
		};

		class patchInstance {
		public:
			void create(RenderTerrain* terrain, Shader* shad);
			void render(RenderRequest& request, MyQuadTree::Node* node);

		private:
			RenderTerrain* m_terrain = nullptr;
			SPtr<Material> m_material;
		};

		void createFromHeightMapFile(u8 maxLod,		  StrView heightMapFilename);
		void createFromHeightMap	(u8 maxLod, const Image&  heightMap);

		void render(RenderRequest& request);

		int patchCellsPerRow	() const { return Patch::cellsPerRow(m_maxLod); }
		int patchVerticesPerRow () const { return patchCellsPerRow() + 1;		}
		u8  maxLod				() const { return m_maxLod;						}

		const VertexLayout*	 vertexLayout() const { return m_vertexLayout; }
		size_t				 vertexCount () const { return m_vertexCount;  }
		RenderGpuBuffer*	 vertexBuffer()		  { return m_vertexBuffer; }


		MyQuadTree* quadTree() { return &m_quadTree; }
		Span<Patch>	patches () { return m_patches;   }

	private:

		void	_createVerts(const VertexType& type);
		void	_createIndxs();

		Vec2i					  m_patchCount {0, 0};
		u8						  m_maxLod    = 0;
								  
		const VertexLayout*		  m_vertexLayout = nullptr;
		size_t					  m_vertexCount  = 0;
								  
		SPtr<RenderGpuBuffer>	  m_vertexBuffer;
		Vector<Patch, 16>		  m_patches;
								  
		SPtr<Texture2D>			  m_heightMap;
		SPtr<Material>			  m_material;
								  
		MyQuadTree				  m_quadTree;
		Vector<patchInstance>	  m_patchInstances;

		u8						  m_patchLod	  = 5;
		float					  m_terrainSize   = 10.0f;
		float					  m_terrainHeight = 10.0f;
	};
}