project "common"
	kind "StaticLib"
	language "C"

	files {
		"premake5.lua",

		"common.h",
		"dll.h",
		"gameinfo.h",
		"ini.h",
		"pack.h",
		"util.h",
		"xxhash.h",

		_TARGET_OS .. "/dll.c",
		"dll.c",
		"gameinfo.c",
		"ini.c",
		"pack.c",
		"util.c",
	}

	filter "system:windows"
		files {
			"_dirent.h",
		}
	filter "architecture:x86 or x64"
		files {
			"xxh_x86dispatch.h",
			"xxh_x86dispatch.c",
		}
	filter ""
