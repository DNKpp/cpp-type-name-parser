#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace ctnp::testing
{
    template <typename Value, std::predicate<Value const&> Comparator>
    class VariantMatchesMatcher final
        : public Catch::Matchers::MatcherGenericBase
    {
    public:
        [[nodiscard]]
        explicit constexpr VariantMatchesMatcher(std::string description, Comparator comparer = Comparator{})
            : m_description{std::move(description)},
              m_compare{std::move(comparer)}
        {
        }

        template <typename... Alternatives>
        [[nodiscard]]
        bool match(std::variant<Alternatives...> const& other) const
            requires requires { { std::holds_alternative<Value>(other) } -> std::convertible_to<bool>; }
        {
            UNSCOPED_CAPTURE(other.index(), other);

            return std::holds_alternative<Value>(other)
                && std::invoke(m_compare, std::get<Value>(other));
        }

        [[nodiscard]]
        constexpr std::string describe() const override
        {
            return m_description;
        }

    private:
        std::string m_description;
        Comparator m_compare;
    };

    template <typename Alternative, std::predicate<Alternative const&> Comparator>
    [[nodiscard]]
    constexpr auto variant_matches(Comparator compare, std::string description = "Variant matches predicate")
    {
        return VariantMatchesMatcher<Alternative, Comparator>{std::move(description), std::move(compare)};
    }

    template <typename Value>
    [[nodiscard]]
    constexpr auto variant_equals(Value&& value)
    {
        auto description = std::string{"Variant state equals: "}
                         + Catch::Detail::stringify(value);
        return variant_matches<std::remove_cvref_t<Value>>(
            std::bind_front(std::equal_to<>{}, std::forward<Value>(value)),
            std::move(description));
    }
}

