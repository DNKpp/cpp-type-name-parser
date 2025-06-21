//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Prettify.hpp"

#if CTNP_TESTING_TYPENAME_PROVIDER == 1

    #if __has_include(<cxxabi.h>)

        #include <cstdlib>
        #include <cxxabi.h>
        #include <memory>

namespace
{
    template <typename T>
    std::string type_name()
    {
        auto* const rawName = typeid(T).name();

        // see: https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
        int status{};
        using free_deleter_t = decltype([](char* c) noexcept { std::free(c); });
        std::unique_ptr<char, free_deleter_t> const demangledName{
            abi::__cxa_demangle(rawName, nullptr, nullptr, &status)};
        if (0 == status)
        {
            return {demangledName.get()};
        }

        return {rawName};
    }
}

    #else

namespace
{
    template <typename T>
    std::string type_name()
    {
        return typeid(T).name();
    }
}

    #endif

#elif CTNP_TESTING_TYPENAME_PROVIDER == 2

namespace
{
    template <typename T>
    [[nodiscard]]
    constexpr std::string_view type_name() noexcept
    {
        std::string_view const fnName = mimicpp::util::SourceLocation{}.function_name();

        auto const fnPart = std::ranges::search(fnName, std::string_view{"type_name<"});
        auto const end = std::ranges::find(
            fnName.rbegin(),
            std::make_reverse_iterator(fnPart.end()),
            '>');
        CTNP_ASSERT(end.base() != fnPart.end(), "Unexpected.");
        std::string_view const result{fnPart.end(), end.base() - 1};

        return result;
    }
}

#else

    #error "No provider set."

#endif

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

        auto my_typeFunction()
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeNoexceptFunction() noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeConstFunction() const
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeLvalueFunction() &
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeConstLvalueFunction() const&
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeRvalueFunction() &&
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto my_typeConstRvalueFunction() const&&
        {
            struct my_type
            {
            };

            return my_type{};
        }

        static auto my_typeStaticFunction()
        {
            struct my_type
            {
            };

            return my_type{};
        }

        auto operator+(int)
        {
            struct my_type
            {
            };

            return my_type{};
        }

    private:
        auto my_typePrivateFunction()
        {
            struct my_type
            {
            };

            return my_type{};
        }

    public:
        auto my_typeIndirectlyPrivateFunction()
        {
            return my_typePrivateFunction();
        }
    };

    struct my_type
    {
    };

    constexpr auto my_typeLambda = [] {
        struct my_type
        {
        };

        return my_type{};
    };

    [[maybe_unused]] auto my_typeMutableLambda = []() mutable {
        struct my_type
        {
        };

        return my_type{};
    };

    constexpr auto my_typeNoexceptLambda = []() noexcept {
        struct my_type
        {
        };

        return my_type{};
    };

    constexpr auto my_typeNestedLambda = [] {
        constexpr auto inner = [] {
            struct my_type
            {
            };

            return my_type{};
        };

        return inner();
    };

    constexpr auto my_typeNestedLambda2 = [] {
        [[maybe_unused]] constexpr auto dummy = [] {};
        [[maybe_unused]] constexpr auto dummy2 = [] {};

        constexpr auto inner = [] {
            struct my_type
            {
            };

            return my_type{};
        };
        return inner();
    };

    [[maybe_unused]] auto my_typeFreeFunction()
    {
        struct my_type
        {
        };

        return my_type{};
    }

    std::string const topLevelLambdaPattern =
        R"((\$_\d+|lambda(#\d+|\d+)?))";

    std::string const lambdaScopePattern = topLevelLambdaPattern + "::";

    std::string const anonNsScopePattern = R"(\{anon-ns\}::)";
    std::string const anonTypePattern = R"((\$_\d+|\{unnamed type#\d+\}|<unnamed( |-type-anon_)(class|struct|enum)>))";
    std::string const testCaseScopePattern = R"(CATCH2_INTERNAL_TEST_\d+::)";
    std::string const callOpScopePattern = R"(operator\s?\(\)::)";
    std::string const omittedFnArgsPattern = R"(\(\.{3}\))";
    std::string const omittedTemplateArgsPattern = R"(<\.{3}>)";
}

TEMPLATE_TEST_CASE(
    "prettify_type handles built-in types correctly.",
    "[prettify]",
    char,
    wchar_t,
    // char8_t, this causes some linker-issues on AppleClang-16 and 17
    char16_t,
    char32_t,
    short,
    int,
    long,
    long long)
{
    auto const rawName = type_name<TestType>();
    CAPTURE(rawName);

    std::ostringstream ss{};

    SECTION("When explicit signed name is given.")
    {
        using T = std::make_signed_t<TestType>;
        auto const name = type_name<T>();
        CAPTURE(name);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            name);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(std::string{name}));
    }

    SECTION("When unsigned name is given.")
    {
        using T = std::make_unsigned_t<TestType>;
        auto const name = type_name<T>();
        CAPTURE(name);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            name);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(std::string{name}));
    }
}

TEST_CASE(
    "prettify_type enhances names appearance.",
    "[prettify]")
{
    std::ostringstream ss{};

    SECTION("When type-name in anonymous-namespace is given.")
    {
        auto const rawName = type_name<my_type>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(anonNsScopePattern + "my_type"));
    }

    SECTION("When anon-class is given.")
    {
        class
        {
        } constexpr anon_class [[maybe_unused]]{};

        auto const rawName = type_name<std::remove_const_t<decltype(anon_class)>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(testCaseScopePattern + anonTypePattern));
    }

    SECTION("When anon-struct is given.")
    {
        class
        {
        } constexpr anon_struct [[maybe_unused]]{};

        auto const rawName = type_name<std::remove_const_t<decltype(anon_struct)>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(testCaseScopePattern + anonTypePattern));
    }

    SECTION("When anon-enum is given.")
    {
        enum
        {
            dummy
        } constexpr anon_enum [[maybe_unused]]{};

        auto const rawName = type_name<std::remove_const_t<decltype(anon_enum)>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(testCaseScopePattern + anonTypePattern));
    }

    SECTION("When nested type-name is given.")
    {
        auto const rawName = type_name<outer_type::my_type>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(anonNsScopePattern + "outer_type::my_type"));
    }

    SECTION("When lambda is given.")
    {
        auto const rawName = type_name<std::remove_const_t<decltype(my_typeLambda)>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                R"((auto )?)"
                + anonNsScopePattern
                + R"((?:my_typeLambda::)?)" // gcc produces this extra scope
                + topLevelLambdaPattern));
    }

    SECTION("When lambda with params is given.")
    {
        [[maybe_unused]] constexpr auto lambda = [](std::string const&) {};
        auto const rawName = type_name<std::remove_const_t<decltype(lambda)>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(testCaseScopePattern + topLevelLambdaPattern));
    }

    SECTION("When lambda-local type-name is given.")
    {
        auto const rawName = type_name<decltype(my_typeLambda())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeLambda::)?)" // gcc and clang produce this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type"));
    }

    SECTION("When mutable lambda-local type-name is given.")
    {
        auto const rawName = type_name<decltype(my_typeMutableLambda())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeMutableLambda::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type"));
    }

    SECTION("When noexcept lambda-local type-name is given.")
    {
        // noexcept doesn't seem to be part of the spec list
        auto const rawName = type_name<decltype(my_typeNoexceptLambda())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeNoexceptLambda::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type"));
    }

    SECTION("When nested lambda-local type-name is given.")
    {
        auto const rawName = type_name<decltype(my_typeNestedLambda())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeNestedLambda::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type"));
    }

    SECTION("When nested lambda-local type-name is given (more inner lambdas).")
    {
        auto const rawName = type_name<decltype(my_typeNestedLambda2())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeNestedLambda2::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type"));
    }

    SECTION("When free-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(my_typeFreeFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "my_typeFreeFunction::"
                  "my_type"));
    }

    SECTION("When public function local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type{}.my_typeFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeFunction::"
                  "my_type"));
    }

    SECTION("When public noexcept function local type-name is given.")
    {
        // noexcept has no effect
        auto const rawName = type_name<decltype(outer_type{}.my_typeNoexceptFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeNoexceptFunction::"
                  "my_type"));
    }

    SECTION("When public const-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type{}.my_typeConstFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeConstFunction::"
                  "my_type"));
    }

    SECTION("When public static-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type::my_typeStaticFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeStaticFunction::"
                  "my_type"));
    }

    SECTION("When public lvalue-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(std::declval<outer_type&>().my_typeLvalueFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeLvalueFunction::"
                  "my_type"));
    }

    SECTION("When public const lvalue-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(std::declval<outer_type const&>().my_typeConstLvalueFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeConstLvalueFunction::"
                  "my_type"));
    }

    SECTION("When public rvalue-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type{}.my_typeRvalueFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeRvalueFunction::"
                  "my_type"));
    }

    SECTION("When public const rvalue-function local type-name is given.")
    {
        auto const rawName = type_name<decltype(std::declval<outer_type const&&>().my_typeConstRvalueFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typeConstRvalueFunction::"
                  "my_type"));
    }

    SECTION("When private function local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type{}.my_typeIndirectlyPrivateFunction())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  "my_typePrivateFunction::"
                  "my_type"));
    }

    SECTION("When public operator local type-name is given.")
    {
        auto const rawName = type_name<decltype(outer_type{}.operator+(42))>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "outer_type::"
                  R"(operator\s?\+::)"
                  "my_type"));
    }
}

TEST_CASE(
    "prettify_type enhances local type-names appearance.",
    "[!mayfail][prettify]")
{
    std::ostringstream ss{};

    SECTION("When local type is queried inside the current scope.")
    {
        struct my_type
        {
        };

        auto const rawName = type_name<my_type>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            std::move(ss).str(),
            Catch::Matchers::Matches(testCaseScopePattern + R"(my_type)"));
    }

    SECTION("When local type is queried inside a lambda.")
    {
        std::invoke(
            [&] {
                struct my_type
                {
                };

                auto const rawName = type_name<my_type>();
                CAPTURE(rawName);

                ctnp::prettify_type(
                    std::ostreambuf_iterator{ss},
                    rawName);
                REQUIRE_THAT(
                    std::move(ss).str(),
                    Catch::Matchers::Matches(
                        testCaseScopePattern
                        + lambdaScopePattern
                        + callOpScopePattern
                        + "my_type"));
            });
    }

    SECTION("When local type is queried inside a member-function.")
    {
        struct outer
        {
            void operator()(std::ostringstream& _ss) const
            {
                struct my_type
                {
                };

                auto const rawName = type_name<my_type>();
                CAPTURE(rawName);

                ctnp::prettify_type(
                    std::ostreambuf_iterator{_ss},
                    rawName);
                REQUIRE_THAT(
                    std::move(_ss).str(),
                    Catch::Matchers::Matches(
                        testCaseScopePattern
                        + "outer::"
                        + callOpScopePattern
                        + "my_type"));
            }
        };

        outer{}(ss);
    }

    SECTION("When local type is queried inside a lambda with higher arity.")
    {
        // Todo: This case will currently fail, because parser does not handle arrays.

        int d1{};
        int d2[1]{};
        int* ptr = &d1;
        std::invoke(
            [](
                std::ostringstream* _ss,
                [[maybe_unused]] int&& ref,
                [[maybe_unused]] int(&arrRef)[1],
                [[maybe_unused]] int*& ptrRef) {
                struct my_type
                {
                };

                auto const rawName = type_name<my_type>();
                CAPTURE(rawName);

                ctnp::prettify_type(
                    std::ostreambuf_iterator{*_ss},
                    rawName);
                REQUIRE_THAT(
                    std::move(*_ss).str(),
                    Catch::Matchers::Matches(
                        testCaseScopePattern
                        + lambdaScopePattern
                        + callOpScopePattern
                        + "my_type"));
            },
            &ss,
            std::move(d1),
            d2,
            ptr);
    }

    SECTION("When local type is queried inside a nested-lambda with higher arity.")
    {
        std::invoke(
            [](std::ostringstream* _ss) {
                struct other_type
                {
                };

                std::invoke(
                    [&]([[maybe_unused]] other_type const& dummy) {
                        struct my_type
                        {
                        };

                        auto const rawName = type_name<my_type>();
                        CAPTURE(rawName);

                        ctnp::prettify_type(
                            std::ostreambuf_iterator{*_ss},
                            rawName);
                        REQUIRE_THAT(
                            std::move(*_ss).str(),
                            Catch::Matchers::Matches(
                                testCaseScopePattern
                                + lambdaScopePattern
                                + callOpScopePattern
                                + lambdaScopePattern
                                + callOpScopePattern
                                + "my_type"));
                    },
                    other_type{});
            },
            &ss);
    }
}

TEST_CASE(
    "prettify_type type-names enhances function type-names appearance.",
    "[prettify]")
{
    std::ostringstream ss{};

    SECTION("When function-local type is returned.")
    {
        using return_t = decltype(my_typeLambda());
        auto const rawName = type_name<return_t()>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeLambda::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type "
                  R"(\(\))"));
    }

    SECTION("When function-local type is parameter.")
    {
        using param_t = decltype(my_typeLambda());
        auto const rawName = type_name<void(param_t)>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches("void " + omittedFnArgsPattern));
    }
}

TEST_CASE(
    "prettify_type enhances function-pointer type-names appearance.",
    "[prettify]")
{
    std::ostringstream ss{};

    SECTION("When function-local type is returned.")
    {
        using return_t = decltype(my_typeLambda());
        auto const rawName = type_name<return_t (*)()>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"((?:my_typeLambda::)?)" // gcc produces this extra scope
                + lambdaScopePattern
                + callOpScopePattern
                + "my_type "
                  R"(\(\*\)\(\))"));
    }

    SECTION("When function-local type is parameter.")
    {
        using param_t = decltype(my_typeLambda());
        auto const rawName = type_name<void (*)(param_t)>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(R"(void \(\*\))" + omittedFnArgsPattern));
    }

    SECTION("When function-ptr is returned.")
    {
        using ret_t = void (*)();
        auto const rawName = type_name<ret_t()>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Equals("void (*)() ()"));
    }

    SECTION("When function-ptr, which returns a function-ptr, is returned.")
    {
        using ret1_t = void (*)();
        using ret2_t = ret1_t (*)();
        auto const rawName = type_name<ret2_t()>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Equals("void (*)() (*)() ()"));
    }
}

namespace
{
    template <typename... Ts>
    struct my_template
    {
        struct my_type
        {
        };

        auto foo(my_type)
        {
            return mimicpp::util::SourceLocation{};
        }

        auto bar(my_type const&, mimicpp::util::SourceLocation* outLoc)
        {
            if (outLoc)
            {
                *outLoc = mimicpp::util::SourceLocation{};
            }

            struct bar_type
            {
            };

            return bar_type{};
        }
    };
}

TEST_CASE(
    "prettify_type enhances template type-names appearance.",
    "[prettify]")
{
    std::ostringstream ss{};

    SECTION("When template name in anonymous-namespace is given.")
    {
        auto const rawName = type_name<my_template<int>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(anonNsScopePattern + "my_template<...>"));
    }

    SECTION("When template-dependant name is given.")
    {
        auto const rawName = type_name<my_template<int, std::string const&&>::my_type>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(anonNsScopePattern + "my_template::my_type"));
    }

    SECTION("When template-dependant member-function-pointer is given.")
    {
        auto const rawName = type_name<decltype(&my_template<my_template<>>::foo)>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        std::string const pattern =
            "mimicpp::util::SourceLocation " // return type
            R"(\()"
            + anonNsScopePattern
            + R"(my_template::\*\))"
            + omittedFnArgsPattern;

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(pattern));
    }

    SECTION("When template-dependant member-function-pointer, returning local type, is given.")
    {
        auto const rawName = type_name<decltype(&my_template<my_template<>>::bar)>();
        CAPTURE(rawName);

        std::string const returnPattern =
#if MIMICPP_DETAIL_IS_MSVC // it seems msvc applies an address instead of anonymous-namespace
            R"(A0x\w+::)"
#else
            anonNsScopePattern +
#endif
            R"(my_template::bar::bar_type)";
        std::string const pattern =
            returnPattern
            + R"( \()"
            + anonNsScopePattern
            + R"(my_template::\*\))"
            + omittedFnArgsPattern;

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(pattern));
    }

    SECTION("When arbitrary template name is given.")
    {
        using type_t = decltype(my_typeLambda());
        auto const rawName = type_name<my_template<type_t&, std::string const&&>>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"(my_template)"
                + omittedTemplateArgsPattern));
    }
}

namespace
{
    class special_operators
    {
    public:
        [[nodiscard]]
        auto operator<(int) const noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        [[nodiscard]]
        auto operator<=(int) const noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        [[nodiscard]]
        auto operator>(int) const noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        [[nodiscard]]
        auto operator>=(int) const noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        [[nodiscard]]
        auto operator<(std::string) const noexcept
        {
            struct my_type
            {
                [[nodiscard]]
                auto operator>=(int) const noexcept
                {
                    struct my_type
                    {
                    };

                    return my_type{};
                }
            };

            return my_type{}.operator>=(42);
        }

        [[nodiscard]]
        auto operator<=(std::string) const noexcept
        {
            struct my_type
            {
                [[nodiscard]]
                auto operator>(int) const noexcept
                {
                    struct my_type
                    {
                    };

                    return my_type{};
                }
            };

            return my_type{}.operator>(42);
        }

        [[nodiscard]]
        auto operator>(std::string) const noexcept
        {
            struct my_type
            {
                [[nodiscard]]
                auto operator<=(int) const noexcept
                {
                    struct my_type
                    {
                    };

                    return my_type{};
                }
            };

            return my_type{}.operator<=(42);
        }

        [[nodiscard]]
        auto operator>=(std::string) const noexcept
        {
            struct my_type
            {
                [[nodiscard]]
                auto operator<(int) const noexcept
                {
                    struct my_type
                    {
                    };

                    return my_type{};
                }
            };

            return my_type{}.operator<(42);
        }

        [[nodiscard]]
        auto operator<=>(int) const noexcept
        {
            struct my_type
            {
            };

            return my_type{};
        }

        [[nodiscard]]
        auto operator()(int) const
        {
            struct my_type
            {
            };

            return my_type{};
        }
    };
}

TEST_CASE(
    "prettify_type supports operator<, <=, >, >= and <=>.",
    "[prettify]")
{
    using Input = decltype(type_name<void>());

    std::ostringstream ss{};

    SECTION("When ordering operator is used.")
    {
        auto const [expectedFunctionName, rawName] = GENERATE(
            (table<std::string, Input>)({
                { R"(operator\s?<)",  type_name<decltype(special_operators{}.operator<(42))>()},
                {R"(operator\s?<=)", type_name<decltype(special_operators{}.operator<=(42))>()},
                { R"(operator\s?>)",  type_name<decltype(special_operators{}.operator>(42))>()},
                {R"(operator\s?>=)",type_name<decltype(special_operators{}.operator>=(42))>()}
        }));
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"(special_operators::)"
                + expectedFunctionName
                + "::my_type"));
    }

    SECTION("When nested ordering operator is used.")
    {
        auto const [expectedFunctionName, expectedNestedFunctionName, rawName] = GENERATE(
            (table<std::string, std::string, Input>)({
                { R"(operator\s?<)", R"(operator\s?>=)",  type_name<decltype(special_operators{}.operator<(""))>()},
                {R"(operator\s?<=)",  R"(operator\s?>)", type_name<decltype(special_operators{}.operator<=(""))>()},
                { R"(operator\s?>)", R"(operator\s?<=)",  type_name<decltype(special_operators{}.operator>(""))>()},
                {R"(operator\s?>=)",  R"(operator\s?<)", type_name<decltype(special_operators{}.operator>=(""))>()}
        }));
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + R"(special_operators::)"
                + expectedFunctionName
                + "::my_type::"
                + expectedNestedFunctionName
                + "::my_type"));
    }

    SECTION("When spaceship-operator is used.")
    {
        auto const rawName = type_name<decltype(special_operators{}.operator<=>(42))>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "special_operators::"
                + R"(operator\s?<=>::)"
                + "my_type"));
    }
}

TEST_CASE(
    "prettify_type supports operator().",
    "[prettify]")
{
    std::ostringstream ss{};

    SECTION("When identifier contains operator() scope.")
    {
        auto const rawName = type_name<decltype(special_operators{}.operator()(42))>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);
        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                anonNsScopePattern
                + "special_operators::"
                + R"(operator\s?\(\)::)"
                + "my_type"));
    }

    SECTION("When member-function-pointer to operator() is given.")
    {
        auto const rawName = type_name<decltype(&special_operators::operator())>();
        CAPTURE(rawName);

        ctnp::prettify_type(
            std::ostreambuf_iterator{ss},
            rawName);

        std::string const returnPattern =
#if MIMICPP_DETAIL_IS_MSVC // it seems msvc applies an address instead of anonymous-namespace
            R"(A0x\w+::)"
#else
            anonNsScopePattern +
#endif
            R"(special_operators::operator\s?\(\)::my_type )";

        REQUIRE_THAT(
            ss.str(),
            Catch::Matchers::Matches(
                returnPattern
                + R"(\()"
                + anonNsScopePattern
                + R"(special_operators::\*\))"
                + omittedFnArgsPattern
                + R"(\s?const)"));
    }
}

TEST_CASE(
    "prettify_function omits function args with just `void` content.",
    "[prettify]")
{
    std::string const name = "ret my_function<void>(void)";

    std::ostringstream ss{};
    ctnp::prettify_function(
        std::ostreambuf_iterator{ss},
        name);

    REQUIRE_THAT(
        ss.str(),
        Catch::Matchers::Equals(+"ret my_function<...>()"));
}
