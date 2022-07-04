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
		"zstd_static_" .. _TARGET_OS
	}
