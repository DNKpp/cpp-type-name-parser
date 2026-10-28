#          Copyright Dominic (DNKpp) Koepke 2026
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

CPMAddPackage("gh:DNKpp/constest#76da57ec24551d28ae2f3daeca13e543a38ae1a3")
add_library(constest::constest ALIAS constest)
