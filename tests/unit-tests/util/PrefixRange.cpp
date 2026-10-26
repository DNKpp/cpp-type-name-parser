//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/util/PrefixRange.hpp"

#include <array>
#include <string>
#include <string_view>
#include <vector>

using namespace ctnp::detail;

TEST_CASE(
    "util::prefix_range returns the subrange to the elements, which share the given prefix.",
    "[detail][util]")
{
    CONSTEXPR_SECTION("Empty collection is supported.")
    {
        std::vector<std::string> const collection{};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CONSTEXPR_CHECK_THAT(result, Catch::Matchers::IsEmpty());
    };

    CONSTEXPR_SECTION("Empty prefix is supported.")
    {
        std::vector<std::string_view> const collection{"bar", "bfoo"};

        auto const result = util::prefix_range(collection, std::string_view{});

        CONSTEXPR_CHECK(collection.cbegin() == result.begin());
        CONSTEXPR_CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::vector{"bar", "bfoo"}));
    };

    CONSTEXPR_SECTION("When no element starts with prefix.")
    {
        std::vector<std::string_view> const collection{"bar", "bfoo"};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CONSTEXPR_CHECK_THAT(
            result,
            Catch::Matchers::IsEmpty());
    };

    CONSTEXPR_SECTION("When some elements starts with prefix.")
    {
        std::vector<std::string_view> const collection{"a foo", "foo", "foo-bar", "foofoo", "no-foo"};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CONSTEXPR_CHECK(collection.cbegin() + 1 == result.begin());
        CONSTEXPR_CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::vector{"foo", "foo-bar", "foofoo"}));
    };
}
