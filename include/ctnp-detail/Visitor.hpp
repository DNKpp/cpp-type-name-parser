//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_VISITOR_HPP
#define CTNP_DETAIL_VISITOR_HPP

#pragma once

#include "ctnp-detail/parsing/State.hpp"
#include "ctnp-detail/util/Unreachable.hpp"
#include "ctnp-detail/util/Overloaded.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>
#include <variant>

namespace ctnp::detail
{
    template <typename OutIter>
    class Visitor
    {
    public:
        [[nodiscard]]
        explicit constexpr Visitor(OutIter out)
            : m_outIter{std::move(out)}
        {
        }

        [[nodiscard]]
        constexpr OutIter out() const
        {
            return m_outIter;
        }

        constexpr void visit(parsing::state::TypeId const& typeId)
        {
            std::visit(
                [&](auto& base) { visit(base); },
                typeId.base);
            visit(typeId.qualifications);
            visit(typeId.declarator.root);
        }

        constexpr void visit(parsing::state::FunctionId const& functionId)
        {
            if (functionId.returnType)
            {
                visit(*functionId.returnType);
                put(" ");
            }

            visit(functionId.identifier);
        }

    private:
        OutIter m_outIter;
        int m_nestedDepth{0};

        constexpr void put(std::string_view const text)
        {
            m_outIter = std::ranges::copy(text, std::move(m_outIter)).out;
        }

        template <typename State>
        constexpr void visit(parsing::state::Recursive<State> const& state)
        {
            visit(*state);
        }

        constexpr void visit(parsing::state::BuiltinType::SignedSpec const spec)
        {
            switch (spec)
            {
                using Spec = parsing::state::BuiltinType::SignedSpec;
            case Spec::id_signed:
                put("signed");
                break;
            case Spec::id_unsigned:
                put("unsigned");
                break;

            default:
                util::unreachable();
            }
        }

        constexpr void visit(parsing::state::BuiltinType::SizeSpec const spec)
        {
            switch (spec)
            {
                using Spec = parsing::state::BuiltinType::SizeSpec;
            case Spec::id_short:
                put("short");
                break;
            case Spec::id_long:
                put("long");
                break;
            case Spec::id_longlong:
                put("long long");
                break;

            default:
                util::unreachable();
            }
        }

        constexpr void visit(parsing::state::BuiltinType const& type)
        {
            bool first = true;

            if (type.signedSpec)
            {
                visit(*type.signedSpec);
                first = false;
            }

            if (type.sizeSpec)
            {
                if (!std::exchange(first, false))
                {
                    put(" ");
                }

                visit(*type.sizeSpec);
            }

            if (type.base
                // Do not print redundant `int` when a size-spec is applied.
                && (!type.sizeSpec || lexing::keyword{"int"} != *type.base))
            {
                if (!std::exchange(first, false))
                {
                    put(" ");
                }

                put(type.base->text());
            }
        }

        constexpr void visit(parsing::state::RefQualifier const qualifier)
        {
            switch (qualifier)
            {
                using Qualifier = parsing::state::RefQualifier;
            case Qualifier::id_ref:
                put("&");
                break;
            case Qualifier::id_refref:
                put("&&");
                break;

            default:
                util::unreachable();
            }
        }

        constexpr void visit(parsing::state::CVQualifierSeq const qualifiers)
        {
            if (qualifiers.isConst)
            {
                put(" const");
            }

            if (qualifiers.isVolatile)
            {
                put(" volatile");
            }
        }

        constexpr void visit(parsing::state::ConstantExpression const& expression)
        {
            put(expression.content);
        }

        static constexpr std::array syntheticAliases = std::to_array<std::pair<std::string_view, std::string_view>>({
            {"{anonymous}",           "{anon-ns}"    },
            {"(anonymous namespace)", "{anon-ns}"    },

            {"`anonymous-namespace'", "{anon-ns}"    }, // msvc
            {"`anonymous namespace'", "{anon-ns}"    }, // msvc
            {"<unnamed-tag>",         "<unnamed tag>"}, // msvc
        });

        constexpr void visit(parsing::state::Identifier const& id)
        {
            if (id.isSynthetic)
            {
                if (auto const iter = std::ranges::find(syntheticAliases, id.content, [](auto const& e) { return e.first; });
                    iter != syntheticAliases.cend())
                {
                    put(iter->second);
                    return;
                }

                // These synthetic ids will be generated on clang:
                if (constexpr std::string_view prefix{"(unnamed "}, suffix{")"};
                    id.content.starts_with(prefix)
                    && id.content.ends_with(suffix))
                {
                    std::string_view classType = id.content;
                    classType.remove_prefix(prefix.size());
                    classType.remove_suffix(suffix.size());
                    classType = classType.substr(0, classType.find(" at "));

                    put("<unnamed ");
                    put(classType);
                    put(">");
                    return;
                }

                // These synthetic ids will be generated on clang as well.
                if (constexpr std::string_view prefix{"(anonymous "}, suffix{")"};
                    id.content.starts_with(prefix)
                    && id.content.ends_with(suffix))
                {
                    std::string_view classType = id.content;
                    classType.remove_prefix(prefix.size());
                    classType.remove_suffix(suffix.size());
                    classType = classType.substr(0, classType.find(" at "));

                    put("<unnamed ");
                    put(classType);
                    put(">");
                    return;
                }

                // These synthetic ids will be generated on msvc.
                if (constexpr std::string_view prefix{"<unnamed-type-anon_"}, suffix{">"};
                    id.content.starts_with(prefix)
                    && id.content.ends_with(suffix))
                {
                    std::string_view classType = id.content;
                    classType.remove_prefix(prefix.size());
                    classType.remove_suffix(suffix.size());

                    put("<unnamed ");
                    put(classType);
                    put(">");
                    return;
                }
            }

            put(id.content);
        }

        constexpr void visit(parsing::state::PointerDeclarator const& declarator)
        {
            if (declarator.scopes)
            {
                visit(*declarator.scopes);
            }

            put("*");

            if (declarator.qualifiers)
            {
                visit(*declarator.qualifiers);
            }
        }

        constexpr void visit(parsing::state::ReferenceDeclarator const& declarator)
        {
            visit(declarator.qualifier);
        }

        constexpr void visit(parsing::state::ArrayDeclarator const& declarator)
        {
            put("[");

            if (declarator.size)
            {
                visit(*declarator.size);
            }

            put("]");
        }

        constexpr void visit(parsing::state::FunctionDeclarator const& declarator)
        {
            if (0 < m_nestedDepth)
            {
                return;
            }

            put("(");

            // Sometimes params just contain a single (`ret (void)`), which will be suppressed.
            parsing::state::TypeId const voidTypeId{
                .base = parsing::state::BuiltinType{.base = lexing::keyword{"void"}},
            };
            if (1u != declarator.params.size()
                || voidTypeId != declarator.params.front())
            {
                ++m_nestedDepth;
                join(
                    declarator.params,
                    ", ",
                    "",
                    [&](auto const& arg) { visit(arg); });
                --m_nestedDepth;
            }

            put(")");

            visit(declarator.qualifiers);
            if (declarator.refQualifier)
            {
                visit(*declarator.refQualifier);
            }

            if (declarator.isNoexcept)
            {
                put(" noexcept");
            }
        }

        constexpr void visit(parsing::state::TemplateArgumentList const& args)
        {
            if (0 < m_nestedDepth)
            {
                return;
            }

            put("<");

            ++m_nestedDepth;
            join(
                args,
                ", ",
                "",
                [this](auto const& arg) {
                    std::visit(
                        [&](auto const& inner) { visit(inner); },
                        arg);
                });
            --m_nestedDepth;

            put(">");
        }

        constexpr void visit(parsing::state::OperatorFunctionId const& id)
        {
            put("operator");
            std::visit(
                util::overloaded{
                    [this](lexing::operator_or_punctuator const& op) {
                        put(op.text());
                    },
                    [this](std::pair<lexing::keyword const, bool> const& op) {
                        put(op.first.text());
                        if (op.second)
                        {
                            put("[]");
                        }
                    },
                    [this](std::span<lexing::operator_or_punctuator const, 2u> const op) {
                        put(op.front().text());
                        put(op.back().text());
                    }},
                id.symbol);
        }

        constexpr void visit(parsing::state::ConversionFunctionId const& id)
        {
            put("operator ");
            visit(id.target);
        }

        constexpr void visit(parsing::state::DestructorFunctionId const& id)
        {
            put("~");
            put(id.name.content);
        }

        constexpr void visit(parsing::state::LambdaFunctionId const& id)
        {
            put("<lambda>");

            if (id.isMutable)
            {
                put(" mutable");
            }
        }

        constexpr void visit(parsing::state::UnqualifiedId const& nested)
        {
            std::visit(
                [&](auto const& inner) { visit(inner); },
                nested.name);

            if (nested.templateArgs)
            {
                visit(*nested.templateArgs);
            }

            if (nested.functionDeclarator)
            {
                visit(*nested.functionDeclarator);
            }
        }

        constexpr void visit(std::span<parsing::state::UnqualifiedId const> scopes)
        {
            while (!scopes.empty())
            {
                auto const& current = scopes.front();
                scopes = scopes.subspan(1u);

                if (auto const* const identifier = std::get_if<parsing::state::Identifier>(&current.name);
                    identifier
                    && "__cxx11" == identifier->content)
                {
                    continue;
                }

                visit(current);
                put("::");

                // msvc appends an additional `()::` scope to lambdas, which the parser correctly identifies as `operator()::`.
                // However, this is unnecessary noise, which I'd like to omit.
                if (std::get_if<parsing::state::LambdaFunctionId>(&current.name)
                    && !scopes.empty())
                {
                    if (auto const* const next = std::get_if<parsing::state::OperatorFunctionId>(&scopes.front().name);
                        next
                        && next->is_call())
                    {
                        scopes = scopes.subspan(1u);
                    }
                }
                // msvc does not emit `~id` for destructors, but a synthetic `{dtor}` identifier instead.
                // So, let's rebuild the actual destructor identifier with the help of the previous identifier..
                else if (auto const* const currentId = std::get_if<parsing::state::Identifier>(&current.name);
                         currentId
                         && !scopes.empty())
                {
                    if (auto const* const next = std::get_if<parsing::state::Identifier>(&scopes.front().name);
                        next
                        && "{dtor}" == next->content)
                    {
                        visit(parsing::state::DestructorFunctionId{.name = *currentId});
                        put("::");
                        scopes = scopes.subspan(1u);
                    }
                }
            }
        }

        constexpr void visit(parsing::state::ScopeSequence const& sequence)
        {
            if (sequence.explicitRoot)
            {
                put("::");
            }

            ++m_nestedDepth;
            visit(sequence.scopes);
            --m_nestedDepth;
        }

        constexpr void visit(parsing::state::QualifiedId const& id)
        {
            visit(id.scopes);
            visit(id.identifier);
        }

        constexpr void visit(parsing::state::AbstractDeclarator::Layer const& declarator)
        {
            for (auto const& decoration : declarator.decorations)
            {
                std::visit(
                    [&](auto const& inner) { visit(inner); },
                    decoration);
            }

            if (declarator.nested)
            {
                put("(");
                visit(*declarator.nested);
                put(")");
            }

            if (declarator.function)
            {
                visit(*declarator.function);
            }

            std::ranges::for_each(
                declarator.arrays,
                [&](auto const& arr) { visit(arr); });
        }

        template <std::ranges::forward_range Range>
        constexpr void join(
            Range&& range,
            std::string_view const separator,
            std::string_view const terminator,
            std::invocable<std::ranges::range_reference_t<Range>> auto action)
        {
            if (std::ranges::subrange elements{range})
            {
                std::invoke(action, elements.front());
                for (auto const& element : elements.advance(1u))
                {
                    put(separator);
                    std::invoke(action, element);
                }

                put(terminator);
            }
        }
    };
}

#endif
