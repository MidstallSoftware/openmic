cmake_minimum_required(VERSION 3.16)
project(openmic VERSION "0.1.0"
	DESCRIPTION "A free and open source real-time voice changing software"
	HOMEPAGE_URL "http://midstall.com/openmic" LANGUAGES C)

option(OPENMIC_GUI "Enables GUI support in the library and builds a graphical frontend" ON)
option(OPENMIC_TUI "Enables TUI support in the library and builds a terminal UI frontend" ON)
option(OPENMIC_CLI "Builds a CLI utility for working with OpenMic" ON)

if(UNIX OR ANDROID)
	set(OPENMIC_MODULE_EXT "so")
elseif(IOS OR APPLE)
	set(OPENMIC_MODULE_EXT "dylib")
elseif(WIN32 OR MINGW)
	set(OPENMIC_MODULE_EXT "dll")
else()
	message(FATAL_ERROR "Failed to find a compatible platform")
endif()

set(DATA_DIR "${PROJECT_SOURCE_DIR}/data")
include(GNUInstallDirs)
find_package(PkgConfig REQUIRED)
find_package(Git REQUIRED)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0)

if(APPLE OR ISO)
	pkg_check_modules(GMODULE REQUIRED IMPORTED_TARGET gmodule-2.0)
endif()

if(IS_DIRECTORY "${PROJECT_SOURCE_DIR}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		OUTPUT_VARIABLE "PROJECT_VERSION_TAG"
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_VARIABLE "ERROR")
	if(ERROR)
		message(FATAL_ERROR "Failed to get git checksum: ${ERROR}")
	endif()
	set(PROJECT_VERSION_TAG "git.${PROJECT_VERSION_TAG}")
else()
	set(PROJECT_VERSION_TAG "prealpha")
endif()
set(PROJECT_VERSION "${PROJECT_VERSION}-${PROJECT_VERSION_TAG}")

if(OPENMIC_GUI)
	pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
endif()
if(OPENMIC_TUI)
	pkg_check_modules(NCURSES REQUIRED IMPORTED_TARGET ncurses)
endif()

configure_file("${DATA_DIR}/config.h.in" "${PROJECT_BINARY_DIR}/include/openmic-config.h")

include_directories("${PROJECT_BINARY_DIR}/include" "${PROJECT_SOURCE_DIR}/include")

set(LIB_SOURCES "${PROJECT_SOURCE_DIR}/src/lib/context.c" "${PROJECT_SOURCE_DIR}/src/lib/module.c"
	"${PROJECT_SOURCE_DIR}/src/lib/module-loader.c" "${PROJECT_SOURCE_DIR}/src/lib/module-manager.c"
	"${PROJECT_SOURCE_DIR}/src/lib/node.c" "${PROJECT_SOURCE_DIR}/src/lib/openmic.c")

add_library(libopenmic-shared SHARED "${LIB_SOURCES}")
set_target_properties(libopenmic-shared PROPERTIES OUTPUT_NAME "openmic" COMPILE_DEFINITIONS "GETTEXT_PACKAGE=libopenmic")
target_link_libraries(libopenmic-shared PUBLIC PkgConfig::GST)

add_library(libopenmic-static STATIC "${LIB_SOURCES}")
set_target_properties(libopenmic-static PROPERTIES OUTPUT_NAME "openmic" COMPILE_DEFINITIONS "GETTEXT_PACKAGE=libopenmic")
target_link_libraries(libopenmic-static PUBLIC PkgConfig::GST)

if(APPLE OR ISO)
	target_link_libraries(libopenmic-shared PUBLIC PkgConfig::GMODULE)
	target_link_libraries(libopenmic-static PUBLIC PkgConfig::GMODULE)
endif()

if(OPENMIC_GUI)
	target_link_libraries(libopenmic-shared PUBLIC PkgConfig::GTK)
	target_link_libraries(libopenmic-static PUBLIC PkgConfig::GTK)

	add_executable(openmic-gui "${PROJECT_SOURCE_DIR}/src/gui/main.c")
	set_target_properties(openmic-gui PROPERTIES COMPILE_DEFINITIONS "GETTEXT_PACKAGE=openmic-gui")
	target_link_libraries(openmic-gui PUBLIC libopenmic-shared)

	install(TARGETS openmic-gui DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
if(OPENMIC_TUI)
	target_link_libraries(libopenmic-shared PUBLIC PkgConfig::NCURSES)
	target_link_libraries(libopenmic-static PUBLIC PkgConfig::NCURSES)

	add_executable(openmic-tui "${PROJECT_SOURCE_DIR}/src/tui/main.c")
	set_target_properties(openmic-tui PROPERTIES COMPILE_DEFINITIONS "GETTEXT_PACKAGE=openmic-tui")
	target_link_libraries(openmic-tui PUBLIC libopenmic-shared)

	install(TARGETS openmic-tui DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
if(OPENMIC_CLI)
	add_executable(openmic-cli "${PROJECT_SOURCE_DIR}/src/cli/main.c")
	set_target_properties(openmic-cli PROPERTIES COMPILE_DEFINITIONS "GETTEXT_PACKAGE=openmic-cli")
	target_link_libraries(openmic-cli PUBLIC libopenmic-shared)

	install(TARGETS openmic-cli DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

install(TARGETS libopenmic-shared libopenmic-static DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/OpenMic" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES "${PROJECT_BINARY_DIR}/include/openmic-config.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
