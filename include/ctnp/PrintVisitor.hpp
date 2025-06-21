//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_PRINT_VISITOR_HPP
#define CTNP_PRINT_VISITOR_HPP

#pragma once

#include "ctnp/Lexer.hpp"
#include "ctnp/config/Config.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace ctnp
{
    [[nodiscard]]
    inline auto const& alias_map()
    {
        static std::unordered_map<std::string_view, std::string_view> const aliases{
            {"(anonymous namespace)", "{anon-ns}"},
            {          "{anonymous}", "{anon-ns}"},
            {  "anonymous namespace", "{anon-ns}"},
            {  "anonymous-namespace", "{anon-ns}"}
        };

        return aliases;
    }

    [[nodiscard]]
    inline auto const& ignored_identifiers()
    {
        static std::unordered_set<std::string_view> const collection{
            "__cxx11",
            "__1"};

        return collection;
    }

    template <typename T>
    concept print_iterator = std::output_iterator<T, char const>;

    template <print_iterator OutIter>
    class PrintVisitor
    {
    public:
        [[nodiscard]]
        explicit PrintVisitor(OutIter out) noexcept(std::is_nothrow_move_constructible_v<OutIter>)
            : m_Out{std::move(out)}
        {
        }

        [[nodiscard]]
        constexpr OutIter out() const noexcept
        {
            return m_Out;
        }

        constexpr void unrecognized(std::string_view const content)
        {
            print_identifier(content);
        }

        static constexpr void begin()
        {
        }

        static constexpr void end()
        {
        }

        static constexpr void begin_type()
        {
        }

        static constexpr void end_type()
        {
        }

        constexpr void begin_scope()
        {
            m_Context.push_scope();
        }

        constexpr void end_scope()
        {
            m_Context.pop_scope();

            if (!std::exchange(m_IgnoreNextScopeResolution, false))
            {
                print_decoration("::");
            }
        }

        constexpr void add_identifier(std::string_view content)
        {
            if (content.starts_with("{lambda(")
                && content.ends_with('}'))
            {
                auto const closingIter = std::ranges::find(content | std::views::reverse, ')');
                print_identifier("lambda");
                print_identifier(std::string_view{closingIter.base(), content.cend() - 1});

                return;
            }

            // Lambdas can have the form `'lambda\\d*'`. Just print everything between ''.
            if (constexpr std::string_view lambdaPrefix{"'lambda"};
                content.starts_with(lambdaPrefix)
                && content.ends_with('\''))
            {
                print_identifier(content.substr(1u, content.size() - 2u));

                return;
            }

            // Msvc yields lambdas in form of `<lambda_\d+>`
            if (constexpr std::string_view lambdaPrefix{"<lambda_"};
                content.starts_with(lambdaPrefix)
                && content.ends_with('>'))
            {
                print_identifier("lambda");

                auto const numberBegin = content.cbegin() + lambdaPrefix.size();
                if (auto const numberEnd = std::ranges::find_if_not(numberBegin, content.cend() - 1u, lexing::is_digit);
                    numberBegin != numberEnd)
                {
                    print_identifier("#");
                    print_identifier({numberBegin, numberEnd});
                }

                return;
            }

            // gcc source-location yields lambdas in form of `<lambda(args)>`
            if (std::string_view constexpr lambdaPrefix{"<lambda("};
                content.starts_with(lambdaPrefix)
                && content.ends_with(")>"))
            {
                print_identifier("lambda");

                // Todo: There may be a full argument-list, which we should actually parse.
                content.remove_prefix(lambdaPrefix.size());
                print_decoration(2 == content.size() ? "()" : "(...)");

                return;
            }

            if (content.starts_with('`')
                && content.ends_with('\''))
            {
                // msvc injects `\d+' as auxiliar namespaces. Ignore them.
                if (std::ranges::all_of(content.substr(1u, content.size() - 2u), lexing::is_digit))
                {
                    m_IgnoreNextScopeResolution = true;

                    return;
                }

                content = content.substr(1u, content.size() - 2u);
            }

            if (ignored_identifiers().contains(content))
            {
                m_IgnoreNextScopeResolution = true;

                return;
            }

            auto const& aliases = alias_map();
            if (auto const iter = aliases.find(content);
                iter != aliases.cend())
            {
                content = iter->second;
            }
            print_identifier(content);
        }

        constexpr void begin_template_args([[maybe_unused]] std::ptrdiff_t const count)
        {
            print_decoration(0 == count ? "<" : "<...");
            m_Context.push_template_args();
        }

        constexpr void end_template_args()
        {
            m_Context.pop_template_args();
            print_decoration(">");
        }

        constexpr void add_arg()
        {
        }

        static constexpr void begin_function()
        {
        }

        static constexpr void end_function()
        {
        }

        static constexpr void begin_return_type()
        {
        }

        constexpr void end_return_type()
        {
            print_decoration(" ");
        }

        constexpr void begin_function_args(std::ptrdiff_t const count)
        {
            print_decoration(0 == count ? "(" : "(...");
            m_Context.push_function_args();
        }

        constexpr void end_function_args()
        {
            m_Context.pop_function_args();
            print_decoration(")");
        }

        constexpr void begin_function_ptr()
        {
            print_identifier("(");
        }

        constexpr void end_function_ptr()
        {
            print_identifier(")");
        }

        constexpr void begin_operator_identifier()
        {
            print_identifier("operator ");
        }

        static constexpr void end_operator_identifier()
        {
        }

        constexpr void add_const()
        {
            print_decoration(" const");
        }

        constexpr void add_volatile()
        {
            print_decoration(" volatile");
        }

        constexpr void add_noexcept()
        {
            print_decoration(" noexcept");
        }

        constexpr void add_ptr()
        {
            print_decoration("*");
        }

        constexpr void add_lvalue_ref()
        {
            print_decoration("&");
        }

        constexpr void add_rvalue_ref()
        {
            print_decoration("&&");
        }

    private:
        OutIter m_Out;
        bool m_IgnoreNextScopeResolution{false};

        class Context
        {
        public:
            [[nodiscard]]
            constexpr bool can_print_identifier() const noexcept
            {
                CTNP_ASSERT(0 <= m_ScopeDepth, "Invalid scope depth.");
                CTNP_ASSERT(0 <= m_FunctionArgsDepth, "Invalid function-args depth.");
                CTNP_ASSERT(0 <= m_TemplateArgsDepth, "Invalid template-args depth.");

                return m_ScopeDepth <= 1
                    && 0 == m_FunctionArgsDepth + m_TemplateArgsDepth;
            }

            [[nodiscard]]
            constexpr bool can_print_decoration() const noexcept
            {
                CTNP_ASSERT(0 <= m_FunctionArgsDepth, "Invalid function-args depth.");
                CTNP_ASSERT(0 <= m_TemplateArgsDepth, "Invalid template-args depth.");

                return 0 == m_ScopeDepth
                    && 0 == m_FunctionArgsDepth + m_TemplateArgsDepth;
            }

            void push_scope()
            {
                ++m_ScopeDepth;
            }

            void pop_scope()
            {
                CTNP_ASSERT(0 < m_ScopeDepth, "Unbalanced scope depth.");
                --m_ScopeDepth;
            }

            void push_function_args()
            {
                ++m_FunctionArgsDepth;
            }

            void pop_function_args()
            {
                CTNP_ASSERT(0 < m_FunctionArgsDepth, "Unbalanced function-args depth.");
                --m_FunctionArgsDepth;
            }

            void push_template_args()
            {
                ++m_TemplateArgsDepth;
            }

            void pop_template_args()
            {
                CTNP_ASSERT(0 < m_TemplateArgsDepth, "Unbalanced template-args depth.");
                --m_TemplateArgsDepth;
            }

        private:
            int m_ScopeDepth{};
            int m_FunctionArgsDepth{};
            int m_TemplateArgsDepth{};
        };

        Context m_Context{};

        constexpr void print_identifier(std::string_view const text)
        {
            if (m_Context.can_print_identifier())
            {
                m_Out = std::ranges::copy(text, std::move(m_Out)).out;
            }
        }

        constexpr void print_decoration(std::string_view const text)
        {
            if (m_Context.can_print_decoration())
            {
                m_Out = std::ranges::copy(text, std::move(m_Out)).out;
            }
        }
    };
}

#endif
