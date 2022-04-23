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
		
		add_library(discord_game_sdk SHARED IMPORTED)
		if (WIN32)
			set_target_properties(discord_game_sdk PROPERTIES
								IMPORTED_IMPLIB ${DISCORD_SDK_DIR}/lib/${PURPL_PROCESSOR}/discord_game_sdk.dll.lib
								IMPORTED_LOCATION ${DISCORD_SDK_DIR}/lib/${PURPL_PROCESSOR}/discord_game_sdk.dll)
		elseif (APPLE)
			set_target_properties(discord_game_sdk PROPERTIES
								IMPORTED_LOCATION ${DISCORD_SDK_DIR}/lib/${PURPL_PROCESSOR}/discord_game_sdk.dylib)
		elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			set_target_properties(discord_game_sdk PROPERTIES
								IMPORTED_LOCATION ${DISCORD_SDK_DIR}/lib/${PURPL_PROCESSOR}/discord_game_sdk.so)
		endif()
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
		set(STEAMWORKS_SDK_DIR ${CMAKE_BINARY_DIR}/steamworks_sdk/sdk)
		set(STEAMWORKS_SDK_INCLUDE_DIRS ${STEAMWORKS_SDK_DIR}/public PARENT_SCOPE)
		
		add_library(steamworks_sdk SHARED IMPORTED GLOBAL)
		if (WIN32)
			set_target_properties(steamworks_sdk PROPERTIES
								IMPORTED_IMPLIB ${STEAMWORKS_SDK_DIR}/redistributable_bin/win64/steam_api64.lib
								IMPORTED_LOCATION ${STEAMWORKS_SDK_DIR}/redistributable_bin/win64/steam_api64.dll)
		elseif (APPLE)
			set_target_properties(steamworks_sdk PROPERTIES
								IMPORTED_LOCATION ${STEAMWORKS_SDK_DIR}/redistributable_bin/osx/libsteam_api.dylib)
		elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			set_target_properties(steamworks_sdk PROPERTIES
								IMPORTED_LOCATION ${STEAMWORKS_SDK_DIR}/redistributable_bin/linux64/libsteam_api.so)
		endif()
	endif()
endfunction()

