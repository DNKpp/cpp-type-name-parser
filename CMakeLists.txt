#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME ctnp)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTNP-ReadProjectVersion)
read_project_version("include/ctnp/config/Version.hpp" CTNP_VERSION)

project(${PROJECT_NAME}
    VERSION ${CTNP_VERSION}
	LANGUAGES CXX
)

message(STATUS "${PROJECT_NAME}: version: v${PROJECT_VERSION} from: ${${PROJECT_NAME}_SOURCE_DIR}")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(STATUS "${PROJECT_NAME}: targeting 64-bit architecture")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(STATUS "${PROJECT_NAME}: targeting 32-bit architecture")
else ()
	message(WARNING "${PROJECT_NAME}: unable to determine architecture")
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TARGET_NAME ${PROJECT_NAME})
add_library(${TARGET_NAME})
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

include(GNUInstallDirs)
target_include_directories(${TARGET_NAME} PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features(${PROJECT_NAME} PUBLIC
	cxx_std_${CMAKE_CXX_STANDARD}
)

add_subdirectory("src")

if (CMAKE_SOURCE_DIR STREQUAL ${PROJECT_NAME}_SOURCE_DIR)
	set(IS_TOP_LEVEL_PROJECT ON)
else()
	set(IS_TOP_LEVEL_PROJECT OFF)
endif()

OPTION(CTNP_BUILD_TESTS "Determines, whether the tests shall be built." ${IS_TOP_LEVEL_PROJECT})
if (CTNP_BUILD_TESTS)
	include(CTest)
	add_subdirectory("tests")
endif()
