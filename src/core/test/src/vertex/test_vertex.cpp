#include <sge_core/base/UnitTest.h>
#include <sge_render/vertex/Vertex.h>
#include <sge_render/shader/ShaderInfo.h>

#include <typeinfo>

namespace sge {

	void print_VertexType(VertexType type) {
		for (int i = 0; i < array_size(type.formats); i++) {
			SGE_DUMP_VAR(type.formats[i]);
		}
	}

	class Test_Vertex : public UnitTestBase {
	public:
		using ST = VertexSemanticType;
		using FT = RenderFormatType;
		using EL = VertexElmDescLib;
		using UT = RenderFormatTypeUtil;


		void test_renderFormat() {

			using d0 = EL::Pos_Tup3f_C1;
			using d1 = EL::Col_Col4b_C1;
			using d2 = EL::Tex_Tup2f_C1;

			using d3 = EL::Tan_Tup3f_C1;
			using d4 = EL::Norm_Tup3f_C1;
			using d5 = EL::BNorm_tup3f_C1;
			
			using slots		= VertexSlotUtil<d5, d4, d2, d3, d2, d1, d0>;
			using list		= slots::slotList;
			using storage	= slots::storage;
			
			meta::print_list( list{} );
			SGE_LOG("---------------");
			
			using V0 = Vertex<d0, d1, d2, d2, d2, d3, d4, d5, d1>;
			meta::print_list( V0::SlotList{} );
			SGE_LOG("^^^-------------");
			
			V0 v0;
			v0.color   <0>() = {2,3,4,5};
			v0.texcoord<1>() = {2,3};
			v0.binormal<0>() = {1,2,3};
			
			print_VertexType(V0::kType());
			SGE_LOG("----------");
			
			{
				auto type = VertexLib::PosColTex::kType();
				print_VertexType(type);
				SGE_LOG("----------");
			}
			{
				auto type2 = VertexType::make(	ST::BINORMAL, FT::Float16x4,		1,
												ST::POSITION, FT::Float16x4,		1,
												ST::TEXCOORD, FT::Float64x2,		2,
												ST::TEXCOORD, FT::TextureCubeArray, 1,
												ST::TEXCOORD, FT::Int16x2,			1);
				
				print_VertexType(type2);
				SGE_LOG("----------");
			}
			{
				auto type = VertexType::make(ST::POSITION,		UT::get<Tuple3f>(), 1,
											  ST::COLOR,		UT::get<Color4b>(), 0,
											  ST::TEXCOORD,		UT::get<Tuple2f>(), 0,
											  ST::NORMAL,		UT::get<Tuple3f>(), 0,
											  ST::TANGENT,		UT::get<Tuple3f>(), 0,
											  ST::BINORMAL,		UT::get<Tuple3f>(), 0);

				SGE_LOG("printing this <----------------");
				SGE_DUMP_VAR(type == VertexType::None());
				print_VertexType(type);
				SGE_LOG("----------");
			}


			
			VertexLayout layout;
			layout.set(V0{});
			
			SGE_DUMP_VAR(layout.stride);
			
			auto& elms = layout.elements;
			
			for (const auto& e : elms) {
				SGE_DUMP_VAR(e.formatType);
				SGE_DUMP_VAR(e.semantic);
				SGE_DUMP_VAR(e.offset);
			}


		}
	};

} // namespace 

void test_vertex() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vertex, test_renderFormat());
}
