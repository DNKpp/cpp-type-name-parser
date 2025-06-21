//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_PRETTIFY_HPP
#define CTNP_PRETTIFY_HPP

#pragma once

#include "ctnp/PrintVisitor.hpp"
#include "ctnp/config/Config.hpp"
#include "ctnp/lexing/Lexer.hpp"
#include "ctnp/parsing/Parser.hpp"

namespace ctnp
{
    namespace detail
    {
        [[nodiscard]]
        constexpr std::string_view remove_template_details(std::string_view name) noexcept
        {
            if (name.ends_with(']'))
            {
                auto rest = name | std::views::reverse | std::views::drop(1);
                if (auto const openingIter = std::ranges::find(rest, '[');
                    openingIter != rest.end())
                {
                    auto const end = std::ranges::find_if_not(
                        openingIter + 1,
                        rest.end(),
                        lexing::is_space);
                    name = std::string_view{name.cbegin(), end.base()};
                }
            }

            return name;
        }
    }

    template <print_iterator OutIter>
    constexpr OutIter prettify_type(OutIter out, std::string_view name)
    {
        static_assert(parsing::parser_visitor<PrintVisitor<OutIter>>);

        PrintVisitor<OutIter> visitor{std::move(out)};
        parsing::Parser parser{std::ref(visitor), name};
        parser.parse_type();

        return visitor.out();
    }

    template <print_iterator OutIter>
    constexpr OutIter prettify_function(OutIter out, std::string_view name)
    {
        name = detail::remove_template_details(name);

        static_assert(parsing::parser_visitor<PrintVisitor<OutIter>>);

        PrintVisitor<OutIter> visitor{std::move(out)};
        parsing::Parser parser{std::ref(visitor), name};
        parser.parse_function();

        return visitor.out();
    }
}

#endif
