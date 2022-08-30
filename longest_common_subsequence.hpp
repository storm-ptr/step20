// Andrew Naplavkov

#ifndef STEP20_LONGEST_COMMON_SUBSEQUENCE_HPP
#define STEP20_LONGEST_COMMON_SUBSEQUENCE_HPP

#include "detail/hirschberg.hpp"
#include "detail/utility.hpp"
#include <cstdint>

namespace step20::longest_common_subsequence {
namespace detail {

template <class Equal>
struct table {
    const Equal& eq;

    template <std::random_access_iterator I1, std::random_access_iterator I2>
    auto last_row(I1 first1, I1 last1, I2 first2, I2 last2) const
    {
        size_t size1 = std::ranges::distance(first1, last1);
        size_t size2 = std::ranges::distance(first2, last2);
        auto tbl = ring_table<intmax_t, 2>(size2 + 1);
        for (size_t l = 1; l <= size1; ++l)
            for (size_t r = 1; r <= size2; ++r)
                tbl[l][r] = eq(first1[l - 1], first2[r - 1])
                                ? tbl[l - 1][r - 1] + 1
                                : std::max(tbl[l - 1][r], tbl[l][r - 1]);
        return std::move(tbl[size1]);
    }

    template <bool transposed,
              std::random_access_iterator I1,
              std::random_access_iterator I2,
              std::weakly_incrementable O>
    O trace_col(I1 first1, I1 last1, I2 first2, I2 last2, O result) const
    {
        if constexpr (transposed)
            return trace_col<!transposed>(first2, last2, first1, last1, result);
        else {
            auto it = std::find_first_of(first1, last1, first2, last2, eq);
            if (it != last1)
                *result++ = *it;
            return result;
        }
    }
};

}  // namespace detail

/// Find the longest subsequence present in two sequences.

/// Substring is contiguous, while subsequence need not be.
/// Time complexity O(N*M), space complexity O(min(N,M)), where:
/// N - @param r1 length, M - @param r2 length.
template <std::ranges::random_access_range R1,
          std::ranges::random_access_range R2,
          std::weakly_incrementable O,
          class Equal = std::equal_to<>>
O copy(const R1& r1, const R2& r2, O result, const Equal& eq = {})
{
    return hirschberg::trace(detail::table{eq},
                             std::ranges::begin(r1),
                             std::ranges::end(r1),
                             std::ranges::begin(r2),
                             std::ranges::end(r2),
                             result);
}

}  // namespace step20::longest_common_subsequence

#endif  // STEP20_LONGEST_COMMON_SUBSEQUENCE_HPP
