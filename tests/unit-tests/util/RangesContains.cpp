//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/util/RangesContains.hpp"

#include <vector>

using namespace ctnp::detail;

TEST_CASE(
    "util::ranges::contains determines whether the given range contains the specified element.",
    "[detail][util]")
{
    CONSTEXPR_SECTION("When the container is empty.")
    {
        constexpr std::vector<int> collection{};

        CONSTEXPR_CHECK(!util::ranges::contains(collection, 42));
    };

    CONSTEXPR_SECTION("When the container contains just a single element.")
    {
        std::vector const collection = {42};

        CONSTEXPR_CHECK(util::ranges::contains(collection, 42));
        CONSTEXPR_CHECK(!util::ranges::contains(collection, 41));
    };

    CONSTEXPR_SECTION("When the container contains multiple elements.")
    {
        std::vector const collection = {42, 1337, 1338};

        CONSTEXPR_CHECK(util::ranges::contains(collection, 42));
        CONSTEXPR_CHECK(util::ranges::contains(collection, 1337));
        CONSTEXPR_CHECK(util::ranges::contains(collection, 1338));

        CONSTEXPR_CHECK(!util::ranges::contains(collection, 41));
        CONSTEXPR_CHECK(!util::ranges::contains(collection, 1339));
    };
}
