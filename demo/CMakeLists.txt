cmake_minimum_required(VERSION 3.10)

project(purpl-demo VERSION 1.0.0 LANGUAGES C)

# Sources
set(DEMO_SOURCES src/main.c)

# Information about the application
set(PURPL_APP_ID "purpl-demo")
set(PURPL_APP_JAVA_ID "ca.randomcode.purpl_demo")
set(PURPL_APP_UPPER_ID "PurplDemo")
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
purpl_setup(purpl-demo "${DEMO_SOURCES}" ${PURPL_APP_ID} ${PURPL_APP_VERSION})
