workspace "purpl"
	configurations { "Debug", "Release" }
	platforms { "x64", "ARM64" }

	location "proj"
	targetdir "bin"
	objdir "bin-int"

	cdialect "C11"
	warnings "Extra"

	files { "premake5.lua", "config.lua" }

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
		defines "PURPL_X64=1"
	filter "platforms:ARM64"
		architecture "ARM64"
		defines "PURPL_ARM64=1"
	filter ""

	filter "action:vs*"
		defines {
			"_CRT_SECURE_NO_DEPRACATE",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS",
		}
	filter ""

-- Links against libraries and copies them to the output folder on Windows
function sharedlibs(_libs)
	_libs2 = {}
	table.foreachi(_libs, function(_lib)
		_lib2 = _lib
		if _TARGET_OS ~= "windows" then
			_lib2 = string.gsub(_lib, "lib", "")
		end
		_libs2 = table.flatten({ _libs2, { _lib2 } })
	end)
	links(_libs2)
	if _TARGET_OS == "windows" then
		_libpat = "<LIBNAME>.dll"
		_postbuildcmds = {}
		i = 0
		table.foreachi(_libs2, function(_lib)
			_libname = string.gsub(_libpat, "<LIBNAME>", _lib)
			_cmd = "{COPYFILE} " .. _MAIN_SCRIPT_DIR .. "/deps/lib/%{cfg.platform}/" .. _libname .. " %{cfg.linktarget.directory}/" .. _libname
			_postbuildcmds = table.flatten({ _postbuildcmds, { _cmd } })
			i = i + 1
		end)
		postbuildmessage("Copying " .. i .. " libraries for target %{cfg.linktarget.name}")
		postbuildcommands(_postbuildcmds)
	end
end

include "config.lua"

include "common"
include "engine"
include "launcher"
