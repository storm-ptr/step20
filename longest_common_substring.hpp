// Andrew Naplavkov

#ifndef STEP20_LONGEST_COMMON_SUBSTRING_HPP
#define STEP20_LONGEST_COMMON_SUBSTRING_HPP

#include "suffix_array.hpp"
#include <limits>

namespace step20::longest_common_substring {
namespace detail {

template <class Char,
          std::unsigned_integral Size,
          class Compare,
          std::weakly_incrementable O>
O copy(std::basic_string<Char>&& str, Size mid, const Compare& comp, O result)
{
    auto arr = enhanced_suffix_array<Char, Size, Compare>{std::move(str), comp};
    auto first = arr.data();
    auto last = first + arr.size();
    auto substr = std::basic_string_view{last, last};
    auto lcp = arr.lcp_array();
    for (Size i = 1; i < arr.size(); ++i) {
        auto prev = arr[i - 1];
        auto cur = arr[i];
        if ((prev < mid) == (cur < mid))
            continue;
        auto pos = std::min<Size>(prev, cur);
        auto len = std::min<Size>(lcp[i - 1], mid - pos);
        if (len > substr.size())
            substr = std::basic_string_view{first + pos, first + pos + len};
    }
    return std::ranges::copy(substr, result).out;
}

}  // namespace detail

/// Find the longest substring of two strings.

/// Substring is contiguous, while subsequence need not be.
/// Time complexity O((N+M)*log(N+M)*log(N+M)), space complexity O(N+M), where:
/// N = std::ranges::distance(r1), M = std::ranges::distance(r2).
/// @see enhanced_suffix_array is used under the hood.
template <std::ranges::input_range R1,
          std::ranges::input_range R2,
          std::weakly_incrementable O,
          class Compare = std::less<>>
O copy(const R1& r1, const R2& r2, O result, const Compare& comp = {})
{
    using char_t = std::common_type_t<std::ranges::range_value_t<R1>,
                                      std::ranges::range_value_t<R2>>;
    auto str = std::basic_string<char_t>{};
    str.append(std::ranges::begin(r1), std::ranges::end(r1));
    auto mid = str.size();
    str.append(std::ranges::begin(r2), std::ranges::end(r2));
    if (str.size() < std::numeric_limits<uint8_t>::max())
        return detail::copy(std::move(str), (uint8_t)mid, comp, result);
    if (str.size() < std::numeric_limits<uint16_t>::max())
        return detail::copy(std::move(str), (uint16_t)mid, comp, result);
    if (str.size() < std::numeric_limits<uint32_t>::max())
        return detail::copy(std::move(str), (uint32_t)mid, comp, result);
    return detail::copy(std::move(str), mid, comp, result);
}

}  // namespace step20::longest_common_substring

#endif  // STEP20_LONGEST_COMMON_SUBSTRING_HPP
