#          Copyright Dominic (DNKpp) Koepke 2026
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

CPMAddPackage("gh:Teskann/constest@1.0.0")
add_library(constest::constest ALIAS constest)
