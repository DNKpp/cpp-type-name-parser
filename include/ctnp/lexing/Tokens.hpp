//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_LEXING_TOKENS_HPP
#define CTNP_LEXING_TOKENS_HPP

#pragma once

#include "ctnp/Algorithm.hpp"
#include "ctnp/config/Config.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <variant>

namespace ctnp::lexing::detail
{
    namespace texts
    {
        constexpr std::array visibilityKeywords = std::to_array<std::string_view>({"public", "protected", "private"});
        constexpr std::array specKeywords = std::to_array<std::string_view>({"const", "constexpr", "volatile", "noexcept", "static"});
        constexpr std::array contextKeywords = std::to_array<std::string_view>({"operator", "struct", "class", "enum"});
        constexpr std::array typeKeywords = std::to_array<std::string_view>(
            // The `__int64` keyword is used by msvc as an alias for `long long`.
            {"auto", "void", "bool", "char", "char8_t", "char16_t", "char32_t", "wchar_t", "double", "float", "int", "long", "__int64", "short", "signed", "unsigned"});
        constexpr std::array otherKeywords = std::to_array<std::string_view>({"new", "delete", "co_await"});
        constexpr std::array digraphs = std::to_array<std::string_view>({"and", "or", "xor", "not", "bitand", "bitor", "compl", "and_eq", "or_eq", "xor_eq", "not_eq"});

        constexpr std::array braceLikes = std::to_array<std::string_view>({"{", "}", "[", "]", "(", ")", "`", "'"});
        constexpr std::array comparison = std::to_array<std::string_view>({"==", "!=", "<", "<=", ">", ">=", "<=>"});
        constexpr std::array assignment = std::to_array<std::string_view>({"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="});
        constexpr std::array incOrDec = std::to_array<std::string_view>({"++", "--"});
        constexpr std::array arithmetic = std::to_array<std::string_view>({"+", "-", "*", "/", "%"});
        constexpr std::array bitArithmetic = std::to_array<std::string_view>({"~", "&", "|", "^", "<<", ">>"});
        constexpr std::array logical = std::to_array<std::string_view>({"!", "&&", "||"});
        constexpr std::array access = std::to_array<std::string_view>({".", ".*", "->", "->*"});
        constexpr std::array specialAngles = std::to_array<std::string_view>({"<:", ":>", "<%", "%>"});
        constexpr std::array rest = std::to_array<std::string_view>({"::", ";", ",", ":", "...", "?"});
    }

    [[nodiscard]]
    consteval auto make_keyword_collection() noexcept
    {
        std::array collection = util::concat_arrays(
            texts::visibilityKeywords,
            texts::specKeywords,
            texts::contextKeywords,
            texts::otherKeywords,
            texts::typeKeywords,
            texts::digraphs);

        std::ranges::sort(collection);
        CTNP_ASSERT(collection.cend() == std::ranges::unique(collection).begin(), "Fix your input!");

        return collection;
    }

    [[nodiscard]]
    consteval auto make_operator_or_punctuator_collection() noexcept
    {
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
            texts::specialAngles,
            texts::rest);
        std::ranges::sort(collection);
        CTNP_ASSERT(collection.cend() == std::ranges::unique(collection).begin(), "Fix your input!");

        return collection;
    }
}

namespace ctnp::lexing::token
{
    class Space
    {
    public:
        [[nodiscard]]
        bool operator==(Space const&) const = default;
    };

    class Keyword
    {
    public:
        static constexpr std::array textCollection = detail::make_keyword_collection();

        [[nodiscard]]
        explicit constexpr Keyword(std::string_view const& text) noexcept
            : Keyword{
                  std::ranges::distance(
                      textCollection.cbegin(),
                      util::binary_find(textCollection, text))}
        {
        }

        [[nodiscard]]
        explicit constexpr Keyword(std::ptrdiff_t const keywordIndex) noexcept
            : m_KeywordIndex{keywordIndex}
        {
            CTNP_ASSERT(0 <= m_KeywordIndex && m_KeywordIndex < std::ranges::ssize(textCollection), "Invalid keyword.", m_KeywordIndex);
        }

        [[nodiscard]]
        constexpr std::string_view text() const noexcept
        {
            return textCollection[m_KeywordIndex];
        }

        [[nodiscard]]
        bool operator==(Keyword const&) const = default;

    private:
        std::ptrdiff_t m_KeywordIndex;
    };

    class OperatorOrPunctuator
    {
    public:
        static constexpr std::array textCollection = detail::make_operator_or_punctuator_collection();

        [[nodiscard]]
        explicit constexpr OperatorOrPunctuator(std::string_view const& text) noexcept
            : OperatorOrPunctuator{
                  std::ranges::distance(
                      textCollection.cbegin(),
                      util::binary_find(textCollection, text))}
        {
        }

        [[nodiscard]]
        explicit constexpr OperatorOrPunctuator(std::ptrdiff_t const textIndex) noexcept
            : m_TextIndex{textIndex}
        {
            CTNP_ASSERT(0 <= m_TextIndex && m_TextIndex < std::ranges::ssize(textCollection), "Invalid operator or punctuator.", m_TextIndex);
        }

        [[nodiscard]]
        constexpr std::string_view text() const noexcept
        {
            return textCollection[m_TextIndex];
        }

        [[nodiscard]]
        bool operator==(OperatorOrPunctuator const&) const = default;

    private:
        std::ptrdiff_t m_TextIndex;
    };

    class Identifier
    {
    public:
        std::string_view content;

        [[nodiscard]]
        bool operator==(Identifier const&) const = default;
    };

    struct End
    {
        [[nodiscard]]
        bool operator==(End const&) const = default;
    };

    using TokenClass = std::variant<
        End,
        Space,
        Keyword,
        OperatorOrPunctuator,
        Identifier>;
}

namespace ctnp::lexing
{
    class Token
    {
    public:
        std::string_view content;
        token::TokenClass classification;
    };
}

#endif
