project "launcher"
	kind "ConsoleApp"
	language "C"

	targetname(PURPL_GAME_NAME)
	setdirs("", "data")

	files {
		"premake5.lua",

		"main.c",
	}

	links {
		"common",
	}
	
	_cmds = {
			"{COPYDIR} " .. _MAIN_SCRIPT_DIR .. "/game/data/" .. PURPL_GAME_NAME .. " " .. _MAIN_SCRIPT_DIR .. "/data",
			"{MKDIR} " .. _MAIN_SCRIPT_DIR .. "/data/bin",
			"{MKDIR} " .. _MAIN_SCRIPT_DIR .. "/data/purpl/bin",
		}
	prebuildmessage("Copying libraries")
	prebuildcommands(_cmds)
	prebuildcommands(_libcopycmds)
