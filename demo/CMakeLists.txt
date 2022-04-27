cmake_minimum_required(VERSION 3.10)

project(purpl-demo VERSION 1.0.0 LANGUAGES C)

# Sources
set(DEMO_SOURCES src/main.c)

if (WIN32)
	set(DEMO_SOURCES ${DEMO_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/purpl-demo.rc purpl-demo.rc.in)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
	set(DEMO_SOURCES ${DEMO_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/Package.appxmanifest Package.appxmanifest.in)
endif()

# List of assets, icons and splash are mandatory for UWP. Semicolons needed.
set(DEMO_CONTENTS
    assets/textures/icon_44.png;
    assets/textures/icon_50.png;
    assets/textures/icon_150.png;
    assets/textures/splash.png;)

add_executable(purpl-demo ${DEMO_SOURCES} ${DEMO_CONTENTS})

# This library handles loading the shared library
target_link_libraries(purpl-demo PRIVATE engine_preinit)

# Information about the application
set(PURPL_APP_ID "purpl-demo")
set(PURPL_APP_UWP_ID "PurplDemo")
set(PURPL_APP_DISPLAY_NAME "Purpl Demo")
set(PURPL_APP_UUID a8e9f8d2-f8d2-4f0e-b9e2-f9f9f9f9f9f9)
set(PURPL_APP_DESCRIPTION "A demo of the Purpl engine")
set(PURPL_APP_ORGANIZATION_ID Randomcode)
set(PURPL_APP_ORGANIZATION "Randomcode Developers")
set(PURPL_APP_ORGANIZATION_UUID 0797c936-9b6a-4c23-80e8-b0dcb1938410)
set(PURPL_APP_VERSION 1.0.0.0) # Need 4 version digits for Windows stuff
set(PURPL_APP_CITY Toronto)
set(PURPL_APP_TERRITORY Ontario)
set(PURPL_APP_COUNTRY Canada)

# Set up the application (install it, copy the engine and its dependencies as well as the assets)
purpl_setup(purpl-demo ${PURPL_APP_ID} ${PURPL_APP_VERSION} ${DEMO_CONTENTS})
