//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

#include "Common.hpp"
#include <source_location>

namespace testing = ctnp::testing;

namespace
{
    namespace test
    {
        struct my_type
        {
        };
    }

    template <typename T>
    struct my_template
    {
        struct my_type
        {
        };

        auto foo(my_type)
        {
            return std::source_location::current();
        }

        auto bar(my_type const&, std::source_location* outLoc)
        {
            if (outLoc)
            {
                *outLoc = std::source_location::current();
            }

            struct bar_type
            {
            };

            return bar_type{};
        }
    };

    template <typename... Ts>
    struct my_variadic_template
    {
        struct my_type
        {
        };

        auto foo(my_type)
        {
            return std::source_location::current();
        }

        auto bar(my_type const&, std::source_location* outLoc)
        {
            if (outLoc)
            {
                *outLoc = std::source_location::current();
            }

            struct bar_type
            {
            };

            return bar_type{};
        }
    };
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances template-type-names within an anonymous-namespace scope.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    auto const scopePattern = testing::anonNsScopePattern;

    SECTION("When a template-type with a single param is given.")
    {
        using T = testing::mod_type_t<TestType, my_template<int>>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_template<int>" + suffixPattern));
    }

    SECTION("When a variadic template-type without params is given.")
    {
        using T = testing::mod_type_t<TestType, my_variadic_template<>>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_variadic_template<>" + suffixPattern));
    }

    SECTION("When a variadic template-type with multiple params is given.")
    {
        using T = testing::mod_type_t<TestType, my_variadic_template<int&, float const&, test::my_type*>>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argListPattern = "int&, float const&, " + scopePattern + "test::my_type\\*";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_variadic_template<" + argListPattern + ">" + suffixPattern));
    }
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances template dependant type-names.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    auto const scopePattern = testing::anonNsScopePattern;

    SECTION("When a template-type with a single param is given.")
    {
        using T = testing::mod_type_t<TestType, my_template<int>::my_type>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_template::my_type" + suffixPattern));
    }

    SECTION("When a variadic template-type without params is given.")
    {
        using T = testing::mod_type_t<TestType, my_variadic_template<>::my_type>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_variadic_template::my_type" + suffixPattern));
    }

    SECTION("When a variadic template-type with multiple params is given.")
    {
        using T = testing::mod_type_t<TestType, my_variadic_template<int&, float const&, test::my_type*>::my_type>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_variadic_template::my_type" + suffixPattern));
    }
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances template dependant member-fun-ptr type-names.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    std::string const returnTypePattern = "std::source_location";
    std::string const scopePattern = testing::maybe_pattern(testing::anonNsScopePattern);

    SECTION("When a template-type with a single param is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_template<int>::foo)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argTypePattern = "\\(" + testing::anonNsScopePattern + "my_template::my_type\\)";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(
                returnTypePattern
                + "\\(" + scopePattern + "my_template::\\*" + suffixPattern + "\\)"
                + argTypePattern));
    }

    SECTION("When a variadic template-type without params is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_variadic_template<>::foo)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argTypePattern = "\\(" + testing::anonNsScopePattern + "my_variadic_template::my_type\\)";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(
                returnTypePattern
                + "\\(" + scopePattern + "my_variadic_template::\\*" + suffixPattern + "\\)"
                + argTypePattern));
    }

    SECTION("When a variadic template-type with multiple params is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_variadic_template<int&, float const&, test::my_type*>::foo)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argTypePattern = "\\(" + testing::anonNsScopePattern + "my_variadic_template::my_type\\)";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(
                returnTypePattern
                + "\\(" + scopePattern + "my_variadic_template::\\*" + suffixPattern + "\\)"
                + argTypePattern));
    }
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances template dependant member-fun-ptr, which returns a local-type, type-names.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    SECTION("When a template-type with a single param is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_template<int>::bar)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argListPattern = "\\(" + testing::anonNsScopePattern + R"(my_template::my_type const&, std::source_location\*\))";
        std::string const ptrPattern = "\\(" + testing::maybe_pattern(testing::anonNsScopePattern) + "my_template::\\*" + suffixPattern + "\\)";
        std::string const returnTypePattern = testing::maybe_pattern(testing::anonNsScopePattern + "my_template::bar::") + "bar_type";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnTypePattern + ptrPattern + argListPattern));
    }

    SECTION("When a variadic template-type without params is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_variadic_template<>::bar)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argListPattern = "\\(" + testing::anonNsScopePattern + R"(my_variadic_template::my_type const&, std::source_location\*\))";
        std::string const ptrPattern = "\\(" + testing::maybe_pattern(testing::anonNsScopePattern) + "my_variadic_template::\\*" + suffixPattern + "\\)";
        std::string const returnTypePattern = testing::maybe_pattern(testing::anonNsScopePattern + "my_variadic_template::bar::") + "bar_type";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnTypePattern + ptrPattern + argListPattern));
    }

    SECTION("When a variadic template-type with multiple params is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(&my_variadic_template<int&, float const&, test::my_type*>::bar)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        std::string const argListPattern = "\\(" + testing::anonNsScopePattern + R"(my_variadic_template::my_type const&, std::source_location\*\))";
        std::string const ptrPattern = "\\(" + testing::maybe_pattern(testing::anonNsScopePattern) + "my_variadic_template::\\*" + suffixPattern + "\\)";
        std::string const returnTypePattern = testing::maybe_pattern(testing::anonNsScopePattern + "my_variadic_template::bar::") + "bar_type";
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnTypePattern + ptrPattern + argListPattern));
    }
}
