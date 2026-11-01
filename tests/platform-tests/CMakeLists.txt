#          Copyright Dominic (DNKpp) Koepke 2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

set(TARGET_NAME ctnp-platform-tests)
add_executable(${TARGET_NAME}
    "ArrayTypes.cpp"
    "BuiltinTypes.cpp"
    "FunctionIdentifiers.cpp"
    "FunctionLocalTypes.cpp"
    "FunctionPtrTypes.cpp"
    "OperatorLocalTypes.cpp"
    "SpecialTypes.cpp"
)

target_link_libraries(${TARGET_NAME} PRIVATE
    fmt::fmt
    mimicpp::mimicpp
    constest::constest
    Catch2::Catch2WithMain

    ctnp::ctnp
)

target_precompile_headers(${TARGET_NAME} PRIVATE
    <mimic++/mimic++.hpp>
    <catch2/catch_all.hpp>
    <constest.hpp>
    <mimic++_ext/adapters/Catch2.hpp>
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    CONSTEST_CONFIG_CATCH2=1
    CATCH_CONFIG_FALLBACK_STRINGIFIER=mimicpp::print
)

catch_discover_tests(${TARGET_NAME})
