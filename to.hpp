// Andrew Naplavkov

#ifndef STEP20_TO_HPP
#define STEP20_TO_HPP

#include "detail/utility.hpp"
#include <algorithm>

namespace step20 {

/// @see https://en.cppreference.com/w/cpp/ranges/to
template <class To, std::ranges::input_range From>
To to(From&& from)
{
    auto result = To{};
    if constexpr (requires { result.reserve(std::ranges::size(from)); })
        result.reserve(std::ranges::size(from));
    std::ranges::copy(from, insert_iterator(result));
    return result;
}

template <template <class...> class To, std::ranges::input_range From>
auto to(From&& from)
{
    return to<To<std::ranges::range_value_t<From>>>(from);
}

}  // namespace step20

#endif  // STEP20_TO_HPP
