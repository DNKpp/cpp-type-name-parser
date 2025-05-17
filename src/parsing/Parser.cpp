//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/parsing/Parser.hpp"
#include "ctnp/Lexer.hpp"
#include "ctnp/config/Config.hpp"
#include "ctnp/parsing/Reductions.hpp"
#include "ctnp/parsing/Tokens.hpp"

#include <array>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <variant>

namespace ctnp::parsing::detail
{
    using parsing::is_suffix_of;

    namespace
    {
        constexpr lexing::token::OperatorOrPunctuator openingParens{"("};
        constexpr lexing::token::OperatorOrPunctuator closingParens{")"};
        constexpr lexing::token::OperatorOrPunctuator openingAngle{"<"};
        constexpr lexing::token::OperatorOrPunctuator closingAngle{">"};
        constexpr lexing::token::OperatorOrPunctuator openingCurly{"{"};
        constexpr lexing::token::OperatorOrPunctuator closingCurly{"}"};
        constexpr lexing::token::OperatorOrPunctuator openingSquare{"["};
        constexpr lexing::token::OperatorOrPunctuator closingSquare{"]"};
        constexpr lexing::token::OperatorOrPunctuator backtick{"`"};
        constexpr lexing::token::OperatorOrPunctuator singleQuote{"'"};
        constexpr lexing::token::OperatorOrPunctuator scopeResolution{"::"};
        constexpr lexing::token::OperatorOrPunctuator commaSeparator{","};
        constexpr lexing::token::OperatorOrPunctuator pointer{"*"};
        constexpr lexing::token::OperatorOrPunctuator lvalueRef{"&"};
        constexpr lexing::token::OperatorOrPunctuator rvalueRef{"&&"};
        constexpr lexing::token::OperatorOrPunctuator leftShift{"<<"};
        constexpr lexing::token::OperatorOrPunctuator rightShift{">>"};
        constexpr lexing::token::OperatorOrPunctuator plus{"+"};
        constexpr lexing::token::OperatorOrPunctuator exclamationMark{"!"};
        constexpr lexing::token::Keyword operatorKeyword{"operator"};
        constexpr lexing::token::Keyword constKeyword{"const"};
        constexpr lexing::token::Keyword volatileKeyword{"volatile"};
        constexpr lexing::token::Keyword noexceptKeyword{"noexcept"};
        constexpr lexing::token::Keyword coAwaitKeyword{"co_await"};
        constexpr lexing::token::Keyword newKeyword{"new"};
        constexpr lexing::token::Keyword deleteKeyword{"delete"};
        constexpr lexing::token::Keyword classKeyword{"class"};
        constexpr lexing::token::Keyword structKeyword{"struct"};
        constexpr lexing::token::Keyword enumKeyword{"enum"};

        constexpr std::array typeKeywordCollection = {
            lexing::token::Keyword{"auto"},
            lexing::token::Keyword{"void"},
            lexing::token::Keyword{"bool"},
            lexing::token::Keyword{"char"},
            lexing::token::Keyword{"char8_t"},
            lexing::token::Keyword{"char16_t"},
            lexing::token::Keyword{"char32_t"},
            lexing::token::Keyword{"wchar_t"},
            lexing::token::Keyword{"double"},
            lexing::token::Keyword{"float"},
            lexing::token::Keyword{"int"},
            lexing::token::Keyword{"__int64"},
            lexing::token::Keyword{"long"},
            lexing::token::Keyword{"short"},
            lexing::token::Keyword{"signed"},
            lexing::token::Keyword{"unsigned"}};
    }

    ParserImpl::ParserImpl(std::string_view const& content) noexcept
        : m_Content{content},
          m_Lexer{content}
    {
    }

    TypeResult ParserImpl::parse_type()
    {
        parse();
        token::try_reduce_as_type(m_TokenStack);

        TypeResult result{};
        if (auto* const end = match_suffix<token::Type>(m_TokenStack);
            end
            && 1u == m_TokenStack.size())
        {
            result = std::move(*end);
            m_TokenStack.clear();
        }

        return result;
    }

    FunctionResult ParserImpl::parse_function()
    {
        parse();

        if (m_HasConversionOperator)
        {
            token::reduce_as_conversion_operator_function_identifier(m_TokenStack);
        }
        else
        {
            is_suffix_of<token::FunctionIdentifier>(m_TokenStack)
                || token::try_reduce_as_function_identifier(m_TokenStack);
        }

        FunctionResult result{};
        token::try_reduce_as_function(m_TokenStack);
        if (auto* const function = match_suffix<token::Function>(m_TokenStack);
            function
            && 1u == m_TokenStack.size())
        {
            result = std::move(*function);
            m_TokenStack.clear();
        }
        else
        {
            // Well, this is a workaround to circumvent issues with lambdas on some environments.
            // gcc produces lambdas in form `<lambda()>` which are not recognized as actual functions.
            token::try_reduce_as_type(m_TokenStack);
            if (auto* const type = match_suffix<token::Type>(m_TokenStack);
                type
                && 1u == m_TokenStack.size())
            {
                result = std::move(*type);
                m_TokenStack.clear();
            }
        }

        return result;
    }

    void ParserImpl::parse()
    {
        for (lexing::Token next = m_Lexer.next();
             !std::holds_alternative<lexing::token::End>(next.classification);
             next = m_Lexer.next())
        {
            std::visit(
                [&](auto const& tokenClass) { handle_lexer_token(next.content, tokenClass); },
                next.classification);
        }
    }
    bool ParserImpl::merge_with_next_token() const noexcept
    {
        auto const* const keyword = peek_if<lexing::token::Keyword>();

        return keyword
            && util::contains(typeKeywordCollection, *keyword);
    }

    bool ParserImpl::process_simple_operator()
    {
        auto dropSpaceInput = [this] {
            if (std::holds_alternative<lexing::token::Space>(m_Lexer.peek().classification))
            {
                std::ignore = m_Lexer.next();
            }
        };

        dropSpaceInput();

        // As we assume valid input, we do not have to check for the actual symbol.
        if (auto const next = m_Lexer.peek();
            auto const* operatorToken = std::get_if<lexing::token::OperatorOrPunctuator>(&next.classification))
        {
            std::ignore = m_Lexer.next();

            auto const finishMultiOpOperator = [&, this]([[maybe_unused]] lexing::token::OperatorOrPunctuator const& expectedClosingOp) {
                auto const [closingContent, classification] = m_Lexer.next();
                CTNP_ASSERT(lexing::token::TokenClass{expectedClosingOp} == classification, "Invalid input.");

                std::string_view const content{
                    next.content.data(),
                    next.content.size() + closingContent.size()};
                m_TokenStack.emplace_back(
                    token::Identifier{
                        .content = token::Identifier::OperatorInfo{.symbol = content}});
            };

            if (openingParens == *operatorToken)
            {
                finishMultiOpOperator(closingParens);
            }
            else if (openingSquare == *operatorToken)
            {
                finishMultiOpOperator(closingSquare);
            }
            // `operator <` and `operator <<` needs to be handled carefully, as it may come in as a template:
            // `operator<<>` is actually `operator< <>`.
            // Note: No tested c++ compiler actually allows `operator<<>`, but some environments still procude this.
            else if (leftShift == *operatorToken)
            {
                dropSpaceInput();

                if (auto const* const nextOp = peek_if<lexing::token::OperatorOrPunctuator>();
                    nextOp
                    // When next token starts a function or template, we know it's actually `operator <<`.
                    && (openingParens == *nextOp || openingAngle == *nextOp))
                {
                    m_TokenStack.emplace_back(
                        token::Identifier{
                            .content = token::Identifier::OperatorInfo{.symbol = next.content}});
                }
                // looks like an `operator< <>`, so just treat both `<` separately.
                else
                {
                    m_TokenStack.emplace_back(
                        token::Identifier{
                            .content = token::Identifier::OperatorInfo{.symbol = next.content.substr(0u, 1u)}});
                    handle_lexer_token(next.content.substr(1u, 1u), openingAngle);
                }
            }
            else
            {
                m_TokenStack.emplace_back(
                    token::Identifier{
                        .content = token::Identifier::OperatorInfo{.symbol = next.content}});
            }

            dropSpaceInput();

            return true;
        }
        else if (auto const* keywordToken = std::get_if<lexing::token::Keyword>(&next.classification);
                 keywordToken
                 && util::contains(std::array{newKeyword, deleteKeyword, coAwaitKeyword}, *keywordToken))
        {
            std::ignore = m_Lexer.next();

            std::string_view content = next.content;

            if (newKeyword == *keywordToken || deleteKeyword == *keywordToken)
            {
                dropSpaceInput();

                if (auto const* const opAfter = peek_if<lexing::token::OperatorOrPunctuator>();
                    opAfter
                    && openingSquare == *opAfter)
                {
                    // Strip `[]` or `[ ]` from the input.
                    std::ignore = m_Lexer.next();
                    dropSpaceInput();
                    auto const closing = m_Lexer.next();
                    CTNP_ASSERT(closingSquare == std::get<lexing::token::OperatorOrPunctuator>(closing.classification), "Invalid input.");

                    content = std::string_view{
                        next.content.data(),
                        closing.content.data() + closing.content.size()};
                }
            }

            m_TokenStack.emplace_back(
                token::Identifier{
                    .content = token::Identifier::OperatorInfo{.symbol = content}});

            dropSpaceInput();

            return true;
        }

        return false;
    }

    void ParserImpl::unwrap_msvc_like_function()
    {
        CTNP_ASSERT(is_suffix_of<token::FunctionIdentifier>(m_TokenStack), "Invalid state.", m_TokenStack);

        auto funIdentifier = std::get<token::FunctionIdentifier>(m_TokenStack.back());
        m_TokenStack.pop_back();

        std::optional<token::ScopeSequence> scopes{};
        if (auto* const scopeSeq = match_suffix<token::ScopeSequence>(m_TokenStack))
        {
            scopes = std::move(*scopeSeq);
            m_TokenStack.pop_back();
        }

        // Ignore return-types.
        if (is_suffix_of<token::Type>(m_TokenStack))
        {
            m_TokenStack.pop_back();
        }

        CTNP_ASSERT(match_suffix<token::OpeningBacktick>(m_TokenStack), "Invalid state.", m_TokenStack);
        m_TokenStack.pop_back();

        // As we gather spaces in front of backticks, there may be a space here, too.
        if (is_suffix_of<token::Space>(m_TokenStack))
        {
            m_TokenStack.pop_back();
        }

        CTNP_ASSERT(!is_suffix_of<token::ScopeSequence>(m_TokenStack), "Invalid state.", m_TokenStack);
        if (scopes)
        {
            m_TokenStack.emplace_back(*std::move(scopes));
        }

        m_TokenStack.emplace_back(std::move(funIdentifier));
    }

    void ParserImpl::handle_lexer_token([[maybe_unused]] std::string_view const content, [[maybe_unused]] lexing::token::End const& end)
    {
        // util::unreachable();
    }

    void ParserImpl::handle_lexer_token(std::string_view const content, [[maybe_unused]] lexing::token::Space const& space)
    {
        if (auto* const id = match_suffix<token::Identifier>(m_TokenStack))
        {
            // See, whether we need to merge the current builtin identifier with another one.
            // E.g. `long long` or `unsigned int`.
            if (id->is_builtin()
                && merge_with_next_token())
            {
                auto& curContent = std::get<std::string_view>(id->content);
                auto const [nextContent, _] = m_Lexer.next();
                // Merge both keywords by simply treating them as contiguous content.
                CTNP_ASSERT(curContent.data() + curContent.size() == content.data(), "Violated expectation.");
                CTNP_ASSERT(content.data() + content.size() == nextContent.data(), "Violated expectation.");
                curContent = std::string_view{
                    curContent.data(),
                    nextContent.data() + nextContent.size()};

                return;
            }

            token::try_reduce_as_type(m_TokenStack);
        }

        // In certain cases, a space after an identifier has semantic significance.
        // For example, consider the type names `void ()` and `foo()`:
        // - `void ()` represents a function type returning `void`.
        // - `foo()` represents a function named `foo`.
        if (auto const* const nextOp = peek_if<lexing::token::OperatorOrPunctuator>();
            nextOp
            && util::contains(std::array{openingAngle, openingParens, openingCurly, singleQuote, backtick}, *nextOp))
        {
            m_TokenStack.emplace_back(token::Space{});
        }
    }

    void ParserImpl::handle_lexer_token([[maybe_unused]] std::string_view const content, lexing::token::Identifier const& identifier)
    {
        m_TokenStack.emplace_back(
            token::Identifier{.content = identifier.content});
    }

    void ParserImpl::handle_lexer_token(std::string_view const content, lexing::token::Keyword const& keyword)
    {
        if (constKeyword == keyword)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            CTNP_ASSERT(!specs.layers.empty(), "Zero spec layers detected.");
            auto& top = specs.layers.back();
            CTNP_ASSERT(!top.isConst, "Specs is already const.");
            top.isConst = true;
        }
        else if (volatileKeyword == keyword)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            CTNP_ASSERT(!specs.layers.empty(), "Zero spec layers detected.");
            auto& top = specs.layers.back();
            CTNP_ASSERT(!top.isVolatile, "Specs is already volatile.");
            top.isVolatile = true;
        }
        else if (noexceptKeyword == keyword)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            CTNP_ASSERT(!specs.isNoexcept, "Specs already is a noexcept.");
            specs.isNoexcept = true;
        }
        else if (operatorKeyword == keyword && !process_simple_operator())
        {
            // Conversion operators can not be part of a scope, thus they can not appear multiple times in a single type-name.
            CTNP_ASSERT(!m_HasConversionOperator, "Multiple conversion operators detected.");

            m_TokenStack.emplace_back(token::OperatorKeyword{});
            m_HasConversionOperator = true;
        }
        else if (constexpr std::array collection{classKeyword, structKeyword, enumKeyword};
                 util::contains(collection, keyword))
        {
            // This token is needed, so we do not accidentally treat e.g. `(anonymous class)` as function args,
            // because otherwise there would just be the `anonymous` identifier left.
            m_TokenStack.emplace_back(token::TypeContext{.content = content});
        }
        else if (util::contains(typeKeywordCollection, keyword))
        {
            m_TokenStack.emplace_back(
                token::Identifier{
                    .isBuiltinType = true,
                    .content = content});
        }
    }

    void ParserImpl::handle_lexer_token(std::string_view const content, lexing::token::OperatorOrPunctuator const& token)
    {
        if (scopeResolution == token)
        {
            token::try_reduce_as_function_identifier(m_TokenStack);

            m_TokenStack.emplace_back(
                std::in_place_type<token::ScopeResolution>,
                content);
            token::try_reduce_as_scope_sequence(m_TokenStack);
        }
        else if (commaSeparator == token)
        {
            if (is_suffix_of<token::Type>(m_TokenStack)
                || token::try_reduce_as_type(m_TokenStack))
            {
                token::try_reduce_as_arg_sequence(m_TokenStack);
            }

            m_TokenStack.emplace_back(
                std::in_place_type<token::ArgSeparator>,
                content);
        }
        else if (lvalueRef == token)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            CTNP_ASSERT(token::Specs::Refness::none == specs.refness, "Specs already is a reference.");
            specs.refness = token::Specs::Refness::lvalue;
        }
        else if (rvalueRef == token)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            CTNP_ASSERT(token::Specs::Refness::none == specs.refness, "Specs already is a reference.");
            specs.refness = token::Specs::Refness::rvalue;
        }
        else if (pointer == token)
        {
            auto& specs = token::get_or_emplace_specs(m_TokenStack);
            specs.layers.emplace_back();
        }
        else if (openingAngle == token)
        {
            m_TokenStack.emplace_back(
                std::in_place_type<token::OpeningAngle>,
                content);
        }
        else if (closingAngle == token)
        {
            if (is_suffix_of<token::Type>(m_TokenStack)
                || token::try_reduce_as_type(m_TokenStack))
            {
                token::try_reduce_as_arg_sequence(m_TokenStack);
            }

            m_TokenStack.emplace_back(
                std::in_place_type<token::ClosingAngle>,
                content);
            token::try_reduce_as_template_identifier(m_TokenStack)
                || token::try_reduce_as_placeholder_identifier_wrapped<token::OpeningAngle, token::ClosingAngle>(m_TokenStack);
        }
        else if (openingParens == token)
        {
            m_TokenStack.emplace_back(
                std::in_place_type<token::OpeningParens>,
                content);
        }
        else if (closingParens == token)
        {
            bool isNextOpeningParens{false};
            if (auto const* const nextOp = peek_if<lexing::token::OperatorOrPunctuator>())
            {
                isNextOpeningParens = (openingParens == *nextOp);
            }

            // There can be no `(` directly after function-args, thus do not perform any reduction if such a token is found.
            // This helps when function-ptrs are given, so that we do not accidentally reduce something like `(__cdecl*)` as function-args.
            if (!isNextOpeningParens)
            {
                if (is_suffix_of<token::Type>(m_TokenStack)
                    || token::try_reduce_as_type(m_TokenStack))
                {
                    token::try_reduce_as_arg_sequence(m_TokenStack);
                }
            }

            m_TokenStack.emplace_back(
                std::in_place_type<token::ClosingParens>,
                content);

            if (bool const result = isNextOpeningParens
                                      ? token::try_reduce_as_function_ptr(m_TokenStack)
                                      : token::try_reduce_as_function_context(m_TokenStack);
                !result)
            {
                token::try_reduce_as_placeholder_identifier_wrapped<token::OpeningParens, token::ClosingParens>(m_TokenStack);
            }
        }
        else if (openingCurly == token)
        {
            m_TokenStack.emplace_back(
                std::in_place_type<token::OpeningCurly>,
                content);
        }
        else if (closingCurly == token)
        {
            m_TokenStack.emplace_back(
                std::in_place_type<token::ClosingCurly>,
                content);
            token::try_reduce_as_placeholder_identifier_wrapped<token::OpeningCurly, token::ClosingCurly>(m_TokenStack);
        }
        else if (backtick == token)
        {
            m_TokenStack.emplace_back(
                std::in_place_type<token::OpeningBacktick>,
                content);
        }
        else if (singleQuote == token)
        {
            if (token::try_reduce_as_function_identifier(m_TokenStack))
            {
                unwrap_msvc_like_function();
            }
            // Something like `id1::id2' should become id1::id2, so just remove the leading backtick.
            else if (is_suffix_of<token::OpeningBacktick, token::ScopeSequence, token::Identifier>(m_TokenStack))
            {
                m_TokenStack.erase(m_TokenStack.cend() - 3u);
            }
            else
            {
                m_TokenStack.emplace_back(
                    std::in_place_type<token::ClosingSingleQuote>,
                    content);
                // Well, some environments wrap in `' (like msvc) and some wrap in '' (libc++).
                token::try_reduce_as_placeholder_identifier_wrapped<token::OpeningBacktick, token::ClosingSingleQuote>(m_TokenStack)
                    || token::try_reduce_as_placeholder_identifier_wrapped<token::ClosingSingleQuote, token::ClosingSingleQuote>(m_TokenStack);
            }
        }
        // The current parsing process will never receive an `<<` or `>>` without a preceding `operator` keyword.
        // As the current `operator` parsing currently consumes the next op-symbol, we will never reach this point
        // with an actual left or right-shift. So, to make that easier, just split them.
        else if (leftShift == token)
        {
            handle_lexer_token(content.substr(0, 1u), openingAngle);
            handle_lexer_token(content.substr(1u, 1u), openingAngle);
        }
        else if (rightShift == token)
        {
            handle_lexer_token(content.substr(0, 1u), closingAngle);
            handle_lexer_token(content.substr(1u, 1u), closingAngle);
        }
        // The msvc c++23 `std::stacktrace` implementation adds `+0x\d+` to function identifiers.
        // The only reason to receive a `+`-token without an `operator`-token is exactly that case.
        // So, just ignore it and skip the next identifier.
        else if (plus == token)
        {
            if (auto const* const nextId = peek_if<lexing::token::Identifier>();
                nextId
                && nextId->content.starts_with("0x"))
            {
                std::ignore = m_Lexer.next();
            }
        }
        // The msvc c++23 `std::stacktrace` implementation seems to add something which looks like the executable-name as prefix.
        // The only reason to receive a `!`-token without an `operator`-token is exactly that case.
        // So, just ignore it and skip the previous identifier.
        else if (exclamationMark == token && is_suffix_of<token::Identifier>(m_TokenStack))
        {
            m_TokenStack.pop_back();
        }
    }
}
