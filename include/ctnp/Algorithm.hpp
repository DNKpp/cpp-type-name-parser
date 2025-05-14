//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_ALGORITHM_HPP
#define CTNP_ALGORITHM_HPP

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <functional>
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>

#include "ctnp/C++Backports.hpp"
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

    namespace detail
    {
        struct binary_find_fn
        {
            template <
                std::forward_iterator Iterator,
                std::sentinel_for<Iterator> Sentinel,
                typename Projection = std::identity,
                typename T = util::projected_value_t<Iterator, Projection>,
                std::indirect_strict_weak_order<
                    T const*,
                    std::projected<Iterator, Projection>> Comparator = std::ranges::less>
            [[nodiscard]]
            constexpr Iterator operator()(
                Iterator const first,
                Sentinel const last,
                T const& value,
                Comparator compare = {},
                Projection projection = {}) const
            {
                if (auto const iter = std::ranges::lower_bound(first, last, value, compare, projection);
                    iter != last
                    && !std::invoke(compare, value, std::invoke(projection, *iter)))
                {
                    return iter;
                }

                return last;
            }

            template <
                std::ranges::forward_range Range,
                typename Projection = std::identity,
                typename T = util::projected_value_t<std::ranges::iterator_t<Range>, Projection>,
                std::indirect_strict_weak_order<
                    T const*,
                    std::projected<std::ranges::iterator_t<Range>, Projection>> Comparator = std::ranges::less>
            [[nodiscard]]
            constexpr std::ranges::borrowed_iterator_t<Range> operator()(
                Range&& range,
                T const& value,
                Comparator compare = {},
                Projection projection = {}) const
            {
                return std::invoke(
                    *this,
                    std::ranges::begin(range),
                    std::ranges::end(range),
                    value,
                    std::move(compare),
                    std::move(projection));
            }
        };
    }

    /**
     * \brief Finds the specified value within the container and returns an iterator pointing to it.
     * If the value is not found, it returns an iterator to the end of the container.
     * \return A borrowed iterator to the element (or end).
     */
    inline constexpr detail::binary_find_fn binary_find{};

    /**
     * \brief Returns a view containing all elements, which start with the given prefix.
     * \tparam Range The range type, which holds elements comparable with `Prefix`.
     * \tparam Prefix The prefix type.
     * \param range The range.
     * \param prefix The prefix.
     * \return A subrange view to `range`.
     *
     * \attention The behaviour is undefined, when `range` is not sorted.
     */
    template <std::ranges::forward_range Range, std::ranges::forward_range Prefix>
        requires std::totally_ordered_with<std::ranges::range_value_t<Range>, Prefix>
    [[nodiscard]]
    constexpr std::ranges::borrowed_subrange_t<Range> prefix_range(Range&& range, Prefix&& prefix)
    {
        auto const lower = std::ranges::lower_bound(range, prefix);
        auto const end = std::ranges::lower_bound(
            lower,
            std::ranges::end(range),
            prefix,
            [](auto const& element, auto const& p) {
                auto const iter = std::ranges::mismatch(element, p).in2;
                return iter == std::ranges::end(p);
            });

        return {lower, end};
    }
}

#endif
