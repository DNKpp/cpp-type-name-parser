//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ctnp/TypeList.hpp"

using namespace ctnp;

TEMPLATE_TEST_CASE_SIG(
    "type_list::size holds the amount of arguments.",
    "[utility]",
    ((std::size_t expected, typename... Args), expected, Args...),
    (0u),
    (1u, int),
    (2u, int, int),
    (2u, float, int),
    (3u, int, int, int))
{
    using type_list_t = util::type_list<Args...>;

    STATIC_REQUIRE(expected == type_list_t::size);
    STATIC_REQUIRE(expected == std::tuple_size_v<type_list_t>);
}

TEMPLATE_TEST_CASE_SIG(
    "type_list_reverse reverses the type_list elements.",
    "[utility]",
    ((auto dummy, typename Expected, typename Input), dummy, Expected, Input),
    (std::ignore, util::type_list<>, util::type_list<>),
    (std::ignore, util::type_list<int>, util::type_list<int>),
    (std::ignore, util::type_list<int, float>, util::type_list<float, int>),
    (std::ignore, util::type_list<double, int, float>, util::type_list<float, int, double>))
{
    STATIC_REQUIRE(std::same_as<Expected, typename util::type_list_reverse<Input>::type>);
    STATIC_REQUIRE(std::same_as<Expected, util::type_list_reverse_t<Input>>);
}
