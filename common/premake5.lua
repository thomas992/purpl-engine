project "common"
	kind "StaticLib"
	language "C"

	files {
		"premake5.lua",

		"common.h",
		"dll.h",
		"ini.h",
		"pack.h",
		"util.h",

		_TARGET_OS .. "/dll.c",
		"dll.c",
		"ini.c",
		"pack.c",
		"util.c",
	}

