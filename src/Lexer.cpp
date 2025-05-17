//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/Lexer.hpp"
#include "ctnp/Algorithm.hpp"
#include "ctnp/config/Config.hpp"

#include <algorithm>
#include <functional>
#include <tuple>

namespace ctnp::lexing
{
    [[nodiscard]]
    Token Lexer::find_next() noexcept
    {
        if (m_Text.empty())
        {
            return Token{
                .content = {m_Text.cend(), m_Text.cend()},
                .classification = token::End{}
            };
        }

        if (is_space(m_Text.front()))
        {
            // Multiple consecutive spaces or any whitespace character other than a single space
            // carry no meaningful semantic value beyond delimitation.
            // Although single spaces may sometimes influence the result and sometimes not,
            // complicating the overall process, we filter out all non-single whitespace characters here.
            if (std::string_view const content = next_as_space();
                " " == content)
            {
                return Token{
                    .content = content,
                    .classification = token::Space{}};
            }

            return find_next();
        }

        if (auto const options = util::prefix_range(
                token::OperatorOrPunctuator::textCollection,
                m_Text.substr(0u, 1u)))
        {
            return next_as_op_or_punctuator(options);
        }

        std::string_view const content = next_as_identifier();
        // As we do not perform any prefix-checks, we need to check now whether the token actually denotes a keyword.
        if (auto const iter = util::binary_find(token::Keyword::textCollection, content);
            iter != token::Keyword::textCollection.cend())
        {
            return Token{
                .content = content,
                .classification = token::Keyword{std::ranges::distance(token::Keyword::textCollection.begin(), iter)}};
        }

        return Token{
            .content = content,
            .classification = token::Identifier{.content = content}};
    }

    constexpr std::string_view Lexer::next_as_space() noexcept
    {
        auto const end = std::ranges::find_if_not(m_Text.cbegin() + 1, m_Text.cend(), is_space);
        std::string_view const content{m_Text.cbegin(), end};
        m_Text = std::string_view{end, m_Text.cend()};

        return content;
    }

    constexpr Token Lexer::next_as_op_or_punctuator(std::span<std::string_view const> options) noexcept
    {
        CTNP_ASSERT(m_Text.substr(0u, 1u) == options.front(), "Assumption does not hold.");

        auto const try_advance = [&, this](std::size_t const n) {
            if (n <= m_Text.size())
            {
                return util::prefix_range(
                    options,
                    std::string_view{m_Text.cbegin(), m_Text.cbegin() + n});
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

        auto const index = std::ranges::distance(token::OperatorOrPunctuator::textCollection.data(), lastMatch);
        std::string_view const content{m_Text.substr(0u, lastMatch->size())};
        m_Text.remove_prefix(lastMatch->size());

        return Token{
            .content = content,
            .classification = token::OperatorOrPunctuator{index}};
    }

    constexpr std::string_view Lexer::next_as_identifier() noexcept
    {
        auto const last = std::ranges::find_if_not(
            m_Text.cbegin() + 1,
            m_Text.cend(),
            [](auto const c) {
                return !is_space(c)
                    && !std::ranges::binary_search(token::OperatorOrPunctuator::textCollection, std::string_view{&c, 1u});
            });

        std::string_view const content{m_Text.cbegin(), last};
        m_Text = {last, m_Text.cend()};

        return content;
    }
}
