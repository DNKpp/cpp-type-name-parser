//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_PARSING_MISC_HPP
#define CTNP_DETAIL_PARSING_MISC_HPP

#pragma once

#include "ctnp-detail/lexing/Lexer.hpp"
#include "ctnp-detail/util/PassKey.hpp"
#include "ctnp/Macros.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace ctnp::detail::parsing
{
    class Transaction;

    class TokenStream
    {
    public:
        [[nodiscard]]
        explicit constexpr TokenStream(lexing::Lexer& lexer) noexcept
        {
            while (!std::holds_alternative<lexing::end>(lexer.peek().classification))
            {
                if (auto token = lexer.next();
                    !std::holds_alternative<lexing::space>(token.classification))
                {
                    m_tokens.emplace_back(std::move(token));
                }
            }

            m_tokens.emplace_back(lexer.next());
        }

        [[nodiscard]]
        constexpr bool is_eof() const noexcept
        {
            return m_index == std::ranges::size(m_tokens) - 1u;
        }

        [[nodiscard]]
        constexpr lexing::token const& peek() const
        {
            CTNP_ASSERT(m_index < std::ranges::size(m_tokens), "Stream is at end.");
            return std::span{m_tokens}[m_index];
        }

        constexpr void consume()
        {
            CTNP_ASSERT(!is_eof(), "EOF cannot be consumed.");
            ++m_index;
        }

        [[nodiscard]]
        constexpr std::size_t pos() const noexcept
        {
            return m_index;
        }

        constexpr void seek(util::pass_key<Transaction> const /*key*/, std::size_t const pos)
        {
            m_index = pos;
        }

    private:
        std::vector<lexing::token> m_tokens;
        std::size_t m_index{};
    };

    class Transaction
    {
    public:
        Transaction(Transaction const&) = delete;
        Transaction& operator=(Transaction const&) = delete;
        Transaction(Transaction&&) = delete;
        Transaction& operator=(Transaction&&) = delete;

        constexpr ~Transaction() noexcept
        {
            if (is_active())
            {
                m_stream->seek(util::pass_key<Transaction>{}, m_Checkpoint);
            }
        }

        [[nodiscard]]
        explicit constexpr Transaction(TokenStream& stream) noexcept
            : m_stream{std::addressof(stream)},
              m_Checkpoint{stream.pos()}
        {
        }

        constexpr void commit() noexcept
        {
            m_stream = nullptr;
        }

        [[nodiscard]]
        constexpr bool is_active() const noexcept
        {
            return m_stream != nullptr;
        }

    private:
        TokenStream* m_stream;
        std::size_t m_Checkpoint;
    };

    template <typename State>
    class StateGuard
    {
    public:
        StateGuard(StateGuard const&) = delete;
        StateGuard& operator=(StateGuard const&) = delete;
        StateGuard(StateGuard&&) = delete;
        StateGuard& operator=(StateGuard&&) = delete;

        ~StateGuard() = default;

        template <typename... Args>
            requires std::constructible_from<State, Args...>
        [[nodiscard]]
        explicit constexpr StateGuard(TokenStream& stream, Args&&... args)
            : m_transaction{stream},
              m_state{std::in_place, std::forward<Args>(args)...}
        {
        }

        [[nodiscard]]
        constexpr State& operator*() noexcept
        {
            CTNP_ASSERT(m_state, "State was already consumed.");
            return *m_state;
        }

        [[nodiscard]]
        constexpr State* operator->() noexcept
        {
            CTNP_ASSERT(m_state, "State was already consumed.");
            return &*m_state;
        }

        [[nodiscard]]
        constexpr std::optional<State> take() &&
        {
            CTNP_ASSERT(m_state, "State was already consumed.");
            m_transaction.commit();
            return std::exchange(m_state, std::nullopt);
        }

    private:
        Transaction m_transaction;
        std::optional<State> m_state;
    };

    template <std::equality_comparable Key, typename Value, std::size_t length>
    [[nodiscard]]
    constexpr auto make_map(std::pair<Key, Value> const (&mappings)[length])
    {
        class Map
        {
        public:
            [[nodiscard]]
            explicit constexpr Map(std::pair<Key, Value> const (&candidates)[length])
                : m_candidates{std::to_array(candidates)}
            {
            }

            [[nodiscard]]
            constexpr std::optional<Value> operator()(Key const& key) const
            {
                if (auto const iter = std::ranges::find(m_candidates, key, &std::pair<Key, Value>::first);
                    iter != std::ranges::end(m_candidates))
                {
                    return iter->second;
                }

                return std::nullopt;
            }

        private:
            std::array<std::pair<Key, Value>, length> m_candidates;
        };

        return Map{mappings};
    }

    template <typename LexerTokenClass>
    [[nodiscard]]
    constexpr LexerTokenClass const* peek_if(TokenStream const& stream) noexcept
    {
        return std::get_if<LexerTokenClass>(&stream.peek().classification);
    }

    template <std::equality_comparable LexerTokenClass>
    [[nodiscard]]
    constexpr std::optional<LexerTokenClass> expect(TokenStream& stream, LexerTokenClass const& expected)
    {
        if (auto const* const token = peek_if<LexerTokenClass>(stream);
            token
            && *token == expected)
        {
            std::optional result = *token;
            stream.consume();

            return result;
        }

        return std::nullopt;
    }
}

#endif
