#          Copyright Dominic (DNKpp) Koepke 2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

message(STATUS "${MESSAGE_PREFIX} version: v${PROJECT_VERSION} from: ${ctnp_SOURCE_DIR}")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "${PROJECT_NAME}: targeting 64-bit architecture")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "${PROJECT_NAME}: targeting 32-bit architecture")
else ()
    message(WARNING "${PROJECT_NAME}: unable to determine architecture")
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${ctnp_SOURCE_DIR}" CTNP_IS_TOP_LEVEL_PROJECT)
