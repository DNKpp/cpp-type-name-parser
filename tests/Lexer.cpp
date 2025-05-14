//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Lexer.hpp"

#include <optional>

using namespace ctnp;

namespace
{
    template <std::equality_comparable Value>
    class VariantEqualsMatcher final
        : public Catch::Matchers::MatcherGenericBase
    {
    public:
        [[nodiscard]]
        explicit constexpr VariantEqualsMatcher(Value value)
            : m_Value{std::move(value)}
        {
        }

        template <typename... Alternatives>
        [[nodiscard]]
        constexpr bool match(const std::variant<Alternatives...>& other) const
            requires requires { { std::holds_alternative<Value>(other) } -> std::convertible_to<bool>; }
        {
            return std::holds_alternative<Value>(other)
                && m_Value == std::get<Value>(other);
        }

        [[nodiscard]]
        std::string describe() const override
        {
            return std::string{"Variant state equals: "}
                 + mimicpp::print_type<Value>()
                 + ": "
                 + Catch::Detail::stringify(m_Value);
        }

    private:
        Value m_Value;
    };

    template <std::equality_comparable TokenClass>
        requires std::constructible_from<lexing::Token, std::string_view, TokenClass>
    class TokenMatcher final
        : public Catch::Matchers::MatcherGenericBase
    {
    public:
        [[nodiscard]]
        explicit constexpr TokenMatcher(TokenClass tokenClass)
            : m_ClassMatcher{std::move(tokenClass)}
        {
        }

        [[nodiscard]]
        explicit constexpr TokenMatcher(std::string_view content, TokenClass tokenClass)
            : m_ClassMatcher{std::move(tokenClass)},
              m_Content{std::move(content)}
        {
        }

        [[nodiscard]]
        constexpr bool match(lexing::Token const& token) const
        {
            return m_ClassMatcher.match(token.classification)
                && (!m_Content || token.content == m_Content.value());
        }

        [[nodiscard]]
        std::string describe() const override
        {
            std::string description = std::string{"Lexing-Token equals class: "}
                                    + mimicpp::print_type<TokenClass>();
            if (m_Content)
            {
                description += " and contains content: '";
                description.append(*m_Content);
                description += "'";
            }

            return description;
        }

    private:
        VariantEqualsMatcher<TokenClass> m_ClassMatcher;
        std::optional<std::string_view> m_Content{};
    };

    template <typename TokenClass>
    [[nodiscard]]
    constexpr auto matches_class(TokenClass token)
    {
        return TokenMatcher<TokenClass>{std::move(token)};
    }

    template <typename TokenClass>
    [[nodiscard]]
    constexpr auto matches_token(std::string_view const& content, TokenClass token)
    {
        return TokenMatcher<TokenClass>{content, std::move(token)};
    }

    [[nodiscard]]
    auto matches_end_token()
    {
        return matches_token("", lexing::token::End{});
    }
}

TEST_CASE(
    "printing::type::lexing::is_space determines, whether the given character is a space.",
    "[lexer]")
{
    SECTION("When a space is given, returns true.")
    {
        char const input = GENERATE(' ', '\t');

        CHECK(lexing::is_space(input));
    }

    SECTION("When no space is given, returns false.")
    {
        char const input = GENERATE('a', '_', '-', '1');

        CHECK(!lexing::is_space(input));
    }
}

TEST_CASE(
    "printing::type::lexing::is_digit determines, whether the given character is a digit.",
    "[lexer]")
{
    SECTION("When a digit is given, returns true.")
    {
        char const input = GENERATE('0', '1', '2', '3', '4', '5', '6', '7', '8', '9');

        CHECK(lexing::is_digit(input));
    }

    SECTION("When no space is given, returns false.")
    {
        char const input = GENERATE('a', 'B', '_', '-', ' ');

        CHECK(!lexing::is_digit(input));
    }
}

TEST_CASE(
    "printing::type::lexing::NameLexer extracts tokens from given input.",
    "[lexer]")
{
    SECTION("Empty input is supported.")
    {
        lexing::Lexer lexer{""};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Single spaces are detected.")
    {
        lexing::Lexer lexer{" "};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_token(" ", lexing::token::Space{}));

        CHECK_THAT(
            lexer.next(),
            matches_token(" ", lexing::token::Space{}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Multiple spaces or any non-standard space is ignored.")
    {
        std::string_view const input = GENERATE(
            // " ", single spaces are treated specially.
            "\t",
            "  ",
            "\t\t",
            "\t \t",
            " \t ");
        CAPTURE(input);

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common brace-likes are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::braceLikes));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common comparison-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::comparison));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common assignment-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::assignment));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common increment- and decrement-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::incOrDec));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common arithmetic-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::arithmetic));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common bit-arithmetic-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::bitArithmetic));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common logical-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::logical));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common access-operators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::access));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Common special angles are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::specialAngles));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("All other operators or punctuators are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::detail::texts::rest));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::OperatorOrPunctuator{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Keywords are detected.")
    {
        std::string_view const input = GENERATE(from_range(lexing::token::Keyword::textCollection));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::Keyword{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Arbitrary identifiers are detected.")
    {
        std::string_view const input = GENERATE("foo", "_123", "foo456", "const_", "_const");
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::token::Identifier{input});

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            expectedToken);

        CHECK_THAT(
            lexer.next(),
            expectedToken);
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }
}

TEST_CASE(
    "printing::type::lexing::NameLexer supports token compositions.",
    "[lexer]")
{
    SECTION("tab + identifier.")
    {
        using namespace lexing::token;
        constexpr std::string_view input = "\ttest";
        CAPTURE(input);

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(Identifier{"test"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(Identifier{"test"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Operator + operator.")
    {
        using namespace lexing::token;
        constexpr std::string_view input = "++--";
        CAPTURE(input);

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(OperatorOrPunctuator{"++"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(OperatorOrPunctuator{"++"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(OperatorOrPunctuator{"--"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(OperatorOrPunctuator{"--"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("keyword + space + identifier + operator + operator + space + keyword + operator.")
    {
        using namespace lexing::token;
        constexpr std::string_view input = "const\t foo123[] volatile&&";
        CAPTURE(input);

        std::tuple const sequence = {
            matches_class(Keyword{"const"}),
            matches_class(Identifier{"foo123"}),
            matches_class(OperatorOrPunctuator{"["}),
            matches_class(OperatorOrPunctuator{"]"}),
            matches_class(Space{}),
            matches_class(Keyword{"volatile"}),
            matches_class(OperatorOrPunctuator{"&&"})};

        lexing::Lexer lexer{input};

        std::apply(
            [&](auto&... matchers) {
                auto check = [&, i = 0](auto const& matcher) mutable {
                    CAPTURE(i);
                    ++i;
                    CHECK_THAT(
                        std::as_const(lexer).peek(),
                        matcher);
                    CHECK_THAT(
                        lexer.next(),
                        matcher);
                };

                (check(matchers), ...);
            },
            sequence);

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }
}

TEST_CASE(
    "lexing::token::OperatorOrPunctuator::text yields the token text.",
    "[lexer]")
{
    std::string const tokenText{GENERATE(from_range(lexing::token::OperatorOrPunctuator::textCollection))};
    lexing::token::OperatorOrPunctuator const token{tokenText};

    CHECK_THAT(
        std::string{token.text()},
        Catch::Matchers::Equals(tokenText));
}

TEST_CASE(
    "lexing::keyword::text yields the token text.",
    "[lexer]")
{
    std::string const tokenText{GENERATE(from_range(lexing::token::Keyword::textCollection))};
    lexing::token::Keyword const token{tokenText};

    CHECK_THAT(
        std::string{token.text()},
        Catch::Matchers::Equals(tokenText));
}
