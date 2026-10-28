//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <string_view>
#include <tuple>

namespace ctnp::testing
{
    template <typename Char, std::size_t length>
    class StaticString
    {
    public:
        // We intentionally keep the null-terminator.
        std::array<Char, length + 1u> data;

        [[nodiscard]] //
        explicit(false) consteval StaticString(Char const (&arr)[length + 1u]) noexcept
        {
            std::ranges::copy(arr, data.begin());
        }

        [[nodiscard]]
        static constexpr bool empty() noexcept
        {
            return length == 0u;
        }

        [[nodiscard]]
        static constexpr std::size_t size() noexcept
        {
            return length;
        }

        [[nodiscard]]
        constexpr auto begin() const noexcept
        {
            return data.cbegin();
        }

        [[nodiscard]]
        constexpr auto end() const noexcept
        {
            return begin() + size();
        }

        [[nodiscard]]
        constexpr std::basic_string_view<Char> view() const noexcept
        {
            return {begin(), end()};
        }

        [[nodiscard]]
        constexpr std::basic_string<Char> str() const noexcept
        {
            return {begin(), end()};
        }

        [[nodiscard]]
        friend bool operator==(StaticString const&, StaticString const&) = default;

        template <std::convertible_to<std::basic_string_view<Char>> String>
        [[nodiscard]]
        constexpr bool operator==(String const& other) const noexcept
        {
            return view() == std::basic_string_view<Char>{other};
        }
    };

    template <typename Char, std::size_t length>
    StaticString(Char const (&)[length]) -> StaticString<Char, length - 1u>;

    inline std::string const anonNsScopePattern = R"(\{anon-ns\}::)";
    inline std::string const anonTypePattern = "<unnamed (class|struct|enum|tag)>";
    inline std::string const lambdaPattern = "<lambda>";

    [[nodiscard, maybe_unused]]
    inline std::string maybe_pattern(std::string const& pattern)
    {
        return "(:?" + pattern + ")?";
    }

    [[nodiscard, maybe_unused]]
    std::string alternative_pattern(std::string const& first, auto const&... alts)
    {
        return "(:?" + first + ((std::string{"|"} + alts) + ...) + ")";
    }

    template <typename T, StaticString suffixText>
    struct modifier
    {
        using type = T;
        static constexpr std::string_view suffix = suffixText.view();
    };

    namespace mods
    {
        template <typename T>
        using decayed = modifier<T, "">;

        template <typename T>
        using add_lvalue_ref = modifier<T&, "&">;
        template <typename T>
        using add_const_lvalue_ref = modifier<T const&, " const&">;
        template <typename T>
        using add_volatile_lvalue_ref = modifier<T volatile&, " volatile&">;
        template <typename T>
        using add_cv_lvalue_ref = modifier<T const volatile&, " const volatile&">;

        template <typename T>
        using add_rvalue_ref = modifier<T&&, "&&">;
        template <typename T>
        using add_const_rvalue_ref = modifier<T const&&, " const&&">;
        template <typename T>
        using add_volatile_rvalue_ref = modifier<T volatile&&, " volatile&&">;
        template <typename T>
        using add_cv_rvalue_ref = modifier<T const volatile&&, " const volatile&&">;

        template <typename T>
        using add_pointer = modifier<T*, "\\*">;
        template <typename T>
        using add_const_pointer = modifier<T const*, " const\\*">;
    }

    template <template <typename> typename Mod>
    struct modifier_maker
    {
        template <typename T>
        using type = Mod<T>::type;

        static constexpr std::string_view suffix = Mod<int>::suffix;
    };

    template <typename Mod, typename T>
    using mod_type_t = Mod::template type<T>;

    using common_mod_list = std::tuple<
        modifier_maker<mods::decayed>,

        modifier_maker<mods::add_lvalue_ref>,
        modifier_maker<mods::add_const_lvalue_ref>,
        modifier_maker<mods::add_volatile_lvalue_ref>,
        modifier_maker<mods::add_cv_lvalue_ref>,

        modifier_maker<mods::add_rvalue_ref>,
        modifier_maker<mods::add_const_rvalue_ref>,
        modifier_maker<mods::add_volatile_rvalue_ref>,
        modifier_maker<mods::add_cv_rvalue_ref>,

        modifier_maker<mods::add_pointer>,
        modifier_maker<mods::add_const_pointer>>;
}

#define TESTING_COMMON_MODS (               \
    testing::mods::decayed,                 \
                                            \
    testing::mods::add_lvalue_ref,          \
    testing::mods::add_const_lvalue_ref,    \
    testing::mods::add_volatile_lvalue_ref, \
    testing::mods::add_cv_lvalue_ref,       \
                                            \
    testing::mods::add_rvalue_ref,          \
    testing::mods::add_const_rvalue_ref,    \
    testing::mods::add_volatile_rvalue_ref, \
    testing::mods::add_cv_rvalue_ref,       \
                                            \
    testing::mods::add_pointer,             \
    testing::mods::add_const_pointer)

