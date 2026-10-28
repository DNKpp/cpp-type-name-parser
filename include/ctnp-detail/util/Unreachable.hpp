//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_UNREACHABLE_HPP
#define CTNP_DETAIL_UTIL_UNREACHABLE_HPP

#pragma once

#include "ctnp/Macros.hpp"

#include <version>

#ifdef __cpp_lib_unreachable
    #include <utility>

namespace ctnp::detail
{
    using std::unreachable;
}

#else

namespace ctnp::detail::util
{
    /**
     * \brief Invokes undefined behavior
     * \see https://en.cppreference.com/w/cpp/utility/unreachable
     * \note Implementation directly taken from https://en.cppreference.com/w/cpp/utility/unreachable
     */
    [[noreturn]]
    inline void unreachable()
    {
        CTNP_ASSERT(false, "Reached the unreachable.");

    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
    #if _MSC_VER && !defined(__clang__) // MSVC
        __assume(false);
    #else                               // GCC, Clang
        __builtin_unreachable();
    #endif
    }
}
#endif
#endif

