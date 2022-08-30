// Andrew Naplavkov

#ifndef STEP20_LONGEST_REPEATED_SUBSTRING_HPP
#define STEP20_LONGEST_REPEATED_SUBSTRING_HPP

#include "suffix_array.hpp"
#include "suffix_tree.hpp"
#include <span>

namespace step20 {

/// Find the longest substring that occurs at least twice in @param array text.

/// Substring is contiguous, while subsequence need not be.
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

/// @param tree must be explicit - padded with a terminal symbol.
template <class... Ts>
auto longest_repeated_substring(const suffix_tree<Ts...>& tree)
    -> std::basic_string_view<typename suffix_tree<Ts...>::value_type>
{
    using Char = suffix_tree<Ts...>::value_type;
    auto first = tree.data();
    auto last = tree.data() + tree.size();
    auto result = std::basic_string_view<Char>{last, last};
    if (auto edge = tree.branch(std::basic_string_view<Char>{}))
        for (const auto& edge : tree.depth_first_search(*edge)) {
            if (tree.leaf(edge.child_node) || edge.labels_len <= result.size())
                continue;
            auto labels = tree.labels(edge);
            result = std::basic_string_view<Char>{first + labels.first,
                                                  first + labels.second};
        }
    return result;
}

}  // namespace step20

#endif  // STEP20_LONGEST_REPEATED_SUBSTRING_HPP
