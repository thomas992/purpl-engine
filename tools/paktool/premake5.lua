project "paktool"
	kind "ConsoleApp"
	language "C"

	setdirs("tools", "data/bin")

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
		"libzstd_static",
	}
