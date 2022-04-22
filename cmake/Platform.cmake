#  Platform setup functions for applications
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

function(purpl_win32_setup target app_id version)
	if (WIN32)
		string(REPLACE "." "," version_string "${version}")
		configure_file(${app_id}.rc.in ${app_id}.rc @ONLY)
		set_property(SOURCE ${app_id}.rc PROPERTY GENERATED TRUE)
	endif()
endfunction()

function(purpl_winrt_setup content_files)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
		configure_file(Package.appxmanifest.in Package.appxmanifest @ONLY)
		set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
		set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest PROPERTY GENERATED TRUE)

		foreach(file ${content_files})
			set_property(SOURCE ${file} PROPERTY VS_DEPLOYMENT_CONTENT TRUE)
			set_property(SOURCE ${file} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")
		endforeach()

		configure_file(create_test_cert.ps1.in create_test_cert.ps1 @ONLY)
		message(STATUS "To create a certificate, run the following command: ${CMAKE_CURRENT_BINARY_DIR}/create_test_cert.ps1")
	endif()
endfunction()

