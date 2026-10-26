//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_LEXING_TOKENS_HPP
#define CTNP_DETAIL_LEXING_TOKENS_HPP

#pragma once

#include "ctnp-detail/util/ConcatArrays.hpp"
#include "ctnp-detail/util/RangesBinaryFind.hpp"
#include "ctnp-detail/util/RangesContains.hpp"
#include "ctnp/Macros.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <string_view>
#include <variant>

namespace ctnp::detail::lexing
{
    /**
     * \defgroup DETAIL_LEXING
     * \ingroup DETAIL
     *
     * \{
     */

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

    struct space
    {
        [[nodiscard]]
        bool operator==(space const&) const = default;
    };

    struct identifier
    {
        std::string_view content;

        [[nodiscard]]
        bool operator==(identifier const&) const = default;
    };

    struct literal
    {
        std::string_view content;

        [[nodiscard]]
        bool operator==(literal const&) const = default;
    };

    struct end
    {
        [[nodiscard]]
        bool operator==(end const&) const = default;
    };

    namespace texts
    {
        // just list the noteworthy ones here
        inline constexpr std::array visibilityKeywords = std::to_array<std::string_view>({"public", "protected", "private"});
        inline constexpr std::array specKeywords = std::to_array<std::string_view>({"const", "constexpr", "volatile", "noexcept", "static", "mutable"});
        inline constexpr std::array contextKeywords = std::to_array<std::string_view>({"operator", "struct", "class", "enum"});
        inline constexpr std::array typeKeywords = std::to_array<std::string_view>(
            // The `__int64` keyword is used by msvc as an alias for `long long`.
            {"auto", "void", "bool", "char", "char8_t", "char16_t", "char32_t", "wchar_t", "double", "float", "int", "long", "__int64", "short", "signed", "unsigned"});
        inline constexpr std::array otherKeywords = std::to_array<std::string_view>({"new", "delete", "co_await"});
        inline constexpr std::array digraphs = std::to_array<std::string_view>(
            {"and", "or", "xor", "not", "bitand", "bitor", "compl", "and_eq", "or_eq", "xor_eq", "not_eq"});

        inline constexpr std::array braceLikes = std::to_array<std::string_view>({"{", "}", "[", "]", "(", ")", "`", "'"});
        inline constexpr std::array comparison = std::to_array<std::string_view>({"==", "!=", "<", "<=", ">", ">=", "<=>"});
        inline constexpr std::array assignment = std::to_array<std::string_view>({"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="});
        inline constexpr std::array incOrDec = std::to_array<std::string_view>({"++", "--"});
        inline constexpr std::array arithmetic = std::to_array<std::string_view>({"+", "-", "*", "/", "%"});
        inline constexpr std::array bitArithmetic = std::to_array<std::string_view>({"~", "&", "|", "^", "<<", ">>"});
        inline constexpr std::array logical = std::to_array<std::string_view>({"!", "&&", "||"});
        inline constexpr std::array access = std::to_array<std::string_view>({".", ".*", "->", "->*"});
        inline constexpr std::array rest = std::to_array<std::string_view>({"::", ";", ",", ":", "...", "?"});

        // These operators exist, but are in fact alternative versions and will probably never encountered.
        // inline constexpr std::array specialAngles = std::to_array<std::string_view>({"<:", ":>", "<%", "%>"});
    }

    struct keyword
    {
    public:
        static constexpr std::array textCollection = [] {
            std::array collection = util::concat_arrays(
                texts::visibilityKeywords,
                texts::specKeywords,
                texts::contextKeywords,
                texts::otherKeywords,
                texts::typeKeywords,
                texts::digraphs);

            std::ranges::sort(collection);
            CTNP_ASSERT(collection.cend() == std::ranges::unique(collection).begin(), "Fix the input!");

            return collection;
        }();

        [[nodiscard]]
        explicit constexpr keyword(std::string_view const text) noexcept
            : keyword{
                  std::ranges::distance(
                      textCollection.cbegin(),
                      util::ranges::binary_find(textCollection, text))}
        {
        }

        [[nodiscard]]
        explicit constexpr keyword(std::ptrdiff_t const keywordIndex) noexcept
            : m_KeywordIndex{keywordIndex}
        {
            CTNP_ASSERT(0 <= m_KeywordIndex && m_KeywordIndex < std::ranges::ssize(textCollection), "Invalid keyword.");
        }

        [[nodiscard]]
        constexpr std::string_view text() const noexcept
        {
            return textCollection[m_KeywordIndex];
        }

        [[nodiscard]]
        constexpr std::ptrdiff_t index() const noexcept
        {
            return m_KeywordIndex;
        }

        [[nodiscard]]
        bool operator==(keyword const&) const = default;

    private:
        std::ptrdiff_t m_KeywordIndex;
    };

    struct operator_or_punctuator
    {
    public:
        static constexpr std::array textCollection = [] {
            // see: https://eel.is/c++draft/lex.operators#nt:operator-or-punctuator
            std::array collection = util::concat_arrays(
                texts::braceLikes,
                texts::comparison,
                texts::assignment,
                texts::incOrDec,
                texts::arithmetic,
                texts::bitArithmetic,
                texts::logical,
                texts::access,
                texts::rest);
            std::ranges::sort(collection);
            CTNP_ASSERT(collection.cend() == std::ranges::unique(collection).begin(), "Fix the input!");

            return collection;
        }();

        [[nodiscard]]
        explicit constexpr operator_or_punctuator(std::string_view const& text) noexcept
            : operator_or_punctuator{
                  std::ranges::distance(
                      textCollection.cbegin(),
                      util::ranges::binary_find(textCollection, text))}
        {
        }

        [[nodiscard]]
        explicit constexpr operator_or_punctuator(std::ptrdiff_t const textIndex) noexcept
            : m_TextIndex{textIndex}
        {
            CTNP_ASSERT(0 <= m_TextIndex && m_TextIndex < std::ranges::ssize(textCollection), "Invalid operator or punctuator.");
        }

        [[nodiscard]]
        constexpr std::string_view text() const noexcept
        {
            return textCollection[m_TextIndex];
        }

        [[nodiscard]]
        constexpr std::ptrdiff_t index() const noexcept
        {
            return m_TextIndex;
        }

        [[nodiscard]]
        bool operator==(operator_or_punctuator const&) const = default;

    private:
        std::ptrdiff_t m_TextIndex;
    };

    using token_class = std::variant<
        end,
        space,
        keyword,
        operator_or_punctuator,
        identifier,
        literal>;

    struct token
    {
        std::string_view content;
        token_class classification;
    };

    /**
     * \}
     */
}

#endif
