//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_RANGES_BINARY_FIND_HPP
#define CTNP_DETAIL_UTIL_RANGES_BINARY_FIND_HPP

#pragma once

#include "ctnp-detail/util/ProjectedValue.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

namespace ctnp::detail::util::ranges
{
    struct binary_find_fn
    {
        template <
            std::forward_iterator Iterator,
            std::sentinel_for<Iterator> Sentinel,
            typename Projection = std::identity,
            typename T = projected_value_t<Iterator, Projection>,
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
            typename T = projected_value_t<std::ranges::iterator_t<Range>, Projection>,
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

    /**
     * \brief Finds the specified value within the container and returns an iterator pointing to it.
     * If the value is not found, it returns an iterator to the end of the container.
     * \ingroup DETAIL_UTIL
     * \return A borrowed iterator to the element (or end).
     */
    inline constexpr binary_find_fn binary_find{};
}

#endif

