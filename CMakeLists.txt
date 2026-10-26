#          Copyright Dominic (DNKpp) Koepke 2026
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.15...3.31)
set(MESSAGE_PREFIX "ctnp:")

set(PROJECT_NAME ctnp)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTNP-ReadProjectVersion)
ctnp_read_project_version("include/ctnp/Version.hpp" CTNP_VERSION)

project(${PROJECT_NAME}
    VERSION ${CTNP_VERSION}
    LANGUAGES CXX
)

include(CTNP-Config)

set(TARGET_NAME ${PROJECT_NAME})
add_library(${TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${TARGET_NAME} ALIAS ${TARGET_NAME})

include(GNUInstallDirs)
target_include_directories(${TARGET_NAME} INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_${CMAKE_CXX_STANDARD}
)

option(CTNP_BUILD_TESTS "Determines whether the tests shall be built." ${CTNP_IS_TOP_LEVEL_PROJECT})
if (CTNP_BUILD_TESTS)
    include(CTest)
    add_subdirectory("tests")
endif()
