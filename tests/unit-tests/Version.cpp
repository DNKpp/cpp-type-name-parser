//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Version.hpp"

TEST_CASE(
    "ctnp::major is a non-negative integral number.",
    "[version]")
{
    STATIC_CHECK(0 <= ctnp::major);
}

TEST_CASE(
    "ctnp::minor is a non-negative integral number.",
    "[version]")
{
    STATIC_CHECK(0 <= ctnp::minor);
}

TEST_CASE(
    "ctnp::patch is a non-negative integral number.",
    "[version]")
{
    STATIC_CHECK(0 <= ctnp::patch);
}

TEST_CASE(
    "Accumulated version is greater 0.",
    "[version]")
{
    constexpr int accumulatedVersion{ctnp::major + ctnp::minor + ctnp::patch};
    STATIC_CHECK(0 < accumulatedVersion);
}
