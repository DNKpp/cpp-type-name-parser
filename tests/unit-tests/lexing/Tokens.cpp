//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/lexing/Tokens.hpp"

using namespace ctnp::detail;

TEMPLATE_TEST_CASE(
    "lexing::is_space determines whether the given character is a space.",
    "[detail][lexing]",
    char,
    wchar_t,
    char8_t,
    char16_t,
    char32_t)
{
    using Char = TestType;
    STATIC_CHECK(lexing::is_space(Char{' '}));
    STATIC_CHECK(lexing::is_space(Char{'\f'}));
    STATIC_CHECK(lexing::is_space(Char{'\n'}));
    STATIC_CHECK(lexing::is_space(Char{'\r'}));
    STATIC_CHECK(lexing::is_space(Char{'\t'}));
    STATIC_CHECK(lexing::is_space(Char{'\v'}));

    STATIC_CHECK(!lexing::is_space(Char{'a'}));
    STATIC_CHECK(!lexing::is_space(Char{'0'}));
}

TEMPLATE_TEST_CASE(
    "lexing::is_digit determines whether the given character is a digit.",
    "[detail][lexing]",
    char,
    wchar_t,
    char8_t,
    char16_t,
    char32_t)
{
    using Char = TestType;
    STATIC_CHECK(lexing::is_digit(Char{'0'}));
    STATIC_CHECK(lexing::is_digit(Char{'1'}));
    STATIC_CHECK(lexing::is_digit(Char{'2'}));
    STATIC_CHECK(lexing::is_digit(Char{'3'}));
    STATIC_CHECK(lexing::is_digit(Char{'4'}));
    STATIC_CHECK(lexing::is_digit(Char{'5'}));
    STATIC_CHECK(lexing::is_digit(Char{'6'}));
    STATIC_CHECK(lexing::is_digit(Char{'7'}));
    STATIC_CHECK(lexing::is_digit(Char{'8'}));
    STATIC_CHECK(lexing::is_digit(Char{'9'}));

    STATIC_CHECK(!lexing::is_digit(Char{'a'}));
    STATIC_CHECK(!lexing::is_digit(Char{' '}));
}
