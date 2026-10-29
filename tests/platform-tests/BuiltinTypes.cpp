//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

#include "Common.hpp"

namespace testing = ctnp::testing;
using testing::StaticString;

TEMPLATE_TEST_CASE_SIG(
    "ctnp::prettify_type handles built-in integral types correctly.",
    "[pretty]",
    ((auto expected, typename T), expected, T),
    (StaticString{"char"}, char),
    (StaticString{"short"}, short),
    (StaticString{"short"}, short int),
    (StaticString{"int"}, int),
    (StaticString{"long"}, long),
    (StaticString{"long"}, long int),
    (StaticString{"long long"}, long long),
    (StaticString{"long long"}, long long int))
{
    std::string const rawName{ctnp::type_name<T>()};
    CAPTURE(rawName);

    auto const result = ctnp::prettify_type(rawName);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(expected.str()));
}

TEMPLATE_TEST_CASE_SIG(
    "ctnp::prettify_type handles built-in signed integral types correctly.",
    "[pretty]",
    ((auto expected, typename T), expected, T),
    (StaticString{"signed char"}, char),
    (StaticString{"short"}, short),
    (StaticString{"short"}, short int),
    (StaticString{"int"}, int),
    (StaticString{"long"}, long),
    (StaticString{"long"}, long int),
    (StaticString{"long long"}, long long),
    (StaticString{"long long"}, long long int))
{
    std::string const name{ctnp::type_name<std::make_signed_t<T>>()};
    CAPTURE(name);

    auto const result = ctnp::prettify_type(name);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(expected.str()));
}

TEMPLATE_TEST_CASE_SIG(
    "ctnp::prettify_type handles built-in unsigned integral types correctly.",
    "[pretty]",
    ((auto expected, typename T), expected, T),
    (StaticString{"unsigned char"}, char),
    (StaticString{"unsigned short"}, short),
    (StaticString{"unsigned short"}, short int),
    (StaticString{"unsigned int"}, int),
    (StaticString{"unsigned long"}, long),
    (StaticString{"unsigned long"}, long int),
    (StaticString{"unsigned long long"}, long long),
    (StaticString{"unsigned long long"}, long long int))
{
    std::ostringstream ss{};

    std::string const name{ctnp::type_name<std::make_unsigned_t<T>>()};
    CAPTURE(name);

    auto const result = ctnp::prettify_type(name);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(expected.str()));
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type handles bool correctly.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);
    using T = testing::mod_type_t<TestType, bool>;

    std::string const name{ctnp::type_name<T>()};
    CAPTURE(name);

    auto const result = ctnp::prettify_type(name);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches("bool" + suffixPattern));
}

namespace
{
    template <typename T>
    [[nodiscard]]
    std::pair<std::string, std::string> make_case(std::string expected)
    {
        return {
            std::move(expected),
            std::string{ctnp::type_name<T>()}};
    }
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type handles built-in float types correctly.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    auto const [expectedBase, rawName] = GENERATE(
        (make_case<testing::mod_type_t<TestType, float>>("float")),
        (make_case<testing::mod_type_t<TestType, double>>("double")),
        (make_case<testing::mod_type_t<TestType, long double>>("long double")));
    CAPTURE(rawName);

    auto const result = ctnp::prettify_type(rawName);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(expectedBase + suffixPattern));
}

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type handles built-in character types correctly.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    CAPTURE(suffixPattern);

    auto const [expectedBase, rawName] = GENERATE(
        (make_case<testing::mod_type_t<TestType, char>>("char")),
        (make_case<testing::mod_type_t<TestType, wchar_t>>("wchar_t")),
        (make_case<testing::mod_type_t<TestType, char8_t>>("char8_t")),
        (make_case<testing::mod_type_t<TestType, char16_t>>("char16_t")),
        (make_case<testing::mod_type_t<TestType, char32_t>>("char32_t")));
    CAPTURE(rawName);

    auto const result = ctnp::prettify_type(rawName);
    REQUIRE(result);

    CHECK_THAT(
        *result,
        Catch::Matchers::Matches(expectedBase + suffixPattern));
}
