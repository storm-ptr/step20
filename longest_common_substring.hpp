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
/// @param less - to compare characters
template <std::ranges::input_range R1,
          std::ranges::input_range R2,
          std::weakly_incrementable O,
          class Less = std::less<>>
O copy(const R1& r1, const R2& r2, O result, const Less& less = {})
{
    using Char = std::common_type_t<std::ranges::range_value_t<R1>,
                                    std::ranges::range_value_t<R2>>;
    using Size = size_t;
    auto str = std::basic_string<Char>{};
    str.append(std::ranges::begin(r1), std::ranges::end(r1));
    Size mid = str.size();
    str.append(std::ranges::begin(r2), std::ranges::end(r2));
    auto array = enhanced_suffix_array<Char, Size, Less>(std::move(str), less);
    auto first = array.data();
    auto last = array.data() + array.size();
    auto substr = std::basic_string_view(last, last);
    for (Size i = 1; i < array.size(); ++i) {
        auto prev = array.sorted_suffixes()[i - 1];
        auto cur = array.sorted_suffixes()[i];
        if ((prev < mid) == (cur < mid))
            continue;
        auto pos = std::min(prev, cur);
        auto len =
            std::min(array.longest_common_prefixes()[i - 1], Size(mid - pos));
        if (len > Size(substr.size()))
            substr = std::basic_string_view(first + pos, first + pos + len);
    }
    return std::ranges::copy(substr, result).out;
}

}  // namespace step20::longest_common_substring

#endif  // STEP20_LONGEST_COMMON_SUBSTRING_HPP
