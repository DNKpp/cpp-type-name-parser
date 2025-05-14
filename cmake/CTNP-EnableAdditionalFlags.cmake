#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

function(create_ctnp_enable_additional_flags_target)

    #[[ This module is used to enable specific compiler and linker flags, which shouldn't be specified for consumed targets.
        This is for example used when applying code-coverage flags, which result in increased compilation times.
        These flags are not needed for compiled dependencies and would just heavily increase compilation times, without any benefit.
    #]]

    set(TARGET_NAME ctnp-internal-enable-additional-flags)
    if (NOT TARGET ${TARGET_NAME})

        add_library(${TARGET_NAME} INTERFACE)
        add_library(ctnp::internal::enable-additional-flags ALIAS ${TARGET_NAME})

        if (CTNP_ADDITIONAL_COMPILER_FLAGS)

            message(DEBUG "${PROJECT_NAME}: Enabled additional compiler-flags: ${CTNP_ADDITIONAL_COMPILER_FLAGS}")

            target_compile_options(${TARGET_NAME}
                INTERFACE
                ${CTNP_ADDITIONAL_COMPILER_FLAGS}
            )

        endif ()

        if (CTNP_ADDITIONAL_LINKER_FLAGS)

            message(DEBUG "$${PROJECT_NAME}: enabled additional linker-flags: ${CTNP_ADDITIONAL_LINKER_FLAGS}")

            target_link_options(${TARGET_NAME}
                INTERFACE
                ${CTNP_ADDITIONAL_LINKER_FLAGS}
            )

        endif ()
    endif ()

endfunction()

create_ctnp_enable_additional_flags_target()
