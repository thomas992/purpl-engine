include "config.lua"

-- Set the location of the project
function setdirs(_location, _bindir)
	_action = "/" .. iif(_ACTION ~= nil, _ACTION, "")
	location(_MAIN_SCRIPT_DIR .. "/proj" .. _action .. "/" .. _location)
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
			if _TARGET_OS ~= "macosx" then
				_lib2 = _lib2 .. "_" .. _TARGET_OS
			end
		end
		if not table.contains(_libs2, _lib2) then
			-- print("Adding " .. _lib2 .. " to list of libraries")
			_libs2 = table.flatten({ _libs2, { _lib2 } })
		end
	end)
	links(_libs2)
	table.foreachi(_libs2, function(_lib)
		_libpath = _MAIN_SCRIPT_DIR .. "/deps/lib/%{cfg.platform}/<NAME>"
		if _TARGET_OS == "windows" then
			_libname = _lib .. ".dll"
		else
			if _TARGET_OS == "macosx" then
				_libname = "lib" .._lib .. ".dylib"
			else
				_libname = "lib" .. _lib .. ".so"
			end
		end
	
		_lib2 =  string.gsub(_libpath, "<NAME>", _libname)
		_cmd = "{COPYFILE} " .. _lib2 .. " " .. _MAIN_SCRIPT_DIR .. "/data/bin/<NAME>"
		_libcmd = string.gsub(_cmd, "<NAME>", _libname)
		if not table.contains(_libcopycmds, _libcmd) then
			-- print("Adding " .. _libcmd .. " to list of commands")
			_libcopycmds = table.flatten({ _libcopycmds, { _libcmd } })
		end
	end)
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
		PURPL_VULKAN_SDK .. "/include",
		"deps/cglm/include",
		"deps/imgui",
		"deps/imgui/backends",
		"deps/include",
		"deps/include/SDL2",
		"deps/stb",
		_MAIN_SCRIPT_DIR,
	}

	libdirs {
		PURPL_VULKAN_SDK .. "/lib",
		"deps/lib/%{cfg.platform}",
	}

	pic "On"
	systemversion "latest"
	visibility "hidden"

	filter "system:windows"
		toolset "msc"
	filter "system:not windows"
		-- GCC is bad, Clang is somewhat less bad, MSVC is weird as fuck but has some interesting features
		toolset "clang"
	filter "configurations:Debug"
		defines "PURPL_DEBUG=1"
		runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines "PURPL_RELEASE=1"
		optimize "On"
		runtime "Release"
		symbols "Off"
	filter "platforms:x64"
		architecture "x64"
	filter "architecture:x64"
		defines "PURPL_X64=1"
	filter "platforms:ARM64"
		architecture "ARM64"
	filter "architecture:ARM64"
		defines "PURPL_ARM64=1"
	filter {}

	filter "toolset:msc"
		defines {
			"_CRT_SECURE_NO_DEPRACATE",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS",
		}
		disablewarnings {
			4201,
			4206,
			26439,
		}
		fatalwarnings {
			4013,
		}
	filter {}

project "files"
	kind "Utility"

	files { "premake5.lua",
		"config.lua",
		"tools/premake5.lua",
	}

include "common"
include "engine"
include "game/client"
include "game/server"
include "launcher"
include "tools"

