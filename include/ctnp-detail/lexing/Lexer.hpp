//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_LEXING_LEXER_HPP
#define CTNP_DETAIL_LEXING_LEXER_HPP

#pragma once

#include "ctnp-detail/lexing/Tokens.hpp"
#include "ctnp-detail/util/PrefixRange.hpp"
#include "ctnp-detail/util/RangesBinaryFind.hpp"
#include "ctnp/Macros.hpp"

#include <algorithm>
#include <optional>
#include <string_view>
#include <utility>

namespace ctnp::detail::lexing
{
    /**
     * \addtogroup DETAIL_LEXING
     *
     * \{
     */

    class Lexer
    {
    public:
        [[nodiscard]]
        explicit constexpr Lexer(std::string_view text) noexcept
            : m_text{std::move(text)},
              m_next{find_next()}
        {
        }

        [[nodiscard]]
        constexpr token next() noexcept
        {
            return std::exchange(m_next, find_next());
        }

        [[nodiscard]]
        constexpr token const& peek() const noexcept
        {
            return m_next;
        }

    private:
        std::string_view m_text;
        token m_next;

        [[nodiscard]]
        constexpr token find_next() noexcept
        {
            if (m_text.empty())
            {
                return token{
                    .content = {m_text.cend(), m_text.cend()},
                    .classification = end{}
                };
            }

            if (is_space(m_text.front()))
            {
                // Multiple consecutive spaces or any whitespace character other than a single space
                // carry no meaningful semantic value beyond delimitation.
                // Although single spaces may sometimes influence the result and sometimes not,
                // complicating the overall process, we filter out all non-single whitespace characters here.
                if (std::string_view const content = next_as_space();
                    " " == content)
                {
                    return token{
                        .content = content,
                        .classification = space{}};
                }

                return find_next();
            }

            if (auto const options = util::prefix_range(
                    operator_or_punctuator::textCollection,
                    m_text.substr(0u, 1u));
                !options.empty())
            {
                return next_as_op_or_punctuator(options);
            }

            if (std::optional literal = try_next_as_literal())
            {
                return token{
                    .content = literal->content,
                    .classification = *std::move(literal)};
            }

            std::string_view const content = next_as_identifier();
            // As we do not perform any prefix-checks, we need to check now whether the token actually denotes a keyword.
            if (auto const iter = util::ranges::binary_find(keyword::textCollection, content);
                iter != keyword::textCollection.cend())
            {
                return token{
                    .content = content,
                    .classification = keyword{std::ranges::distance(keyword::textCollection.begin(), iter)}};
            }

            return token{
                .content = content,
                .classification = identifier{.content = content}};
        }

        [[nodiscard]]
        constexpr std::string_view next_as_space() noexcept
        {
            auto const end = std::ranges::find_if_not(m_text.cbegin() + 1, m_text.cend(), is_space);
            std::string_view const content{m_text.cbegin(), end};
            m_text = std::string_view{end, m_text.cend()};

            return content;
        }

        /**
         * \brief Extracts the next operator or punctuator.
         * \details Performs longest-prefix matching.
         */
        [[nodiscard]]
        constexpr token next_as_op_or_punctuator(std::span<std::string_view const> options) noexcept
        {
            CTNP_ASSERT(m_text.substr(0u, 1u) == options.front(), "Assumption does not hold.");

            auto const try_advance = [&, this](std::size_t const n) {
                if (n <= m_text.size())
                {
                    return util::prefix_range(
                        options,
                        std::string_view{m_text.cbegin(), m_text.cbegin() + n});
                }

                return std::ranges::subrange{options.end(), options.end()};
            };

            std::size_t length{1u};
            std::string_view const* lastMatch = &options.front();
            while (auto const nextOptions = try_advance(length + 1))
            {
                ++length;
                options = {nextOptions.begin(), nextOptions.end()};

                // If the first string is exactly the size of the prefix, it's a match.
                if (auto const& front = options.front();
                    length == front.size())
                {
                    lastMatch = &front;
                }
            }

            CTNP_ASSERT(!options.empty(), "Invalid state.");
            CTNP_ASSERT(lastMatch, "Invalid state.");

            auto const index = std::ranges::distance(operator_or_punctuator::textCollection.data(), lastMatch);
            std::string_view const content{m_text.substr(0u, lastMatch->size())};
            m_text.remove_prefix(lastMatch->size());

            return token{
                .content = content,
                .classification = operator_or_punctuator{index}};
        }

        /**
         * \brief Extracts the next identifier.
         * \details This approach differs a lot from the general c++ process. Instead of utilizing a specific alphabet
         * of valid characters (and thus performing a whitelist-test), we use a more permissive approach here and check
         * whether the next character is not a space and not prefix of a operator or punctuator.
         * This has to be done, because demangled names may (and will!) contain various non-allowed tokens.
         *
         * As we make the assumption that the underlying name is actually correct, we do not need to check for validity here.
         * Just treat everything else as identifier and let the parser do the rest.
         */
        [[nodiscard]]
        constexpr std::string_view next_as_identifier() noexcept
        {
            auto const last = std::ranges::find_if_not(
                m_text.cbegin() + 1,
                m_text.cend(),
                [](auto const c) {
                    return !is_space(c)
                        && !std::ranges::binary_search(operator_or_punctuator::textCollection, std::string_view{&c, 1u});
                });

            std::string_view const content{m_text.cbegin(), last};
            m_text = {last, m_text.cend()};

            return content;
        }

        // This function currently implements just a very basic integer-literal detection.
        // see: https://eel.is/c++draft/lex.literal.kinds#nt:literal
        [[nodiscard]]
        constexpr std::optional<literal> try_next_as_literal() noexcept
        {
            CTNP_ASSERT(!m_text.empty(), "Empty text.");

            if (is_digit(m_text.front()))
            {
                if ('0' == m_text.front()
                    && 2u <= m_text.size())
                {
                    // Todo: one of
                    // https://eel.is/c++draft/lex.icon#nt:binary-literal
                    // https://eel.is/c++draft/lex.icon#nt:octal-literal
                    // https://eel.is/c++draft/lex.icon#nt:hexadecimal-prefix
                }

                // Todo: this is just a very naive approach.
                auto const last = std::ranges::find_if_not(m_text.cbegin() + 1, m_text.cend(), is_digit);
                std::string_view const content{m_text.cbegin(), last};
                m_text = {last, m_text.cend()};

                return literal{.content = content};
            }

            return std::nullopt;
        }
    };

    /**
     * \}
     */
}

#endif
