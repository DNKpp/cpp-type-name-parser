//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_LEXING_LEXER_HPP
#define CTNP_LEXING_LEXER_HPP

#pragma once

#include "ctnp/lexing/Tokens.hpp"

#include <cctype>
#include <span>
#include <string_view>
#include <utility>

namespace ctnp::lexing
{
    // see: https://en.cppreference.com/w/cpp/string/byte/isspace
    constexpr auto is_space = [](char const c) noexcept {
        return static_cast<bool>(
            std::isspace(static_cast<unsigned char>(c)));
    };

    // see: https://en.cppreference.com/w/cpp/string/byte/isdigit
    constexpr auto is_digit = [](char const c) noexcept {
        return static_cast<bool>(
            std::isdigit(static_cast<unsigned char>(c)));
    };

    class Lexer
    {
    public:
        [[nodiscard]]
        explicit Lexer(std::string_view text) noexcept
            : m_Text{std::move(text)},
              m_Next{find_next()}
        {
        }

        [[nodiscard]]
        Token next() noexcept
        {
            return std::exchange(m_Next, find_next());
        }

        [[nodiscard]]
        constexpr Token const& peek() const noexcept
        {
            return m_Next;
        }

    private:
        std::string_view m_Text;
        Token m_Next;

        [[nodiscard]]
        Token find_next() noexcept;

        [[nodiscard]]
        constexpr std::string_view next_as_space() noexcept;

        /**
         * \brief Extracts the next operator or punctuator.
         * \details Performs longest-prefix matching.
         */
        [[nodiscard]]
        constexpr Token next_as_op_or_punctuator(std::span<std::string_view const> options) noexcept;

        /**
         * \brief Extracts the next identifier.
         * \details This approach differs a lot from the general c++ process. Instead of utilizing a specific alphabet
         * of valid characters (and thus performing a whitelist-test), we use a more permissive approach here and check
         * whether the next character is not a space and not prefix of a operator or punctuator.
         * This has to be done, because demangled names may (and will!) contain various non-allowed tokens.
         *
         * As we make the assumption that the underlying name is actually correct, we do not need to check for validity
         * here. Just treat everything else as identifier and let the parser do the rest.
         */
        [[nodiscard]]
        constexpr std::string_view next_as_identifier() noexcept;
    };
}

#endif
