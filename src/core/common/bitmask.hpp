#pragma once

#include <type_traits>
#include <utility>

namespace core
{
template<typename T>
requires std::is_enum_v<T>
constexpr inline bool ENABLE_BIT_OPERATORS = false;
}

template<typename T>
requires core::ENABLE_BIT_OPERATORS<T>
constexpr auto operator&(T left, T right) noexcept
{
    return std::to_underlying(left) & std::to_underlying(right);
}

template<typename T>
requires core::ENABLE_BIT_OPERATORS<T>
constexpr auto operator|(T left, T right) noexcept
{
    return std::to_underlying(left) | std::to_underlying(right);
}

template<typename T>
requires core::ENABLE_BIT_OPERATORS<T>
constexpr auto operator^(T left, T right) noexcept
{
    return std::to_underlying(left) ^ std::to_underlying(right);
}

template<typename T>
requires core::ENABLE_BIT_OPERATORS<T>
constexpr auto operator~(T value) noexcept
{
    return ~std::to_underlying(value);
}
