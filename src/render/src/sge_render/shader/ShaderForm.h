
namespace sge {
	struct ShaderForm : public NonCopyable {

		struct VarSpec {
			String tag;
			String val;
		};

		struct MemVar {
			String type;
			String name;
			String dval;
		};

		struct ScriptVar {
			VarSpec spec;
			MemVar  mvar;
		};

		struct Properties {
			Vector<ScriptVar> svars;
		};

		struct Pass {
			String name;
			String vertFuncName;
			String fragFuncName;
		};

		struct Struct {
			String			   name;
			Vector_<MemVar, 2> vars;
		};

		String			   name;
		Properties		   properties;
		Vector_<Pass,	1> passes;
		Vector_<Struct, 1> structs;
		Vector_<MemVar, 4> memvars;
	};
}
