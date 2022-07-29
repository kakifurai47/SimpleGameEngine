

namespace sge {

	struct FilePath {
		static StrView	dirname		(StrView path);
		static StrView	extension	(StrView path);		

		static bool		isRealpath(const StrView& path);
		static String	realpath  (StrView path);
	};





}