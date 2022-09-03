// Andrew Naplavkov

#ifndef STEP20_LONGEST_REPEATED_SUBSTRING_HPP
#define STEP20_LONGEST_REPEATED_SUBSTRING_HPP

#include "suffix_array.hpp"
#include "suffix_tree.hpp"

namespace step20 {

/// Find the longest substring that occurs at least twice.

/// Substring is contiguous, while subsequence need not be.
/// Time complexity O(N), where: N - @param array text length.
template <class... Ts>
auto longest_repeated_substring(const enhanced_suffix_array<Ts...>& array)
    -> std::basic_string_view<typename enhanced_suffix_array<Ts...>::value_type>
{
    using Char = enhanced_suffix_array<Ts...>::value_type;
    auto first = array.data();
    auto last = array.data() + array.size();
    auto result = std::basic_string_view<Char>{last, last};
    auto lcp = std::span{array.longest_common_prefixes(), array.size()};
    auto max = std::ranges::max_element(lcp);
    if (max != lcp.end() && *max > 0) {
        auto pos = array.sorted_suffixes()[std::distance(lcp.begin(), max)];
        result = std::basic_string_view<Char>{first + pos, first + pos + *max};
    }
    return result;
}

/// Find the longest substring that occurs at least twice.

/// Suffix tree must be explicit - padded with a terminal symbol.
/// Time complexity O(N), space complexity O(H), where:
/// N - text length, H - height of @param tree.
/// H is asymptotically close to O(log(N)), O(N) at worst.
template <class... Ts>
auto longest_repeated_substring(const suffix_tree<Ts...>& tree)
    -> std::basic_string_view<typename suffix_tree<Ts...>::value_type>
{
    using Char = suffix_tree<Ts...>::value_type;
    auto first = tree.data();
    auto last = tree.data() + tree.size();
    auto result = std::basic_string_view<Char>{last, last};
    if (auto start = tree.branch(std::basic_string_view<Char>{}))
        for (auto edge : tree.depth_first_search(*start)) {
            if (tree.leaf(edge.child_node) || edge.labels_len <= result.size())
                continue;
            auto labels = tree.labels(edge);
            result = std::basic_string_view<Char>{first + labels.first,
                                                  first + labels.last};
        }
    return result;
}

}  // namespace step20

#endif  // STEP20_LONGEST_REPEATED_SUBSTRING_HPP
