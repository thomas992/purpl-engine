cmake_minimum_required(VERSION 3.15)

function(purpl_winrt_setup content_files)
	if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
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

