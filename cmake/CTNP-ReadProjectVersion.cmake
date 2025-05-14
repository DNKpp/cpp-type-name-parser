#          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

function(read_project_version FILE_PATH OUT_VERSION)

    file(READ ${FILE_PATH} FILE_CONTENT)
    if (NOT FILE_CONTENT)
        message(FATAL_ERROR "${PROJECT_NAME}: read_project_version failed - Unable to read file from: ${FILE_PATH}")
    endif ()

    set(MAJOR_VERSION_REGEX "#define[ \t]+CTNP_VERSION_MAJOR[ \t]+([0-9]+)")
    set(MINOR_VERSION_REGEX "#define[ \t]+CTNP_VERSION_MINOR[ \t]+([0-9]+)")
    set(PATCH_VERSION_REGEX "#define[ \t]+CTNP_VERSION_PATCH[ \t]+([0-9]+)")
    string(REGEX MATCH ${MAJOR_VERSION_REGEX} _ ${FILE_CONTENT})
    set(MAJOR_TOKEN ${CMAKE_MATCH_1})

    string(REGEX MATCH ${MINOR_VERSION_REGEX} _ ${FILE_CONTENT})
    set(MINOR_TOKEN ${CMAKE_MATCH_1})

    string(REGEX MATCH ${PATCH_VERSION_REGEX} _ ${FILE_CONTENT})
    set(PATCH_TOKEN ${CMAKE_MATCH_1})

    if (NOT MAJOR_TOKEN VERSION_GREATER_EQUAL 0
        AND NOT MINOR_TOKEN VERSION_GREATER_EQUAL 0
        AND NOT PATCH_TOKEN VERSION_GREATER_EQUAL 0)
        message(FATAL_ERROR "${PROJECT_NAME}: read_project_version failed - Unable to read version.")
    endif ()

    set(VERSION "${MAJOR_TOKEN}.${MINOR_TOKEN}.${PATCH_TOKEN}")
    message(DEBUG "${PROJECT_NAME}: read_project_version succeeded - Version is ${VERSION}.")
    set(${OUT_VERSION} ${VERSION} PARENT_SCOPE)

endfunction()
