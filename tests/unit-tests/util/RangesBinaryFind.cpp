//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/util/RangesBinaryFind.hpp"

using namespace ctnp::detail;

TEST_CASE(
    "util::ranges::binary_find finds the required element in the container.",
    "[util][util::algorithm]")
{
    CONSTEXPR_SECTION("When the container contains just a single element.")
    {
        std::vector const collection = {42};

        auto const result = util::ranges::binary_find(collection, 42);

        CONSTEXPR_CHECK(result == collection.cbegin());
    };

    CONSTEXPR_SECTION("When the value is the first element.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::ranges::binary_find(collection, 42);

        CONSTEXPR_CHECK(result == collection.cbegin());
    };

    CONSTEXPR_SECTION("When the value is the last element.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::ranges::binary_find(collection, 1338);

        CONSTEXPR_CHECK(result == collection.cbegin() + 2);
    };

    CONSTEXPR_SECTION("When the value is somewhere in the middle.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::ranges::binary_find(collection, 1337);

        CONSTEXPR_CHECK(result == collection.cbegin() + 1);
    };

    CONSTEXPR_SECTION("When the value is not contained.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::ranges::binary_find(collection, 56);

        CONSTEXPR_CHECK(result == collection.cend());
    };
}
