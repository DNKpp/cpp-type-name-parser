//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_ALGORITHM_HPP
#define CTNP_ALGORITHM_HPP

#pragma once

#include <concepts>
#include <array>
#include <tuple>
#include <utility>
#include <ranges>

#include "ctnp/config/Config.hpp"

namespace ctnp::util
{
  /**
     * \brief Concatenates the given arrays by copying all elements into a new array.
     * \tparam T The element type.
     * \tparam firstN The size of the first array.
     * \tparam secondN The size of the second array.
     * \param first The first array.
     * \param second The second array.
     * \return A newly constructed arrays with copied elements.
     */
    template <std::copyable T, std::size_t firstN, std::size_t secondN>
    [[nodiscard]]
    constexpr std::array<T, firstN + secondN> concat_arrays(std::array<T, firstN> const& first, std::array<T, secondN> const& second)
    {
        return std::invoke(
            [&]<std::size_t... firstIs, std::size_t... secondIs>(
                [[maybe_unused]] std::index_sequence<firstIs...> const,
                [[maybe_unused]] std::index_sequence<secondIs...> const) {
                return std::array<T, firstN + secondN>{
                    std::get<firstIs>(first)...,
                    std::get<secondIs>(second)...};
            },
            std::make_index_sequence<firstN>{},
            std::make_index_sequence<secondN>{});
    }

    /**
     * \copybrief concat_arrays
     * \tparam T The element type.
     * \tparam firstN The size of the first array.
     * \tparam Others Other array types which share the same element-type.
     * \param first The first array.
     * \param others The second array.
     * \return A newly constructed arrays with copied elements.
     */
    template <std::copyable T, std::size_t firstN, typename... Others>
        requires(... && std::same_as<T, std::ranges::range_value_t<Others>>)
             // Not how I would like to formulate that constraint, but msvc does not accept it otherwise.
             && (... && (0u <= std::tuple_size_v<Others>))
    [[nodiscard]]
    constexpr auto concat_arrays(std::array<T, firstN> const& first, Others const&... others)
    {
        if constexpr (0u == sizeof...(Others))
        {
            return first;
        }
        else
        {
            return concat_arrays(
                first,
                concat_arrays(others...));
        }
    }
}

#endif
