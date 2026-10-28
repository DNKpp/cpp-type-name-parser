//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/util/CopyableBox.hpp"

using namespace ctnp::detail;

TEST_CASE(
    "util::CopyableBox is a copyable type.",
    "[detail][util]")
{
    STATIC_CHECK(std::is_default_constructible_v<util::CopyableBox<int>>);
    STATIC_CHECK(std::is_nothrow_destructible_v<util::CopyableBox<int>>);
    STATIC_CHECK(std::is_copy_constructible_v<util::CopyableBox<int>>);
    STATIC_CHECK(std::is_copy_assignable_v<util::CopyableBox<int>>);
    STATIC_CHECK(std::is_nothrow_move_constructible_v<util::CopyableBox<int>>);
    STATIC_CHECK(std::is_nothrow_move_assignable_v<util::CopyableBox<int>>);
}

TEST_CASE(
    "util::CopyableBox can be default constructed.",
    "[detail][util]")
{
    CONSTEXPR_SECTION("When default constructing.")
    {
        util::CopyableBox<int> source{};
        CONSTEXPR_CHECK(0 == *source);
        CONSTEXPR_CHECK(0 == *std::as_const(source));
    };
}

TEST_CASE(
    "util::CopyableBox supports copy operations.",
    "[detail][util]")
{
    CONSTEXPR_SECTION("When copy-ctor is used.")
    {
        util::CopyableBox source{42};

        util::CopyableBox const other{source};

        CONSTEXPR_CHECK(42 == *other);
        CONSTEXPR_CHECK(source);
    };

    CONSTEXPR_SECTION("When copy-assignment is used.")
    {
        util::CopyableBox source{42};
        util::CopyableBox<int> other{};

        other = source;

        CONSTEXPR_CHECK(42 == *other);
    };

    CONSTEXPR_SECTION("When self copy-assigned.")
    {
        util::CopyableBox source{42};

        source = source;

        CONSTEXPR_CHECK(42 == *source);
    };
}

TEST_CASE(
    "util::CopyableBox supports move operations.",
    "[detail][util]")
{
    CONSTEXPR_SECTION("When move-ctor is used.")
    {
        util::CopyableBox source{42};

        util::CopyableBox const other{std::move(source)};

        CONSTEXPR_CHECK(42 == *other);
        CONSTEXPR_CHECK(!source);
    };

    CONSTEXPR_SECTION("When move-assignment is used.")
    {
        util::CopyableBox source{42};
        util::CopyableBox<int> other{};

        other = std::move(source);

        CONSTEXPR_CHECK(42 == *other);
    };

    CONSTEXPR_SECTION("When self move-assigned.")
    {
        util::CopyableBox source{42};

        source = std::move(source);

        CONSTEXPR_CHECK(42 == *source);
    };
}

TEST_CASE(
    "util::CopyableBox can be equality compared.",
    "[detail][util]")
{
    auto const [expected, firstValue, secondValue] = GENERATE((table<bool, int, int>)({
        {true,  42, 42  },
        {false, 42, 1337}
    }));

    util::CopyableBox const first{firstValue};
    util::CopyableBox const second{secondValue};

    CHECK(expected == (first == second));
    CHECK(expected == (second == first));
}

TEST_CASE(
    "util::CopyableBox supports operator->.",
    "[utility]")
{
    CONSTEXPR_SECTION("Using operator->")
    {
        util::CopyableBox const first{42};
        util::CopyableBox const second{42};

        CONSTEXPR_CHECK(first.operator->() == &*first);
        CONSTEXPR_CHECK(std::as_const(first).operator->() == &*first);
        CONSTEXPR_CHECK(second.operator->() == &*second);
        CONSTEXPR_CHECK(std::as_const(second).operator->() == &*second);
        CONSTEXPR_CHECK(first.operator->() != second.operator->());
    };
}
