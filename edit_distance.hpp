// Andrew Naplavkov

#ifndef STEP20_EDIT_DISTANCE_HPP
#define STEP20_EDIT_DISTANCE_HPP

#include "detail/hirschberg.hpp"
#include "detail/utility.hpp"
#include <cstdint>
#include <optional>

namespace step20::edit_distance {
namespace detail {

template <class Equal>
struct table {
    const Equal& eq;

    template <std::random_access_iterator I1, std::random_access_iterator I2>
    auto last_row(I1 first1, I1 last1, I2 first2, I2 last2) const
    {
        auto size1 = last1 - first1;
        auto size2 = last2 - first2;
        auto tbl = ring_table<intmax_t, 2>(size2 + 1);
        for (decltype(size1) l = 0; l <= size1; ++l)
            for (decltype(size2) r = 0; r <= size2; ++r) {
                if (l == 0)
                    tbl[l][r] = -r;
                else if (r == 0)
                    tbl[l][r] = -l;
                else if (eq(first1[l - 1], first2[r - 1]))
                    tbl[l][r] = 1 + tbl[l - 1][r - 1];
                else
                    tbl[l][r] = -1 + std::max({tbl[l - 1][r - 1],
                                               tbl[l - 1][r],
                                               tbl[l][r - 1]});
            }
        return std::move(tbl[size1]);
    }

    template <bool transposed,
              std::random_access_iterator I1,
              std::random_access_iterator I2,
              std::weakly_incrementable O>
    O trace_col(I1 first1, I1 last1, I2 first2, I2 last2, O result) const
    {
        static auto make_pair = [](auto&& lhs, auto&& rhs) {
            if constexpr (transposed)
                return std::pair{rhs, lhs};
            else
                return std::pair{lhs, rhs};
        };
        while (first1 != last1) {
            if (first2 == last2 ||
                (first1 + 1 != last1 && !eq(*first1, *first2)))
                *result++ = make_pair(*first1++, std::nullopt);
            else
                *result++ = make_pair(*first1++, *first2++);
        }
        return result;
    }
};

}  // namespace detail

/// Find the optimal sequence alignment between two strings.

/// Optimality is measured with the Levenshtein distance,
/// defined to be the sum of the costs of
/// insertions, replacements, deletions, and null actions needed
/// to change one string into the other.
/// Time complexity O(N*M), space complexity O(min(N,M)), where:
/// N = std::ranges::distance(r1), M = std::ranges::distance(r2).
template <std::ranges::random_access_range R1,
          std::ranges::random_access_range R2,
          std::weakly_incrementable O,
          class Equal = std::equal_to<>>
O zip(const R1& r1, const R2& r2, O result, const Equal& eq = {})
{
    return hirschberg::trace(detail::table{eq},
                             std::ranges::begin(r1),
                             std::ranges::end(r1),
                             std::ranges::begin(r2),
                             std::ranges::end(r2),
                             result);
}

}  // namespace step20::edit_distance

#endif  // STEP20_EDIT_DISTANCE_HPP
