#pragma once

#include <mimic++/printing/StatePrinter.hpp>

#include <string>

// see: https://github.com/catchorg/Catch2/blob/devel/docs/configuration.md#fallback-stringifier
[[nodiscard]]
constexpr std::string catch2_fallback_stringifier(auto&& object)
{
    return mimicpp::print(object);
}

#define CATCH_CONFIG_FALLBACK_STRINGIFIER catch2_fallback_stringifier
