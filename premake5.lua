-- Set the location of the project
function setdirs(_location, _bindir)
	location(_MAIN_SCRIPT_DIR .. "/proj" .. iif(_ACTION ~= nil, "/" .. _ACTION, "") .. "/" .. _location)
	targetdir(_MAIN_SCRIPT_DIR .. "/" .. _bindir)
end

-- Links against libraries and copies them to the output folder on Windows
_libs2 = {}
_outlibs = {}
_libcopycmds = {}
function sharedlibs(_libs)
	table.foreachi(_libs, function(_lib)
		_lib2 = _lib
		if _TARGET_OS ~= "windows" then
			_lib2 = string.gsub(_lib, "lib", "")
		end
		if not table.contains(_libs2, _lib2) then
			-- print("Adding " .. _lib2 .. " to list of libraries")
			_libs2 = table.flatten({ _libs2, { _lib2 } })
		end
	end)
	links(_libs2)
	if _TARGET_OS == "windows" then
		table.foreachi(_libs2, function(_lib)
			_libname = _lib .. ".dll"
			_pdbname = _lib .. ".pdb"
			_lib2 = _MAIN_SCRIPT_DIR .. "/deps/lib/%{cfg.platform}/" .. _libname
			_cmd = "{COPYFILE} " .. _lib2 .. " " .. _MAIN_SCRIPT_DIR .. "/data/bin/<NAME>"
			_libcmd = string.gsub(_cmd, "<NAME>", _libname)
			if not table.contains(_libcopycmds, _libcmd) then
				-- print("Adding " .. _libcmd .. " to list of commands")
				_libcopycmds = table.flatten({ _libcopycmds, { _libcmd } })
			end
		end)
	end
end

workspace "purpl"
	configurations { "Debug", "Release" }
	platforms { "x64", "ARM64" }

	setdirs("", "bin")
	objdir "bin-int"

	startproject "launcher"

	cdialect "C11"
	warnings "Extra"

	includedirs {
		"deps/include",
		"deps/stb",
		"common"
	}

	libdirs {
		"deps/lib/%{cfg.platform}"
	}

	filter "configurations:Debug"
		defines "PURPL_DEBUG=1"
		symbols "On"
	filter "configurations:Release"
		defines "PURPL_RELEASE=1"
		symbols "Off"
		optimize "On"
	filter "platforms:x64"
		architecture "x64"
	filter "architecture:x64"
		defines "PURPL_X64=1"
	filter "platforms:ARM64"
		architecture "ARM64"
	filter "architecture:ARM64"
		defines "PURPL_ARM64=1"
	filter ""

	filter "action:vs*"
		defines {
			"_CRT_SECURE_NO_DEPRACATE",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS",
		}
	filter ""

project "files"
	kind "None"

	files { "premake5.lua",
		"config.lua",
		"tools/premake5.lua",
	}

include "config.lua"

include "common"
include "engine"
include "game/client"
include "game/server"
include "launcher"
include "tools"
