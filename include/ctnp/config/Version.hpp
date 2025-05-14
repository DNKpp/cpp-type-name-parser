//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_CONFIG_VERSION_HPP
#define CTNP_CONFIG_VERSION_HPP

#pragma once

#include "ctnp/config/Macro.hpp"

#define CTNP_VERSION_MAJOR 0
#define CTNP_VERSION_MINOR 1
#define CTNP_VERSION_PATCH 0

#define CTNP_VERSION \
	CTNP_DETAIL_XSTR(CTNP_VERSION_MAJOR) \
	"." CTNP_DETAIL_XSTR(CTNP_VERSION_MINOR) \
	"." CTNP_DETAIL_XSTR(CTNP_VERSION_PATCH)

#endif
