//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_TYPE_LIST_HPP
#define CTNP_TYPE_LIST_HPP

#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

namespace ctnp::util
{
    /**
     * \brief A very basic type-list template.
     * \tparam Args The types.
     */
    template <typename... Args>
    struct type_list
    {
        static constexpr std::size_t size = sizeof...(Args);
    };

    namespace detail
    {
        template <typename ProcessedList, typename PendingList>
        struct type_list_reverse;

        template <typename ProcessedList>
        struct type_list_reverse<ProcessedList, type_list<>>
        {
            using type = ProcessedList;
        };

        template <typename... ProcessedArgs, typename First, typename... Args>
        struct type_list_reverse<type_list<ProcessedArgs...>, type_list<First, Args...>>
            : public type_list_reverse<type_list<First, ProcessedArgs...>, type_list<Args...>>
        {
        };
    }

    template <typename TypeList>
    struct type_list_reverse
    {
        using type = typename detail::type_list_reverse<type_list<>, TypeList>::type;
    };

    template <typename TypeList>
    using type_list_reverse_t = typename type_list_reverse<TypeList>::type;
}

template <typename... Args>
struct std::tuple_size<ctnp::util::type_list<Args...>> // NOLINT(*-dcl58-cpp)
    : std::integral_constant<std::size_t, ctnp::util::type_list<Args...>::size>
{
};

#endif
