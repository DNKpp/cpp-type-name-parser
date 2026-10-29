//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_PRETTIFY_HPP
#define CTNP_DETAIL_PRETTIFY_HPP

#pragma once

#include "ctnp-detail/Visitor.hpp"
#include "ctnp-detail/parsing/Parse.hpp"

#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace ctnp
{
    [[nodiscard]]
    constexpr std::optional<std::string> prettify_type(std::string_view const name)
    {
        if (std::optional const typeId = detail::parse_type(name))
        {
            std::string out{};
            detail::Visitor visitor{std::back_inserter(out)};
            visitor.visit(*typeId);

            return out;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    constexpr std::optional<std::string> prettify_function(std::string_view const name)
    {
        if (std::optional const functionId = detail::parse_function(name))
        {
            std::string out{};
            detail::Visitor visitor{std::back_inserter(out)};
            visitor.visit(*functionId);

            return out;
        }

        return std::nullopt;
    }
}

#endif
