//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_RANGES_CONTAINS_HPP
#define CTNP_DETAIL_UTIL_RANGES_CONTAINS_HPP

#pragma once

#include "ctnp-detail/util/ProjectedValue.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

namespace ctnp::detail::util::ranges
{
    struct contains_fn
    {
        template <
            std::input_iterator Iterator,
            std::sentinel_for<Iterator> Sentinel,
            typename Projection = std::identity,
            typename T = projected_value_t<Iterator, Projection>>
            requires std::indirect_binary_predicate<
                std::ranges::equal_to,
                std::projected<Iterator, Projection>,
                T const*>
        [[nodiscard]]
        constexpr bool operator()(Iterator first, Sentinel last, T const& value, Projection projection = {}) const
        {
            auto const iter = std::ranges::find(std::move(first), last, value, std::move(projection));
            return iter != last;
        }

        template <
            std::ranges::input_range Range,
            typename Projection = std::identity,
            typename T = projected_value_t<std::ranges::iterator_t<Range>, Projection>>
            requires std::indirect_binary_predicate<
                std::ranges::equal_to,
                std::projected<std::ranges::iterator_t<Range>, Projection>,
                T const*>
        [[nodiscard]]
        constexpr bool operator()(Range&& r, T const& value, Projection projection = {}) const
        {
            return std::invoke(
                *this,
                std::ranges::begin(r),
                std::ranges::end(r),
                value,
                std::move(projection));
        }
    };

    /**
     * \brief Determines, whether the specified value is contained in the given range.
     * \return Returns `true`, if the value is contained.
     * \ingroup DETAIL_UTIL
     *
     * \note This is a backport from c++23 `std::ranges::contains`.
     * \see https://en.cppreference.com/w/cpp/algorithm/ranges/contains
     */
    inline constexpr contains_fn contains{};
}

#endif

