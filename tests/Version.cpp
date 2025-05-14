//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/config/Version.hpp"

TEST_CASE(
	"Version macros are set accordingly.",
	"[config]")
{
	constexpr int major{CTNP_VERSION_MAJOR};
	constexpr int minor{CTNP_VERSION_MINOR};
	constexpr int patch{CTNP_VERSION_PATCH};
	constexpr char const* full{CTNP_VERSION};

	STATIC_CHECK(0 <= major);
	STATIC_CHECK(0 <= minor);
	STATIC_CHECK(0 <= patch);
	CHECK_THAT(
		full,
		Catch::Matchers::Equals(mimicpp::format::format("{}.{}.{}", major, minor, patch)));
}
