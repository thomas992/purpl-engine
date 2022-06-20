project "launcher"
	kind "ConsoleApp"
	language "C"

	targetname(PURPL_LAUNCHER_NAME)

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
	}

	sharedlibs {
		"SDL2",
	}
