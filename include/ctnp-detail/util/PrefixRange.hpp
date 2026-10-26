//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_PREFIX_RANGE_HPP
#define CTNP_DETAIL_UTIL_PREFIX_RANGE_HPP

#pragma once

#include <algorithm>
#include <concepts>
#include <ranges>

namespace ctnp::detail::util
{
    /**
     * \brief Returns a view containing all elements, which start with the given prefix.
     * \ingroup DETAIL_UTIL
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

