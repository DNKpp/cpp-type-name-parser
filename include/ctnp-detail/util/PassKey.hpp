//          Copyright Dominic (DNKpp) Koepke 2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CTNP_DETAIL_UTIL_PASS_KEY_HPP
#define CTNP_DETAIL_UTIL_PASS_KEY_HPP

#pragma once

namespace ctnp::detail::util
{
    /**
     * \brief A helper type that enables fine-grained access control to specific functions without using friend declarations.
     * \ingroup DETAIL_UTIL
     * \tparam Trusted The type that is allowed to create this key and therefore granted access.
     *
     * \details
     * This type provides a way to restrict access to certain functions by requiring a special key object.
     * Only the designated `Trusted` type can construct this key, allowing it to call functions that accept the key as a parameter.
     * This avoids broad friendship declarations and keeps access control explicit and localized.
     *
     * \see https://speakerdeck.com/rollbear/using-types-to-save-your-codes-future?slide=54
     */
    template <typename Trusted>
    struct pass_key
    {
    private:
        friend Trusted;
        pass_key() = default;
    };
}

#endif
