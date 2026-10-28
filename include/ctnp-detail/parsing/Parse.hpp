//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_PARSING_PARSER_HPP
#define CTNP_DETAIL_PARSING_PARSER_HPP

#pragma once

#include "ctnp-detail/parsing/State.hpp"
#include "ctnp-detail/parsing/Rules.hpp"
#include "ctnp-detail/lexing/Lexer.hpp"

#include <optional>

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
}

#endif
