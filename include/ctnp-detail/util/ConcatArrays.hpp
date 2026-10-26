//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_CONCAT_ARRAYS_HPP
#define CTNP_DETAIL_UTIL_CONCAT_ARRAYS_HPP

#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <utility>

namespace ctnp::detail::util
{
    /**
     * \brief Concatenates the given arrays by copying all elements into a new array.
     * \ingroup DETAIL_UTIL
     * \tparam T The element type.
     * \tparam firstN The size of the first array.
     * \tparam secondN The size of the second array.
     * \param first The first array.
     * \param second The second array.
     * \return A newly constructed array with moved elements.
     */
    template <std::movable T, std::size_t firstN, std::size_t secondN>
    [[nodiscard]]
    constexpr std::array<T, firstN + secondN> concat_arrays(std::array<T, firstN> first, std::array<T, secondN> second)
        noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        return std::invoke(
            [&]<std::size_t... firstIs, std::size_t... secondIs>(
                std::index_sequence<firstIs...> const /*seq1*/,
                std::index_sequence<secondIs...> const /*seq2*/) {
                return std::array<T, firstN + secondN>{
                    std::move(std::get<firstIs>(first))...,
                    std::move(std::get<secondIs>(second))...};
            },
            std::make_index_sequence<firstN>{},
            std::make_index_sequence<secondN>{});
    }

    /**
     * \copybrief concat_arrays
     * \ingroup DETAIL_UTIL
     * \tparam T The element type.
     * \tparam firstN The size of the first array.
     * \tparam Others Other array types which share the same element-type.
     * \param first The first array.
     * \param others The second array.
     * \return A newly constructed array with moved elements.
     */
    template <std::movable T, std::size_t firstN, typename... Others>
        requires(... && std::same_as<T, std::ranges::range_value_t<Others>>)
             // Not how I would like to formulate that constraint, but msvc does not accept it otherwise.
             && (... && (0u <= std::tuple_size_v<Others>))
    [[nodiscard]]
    constexpr auto concat_arrays(std::array<T, firstN> first, Others... others)
        noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if constexpr (0u == sizeof...(Others))
        {
            return first;
        }
        else
        {
            return concat_arrays(
                std::move(first),
                concat_arrays(std::move(others)...));
        }
    }
}

#endif

