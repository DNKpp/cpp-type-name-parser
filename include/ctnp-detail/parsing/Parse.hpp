//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_PARSING_PARSER_HPP
#define CTNP_DETAIL_PARSING_PARSER_HPP

#pragma once

#include "ctnp-detail/parsing/State.hpp"
#include "ctnp-detail/parsing/Rules.hpp"
#include "ctnp-detail/parsing/Misc.hpp"
#include "ctnp-detail/lexing/Lexer.hpp"

#include <optional>
#include <string_view>

namespace ctnp::detail
{
    [[nodiscard]]
    constexpr std::optional<parsing::state::TypeId> parse_type(std::string_view const text)
    {
        lexing::Lexer lexer{text};
        parsing::TokenStream stream{lexer};
        if (std::optional typeId = parsing::parse_type_id(stream);
            typeId
            && stream.is_eof())
        {
            return typeId;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    constexpr std::optional<parsing::state::FunctionId> parse_function(std::string_view const text)
    {
        lexing::Lexer lexer{text};
        parsing::TokenStream stream{lexer};
        if (std::optional functionId = parsing::parse_function(stream))
        {
            if (stream.is_eof()
                // Sometimes certain template details are added in separate square-brackets
                || (text.ends_with(']')
                    && parsing::expect(stream, lexing::operator_or_punctuator{"["})))
            {
                return functionId;
            }
        }

        return std::nullopt;
    }
}

#endif
