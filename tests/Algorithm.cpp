//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Algorithm.hpp"

using namespace ctnp;

TEST_CASE(
    "util::concat_arrays concatenates an arbitrary amount of arrays.",
    "[util][util::algorithm]")
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

TEST_CASE(
    "util::binary_find finds the required element in the container.",
    "[util][util::algorithm]")
{
    SECTION("When container contains just a single element.")
    {
        std::vector const collection = {42};

        auto const result = util::binary_find(collection, 42);

        CHECK(result == collection.cbegin());
    }

    SECTION("When value is first element.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::binary_find(collection, 42);

        CHECK(result == collection.cbegin());
    }

    SECTION("When value is last element.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::binary_find(collection, 1338);

        CHECK(result == collection.cbegin() + 2);
    }

    SECTION("When value is somewhere in the middle.")
    {
        std::vector const collection = {42, 1337, 1338};

        auto const result = util::binary_find(collection, 1337);

        CHECK(result == collection.cbegin() + 1);
    }
}

TEST_CASE(
    "util::binary_find returns end-iterator, when element is not contained.",
    "[util][util::algorithm]")
{
    SECTION("When container is empty.")
    {
        std::vector<int> const collection{};

        auto const result = util::binary_find(collection, 42);

        CHECK(result == collection.cend());
    }

    SECTION("When container is not empty, but value is not contained.")
    {
        std::vector const collection = {42, 1337, 1338};
        auto const value = GENERATE(-1, 0, 43, 1336, 1339);

        auto const result = util::binary_find(collection, value);

        CHECK(result == collection.cend());
    }
}

TEST_CASE(
    "util::prefix_range returns the subrange to the elements, which have the given prefix.",
    "[util][util::algorithm]")
{
    SECTION("Empty collection is supported.")
    {
        std::vector<std::string> const collection{};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CHECK_THAT(
            result,
            Catch::Matchers::IsEmpty());
    }

    SECTION("Empty prefix is supported.")
    {
        std::vector<std::string> const collection{"bar", "bfoo"};

        auto const result = util::prefix_range(collection, std::string_view{});

        CHECK(collection.cbegin() == result.begin());
        CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::vector{"bar", "bfoo"}));
    }

    SECTION("When no element starts with prefix.")
    {
        std::vector<std::string> const collection{"bar", "bfoo"};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CHECK_THAT(
            result,
            Catch::Matchers::IsEmpty());
    }

    SECTION("When some elements starts with prefix.")
    {
        std::vector<std::string> const collection{"a foo", "foo", "foo-bar", "foofoo", "no-foo"};

        auto const result = util::prefix_range(collection, std::string_view{"foo"});

        CHECK(collection.cbegin() + 1 == result.begin());
        CHECK_THAT(
            result,
            Catch::Matchers::RangeEquals(std::vector{"foo", "foo-bar", "foofoo"}));
    }
}
