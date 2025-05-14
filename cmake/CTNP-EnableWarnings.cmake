#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

function(create_ctnp_enable_warnings_target)

	set(TARGET_NAME "ctnp-internal-enable-warnings")
	if (NOT TARGET ${TARGET_NAME})

		add_library(${TARGET_NAME} INTERFACE)
		add_library(ctnp::internal::enable-warnings ALIAS ${TARGET_NAME})

		# We need to circumvent the huge nonsense warnings from clang-cl
		# see: https://discourse.cmake.org/t/wall-with-visual-studio-clang-toolchain-results-in-way-too-many-warnings/7927
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
			AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")

			set(WARNING_FLAGS /W4 -Wextra -Wpedantic -Werror -Wno-unknown-attributes)
		else()
			string(CONCAT WARNING_FLAGS
				"$<IF:"
					"$<CXX_COMPILER_ID:MSVC>,"
					"/W4;/WX,"
					"-Wall;-Wextra;-Wpedantic;-Werror"
				">"
			)
		endif()

		target_compile_options(${TARGET_NAME}
			INTERFACE
			${WARNING_FLAGS}
		)

	endif()

endfunction()

create_ctnp_enable_warnings_target()
