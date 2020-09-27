cmake_minimum_required(VERSION 3.16)
project(openmic VERSION "0.1.0" DESCRIPTION "A free and open source real-time voice changing software" HOMEPAGE_URL "http://midstall.com/openmic")

option(OPENMIC_GUI "Enables GUI support in the library and builds a graphical frontend" ON)

if(UNIX OR ANDROID OR IOS OR APPLE)
	set(OPENMIC_MODULE_EXT "so")
elseif(WIN32 OR MINGW)
	set(OPENMIC_MODULE_EXT "dll")
else()
	message(FATAL_ERROR "Failed to find a compatible platform")
endif()

set(DATA_DIR "${PROJECT_SOURCE_DIR}/data")
include(GNUInstallDirs)
find_package(PkgConfig)

if(OPENMIC_GUI)
	pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
endif()

configure_file("${DATA_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/include/openmic-config.h")

include_directories("${PROJECT_BINARY_DIR}/include" "${PROJECT_SOURCE_DIR}/include")

set(LIB_SOURCES "${PROJECT_SOURCE_DIR}/src/lib/context.c" "${PROJECT_SOURCE_DIR}/src/lib/module.c" "${PROJECT_SOURCE_DIR}/src/lib/module-loader.c" "${PROJECT_SOURCE_DIR}/src/lib/module-manager.c")

add_library(libopenmic-shared SHARED "${LIB_SOURCES}")
set_target_properties(libopenmic-shared PROPERTIES OUTPUT_NAME "openmic")

add_library(libopenmic-static STATIC "${LIB_SOURCES}")
set_target_properties(libopenmic-static PROPERTIES OUTPUT_NAME "openmic")

if(OPENMIC_GUI)
	target_link_libraries(libopenmic-shared PUBLIC PkgConfig::GTK)
	target_link_libraries(libopenmic-static PUBLIC PkgConfig::GTK)

	add_executable(openmic "${PROJECT_SOURCE_DIR}/src/main.c")
	target_link_libraries(openmic PUBLIC libopenmic-shared)

	install(TARGETS openmic DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

install(TARGETS libopenmic-shared libopenmic-static DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/OpenMic" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES "${PROJECT_BINARY_DIR}/include/openmic-config.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
