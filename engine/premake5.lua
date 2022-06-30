project "engine"
	kind "SharedLib"
	language "C"
	
	setdirs("", "data/bin")

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
