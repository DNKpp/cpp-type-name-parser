//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_VERSION_HPP
#define CTNP_VERSION_HPP

#pragma once

#include "ctnp/Macros.hpp"

#define CTNP_VERSION_MAJOR 0
#define CTNP_VERSION_MINOR 1
#define CTNP_VERSION_PATCH 0

namespace ctnp
{
    inline constexpr int version_major{CTNP_VERSION_MAJOR};
    inline constexpr int version_minor{CTNP_VERSION_MINOR};
    inline constexpr int version_patch{CTNP_VERSION_PATCH};
    inline constexpr char const* version{CTNP_STRINGIFY(CTNP_VERSION_MAJOR) "." CTNP_STRINGIFY(CTNP_VERSION_MINOR) "." CTNP_STRINGIFY(CTNP_VERSION_PATCH)};
}

#endif
