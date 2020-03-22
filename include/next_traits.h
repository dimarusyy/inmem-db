#pragma once

#include <type_traits>

namespace traits
{
    constexpr size_t kMaxStates = 500;

    template <size_t N> struct Flag
    {
        friend constexpr int AdlFlag(Flag<N>);
    };

    template <typename T>
    struct Writer
    {
        friend constexpr int AdlFlag(T)
        {
            return 0;
        }
    };

    template <typename T, int = AdlFlag(T{}) >
    constexpr size_t IsAdlFlagDefined(int)
    {
        return 1;
    }

    template <typename T> constexpr size_t IsAdlFlagDefined(...)
    {
        return 0;
    }

    template<size_t Z, size_t... Sizes,
        size_t R = ((IsAdlFlagDefined<Flag<Z + Sizes>>(0)) + ...)>
        constexpr size_t CountDefinedAdlFlags(
            std::integer_sequence<size_t, Sizes...>)
    {
        return R;
    }

    template <size_t MaxN = kMaxStates - 1, size_t Z = 0,
        typename T = std::make_integer_sequence<size_t, MaxN>,
        size_t CounterValue = CountDefinedAdlFlags<Z>(T{}),
        size_t = sizeof(Writer<Flag<CounterValue>>) >
        using Counter = std::integral_constant<unsigned int, CounterValue>;
}