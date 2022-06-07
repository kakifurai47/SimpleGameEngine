#include <sge_core/base/UnitTest.h>
#include <sge_render/shader/ShaderInfo.h>
#include <sge_core/serializer/json/JsonUtil.h>

namespace sge {



	enum class MyEnum {
		A, B, C,
	};

	#define MyEnum_ENUM_LIST(E) \
		E(A) \
		E(B) \
		E(C) \

	SGE_ENUM_STR_UTIL(MyEnum)

	struct TestNestedStruct {
		u64 myUint8A;
		bool myBoolA = false;

		Vector_<bool, 3> MyBoolVec;
	};

	struct TestStruct {
		String myStringA;
		String myStringB;
		String myStringC;

		Vector_<u8, 3> myVectorA;

		MyEnum myEnum;

		u64 myUint64A;
		Vector_<TestNestedStruct, 3> myNestedStructVec;

	};

	template<class SE>
	void onSerDes(SE& se, TestStruct& st) {
		SGE_SERDES_IO(se, st, myStringC);
		SGE_SERDES_IO(se, st, myStringB);
		SGE_SERDES_IO(se, st, myVectorA);
		SGE_SERDES_IO(se, st, myUint64A);
		SGE_SERDES_IO(se, st, myStringA);
		SGE_SERDES_IO(se, st, myNestedStructVec);
		SGE_SERDES_IO(se, st, myEnum);
	}

	template<class SE>
	void onSerDes(SE& se, TestNestedStruct& st) {
		SGE_SERDES_IO(se, st, myUint8A);
		SGE_SERDES_IO(se, st, myBoolA);
		SGE_SERDES_IO(se, st, MyBoolVec);
	}

	class Test_JsonSerializer : public UnitTestBase {
	public:
		void test_se() {
			{
				TestStruct testStruct;
				testStruct.myStringA = "ABC";
				testStruct.myStringB = "YUI";
				testStruct.myStringC = "XYZ";

				auto& vec = testStruct.myVectorA;
				vec.emplace_back(1);
				vec.emplace_back(2);
				vec.emplace_back(3);

				testStruct.myUint64A = 123;

				auto& a = testStruct.myNestedStructVec.emplace_back();
				a.myBoolA = true;
				a.myUint8A = 1024;
				a.MyBoolVec = { true, false, true };

				auto& b = testStruct.myNestedStructVec.emplace_back();
				b.myBoolA = false;
				b.myUint8A = 789;
				b.MyBoolVec = { false, true, true };

				testStruct.myEnum = MyEnum::A;

				float m_f = 32.f;
				
				Json outjson;
				JsonSerializer2 se(outjson);
				se.io(testStruct);
				
				String jsonOutput;
				jsonOutput.reserve( outjson.size() );
				jsonOutput = outjson.dump(4).c_str();
				SGE_DUMP_VAR(jsonOutput);

				SGE_LOG("=======================");

				TestStruct testStructDE;
				JsonDeserializer2 de(outjson);
				de.io(testStructDE);


			}
		}
	};

} // namespace 

void test_JsonSE() {
	using namespace sge;
	SGE_TEST_CASE(Test_JsonSerializer, test_se());
}
