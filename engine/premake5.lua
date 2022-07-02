project "engine"
	kind "SharedLib"
	language "C"
	
	setdirs("", "data/bin")

	files {
		"premake5.lua",

		"render.h",

		"engine.c",
		"render.c",
	}

	links {
		"common",
	}

	sharedlibs {
		"libzstd",
		"SDL2",
	}
