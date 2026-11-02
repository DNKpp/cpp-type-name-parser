//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

namespace
{
    template <typename...>
    struct my_type
    {
    };
}

TEST_CASE(
    "ctnp::prettify_type can be constant evaluated.",
    "[compile-time][pretty]")
{
    CONSTEXPR_SECTION("Compile-time tests")
    {
        constexpr auto rawName= ctnp::type_name<my_type<const int&>>();
        auto const result = ctnp::prettify_type(rawName);
        CONSTEXPR_REQUIRE(result);

        CONSTEXPR_CHECK(*result == "{anon-ns}::my_type<int const&>");
    };

    CONSTEXPR_SECTION("Compile-time tests #2")
    {
        constexpr auto rawName= ctnp::type_name<my_type<const int&, float* const* (&)[42]>>();
        auto const result = ctnp::prettify_type(rawName);
        CONSTEXPR_REQUIRE(result);

        CONSTEXPR_CHECK(*result == "{anon-ns}::my_type<int const&, float* const*(&)[42]>");
    };
}

namespace
{
    constexpr char const* foo(std::source_location& out)
    {
        out = std::source_location::current();
        return nullptr;
    }
}

TEST_CASE(
    "ctnp::prettify_function can be constant evaluated.",
    "[compile-time][pretty]")
{
    CONSTEXPR_SECTION("Compile-time tests")
    {
        std::source_location loc{};
        foo(loc);
        auto const result = ctnp::prettify_function(loc.function_name());
        CONSTEXPR_REQUIRE(result);

        CONSTEXPR_CHECK(*result == "char const* {anon-ns}::foo(std::source_location&)");
    };
}

