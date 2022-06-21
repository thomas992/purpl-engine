project "engine"
	kind "SharedLib"
	language "C"

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
	}

	sharedlibs {
		"libzstd",
		"SDL2",
	}
