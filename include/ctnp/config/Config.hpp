//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_CONFIG_CONFIG_HPP
#define CTNP_CONFIG_CONFIG_HPP

#pragma once

#include <version>

#ifndef CTNP_ASSERT
	#include <cassert>
	#define CTNP_ASSERT(condition, msg, ...) assert((condition) && msg)
#endif

// clang-format off
// Prevent number from getting decorated with '.
#if 201907L <= __cpp_lib_constexpr_vector
    // clang-format on
    #define CTNP_DETAIL_CONSTEXPR_VECTOR constexpr
#else
    #define CTNP_DETAIL_CONSTEXPR_VECTOR inline
#endif

#endif
