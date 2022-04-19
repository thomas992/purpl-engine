cmake_minimum_required(VERSION 3.10)

set(DEMO_SOURCES src/main.c)

if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
	set(DEMO_SOURCES ${DEMO_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest Package.appxmanifest.in)
endif()

# Needs semicolons
set(DEMO_CONTENTS
    assets/textures/icon_44.png;
    assets/textures/icon_50.png;
    assets/textures/icon_150.png;
    assets/textures/splash.png;)

add_executable(demo ${DEMO_SOURCES} ${DEMO_CONTENTS})

# This library handles loading the shared library
target_link_libraries(demo PRIVATE engine_preinit)

if (WIN32)
	add_custom_command(TARGET demo PRE_BUILD
			   COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:demo>
			   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ENGINE_RUNTIME_FILES} $<TARGET_FILE_DIR:demo>
			   DEPENDS ${ENGINE_RUNTIME_FILES})
endif()

# Manifest information for MSIX
set(PURPL_APP_ID "PurplDemo")
set(PURPL_APP_DISPLAY_NAME "Purpl Demo")
set(PURPL_APP_UUID a8e9f8d2-f8d2-4f0e-b9e2-f9f9f9f9f9f9)
set(PURPL_APP_DESCRIPTION "A demo of the Purpl engine")
set(PURPL_APP_ORGANIZATION_ID RandomCode)
set(PURPL_APP_ORGANIZATION RandomCode)
set(PURPL_APP_ORGANIZATION_UUID 0797c936-9b6a-4c23-80e8-b0dcb1938410)
set(PURPL_APP_VERSION 1.0.0.0) # Need 4 version digits
set(PURPL_APP_CITY Toronto)
set(PURPL_APP_TERRITORY Ontario)
set(PURPL_APP_COUNTRY Canada)

# If the engine is built for Windows Store, this sets stuff up
purpl_winrt_setup("${DEMO_CONTENTS}")

install(TARGETS demo
	RUNTIME
	DESTINATION ${CMAKE_INSTALL_PREFIX})

if (MSVC AND ${CMAKE_BUILD_TYPE} MATCHES "Deb")
	install(PROGRAMS $<TARGET_PDB_FILE:demo>
		DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
