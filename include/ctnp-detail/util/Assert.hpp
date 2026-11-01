//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_ASSERT_HPP
#define CTNP_DETAIL_ASSERT_HPP

#pragma once

#ifndef CTNP_ASSERT
    #include <cassert>
    /**
     * \brief A custom assertion macro.
     */
    #define CTNP_ASSERT(condition, message, ...) assert((condition) && message)
#endif

#endif
