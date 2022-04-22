#  Proprietary SDKs
#
#  Copyright 2022 MobSlicer152
#  This file is part of Purpl Engine
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

cmake_minimum_required(VERSION 3.15)

function(get_discord_sdk version)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/discord_game_sdk.zip)
		message(STATUS "Getting Discord SDK v${version}")
		file(DOWNLOAD https://dl-game-sdk.discordapp.net/${version}/discord_game_sdk.zip ${CMAKE_BINARY_DIR}/discord_game_sdk.zip)
	endif()
	file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/discord_game_sdk)
	file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/discord_game_sdk.zip
			     DESTINATION ${CMAKE_BINARY_DIR}/discord_game_sdk)
	message(STATUS "Discord SDK v${version} extracted to ${CMAKE_BINARY_DIR}/discord_game_sdk")

	if (${PURPL_PROCESSOR} MATCHES "x86")
		set(DISCORD_SDK_DIR ${CMAKE_BINARY_DIR}/discord_game_sdk)
		set(DISCORD_SDK_INCLUDE_DIRS ${DISCORD_SDK_DIR}/c;${DISCORD_SDK_DIR}/discord_game_sdk/cpp PARENT_SCOPE)
		set(DISCORD_HEADERS ${DISCORD_SDK_DIR}/cpp/achievement_manager.h
				    ${DISCORD_SDK_DIR}/cpp/activity_manager.h
				    ${DISCORD_SDK_DIR}/cpp/application_manager.h
				    ${DISCORD_SDK_DIR}/cpp/core.h
				    ${DISCORD_SDK_DIR}/cpp/discord.h
				    ${DISCORD_SDK_DIR}/cpp/event.h
				    ${DISCORD_SDK_DIR}/cpp/ffi.h
				    ${DISCORD_SDK_DIR}/cpp/image_manager.h
				    ${DISCORD_SDK_DIR}/cpp/lobby_manager.h
				    ${DISCORD_SDK_DIR}/cpp/network_manager.h
				    ${DISCORD_SDK_DIR}/cpp/overlay_manager.h
				    ${DISCORD_SDK_DIR}/cpp/relationship_manager.h
				    ${DISCORD_SDK_DIR}/cpp/storage_manager.h
				    ${DISCORD_SDK_DIR}/cpp/store_manager.h
				    ${DISCORD_SDK_DIR}/cpp/user_manager.h
				    ${DISCORD_SDK_DIR}/cpp/voice_manager.h)
		set(DISCORD_SOURCES ${DISCORD_SDK_DIR}/cpp/achievement_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/activity_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/application_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/core.cpp
				    ${DISCORD_SDK_DIR}/cpp/image_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/lobby_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/network_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/overlay_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/relationship_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/storage_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/store_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/types.cpp
				    ${DISCORD_SDK_DIR}/cpp/user_manager.cpp
				    ${DISCORD_SDK_DIR}/cpp/voice_manager.cpp)
		add_library(discord_game_sdk STATIC ${DISCORD_HEADERS} ${DISCORD_SOURCES})
		set_target_properties(discord_game_sdk PROPERTIES CXX_STANDARD 17)
	endif()
endfunction()

function(get_steamworks_sdk)
	if (EXISTS ${CMAKE_SOURCE_DIR}/steamworks_sdk.zip)
		file(COPY ${CMAKE_SOURCE_DIR}/steamworks_sdk.zip DESTINATION ${CMAKE_BINARY_DIR})
	elseif (NOT EXISTS ${CMAKE_BINARY_DIR}/steamworks_sdk.zip)
		message(STATUS "Steamworks SDK not found, go to https://partner.steamgames.com/downloads/steamworks_sdk.zip, sign in, download the file, and copy it to ${CMAKE_SOURCE_DIR} or ${CMAKE_BINARY_DIR} and make sure it's named steamworks_sdk.zip, then run CMake again.")
		message(FATAL_ERROR "")
	endif()
	message(STATUS "Found Steamworks SDK")
	file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/steamworks_sdk)
	file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/steamworks_sdk.zip
			     DESTINATION ${CMAKE_BINARY_DIR}/steamworks_sdk)
	message(STATUS "Steamworks SDK extracted to ${CMAKE_BINARY_DIR}/steamworks_sdk")

	if (${PURPL_PROCESSOR} STREQUAL "x86_64")
		set(STEAMWORKS_SDK_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/public PARENT_SCOPE)
		if (WIN32)
			set(STEAMWORKS_SDK_LIBRARY ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.lib)
			set(STEAMWORKS_SDK_BINARY ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.dll)
		elseif (APPLE)
			set(STEAMWORKS_SDK_LIBRARY ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/osx/libsteam_api.dylib)
			set(STEAMWORKS_SDK_BINARY ${STEAMWORKS_SDK_LIBRARY})
		elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			set(STEAMWORKS_SDK_LIBRARY ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/linux64/libsteam_api.so)
			set(STEAMWORKS_SDK_BINARY ${STEAMWORKS_SDK_LIBRARY})
		endif()
		add_library(steamworks_sdk SHARED IMPORTED GLOBAL)
		get_filename_component(STEAMWORKS_SDK_SONAME ${STEAMWORKS_SDK_LIBRARY} NAME)
		set_target_properties(steamworks_sdk PROPERTIES IMPORTED_LOCATION ${STEAMWORKS_SDK_BINARY})
		if (WIN32)
			set_target_properties(steamworks_sdk PROPERTIES IMPORTED_IMPLIB ${STEAMWORKS_SDK_LIBRARY})
		endif()
	endif()
endfunction()

