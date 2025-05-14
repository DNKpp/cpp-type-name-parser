//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Algorithm.hpp"

using namespace ctnp;

TEST_CASE(
    "util::concat_arrays concatenates an arbitrary amount of arrays.",
    "[algorithm]")
{
    SECTION("Single array is supported.")
    {
        std::array const source{42, 1337};

        auto const result = util::concat_arrays(source);
        STATIC_CHECK(std::same_as<int, std::ranges::range_value_t<decltype(result)>>);

        CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(source));
    }

    SECTION("Two arrays are supported.")
    {
        std::array const first{42, 1337};
        std::array const second{-42, -1337, 42};

        auto const result = util::concat_arrays(first, second);
        STATIC_CHECK(std::same_as<int, std::ranges::range_value_t<decltype(result)>>);

        CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::array{42, 1337, -42, -1337, 42}));
    }

    SECTION("Arbitrary amount of arrays are supported.")
    {
        std::array const first{42, 1337};
        std::array const second{-42, -1337, 42};

        auto const result = util::concat_arrays(first, second, first);
        STATIC_CHECK(std::same_as<int, std::ranges::range_value_t<decltype(result)>>);

        CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::array{42, 1337, -42, -1337, 42, 42, 1337}));
    }

    SECTION("Empty arrays are supported.")
    {
        std::array<int, 0> const source{};

        auto const result = util::concat_arrays(source, source);
        STATIC_CHECK(std::same_as<int, std::ranges::range_value_t<decltype(result)>>);

        CHECK_THAT(
            result,
            Catch::Matchers::IsEmpty());
    }
}
