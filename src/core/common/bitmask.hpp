#pragma once

#include <type_traits>
#include <utility>

template<typename T>
requires std::is_enum_v<T>
constexpr static bool CORE_ENABLE_BIT_OPERATORS = false;

template<typename T>
requires CORE_ENABLE_BIT_OPERATORS<T>
constexpr auto operator&(T left, T right) noexcept
{
    return std::to_underlying(left) & std::to_underlying(right);
}

template<typename T>
requires CORE_ENABLE_BIT_OPERATORS<T>
constexpr auto operator|(T left, T right) noexcept
{
    return std::to_underlying(left) | std::to_underlying(right);
}

template<typename T>
requires CORE_ENABLE_BIT_OPERATORS<T>
constexpr auto operator^(T left, T right) noexcept
{
    return std::to_underlying(left) ^ std::to_underlying(right);
}

template<typename T>
requires CORE_ENABLE_BIT_OPERATORS<T>
constexpr auto operator~(T value) noexcept
{
    return ~std::to_underlying(value);
}
