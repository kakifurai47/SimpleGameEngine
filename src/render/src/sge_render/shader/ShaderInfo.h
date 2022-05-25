
namespace sge {
	struct ShaderInfo : public NonCopyable {

		struct Property {
			String attTag;
			String attDefval;

			String dataType;
			String name;
			String defval;
		};

		struct Pass {
			String vsFunc;
			String psFunc;
		};

		String				 name;
		Vector_<Property, 4> properties;
		Vector_<Pass, 1>	 passes;
	};
}
