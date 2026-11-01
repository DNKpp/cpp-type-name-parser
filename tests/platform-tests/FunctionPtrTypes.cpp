//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

#include "Common.hpp"

namespace testing = ctnp::testing;

TEMPLATE_LIST_TEST_CASE(
    "ctnp::prettify_type enhances function-ptr return types.",
    "[pretty]",
    testing::common_mod_list)
{
    std::string const suffixPattern{TestType::suffix};
    std::string const prefixPattern{R"(void\(\*)"};
    CAPTURE(suffixPattern);

    std::ostringstream ss{};

    SECTION("When a function-type returns a function-ptr.")
    {
        using Return = void (*)();
        using T = testing::mod_type_t<TestType, Return>;
        std::string const rawName{ctnp::type_name<T()>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(prefixPattern + suffixPattern + R"(\(\)\)\(\))"));
    }

    SECTION("When a function-ptr-type returns a function-ptr.")
    {
        using Return = void (*)();
        using T = testing::mod_type_t<TestType, Return>;
        std::string const rawName{ctnp::type_name<T (*)()>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(prefixPattern + suffixPattern + R"(\(\*\)\(\)\)\(\))"));
    }

    SECTION("When a function-type returns a function-ptr, which returns a function-ptr.")
    {
        using Return1 = void (*)();
        using Return2 = Return1 (*)();
        using T = testing::mod_type_t<TestType, Return2>;
        std::string const rawName{ctnp::type_name<T()>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(prefixPattern + R"(\(\*)" + suffixPattern + R"(\(\)\)\(\)\)\(\))"));
    }

    SECTION("When a function-ptr-type returns a function-ptr, which returns a function-ptr.")
    {
        using Return1 = void (*)();
        using Return2 = Return1 (*)();
        using T = testing::mod_type_t<TestType, Return2>;
        std::string const rawName{ctnp::type_name<T (*)()>()};
        CAPTURE(rawName);

        auto const result = ctnp::prettify_type(rawName);
        REQUIRE(result);

        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(prefixPattern + R"(\(\*)" + suffixPattern + R"(\(\*\)\(\)\)\(\)\)\(\))"));
    }
}
