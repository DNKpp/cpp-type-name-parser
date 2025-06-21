//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_PARSING_TOKENS_HPP
#define CTNP_PARSING_TOKENS_HPP

#include "ctnp/config/Config.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace ctnp::parsing
{
    template <typename T>
    concept parser_visitor = std::movable<T>
                          && requires(std::unwrap_reference_t<T> visitor, std::string_view content, std::ptrdiff_t count) {
                                 visitor.unrecognized(content);

                                 visitor.begin();
                                 visitor.end();

                                 visitor.begin_type();
                                 visitor.end_type();

                                 visitor.begin_scope();
                                 visitor.end_scope();

                                 visitor.add_identifier(content);
                                 visitor.add_arg();

                                 visitor.begin_template_args(count);
                                 visitor.end_template_args();

                                 visitor.add_const();
                                 visitor.add_volatile();
                                 visitor.add_noexcept();
                                 visitor.add_ptr();
                                 visitor.add_lvalue_ref();
                                 visitor.add_rvalue_ref();

                                 visitor.begin_function();
                                 visitor.end_function();
                                 visitor.begin_return_type();
                                 visitor.end_return_type();
                                 visitor.begin_function_args(count);
                                 visitor.end_function_args();

                                 visitor.begin_function_ptr();
                                 visitor.end_function_ptr();

                                 visitor.begin_operator_identifier();
                                 visitor.end_operator_identifier();
                             };

    template <parser_visitor Visitor>
    [[nodiscard]]
    constexpr auto& unwrap_visitor(Visitor& visitor) noexcept
    {
        return static_cast<
            std::add_lvalue_reference_t<
                std::unwrap_reference_t<Visitor>>>(visitor);
    }
}

namespace ctnp::parsing::token
{
    class Type;

    class Space
    {
    };

    class OperatorKeyword
    {
    };

    class ScopeResolution
    {
    public:
        std::string_view content;
    };

    class ArgSeparator
    {
    public:
        std::string_view content;
    };

    class OpeningAngle
    {
    public:
        std::string_view content;
    };

    class ClosingAngle
    {
    public:
        std::string_view content;
    };

    class OpeningParens
    {
    public:
        std::string_view content;
    };

    class ClosingParens
    {
    public:
        std::string_view content;
    };

    class OpeningCurly
    {
    public:
        std::string_view content;
    };

    class ClosingCurly
    {
    public:
        std::string_view content;
    };

    class OpeningBacktick
    {
    public:
        std::string_view content;
    };

    class ClosingSingleQuote
    {
    public:
        std::string_view content;
    };

    class TypeContext
    {
    public:
        std::string_view content;
    };

    class Specs
    {
    public:
        struct Layer
        {
            bool isConst{false};
            bool isVolatile{false};

            template <parser_visitor Visitor>
            constexpr void operator()(Visitor& visitor) const
            {
                auto& inner = unwrap_visitor(visitor);

                if (isConst)
                {
                    inner.add_const();
                }

                if (isVolatile)
                {
                    inner.add_volatile();
                }
            }
        };

        std::vector<Layer> layers{1u};

        enum Refness : std::uint8_t
        {
            none,
            lvalue,
            rvalue
        };

        Refness refness{none};
        bool isNoexcept{false};

        [[nodiscard]]
        CTNP_DETAIL_CONSTEXPR_VECTOR bool has_ptr() const noexcept
        {
            return 1u < layers.size();
        }

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            CTNP_ASSERT(!layers.empty(), "Invalid state.");

            auto& unwrapped = unwrap_visitor(visitor);

            std::invoke(layers.front(), unwrapped);

            for (auto const& layer : layers | std::views::drop(1u))
            {
                unwrapped.add_ptr();
                std::invoke(layer, unwrapped);
            }

            switch (refness)
            {
            case lvalue:
                unwrapped.add_lvalue_ref();
                break;

            case rvalue:
                unwrapped.add_rvalue_ref();
                break;

            case none: [[fallthrough]];
            default:   break;
            }

            if (isNoexcept)
            {
                unwrapped.add_noexcept();
            }
        }
    };

    class ArgSequence
    {
    public:
        std::vector<Type> types;

        CTNP_DETAIL_CONSTEXPR_VECTOR ~ArgSequence() noexcept;
        CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence();
        CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence(ArgSequence const&);
        CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence& operator=(ArgSequence const&);
        CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence(ArgSequence&&) noexcept;
        CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence& operator=(ArgSequence&&) noexcept;

        template <parser_visitor Visitor>
        CTNP_DETAIL_CONSTEXPR_VECTOR void operator()(Visitor& visitor) const;

        template <parser_visitor Visitor>
        CTNP_DETAIL_CONSTEXPR_VECTOR void handle_as_template_args(Visitor& visitor) const;
    };

    class Identifier
    {
    public:
        bool isBuiltinType{false};

        struct OperatorInfo
        {
            using Symbol = std::variant<std::string_view, std::shared_ptr<Type>>;
            Symbol symbol{};
        };

        using Content = std::variant<std::string_view, OperatorInfo>;
        Content content{};
        std::optional<ArgSequence> templateArgs{};

        [[nodiscard]]
        constexpr bool is_template() const noexcept
        {
            return templateArgs.has_value();
        }

        [[nodiscard]]
        constexpr bool is_void() const noexcept
        {
            auto const* const id = std::get_if<std::string_view>(&content);

            return id
                && "void" == *id;
        }

        [[nodiscard]]
        constexpr bool is_reserved() const noexcept
        {
            auto const* const id = std::get_if<std::string_view>(&content);

            return id
                && id->starts_with("__");
        }

        [[nodiscard]]
        constexpr bool is_builtin() const noexcept
        {
            return isBuiltinType;
        }

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            auto& unwrapped = unwrap_visitor(visitor);

            std::visit(
                [&](auto const& inner) { handle_content(unwrapped, inner); },
                content);

            if (templateArgs)
            {
                templateArgs->handle_as_template_args(unwrapped);
            }
        }

    public:
        template <parser_visitor Visitor>
        static constexpr void handle_content(Visitor& visitor, std::string_view const& content)
        {
            CTNP_ASSERT(!content.empty(), "Empty identifier is not allowed.");

            visitor.add_identifier(content);
        }

        template <parser_visitor Visitor>
        static constexpr void handle_content(Visitor& visitor, OperatorInfo const& content)
        {
            visitor.begin_operator_identifier();
            std::visit(
                [&](auto const& symbol) { handle_op_symbol(visitor, symbol); },
                content.symbol);
            visitor.end_operator_identifier();
        }

        template <parser_visitor Visitor>
        static constexpr void handle_op_symbol(Visitor& visitor, std::string_view const& symbol)
        {
            CTNP_ASSERT(!symbol.empty(), "Empty symbol is not allowed.");

            visitor.add_identifier(symbol);
        }

        template <parser_visitor Visitor>
        static constexpr void handle_op_symbol(Visitor& visitor, std::shared_ptr<Type> const& type)
        {
            CTNP_ASSERT(type, "Empty type-symbol is not allowed.");

            std::invoke(*type, visitor);
        }
    };

    class FunctionContext
    {
    public:
        ArgSequence args{};
        Specs specs{};

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const;
    };

    class FunctionIdentifier
    {
    public:
        Identifier identifier;
        FunctionContext context{};

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            auto& unwrapped = unwrap_visitor(visitor);

            std::invoke(identifier, unwrapped);
            std::invoke(context, unwrapped);
        }
    };

    class ScopeSequence
    {
    public:
        using Scope = std::variant<Identifier, FunctionIdentifier>;
        std::vector<Scope> scopes{};

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            CTNP_ASSERT(!scopes.empty(), "Empty scope-sequence is not allowed.");

            auto& unwrapped = unwrap_visitor(visitor);

            for (auto const& scope : scopes)
            {
                unwrapped.begin_scope();
                std::visit(
                    [&](auto const& id) { handle_scope(unwrapped, id); },
                    scope);
                unwrapped.end_scope();
            }
        }

    private:
        template <parser_visitor Visitor>
        constexpr void handle_scope(Visitor& visitor, Identifier const& scope) const
        {
            std::invoke(scope, visitor);
        }

        template <parser_visitor Visitor>
        constexpr void handle_scope(Visitor& visitor, FunctionIdentifier const& scope) const
        {
            visitor.begin_function();
            std::invoke(scope, visitor);
            visitor.end_function();
        }
    };

    class RegularType
    {
    public:
        std::optional<ScopeSequence> scopes{};
        Identifier identifier;
        Specs specs{};

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            auto& unwrapped = unwrap_visitor(visitor);

            unwrapped.begin_type();

            if (scopes)
            {
                std::invoke(*scopes, unwrapped);
            }

            std::invoke(identifier, unwrapped);
            std::invoke(specs, unwrapped);

            unwrapped.end_type();
        }
    };

    class FunctionType
    {
    public:
        std::shared_ptr<Type> returnType{};
        FunctionContext context{};

        template <parser_visitor Visitor>
        void operator()(Visitor& visitor) const
        {
            CTNP_ASSERT(returnType, "Return type is mandatory for function-types.");

            auto& unwrapped = unwrap_visitor(visitor);

            unwrapped.begin_function();

            unwrapped.begin_return_type();
            std::invoke(*returnType, visitor);
            unwrapped.end_return_type();

            std::invoke(context, unwrapped);

            unwrapped.end_function();
        }
    };

    class FunctionPtr
    {
    public:
        std::optional<ScopeSequence> scopes{};
        Specs specs{};

        struct NestedInfo
        {
            std::shared_ptr<FunctionPtr> ptr{};
            FunctionContext ctx{};
        };

        std::optional<NestedInfo> nested{};
    };

    class FunctionPtrType
    {
    public:
        std::shared_ptr<Type> returnType{};
        std::optional<ScopeSequence> scopes{};
        Specs specs{};
        FunctionContext context{};

        template <parser_visitor Visitor>
        constexpr void operator()(Visitor& visitor) const
        {
            CTNP_ASSERT(returnType, "Return type is mandatory for function-ptrs.");

            auto& unwrapped = unwrap_visitor(visitor);

            unwrapped.begin_type();

            unwrapped.begin_return_type();
            std::invoke(*returnType, visitor);
            unwrapped.end_return_type();

            unwrapped.begin_function_ptr();
            if (scopes)
            {
                std::invoke(*scopes, unwrapped);
            }

            std::invoke(specs, unwrapped);
            unwrapped.end_function_ptr();

            std::invoke(context, unwrapped);

            unwrapped.end_type();
        }
    };

    class Type
    {
    public:
        using State = std::variant<RegularType, FunctionType, FunctionPtrType>;
        State state;

        [[nodiscard]]
        constexpr bool is_void() const noexcept
        {
            auto const* const regularType = std::get_if<RegularType>(&state);

            return regularType
                && regularType->identifier.is_void();
        }

        [[nodiscard]]
        constexpr Specs& specs() noexcept
        {
            return std::visit(
                [&](auto& inner) noexcept -> Specs& { return specs(inner); },
                state);
        }

        template <parser_visitor Visitor>
        void operator()(Visitor& visitor) const
        {
            auto& unwrapped = unwrap_visitor(visitor);

            std::visit(
                [&](auto const& inner) { std::invoke(inner, unwrapped); },
                state);
        }

    private:
        [[nodiscard]]
        static constexpr Specs& specs(RegularType& type) noexcept
        {
            return type.specs;
        }

        [[nodiscard]]
        static constexpr Specs& specs(FunctionType& type) noexcept
        {
            return type.context.specs;
        }

        [[nodiscard]]
        static constexpr Specs& specs(FunctionPtrType& type) noexcept
        {
            return type.specs;
        }
    };

    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence::~ArgSequence() noexcept = default;
    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence::ArgSequence() = default;
    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence::ArgSequence(ArgSequence const&) = default;
    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence& ArgSequence::operator=(ArgSequence const&) = default;
    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence::ArgSequence(ArgSequence&&) noexcept = default;
    CTNP_DETAIL_CONSTEXPR_VECTOR ArgSequence& ArgSequence::operator=(ArgSequence&&) noexcept = default;

    template <parser_visitor Visitor>
    CTNP_DETAIL_CONSTEXPR_VECTOR void ArgSequence::operator()(Visitor& visitor) const
    {
        if (!types.empty())
        {
            auto& unwrapped = unwrap_visitor(visitor);

            std::invoke(types.front(), unwrapped);

            for (auto const& type : types | std::views::drop(1))
            {
                unwrapped.add_arg();
                std::invoke(type, unwrapped);
            }
        }
    }

    template <parser_visitor Visitor>
    CTNP_DETAIL_CONSTEXPR_VECTOR void ArgSequence::handle_as_template_args(Visitor& visitor) const
    {
        auto& unwrapped = unwrap_visitor(visitor);

        unwrapped.begin_template_args(std::ranges::ssize(types));
        std::invoke(*this, unwrapped);
        unwrapped.end_template_args();
    }

    template <parser_visitor Visitor>
    constexpr void FunctionContext::operator()(Visitor& visitor) const
    {
        auto& unwrapped = unwrap_visitor(visitor);

        unwrapped.begin_function_args(std::ranges::ssize(args.types));
        std::invoke(args, unwrapped);
        unwrapped.end_function_args();
        std::invoke(specs, unwrapped);
    }

    class Function
    {
    public:
        std::shared_ptr<Type> returnType{};
        std::optional<ScopeSequence> scopes{};
        FunctionIdentifier identifier{};

        template <parser_visitor Visitor>
        void operator()(Visitor& visitor) const
        {
            auto& unwrapped = unwrap_visitor(visitor);

            unwrapped.begin_function();

            if (returnType)
            {
                unwrapped.begin_return_type();
                std::invoke(*returnType, visitor);
                unwrapped.end_return_type();
            }

            if (scopes)
            {
                std::invoke(*scopes, unwrapped);
            }

            std::invoke(identifier, unwrapped);

            unwrapped.end_function();
        }
    };
}

namespace ctnp::parsing
{
    using Token = std::variant<
        token::Space,
        token::OperatorKeyword,
        token::ScopeResolution,
        token::ArgSeparator,
        token::OpeningAngle,
        token::ClosingAngle,
        token::OpeningParens,
        token::ClosingParens,
        token::OpeningCurly,
        token::ClosingCurly,
        token::OpeningBacktick,
        token::ClosingSingleQuote,
        token::TypeContext,

        token::Identifier,
        token::FunctionIdentifier,
        token::ScopeSequence,
        token::ArgSequence,
        token::FunctionContext,
        token::FunctionPtr,
        token::Specs,
        token::Type,
        token::Function>;
    using TokenStack = std::vector<Token>;

    template <typename T>
    concept token_type = requires(Token const& token) {
        { std::holds_alternative<Token>(token) } -> std::convertible_to<bool>;
    };
}

#endif
