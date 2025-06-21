#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

set(MIMICPP_CONFIG_USE_FMT ON)
set(MIMICPP_CONFIG_EXPERIMENTAL_STACKTRACE ON)
set(MIMICPP_CONFIG_EXPERIMENTAL_USE_CPPTRACE ON)
CPMAddPackage(mimicpp
	#VERSION 7
    GIT_TAG 779085478419edfc23c5e254db1f51586c771b9e # commit from 20.06.2025
	GITHUB_REPOSITORY DNKpp/mimicpp
	SYSTEM YES
	EXCLUDE_FROM_ALL YES
)
