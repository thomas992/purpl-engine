project "paktool"
	kind "ConsoleApp"
	language "C"

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
		"libzstd_static",
	}
