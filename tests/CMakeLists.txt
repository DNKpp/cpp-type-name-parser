#          Copyright Dominic (DNKpp) Koepke 2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Catch2 REQUIRED)
find_package(mimicpp REQUIRED)
find_package(constest REQUIRED)
find_package(cpptrace REQUIRED)

add_subdirectory("compile-time-tests")
add_subdirectory("platform-tests")
add_subdirectory("unit-tests")
