#          Copyright Dominic (DNKpp) Koepke 2026
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

CPMAddPackage("gh:DNKpp/constest#3a657ab1987c4644a62305ac8d37b2672154fbd6") # commit from 02.11.2026
add_library(constest::constest ALIAS constest)
