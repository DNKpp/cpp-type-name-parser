#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

set(TARGET_NAME ctnp-tests)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_executable(${TARGET_NAME}
    "Algorithm.cpp"
    "Version.cpp"
)

target_include_directories(${TARGET_NAME}
    PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
)

find_package(Catch2 REQUIRED)
find_package(mimicpp REQUIRED)
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ctnp::ctnp

    mimicpp::mimicpp
    Catch2::Catch2WithMain
)

target_precompile_headers(${TARGET_NAME}
    PRIVATE
    <mimic++/mimic++.hpp>
    "Catch2FallbackStringifier.hpp"
    <catch2/catch_all.hpp>
    <mimic++/adapters/Catch2.hpp>
)

catch_discover_tests(${TARGET_NAME})
