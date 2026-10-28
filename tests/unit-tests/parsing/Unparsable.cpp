//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/parsing/Parse.hpp"

using ctnp::detail::parse_type;

TEST_CASE(
    "parse_type returns std::nullopt if input cannot be parsed.",
    "[detail][parsing]")
{
    auto const input = GENERATE(
        "const int const",
        "volatile int const volatile",
        "int const int",
        "int* int",
        "int<int>",
        "int[int]");
    CAPTURE(input);

    CHECK_FALSE(parse_type(input));
}
