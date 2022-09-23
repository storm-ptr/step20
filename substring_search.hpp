// Andrew Naplavkov

#ifndef STEP20_SUBSTRING_SEARCH_HPP
#define STEP20_SUBSTRING_SEARCH_HPP

#include "suffix_array.hpp"
#include "suffix_tree.hpp"

namespace step20::substring_search {

/// Find substring offset.

/// Time complexity O(M*log(N)), where:
/// M - @param str length, N - @param array text length.
template <class... Ts, std::ranges::forward_range R>
auto find_any(const suffix_array<Ts...>& array, R&& str)
    -> std::optional<typename suffix_array<Ts...>::size_type>
{
    if (std::ranges::empty(str))
        return array.size();
    for (auto pos : array.equal_range(str))
        return pos;
    return std::nullopt;
}

/// Find all occurrences of the substring.
template <class... Ts, std::ranges::forward_range R>
auto find_all(const suffix_array<Ts...>& array, R&& str)
    -> generator<typename suffix_array<Ts...>::size_type>
{
    if (std::ranges::empty(str))
        co_yield array.size();
    for (auto pos : array.equal_range(str))
        co_yield pos;
}

/// Find offset of the first occurrence of the substring.

/// Time complexity O(M), where: M - substring length.
template <class... Ts, std::ranges::forward_range R>
auto find_first(const suffix_tree<Ts...>& tree, R&& str)
    -> std::optional<typename suffix_tree<Ts...>::size_type>
{
    if (auto edge = tree.branch(str))
        return tree.labels(*edge).first;
    return std::ranges::empty(str) ? std::optional{0} : std::nullopt;
}

/// Find all occurrences of @param str

/// Suffix tree must be explicit - padded with a terminal symbol.
/// Space complexity asymptotically close to O(log(N)), O(N) at worst,
/// where: N - text length of @param tree
template <class... Ts, std::ranges::forward_range R>
auto find_all(const suffix_tree<Ts...>& tree, R&& str)
    -> generator<typename suffix_tree<Ts...>::size_type>
{
    if (std::ranges::empty(str))
        co_yield tree.size();
    if (auto start = tree.branch(str))
        for (auto edge : tree.depth_first_search(*start))
            if (tree.leaf(edge.child_node))
                co_yield tree.labels(edge).first;
}

}  // namespace step20::substring_search

#endif  // STEP20_SUBSTRING_SEARCH_HPP
