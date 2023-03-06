#pragma once

namespace sge {

    using FileSize = u64;

	enum class FileMode {
		CreateNew,
		OpenExists,
		OpenOrCreate,
	};

	enum class FileAccess {
		Read,
		ReadWrite,
		WriteOnly,
	};

	enum class FileShareMode {
		None,
		Read,
		Write,
		ReadWrite,
	};
}