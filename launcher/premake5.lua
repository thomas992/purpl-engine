project "launcher"
	kind "ConsoleApp"
	language "C"

	filter "system:windows"
		targetname(PURPL_GAME_NAME)
	filter "system:not windows"
		targetname(PURPL_GAME_NAME .. "_" .. _TARGET_OS)
	filter {}
	setdirs("", "data")

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
		"zstd_static_" .. _TARGET_OS,
	}

	filter "system:not windows"
		links {
			"dl"
		}
	filter {}
	
	_cmds = {
			"{COPYDIR} " .. _MAIN_SCRIPT_DIR .. "/game/data/" .. PURPL_GAME_NAME .. " " .. _MAIN_SCRIPT_DIR .. "/data",
			"{MKDIR} " .. _MAIN_SCRIPT_DIR .. "/data/bin",
			"{MKDIR} " .. _MAIN_SCRIPT_DIR .. "/data/purpl/bin",
		}
	prebuildmessage("Copying libraries")
	prebuildcommands(_cmds)
	prebuildcommands(_libcopycmds)
