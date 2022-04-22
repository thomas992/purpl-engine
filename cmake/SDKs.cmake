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

	set(DISCORD_SDK_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/discord_game_sdk/c;${CMAKE_BINARY_DIR}/discord_game_sdk/cpp PARENT_SCOPE)
	if (WIN32)
		set(DISCORD_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/discord_game_sdk/lib/x86_64/discord_game_sdk.dll.lib PARENT_SCOPE)
		set(DISCORD_SDK_BINARIES ${CMAKE_BINARY_DIR}/discord_game_sdk/lib/x86_64/discord_game_sdk.dll PARENT_SCOPE)
	elseif (APPLE)
		set(DISCORD_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/discord_game_sdk/lib/x86_64/libdiscord_game_sdk.dylib PARENT_SCOPE)
		set(DISCORD_SDK_BINARIES ${DISCORD_SDK_LIBRARIES} PARENT_SCOPE)
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(DISCORD_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/discord_game_sdk/lib/x86_64/libdiscord_game_sdk.so PARENT_SCOPE)
		set(DISCORD_SDK_BINARIES ${DISCORD_SDK_LIBRARIES} PARENT_SCOPE)
	endif()
endfunction()

function(get_steamworks_sdk)
	if (NOT EXISTS ${CMAKE_BINARY_DIR}/steamworks_sdk.zip)
		message(STATUS "Open this link: https://partner.steamgames.com/downloads/steamworks_sdk.zip")
		message(STATUS "Sign in, download the file, and copy it to ${CMAKE_BINARY_DIR} and make sure it's named steamworks_sdk.zip, then run CMake again.")
		message(FATAL_ERROR "")
	endif()
	file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/steamworks_sdk)
	file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/steamworks_sdk.zip
			     DESTINATION ${CMAKE_BINARY_DIR}/steamworks_sdk)
	message(STATUS "Steamworks SDK extracted to ${CMAKE_BINARY_DIR}/steamworks_sdk")

	set(STEAMWORKS_SDK_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/public PARENT_SCOPE)
	if (WIN32)
		set(STEAMWORKS_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.lib PARENT_SCOPE)
		set(STEAMWORKS_SDK_BINARIES ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.dll PARENT_SCOPE)
	elseif (APPLE)
		set(STEAMWORKS_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/osx/libsteam_api.dylib PARENT_SCOPE)
		set(STEAMWORKS_SDK_BINARIES ${STEAMWORKS_SDK_LIBRARIES} PARENT_SCOPE)
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(STEAMWORKS_SDK_LIBRARIES ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk/redistributable_bin/linux64/libsteam_api.so PARENT_SCOPE)
		set(STEAMWORKS_SDK_BINARIES ${STEAMWORKS_SDK_LIBRARIES} PARENT_SCOPE)
	endif()
endfunction()

