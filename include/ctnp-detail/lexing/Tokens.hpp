//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_LEXING_TOKENS_HPP
#define CTNP_DETAIL_LEXING_TOKENS_HPP

#pragma once

#include "ctnp-detail/util/RangesContains.hpp"

#include <array>

namespace ctnp::detail::lexing
{
    // This is a constexpr friendly version of `std::isspace`.
    // see: https://en.cppreference.com/w/cpp/string/byte/isspace
    inline auto constexpr is_space = []<typename Char>(Char const c) noexcept {
        return util::ranges::contains(
            std::array{Char{' '}, Char{'\f'}, Char{'\n'}, Char{'\r'}, Char{'\t'}, Char{'\v'}},
            c);
    };

    // This is a constexpr friendly version of `std::isdigit`.
    // see: https://en.cppreference.com/w/cpp/string/byte/isdigit
    inline auto constexpr is_digit = []<typename Char>(Char const c) noexcept {
        return '0' <= c && c <= '9';
    };
}

#endif
