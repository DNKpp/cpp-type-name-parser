//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Version.hpp"

#include <fmt/format.h>

TEST_CASE(
    "CTNP_VERSION_MAJOR is an integral number.",
    "[version]")
{
    constexpr int version{CTNP_VERSION_MAJOR};
    STATIC_CHECK(0 <= version);
    STATIC_CHECK(ctnp::version_major == version);
}

TEST_CASE(
    "CTNP_VERSION_MINOR is an integral number.",
    "[version]")
{
    constexpr int version{CTNP_VERSION_MINOR};
    STATIC_CHECK(0 <= CTNP_VERSION_MINOR);
    STATIC_CHECK(ctnp::version_minor == version);
}

TEST_CASE(
    "CTNP_VERSION_PATCH is an integral number.",
    "[version]")
{
    constexpr int version{CTNP_VERSION_PATCH};
    STATIC_CHECK(0 <= version);
    STATIC_CHECK(ctnp::version_patch == version);
}

TEST_CASE(
    "ctnp::version is a string-literal, containing a dotted triple.",
    "[version]")
{
    std::string const expected = fmt::format(
        "{}.{}.{}",
        ctnp::version_major,
        ctnp::version_minor,
        ctnp::version_patch);

    CHECK_THAT(
        std::string{ctnp::version},
        Catch::Matchers::Equals(expected));
}

TEST_CASE(
    "Accumulated version is greater 0.",
    "[version]")
{
    constexpr int accumulatedVersion{ctnp::version_major + ctnp::version_minor + ctnp::version_patch};
    STATIC_CHECK(0 < accumulatedVersion);
}
