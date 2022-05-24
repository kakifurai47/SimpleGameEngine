#include <sge_core/base/UnitTest.h>
#include <sge_render/vertex/Vertex.h>


#include <typeinfo>

namespace sge {

	void print_VertexType(VertexType type) {
		auto* type_u8 = reinterpret_cast<u8*>(&type.data[0]);
		for (int i = 0; i < array_size(type.formats); i++) {
			SGE_LOG("idx:{} value: {}", i, *type_u8);
			type_u8++;
		}
	}

class Test_Vertex : public UnitTestBase {
public:
	void test_layout() {
		using SMT = Vertex_SemanticType;
		using FMT = Render_FormatType;

		using SmtType = Vertex_SemanticType;
		using FmtType = Render_FormatType;
		VertexType type{};

		type.position	 = (Render_FormatType)0;
		type.color[0]	 = (Render_FormatType)1;
		type.color[1]	 = (Render_FormatType)2;
		type.color[2]	 = (Render_FormatType)3;
		type.color[3]	 = (Render_FormatType)4;
		type.texcoord[0] = (Render_FormatType)0;
		type.texcoord[1] = (Render_FormatType)6;
		type.texcoord[2] = (Render_FormatType)7;
		type.texcoord[3] = (Render_FormatType)8;
		type.texcoord[4] = (Render_FormatType)9;
		type.texcoord[5] = (Render_FormatType)10;
		type.texcoord[6] = (Render_FormatType)11;
		type.texcoord[7] = (Render_FormatType)12;
		type.normal		 = (Render_FormatType)13;
		type.tangent	 = (Render_FormatType)14;
		type.binormal	 = (Render_FormatType)0;

		auto test_fmt = (FmtType)123;

		auto r = VertexType::make(SmtType::Texcoord, test_fmt, 3,
								  SmtType::Texcoord, test_fmt, 2,
								  SmtType::Position, test_fmt, 2,
								  SmtType::Normal,	 test_fmt, 2);

		print_VertexType(r);

		using D0 = VertexElmDescLib::col_c32x4_c1;
		using D1 = VertexElmDescLib::pos_f32x3_c1;
		using D2 = VertexElmDescLib::tex_f32x2_c1;

		using V5 = decltype (meta::cocat(D0{}, D1{}, D2{}, D2{}));;

		auto r3 = VertexType::make(V5{});
		print_VertexType(r3);

		using V6 = VertexLib::PosTexCol;
		auto r4 = V6::kType();

		print_VertexType(r4);
	}

	void printElement(const VertexLayout::Element& elm)
	{
		SGE_DUMP_VAR(static_cast<u64>(elm.format));
		SGE_DUMP_VAR(static_cast<u64>(elm.semantic));
		SGE_DUMP_VAR(static_cast<u64>(elm.smtIdx));
		SGE_DUMP_VAR(static_cast<u64>(elm.offset));
	}


	void create_layout() {
		VertexLayout layout;
		layout.set(VertexLib::PosTexCol{});
		
		//for (const auto& elm : layout.elements) {
		//	printElement(elm);
		//	SGE_LOG("----------------");
		//}
	}

};

} // namespace 

void test_vertex() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vertex, create_layout());
}
