//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_OVERLOADED_HPP
#define CTNP_DETAIL_UTIL_OVERLOADED_HPP

#pragma once

namespace ctnp::detail::util
{
    /**
     * \brief Famous overloaded helper type used for convenient variant visiting.
     * \ingroup DETAIL_UTIL
     * \see https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
     */
    template <typename... Ts>
    struct overloaded
        : public Ts...
    {
        using Ts::operator()...;
    };

    template <typename... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}

#endif
