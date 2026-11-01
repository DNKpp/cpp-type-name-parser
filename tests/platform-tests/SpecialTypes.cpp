//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

#include "Common.hpp"

namespace testing = ctnp::testing;

namespace
{
    std::string const anonTypePattern = R"(<unnamed (class|struct|enum)>)";

    struct my_type
    {
    };
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances type-names within an anonymous-namespace scope.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);
    using T = testing::mod_type_t<TestType, my_type>;

    std::string const rawName{ctnp::type_name<T>()};
    CAPTURE(rawName);

    auto const result = ctnp::prettify_type(rawName);
    REQUIRE(result);

    auto const scopePattern = testing::anonNsScopePattern;
    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(scopePattern + "my_type" + suffixPattern));
}

namespace
{
    class
    {
    } anon_class [[maybe_unused]]{};

    class
    {
    } anon_struct [[maybe_unused]]{};

    enum
    {
        dummy
    } anon_enum [[maybe_unused]]{};
}

TEMPLATE_PRODUCT_TEST_CASE(
    "ctnp::prettify_type enhances anonymous type-names.",
    "[pretty]",
    TESTING_COMMON_MODS,
    (decltype(anon_class),
     decltype(anon_struct),
     decltype(anon_enum)))
{
    using T = TestType::type;
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    std::string const rawName{ctnp::type_name<T>()};
    CAPTURE(rawName);

    auto const result = ctnp::prettify_type(rawName);
    REQUIRE(result);

    auto const scopePattern = testing::anonNsScopePattern;
    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(scopePattern + anonTypePattern + suffixPattern));
}

namespace
{
    struct outer_type
    {
        template <typename T>
        struct my_template
        {
        };

        struct my_type
        {
        };
    };
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances nested type-names.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);
    auto const scopePattern = testing::anonNsScopePattern + "outer_type::";

    SECTION("When nested type-name is given.")
    {
        using T = testing::mod_type_t<TestType, outer_type::my_type>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_type" + suffixPattern));
    }

    SECTION("When nested template type-name is given.")
    {
        using T = testing::mod_type_t<TestType, outer_type::my_template<int>>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(scopePattern + "my_template<int>" + suffixPattern));
    }
}

namespace
{
    [[maybe_unused]] auto nullaryLambda = [] {};
    [[maybe_unused]] auto unaryLambda = [](int volatile* const&) {};
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances lambda appearance.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    SECTION("When lambda is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(nullaryLambda)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::anonNsScopePattern + testing::lambdaPattern + suffixPattern));
    }

    SECTION("When lambda with params is given.")
    {
        using T = testing::mod_type_t<TestType, decltype(unaryLambda)>;
        std::string const rawName{ctnp::type_name<T>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::anonNsScopePattern + testing::lambdaPattern + suffixPattern));
    }
}
