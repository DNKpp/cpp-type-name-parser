//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp-detail/lexing/Lexer.hpp"

using namespace ctnp::detail;

namespace
{
    template <std::equality_comparable TokenClass>
        requires std::constructible_from<lexing::token, std::string_view, TokenClass>
    class TokenMatcher final
        : public Catch::Matchers::MatcherGenericBase
    {
    public:
        [[nodiscard]]
        explicit constexpr TokenMatcher(TokenClass tokenClass)
            : m_Expected{std::move(tokenClass)}
        {
        }

        [[nodiscard]]
        explicit constexpr TokenMatcher(std::string_view content, TokenClass tokenClass)
            : m_Expected{std::move(tokenClass)},
              m_Content{std::move(content)}
        {
        }

        [[nodiscard]]
        constexpr bool match(lexing::token const& token) const
        {
            return std::holds_alternative<TokenClass>(token.classification)
                && m_Expected == std::get<TokenClass>(token.classification)
                && (!m_Content || token.content == m_Content.value());
        }

        [[nodiscard]]
        constexpr std::string describe() const override
        {
            std::string description = std::string{"Lexing-Token equals class: "}
                                    + mimicpp::print_type<TokenClass>();
            if (m_Content)
            {
                description += " and contains content: '";
                description.append(*m_Content);
                description += '\'';
            }

            return description;
        }

    private:
        TokenClass m_Expected;
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
    constexpr auto matches_end_token()
    {
        return matches_token("", lexing::end{});
    }
}

TEST_CASE(
    "lexing::Lexer extracts tokens from given input.",
    "[detail][lexing]")
{
    CONSTEXPR_SECTION("Empty input is supported.")
    {
        lexing::Lexer lexer{""};
        CONSTEXPR_CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CONSTEXPR_CHECK_THAT(
            lexer.next(),
            matches_end_token());
    };

    CONSTEXPR_SECTION("Single spaces are detected.")
    {
        lexing::Lexer lexer{" "};
        CONSTEXPR_CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_token(" ", lexing::space{}));

        CONSTEXPR_CHECK_THAT(
            lexer.next(),
            matches_token(" ", lexing::space{}));
        CONSTEXPR_CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CONSTEXPR_CHECK_THAT(
            lexer.next(),
            matches_end_token());
    };

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
        std::string_view const input = GENERATE(from_range(lexing::texts::braceLikes));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::comparison));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::assignment));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::incOrDec));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::arithmetic));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::bitArithmetic));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::logical));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::access));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::texts::rest));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::operator_or_punctuator{input});

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
        std::string_view const input = GENERATE(from_range(lexing::keyword::textCollection));
        CAPTURE(input);

        auto const expectedToken = matches_token(input, lexing::keyword{input});

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

        auto const expectedToken = matches_token(input, lexing::identifier{input});

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
    "lexing::Lexer supports token compositions.",
    "[detail][lexing]")
{
    SECTION("tab + identifier.")
    {
        constexpr std::string_view input = "\ttest";
        CAPTURE(input);

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(lexing::identifier{"test"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(lexing::identifier{"test"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("Operator + operator.")
    {
        constexpr std::string_view input = "++--";
        CAPTURE(input);

        lexing::Lexer lexer{input};
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(lexing::operator_or_punctuator{"++"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(lexing::operator_or_punctuator{"++"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_class(lexing::operator_or_punctuator{"--"}));

        CHECK_THAT(
            lexer.next(),
            matches_class(lexing::operator_or_punctuator{"--"}));
        CHECK_THAT(
            std::as_const(lexer).peek(),
            matches_end_token());

        CHECK_THAT(
            lexer.next(),
            matches_end_token());
    }

    SECTION("keyword + space + identifier + operator + operator + space + keyword + operator.")
    {
        constexpr std::string_view input = "const\t foo123[] volatile&&";
        CAPTURE(input);

        std::tuple const sequence = {
            matches_class(lexing::keyword{"const"}),
            matches_class(lexing::identifier{"foo123"}),
            matches_class(lexing::operator_or_punctuator{"["}),
            matches_class(lexing::operator_or_punctuator{"]"}),
            matches_class(lexing::space{}),
            matches_class(lexing::keyword{"volatile"}),
            matches_class(lexing::operator_or_punctuator{"&&"})};

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
    "lexing::operator_or_punctuator::text yields the token text.",
    "[detail][lexing]")
{
    std::string const tokenText{GENERATE(from_range(lexing::operator_or_punctuator::textCollection))};
    lexing::operator_or_punctuator const token{tokenText};

    CHECK_THAT(
        std::string{token.text()},
        Catch::Matchers::Equals(tokenText));
}

TEST_CASE(
    "lexing::keyword::text yields the token text.",
    "[detail][lexing]")
{
    std::string const tokenText{GENERATE(from_range(lexing::keyword::textCollection))};
    lexing::keyword const token{tokenText};

    CHECK_THAT(
        std::string{token.text()},
        Catch::Matchers::Equals(tokenText));
}
