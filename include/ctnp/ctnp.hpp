
//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_CTNP_HPP
#define CTNP_CTNP_HPP

#pragma once

#include <ctnp/Macros.hpp>

#include <cstdint>
#include <source_location>
#include <string_view>
#include <optional>
#include <string>

namespace ctnp::detail
{
    template <typename T>
    [[nodiscard]]
    constexpr std::string_view raw_type_name() noexcept
    {
        return std::source_location::current().function_name();
    }

    inline constexpr auto typeNameConfig = [] {
        auto const rawName = raw_type_name<int>();
        std::string_view const intName{"int"};
        std::size_t const prefix = rawName.rfind(intName);
        CTNP_ASSERT(prefix != std::string_view::npos, "Did not find `int` in the type-name string.");

        struct type_name_config
        {
            std::size_t prefix{};
            std::size_t suffix{};
        };

        return type_name_config{
            .prefix = prefix,
            .suffix = rawName.size() - intName.size() - prefix,
        };
    }();
}

namespace ctnp
{
    /**
     * \brief Returns the raw type-name.
     * \tparam T The desired type.
     * \return The raw name.
     * \details
     * This function internally relies on `std::source_location::function_name`
     * and tries to extract the desired type-name from that function-name.
     */
    template <typename T>
    [[nodiscard]]
    consteval std::string_view type_name() noexcept
    {
        auto typeName = detail::raw_type_name<T>();
        typeName.remove_prefix(detail::typeNameConfig.prefix);
        typeName.remove_suffix(detail::typeNameConfig.suffix);

        return typeName;
    }

    /**
     * \brief Prettifies a typen-name.
     * \param name The name to be prettified.
     * \return The prettified typen-name if the input was parsed successfully; Otherwise `std::nullopt`.
     *
     * \details This function formats a type or template name for better readability.
     * The primary strategy is to minimize unnecessary details while retaining essential information.
     *
     * For example, when a template-dependent type is provided, the template arguments are omitted:
     * `std::vector<int, std::allocator>::iterator` => `std::vector::iterator`
     */
    [[nodiscard]]
    constexpr std::optional<std::string> prettify_type(std::string_view name);
}

#include "ctnp-detail/ctnp.inl"

#endif

