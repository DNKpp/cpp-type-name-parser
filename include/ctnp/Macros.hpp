//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_MACROS_COMMON_HPP
#define CTNP_MACROS_COMMON_HPP

#pragma once

namespace ctnp
{
    /**
     * \defgroup MACRO_DETAIL detail macros
     * \brief Contains several internally used macros.
     * \attention Users should never use these macros directly.
     */
}

namespace ctnp
{
    /**
     * \defgroup MACRO_DETAIL_STRINGIFY stringify
     * \ingroup MACRO_DETAIL
     * \brief Stringifies the provided arguments.
     */
}

/**
 * \brief Black-magic.
 * \ingroup MACRO_DETAIL_STRINGIFY
 */
#define CTNP_DETAIL_STRINGIFY_IMPL(...) #__VA_ARGS__

/**
 * \brief Stringifies the provided arguments
 * \ingroup MACRO_DETAIL_STRINGIFY
 */
#define CTNP_STRINGIFY(...) CTNP_DETAIL_STRINGIFY_IMPL(__VA_ARGS__)

#endif
