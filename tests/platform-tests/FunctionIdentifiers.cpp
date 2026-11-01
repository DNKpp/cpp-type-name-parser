//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/ctnp.hpp"

#include "Common.hpp"

namespace testing = ctnp::testing;

namespace
{
    std::string const testCasePattern = R"(CATCH2_INTERNAL_TEST_\d+)";
}

TEST_CASE(
    "ctnp::prettify_function omits function args with just `void` content.",
    "[print][print::type]")
{
    std::string const name{"ret my_function<void>(void)"};

    auto const result = ctnp::prettify_function(name);

    REQUIRE(result);
    CHECK_THAT(
        *result,
        Catch::Matchers::Equals(+"ret my_function<void>()"));
}

constexpr auto type_post_processing_lambda_loc = [] {
    return std::source_location::current();
};

constexpr auto type_post_processing_nested_lambda_loc = [] {
    return [] {
        return std::source_location::current();
    }();
};

namespace
{
    [[nodiscard]]
    constexpr std::source_location loc_fun()
    {
        [[maybe_unused]] constexpr auto dummy = [] {};
        [[maybe_unused]] constexpr auto dummy2 = [] {};
        constexpr auto inner = [] {
            return std::source_location::current();
        };
        [[maybe_unused]] constexpr auto dummy3 = [] {};

        return inner();
    }

    [[nodiscard]]
    constexpr std::source_location loc_anon_lambda_fun()
    {
        return [] {
            return std::source_location::current();
        }();
    }

    constexpr auto my_typeLambda = [] {
        struct my_type
        {
        };

        return my_type{};
    };

    template <typename... Ts>
    struct my_template
    {
        struct my_type
        {
        };

        std::source_location foo(my_type)
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

TEST_CASE(
    "ctnp::prettify_function enhances std::source_location::function_name appearance.",
    "[pretty]")
{
    SECTION("When a general function is given.")
    {
        constexpr std::source_location loc = std::source_location::current();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches("void " + testCasePattern + R"(\(\))"));
    }

    SECTION("When a lambda is given.")
    {
        constexpr auto loc = type_post_processing_lambda_loc();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const alt1 = testing::lambdaPattern; // primarily gcc
        auto const alt2 = testing::alternative_pattern("auto", "std::source_location")
                        + " "
                        + testing::alternative_pattern(testing::anonTypePattern, testing::lambdaPattern)
                        + R"(::operator\(\)\(\) const)";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::alternative_pattern(alt1, alt2)));
    }

    SECTION("When a nested lambda is given.")
    {
        constexpr auto loc = type_post_processing_nested_lambda_loc();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const alt1 = testing::lambdaPattern + "::" + testing::lambdaPattern; // primarily gcc
        auto const alt2 = "auto " + testing::anonTypePattern + R"(::operator\(\)::)" + testing::anonTypePattern + R"(::operator\(\)\(\) const)";
        auto const alt3 = "std::source_location " + testing::lambdaPattern + "::" + testing::lambdaPattern + R"(::operator\(\)\(\) const)"; // msvc
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::alternative_pattern(alt1, alt2, alt3)));
    }

    SECTION("When a function-local lambda is given.")
    {
        constexpr auto loc = loc_fun();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const scopePattern = testing::anonNsScopePattern + "loc_fun::";
        auto const alt1 = scopePattern + testing::lambdaPattern; // primarily gcc
        auto const alt2 = testing::alternative_pattern("auto", "std::source_location")
                        + " "
                        + scopePattern
                        + testing::alternative_pattern(testing::anonTypePattern, testing::lambdaPattern)
                        + R"(::operator\(\)\(\) const)";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::alternative_pattern(alt1, alt2)));
    }

    SECTION("When a function-local anon-struct is given.")
    {
        struct
        {
            constexpr std::source_location operator()() const
            {
                return std::source_location::current();
            }
        } constexpr obj{};

        constexpr auto loc = obj();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const returnPattern = testing::maybe_pattern("std::") + "source_location ";
        auto const scopePattern = testCasePattern + "::" + testing::anonTypePattern + "::";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnPattern + scopePattern + R"(operator\(\)\(\) const)"));
    }

    SECTION("When a function-local anon-class is given.")
    {
        class
        {
        public:
            constexpr std::source_location operator()() const
            {
                return std::source_location::current();
            }
        } constexpr obj{};

        constexpr auto loc = obj();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const returnPattern = testing::maybe_pattern("std::") + "source_location ";
        auto const scopePattern = testCasePattern + "::" + testing::anonTypePattern + "::";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnPattern + scopePattern + R"(operator\(\)\(\) const)"));
    }

    SECTION("When a function-local anon-lambda is given.")
    {
        constexpr auto loc = loc_anon_lambda_fun();
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const scopePattern = testing::anonNsScopePattern + "loc_anon_lambda_fun::";
        auto const alt1 = scopePattern + testing::lambdaPattern; // primarily gcc
        auto const alt2 = testing::alternative_pattern("auto", "std::source_location")
                        + " "
                        + scopePattern
                        + testing::alternative_pattern(testing::anonTypePattern, testing::lambdaPattern)
                        + R"(::operator\(\)\(\) const)";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(testing::alternative_pattern(alt1, alt2)));
    }

    SECTION("When a template-dependant function is given.")
    {
        using type_t = decltype(my_typeLambda());
        auto const loc = my_template<type_t, int>{}.foo({});
        CAPTURE(loc.function_name());

        auto const result = ctnp::prettify_function(loc.function_name());

        auto const returnPattern = testing::maybe_pattern("std::") + "source_location ";
        auto const scopePattern = testing::anonNsScopePattern + "my_template::";
        auto const argPattern = testing::maybe_pattern(scopePattern) + "my_type";
        REQUIRE(result);
        CHECK_THAT(
            *result,
            Catch::Matchers::Matches(returnPattern + scopePattern + R"(foo\()" + argPattern + R"(\))"));
    }
}

namespace
{
    struct stacktrace
    {
        cpptrace::stacktrace inner;
    };

    struct conversion
    {
        operator std::source_location()
        {
            return std::source_location::current();
        }

        operator std::source_location() const
        {
            return std::source_location::current();
        }

        operator stacktrace()
        {
            return stacktrace{cpptrace::stacktrace::current()};
        }

        operator stacktrace() const
        {
            return stacktrace{cpptrace::stacktrace::current()};
        }
    };
}

TEST_CASE(
    "ctnp::prettify_function supports conversion-operators.",
    "[pretty]")
{
    // In general, conversion-functions won't have an explicit return type, but some clang versions generate them.
    auto const returnPattern = testing::maybe_pattern("std::source_location ");
    auto const scopePattern = testing::maybe_pattern(testing::anonNsScopePattern + "conversion::");

    SECTION("When getting function name via source_location")
    {
        auto const targetId = testing::maybe_pattern("std::") + "source_location";

        SECTION("and when converted to simple type via non-const function.")
        {
            conversion conv{};
            auto const loc = static_cast<std::source_location>(conv);
            std::string const fnName{loc.function_name()};
            CAPTURE(fnName);

            auto const result = ctnp::prettify_function(loc.function_name());

            REQUIRE(result);
            CHECK_THAT(
                *result,
                Catch::Matchers::Matches(returnPattern + scopePattern + "operator " + targetId + R"(\(\))"));
        }

        SECTION("and when converted to simple type via const function.")
        {
            conversion const conv{};
            auto const loc = static_cast<std::source_location>(conv);
            std::string const fnName{loc.function_name()};
            CAPTURE(fnName);

            auto const result = ctnp::prettify_function(loc.function_name());

            REQUIRE(result);
            CHECK_THAT(
                *result,
                Catch::Matchers::Matches(returnPattern + scopePattern + "operator " + targetId + R"(\(\) const)"));
        }
    }

    SECTION("When getting function name via stacktrace")
    {
        auto const targetId = testing::maybe_pattern("\\{anon-ns\\}::") + "stacktrace";

        SECTION("and when converted to simple type via non-const function.")
        {
            conversion conv{};
            auto const trace = static_cast<stacktrace>(conv);
            REQUIRE(!trace.inner.empty());
            std::string const fnName = trace.inner.begin()->symbol;
            CAPTURE(fnName);

            auto const result = ctnp::prettify_function(fnName);

            REQUIRE(result);
            CHECK_THAT(
                *result,
                Catch::Matchers::Matches(scopePattern + "operator " + targetId + testing::maybe_pattern(R"(\(\))")));
        }

        SECTION("When converted to simple type via const function.")
        {
            conversion const conv{};
            auto const trace = static_cast<stacktrace>(conv);
            REQUIRE(!trace.inner.empty());
            std::string const fnName = trace.inner.begin()->symbol;
            CAPTURE(fnName);

            auto const result = ctnp::prettify_function(fnName);

            REQUIRE(result);
            CHECK_THAT(
                *result,
                Catch::Matchers::Matches(scopePattern + "operator " + targetId + testing::maybe_pattern(R"(\(\) const)")));
        }
    }
}

// Todo: Let's ignore these cases for now.
/*#if MIMICPP_DETAIL_HAS_WORKING_STACKTRACE_BACKEND

constexpr auto function_type_post_processing_lambda_stacktrace = [] {
    return util::stacktrace::current();
};

constexpr auto function_type_post_processing_nested_lambda_stacktrace = [] {
    return [] {
        return util::stacktrace::current();
    }();
};

namespace
{
    [[nodiscard]]
    util::Stacktrace stacktrace_fun()
    {
        [[maybe_unused]] constexpr auto dummy = [] {};
        [[maybe_unused]] constexpr auto dummy2 = [] {};
        constexpr auto inner = [] {
            return util::stacktrace::current();
        };
        [[maybe_unused]] constexpr auto dummy3 = [] {};

        return inner();
    }

    [[nodiscard]]
    util::Stacktrace stacktrace_anon_lambda_fun()
    {
        return [] {
            return util::stacktrace::current();
        }();
    }

    template <typename... Ts>
    struct my_stacktrace_template
    {
        struct my_type
        {
        };

        [[nodiscard]]
        util::Stacktrace foo(my_type)
        {
            return util::stacktrace::current();
        }
    };
}

TEST_CASE(
    "ctnp::prettify_function enhances Stacktrace::description appearance.",
    "[print]")
{
    std::ostringstream ss{};

    SECTION("When a general function is given.")
    {
        auto const trace = util::stacktrace::current();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(testCasePattern + R"((\(\))?)"));
    }

    SECTION("When a lambda is given.")
    {
        auto const trace = function_type_post_processing_lambda_stacktrace();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(testing::lambda_pattern() + R"((\(\)(\s?const)?)?)"));
    }

    SECTION("When a nested lambda is given.")
    {
        auto const trace = function_type_post_processing_nested_lambda_stacktrace();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = testing::lambda_pattern() + " const::";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + testing::lambda_pattern() + " const"));
    }

    SECTION("When a function-local lambda is given.")
    {
        auto const trace = stacktrace_fun();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = testing::anonNsScopePattern + "stacktrace_fun::";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + testing::lambda_pattern() + " const"));
    }

    SECTION("When a function-local anon-struct is given.")
    {
        struct
        {
            [[nodiscard]]
            util::Stacktrace operator()() const
            {
                return util::stacktrace::current();
            }
        } constexpr obj{};

        auto const trace = obj();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = testCasePattern + "stacktrace_fun::" + testing::anonTypePattern + "::";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + R"(operator\(\)\(\) const)"));
    }

    SECTION("When a function-local anon-class is given.")
    {
        class
        {
        public:
            [[nodiscard]]
            util::Stacktrace operator()() const
            {
                return util::stacktrace::current();
            }
        } constexpr obj{};

        auto const trace = obj();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = "::" + testCasePattern + "::" + testing::anonTypePattern + "::";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + R"(::operator\(\)\(\) const)"));
    }

    SECTION("When a function-local anon-lambda is given.")
    {
        auto const trace = stacktrace_anon_lambda_fun();
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = testing::anonNsScopePattern + "::stacktrace_anon_lambda_fun::";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + testing::lambda_pattern()));
    }

    SECTION("When a template-dependant function is given.")
    {
        using type_t = decltype(my_typeLambda());
        auto const trace = my_stacktrace_template<type_t, int>{}.foo({});
        REQUIRE_FALSE(trace.empty());
        std::string const name = trace.description(0u);
        CAPTURE(name);

        ctnp::prettify_function(
            std::ostreambuf_iterator{ss},
            name);

        auto const scopePattern = testing::anonNsScopePattern + "::my_stacktrace_template::";
        auto const argPattern = testing::anonNsScopePattern + "::my_stacktrace_template::my_type";
        CHECK_THAT(
            ss.str(),
            Catch::Matchers::Matches(scopePattern + R"(foo\()" + argPattern + R"(\))"));
    }
}

#endif*/
