//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_PARSING_PARSER_HPP
#define CTNP_PARSING_PARSER_HPP

#pragma once

#include "ctnp/Lexer.hpp"
#include "ctnp/parsing/Tokens.hpp"

#include <string_view>
#include <variant>
#include <vector>
#include <optional>

namespace ctnp::parsing::detail
{
    using TypeResult = std::optional<token::Type>;
    using FunctionResult = std::variant<std::monostate, token::Function, token::Type>;

    class ParserImpl
    {
    public:
        [[nodiscard]]
        explicit ParserImpl(std::string_view const& content) noexcept;

        [[nodiscard]]
        constexpr std::string_view content() const noexcept
        {
            return m_Content;
        }

        [[nodiscard]]
        TypeResult parse_type();

        [[nodiscard]]
        FunctionResult parse_function();

    private:
        std::string_view m_Content;
        lexing::Lexer m_Lexer;
        bool m_HasConversionOperator{false};

        std::vector<Token> m_TokenStack{};

        template <typename LexerTokenClass>
        [[nodiscard]]
        constexpr LexerTokenClass const* peek_if() const noexcept
        {
            return std::get_if<LexerTokenClass>(&m_Lexer.peek().classification);
        }

        void parse();

        [[nodiscard]]
        bool merge_with_next_token() const noexcept;
        [[nodiscard]]
        bool process_simple_operator();
        void unwrap_msvc_like_function();

        static void handle_lexer_token(std::string_view content, lexing::token::End const& end);
        void handle_lexer_token(std::string_view content, lexing::token::Space const& space);
        void handle_lexer_token(std::string_view content, lexing::token::Identifier const& identifier);
        void handle_lexer_token(std::string_view content, lexing::token::Keyword const& keyword);
        void handle_lexer_token(std::string_view content, lexing::token::OperatorOrPunctuator const& token);
    };

    template <typename Visitor>
    struct ResultVisitor
    {
        Visitor& visitor;
        std::string_view content;

        constexpr void operator()([[maybe_unused]] std::monostate const) const
        {
            visitor.unrecognized(content);
        }

        constexpr void operator()(auto const& result) const
        {
            visitor.begin();
            std::invoke(result, visitor);
            visitor.end();
        }
    };
}

namespace ctnp::parsing
{
    template <parser_visitor Visitor>
    class Parser
    {
    public:
        [[nodiscard]]
        explicit constexpr Parser(Visitor visitor, std::string_view content) noexcept(std::is_nothrow_move_constructible_v<Visitor>)
            : m_Visitor{std::move(visitor)},
              m_Parser{std::move(content)}
        {
        }

        void parse_type()
        {
            auto& unwrapped = unwrap_visitor(m_Visitor);
            if (std::optional const result = m_Parser.parse_type())
            {
                unwrapped.begin();
                std::invoke(*result, m_Visitor);
                unwrapped.end();
            }
            else
            {
                unwrapped.unrecognized(m_Parser.content());
            }
        }

        void parse_function()
        {
            detail::ResultVisitor visitor{
                .visitor = unwrap_visitor(m_Visitor),
                .content = m_Parser.content()};
            std::visit(visitor, m_Parser.parse_function());
        }

    private:
        Visitor m_Visitor;
        detail::ParserImpl m_Parser;
    };
}

#endif
