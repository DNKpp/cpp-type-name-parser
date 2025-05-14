#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

CPMAddPackage(mimicpp
	VERSION 7
	GITHUB_REPOSITORY DNKpp/mimicpp
	SYSTEM YES
	EXCLUDE_FROM_ALL YES
	OPTIONS
	"MIMICPP_CONFIG_USE_FMT ON"
	"MIMICPP_CONFIG_EXPERIMENTAL_STACKTRACE ON"
	"MIMICPP_CONFIG_EXPERIMENTAL_USE_CPPTRACE ON"
)
