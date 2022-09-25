// Andrew Naplavkov

#ifndef STEP20_LONGEST_COMMON_SUBSTRING_HPP
#define STEP20_LONGEST_COMMON_SUBSTRING_HPP

#include "suffix_array.hpp"

namespace step20::longest_common_substring {

/// Find the longest substring of two strings.

/// Substring is contiguous, while subsequence need not be.
/// @see enhanced_suffix_array is used under the hood.
/// Time complexity O((N+M)*log(N+M)*log(N+M)), space complexity O(N+M), where:
/// N - @param r1 length
/// M - @param r2 length
/// @param result - the beginning of the destination range
/// @param comp - to determine the order of characters
template <std::ranges::input_range R1,
          std::ranges::input_range R2,
          std::weakly_incrementable O,
          class Compare = std::less<>>
O copy(const R1& r1, const R2& r2, O result, const Compare& comp = {})
{
    using value_t = std::common_type_t<std::ranges::range_value_t<R1>,
                                       std::ranges::range_value_t<R2>>;
    auto str = std::basic_string<value_t>{};
    str.append(std::ranges::begin(r1), std::ranges::end(r1));
    size_t mid = str.size();
    str.append(std::ranges::begin(r2), std::ranges::end(r2));
    auto array = enhanced_suffix_array{std::move(str), comp};
    auto first = array.data();
    auto last = array.data() + array.size();
    auto substr = std::basic_string_view{last, last};
    for (size_t i = 1; i < array.size(); ++i) {
        auto prev = array.sorted_suffixes()[i - 1];
        auto cur = array.sorted_suffixes()[i];
        if ((prev < mid) == (cur < mid))
            continue;
        auto pos = std::min(prev, cur);
        auto len = std::min(array.longest_common_prefixes()[i - 1], mid - pos);
        if (len > substr.size())
            substr = std::basic_string_view{first + pos, first + pos + len};
    }
    return std::ranges::copy(substr, result).out;
}

}  // namespace step20::longest_common_substring

#endif  // STEP20_LONGEST_COMMON_SUBSTRING_HPP
