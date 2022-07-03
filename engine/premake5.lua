project "engine"
	kind "SharedLib"
	language "C"
	
	setdirs("", "data/bin")

	files {
		"premake5.lua",

		"engine.h",
		"render.h",

		"engine.c",
		"render.c",
	}

	links {
		"common",
	}

	sharedlibs {
		"flecs",
		"libzstd",
		"SDL2",
	}

	filter "system:not macos"
		links {
			"vulkan-1"
		}

		files {
			"vk_glad.h",
			"vk_glad_platform.h",
			"vk_stuff.h",

			"vk_glad.c",
			"vk_main.c",
		}
	filter "system:windows"
		links {
			"d3d12",
			"dxgi",
			"dxguid",
		}

		files {
			"dx_stuff.h",

			"dx_device.c",
			"dx_main.c",
		}
	filter "system:macos"
		files {
			"mt_main.swift",
		}
	filter ""
