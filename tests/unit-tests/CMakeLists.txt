#          Copyright Dominic (DNKpp) Koepke 2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

set(TARGET_NAME ctnp-unit-tests)
add_executable(${TARGET_NAME}
    "Version.cpp"
)

add_subdirectory("lexing")
add_subdirectory("parsing")
add_subdirectory("util")

target_link_libraries(${TARGET_NAME} PRIVATE
    constest::constest
    Catch2::Catch2WithMain

    ctnp::ctnp
)

target_precompile_headers(${TARGET_NAME} PRIVATE
    <catch2/catch_all.hpp>
    <constest.hpp>
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    CONSTEST_CONFIG_CATCH2=1
)

catch_discover_tests(${TARGET_NAME})
