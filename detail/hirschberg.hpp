// Andrew Naplavkov

#ifndef STEP20_HIRSCHBERG_HPP
#define STEP20_HIRSCHBERG_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

namespace step20::hirschberg {

template <class Table,
          std::random_access_iterator I1,
          std::random_access_iterator I2>
auto midpoint(const Table& tbl, I1 first1, I1 last1, I2 first2, I2 last2)
    -> std::pair<I1, I2>
{
    auto mid1 = first1 + (last1 - first1) / 2;
    auto head = tbl.last_row(first1, mid1, first2, last2);
    auto tail = tbl.last_row(std::reverse_iterator{last1},
                             std::reverse_iterator{mid1},
                             std::reverse_iterator{last2},
                             std::reverse_iterator{first2});
    std::ranges::transform(head,
                           tail | std::views::reverse,
                           std::ranges::begin(head),
                           std::plus{});
    auto mid2 =
        first2 + (std::ranges::max_element(head) - std::ranges::begin(head));
    return {mid1, mid2};
}

template <bool transposed = false,
          class Table,
          std::random_access_iterator I1,
          std::random_access_iterator I2,
          std::weakly_incrementable O>
O trace(const Table& tbl, I1 first1, I1 last1, I2 first2, I2 last2, O result)
{
    auto size1 = last1 - first1;
    auto size2 = last2 - first2;
    if (size1 < size2)
        return trace<!transposed>(tbl, first2, last2, first1, last1, result);
    if (size2 < 2)
        return tbl.template trace_col<transposed>(
            first1, last1, first2, last2, result);
    auto [mid1, mid2] = midpoint(tbl, first1, last1, first2, last2);
    result = trace<transposed>(tbl, first1, mid1, first2, mid2, result);
    result = trace<transposed>(tbl, mid1, last1, mid2, last2, result);
    return result;
}

}  // namespace step20::hirschberg

#endif  // STEP20_HIRSCHBERG_HPP
