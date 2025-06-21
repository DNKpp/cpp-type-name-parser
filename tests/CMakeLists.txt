#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

set(TARGET_NAME ctnp-tests)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD ${CTNP_CXX_STANDARD})

add_executable(${TARGET_NAME}
    "Algorithm.cpp"
    "Lexer.cpp"
    "TypeList.cpp"
    "Version.cpp"
)

add_subdirectory("parsing")

target_include_directories(${TARGET_NAME} PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
)

include(CTNP-EnableWarnings)
include(CTNP-EnableAdditionalFlags)
find_package(Catch2 REQUIRED)
find_package(mimicpp REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    ctnp::ctnp
    ctnp::internal::enable-warnings
    ctnp::internal::enable-additional-flags

    mimicpp::mimicpp
    Catch2::Catch2WithMain
)

option(CTNP_TESTING_COMPAT_SOURCE_LOC "" OFF)
message(DEBUG "${MESSAGE_PREFIX} CTNP_TESTING_COMPAT_SOURCE_LOC: ${CTNP_TESTING_COMPAT_SOURCE_LOC}")
if (CTNP_TESTING_COMPAT_SOURCE_LOC)

    set(SOURCE_LOCATION_BUILD_TESTS OFF)
    CPMAddPackage("gh:paweldac/source_location@0.4")

    target_link_libraries(${TARGET_NAME} PRIVATE
        nostd::source_location
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE
        "MIMICPP_CONFIG_ALTERNATIVE_SOURCE_LOCATION_BACKEND=nostd::source_location"
    )
endif ()

target_precompile_headers(${TARGET_NAME} PRIVATE
    "$<$<BOOL:${CTNP_TESTING_COMPAT_SOURCE_LOC}>:<mimic++/compatibility/paweldac-source_location.hpp$<ANGLE-R>>"
    <mimic++/mimic++.hpp>
    "Catch2FallbackStringifier.hpp"
    <catch2/catch_all.hpp>
    <mimic++/adapters/Catch2.hpp>
)

catch_discover_tests(${TARGET_NAME})
