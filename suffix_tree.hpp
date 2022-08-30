// Andrew Naplavkov

#ifndef STEP20_SUFFIX_TREE_HPP
#define STEP20_SUFFIX_TREE_HPP

#include "generator.hpp"
#include "to.hpp"
#include <stack>
#include <string>
#include <unordered_map>

namespace step20 {

/// Ukkonen's online algorithm for constructing suffix tree.

/// Time complexity O(N*log(K)), space complexity O(N), where:
/// N - text length, K - alphabet size.
/// @param Char - type of the characters;
/// @param Size - to specify the maximum number / offset of characters;
/// @param Map - to associate characters with nodes.
template <class Char,
          std::unsigned_integral Size = size_t,
          class Map = std::unordered_map<Char, Size>>
class suffix_tree {
public:
    using value_type = Char;
    using size_type = Size;
    const Char* data() const { return str_.data(); }
    Size size() const { return str_.size(); }
    virtual ~suffix_tree() = default;

    void clear() noexcept
    {
        str_.clear();
        nodes_.clear();
        pos_ = node_ = 0;
    }

    /// Basic exception guarantee
    void push_back(Char ch)
    try {
        str_.push_back(ch);
        if (nodes_.empty())
            nodes_.emplace_back();
        auto tie = [this, src = nodes_.size()](Size dest) mutable {
            if (!leaf(src) && src != dest)
                nodes_[src++].link = dest;
        };
        while (pos_ < size()) {
            if (Size& node = nodes_[node_].children[str_[pos_]]) {
                if (skip(node))
                    continue;
                if (!split(node))
                    return tie(node_);
                tie(nodes_.size() - 1);
            }
            else {
                node = flip(pos_);
                tie(node_);
            }
            node_ ? node_ = nodes_[node_].link : ++pos_;
        }
    }
    catch (...) {
        clear();
        throw;
    }

    /// Find offset of the first occurrence of the substring.

    /// Time complexity O(M), where: M - substring length.
    std::optional<Size> find_first(std::ranges::forward_range auto&& str) const
    {
        if (auto edge = branch(str))
            return labels(*edge).first;
        return std::ranges::empty(str) ? std::optional{0} : std::nullopt;
    }

    /// Find all occurrences of the substring for explicit suffix tree.

    /// Explicit means padded with a terminal symbol not seen in the text.
    generator<Size> find_all(std::ranges::forward_range auto&& str) const
    {
        if (std::ranges::empty(str))
            co_yield size();
        if (auto edge = branch(str))
            for (auto edge : depth_first_search(*edge))
                if (leaf(edge.child_node))
                    co_yield labels(edge).first;
    }

    struct edge_type {
        Size parent_node;
        Size child_node;
        Size labels_len;  ///< number of characters in [root_node .. child_node]
    };

    using slice_type = std::pair<Size, Size>;  ///< half-open character range
    static Size length(const slice_type& str) { return str.second - str.first; }
    bool leaf(Size node) const { return node >= nodes_.size(); }

    /// @return characters in node
    slice_type label(Size node) const
    {
        return leaf(node) ? slice_type{flip(node), size()} : nodes_[node].label;
    }

    /// @return concatenation of characters in [root_node .. edge.child_node]
    slice_type labels(const edge_type& edge) const
    {
        auto last = label(edge.child_node).second;
        return {last - edge.labels_len, last};
    }

    /// @return the minimum depth edge which labels start with @param str.
    std::optional<edge_type> branch(std::ranges::forward_range auto&& str) const
    {
        if (nodes_.empty())
            return std::nullopt;
        auto first = std::ranges::begin(str);
        auto last = std::ranges::end(str);
        for (auto edge = edge_type{};;) {
            auto lbl = label(edge.child_node);
            edge.labels_len += length(lbl);
            auto diff = std::mismatch(
                first, last, data() + lbl.first, data() + lbl.second, eq_);
            if (diff.first == last)
                return edge;
            if (diff.second != data() + lbl.second || leaf(edge.child_node))
                return std::nullopt;
            auto it = nodes_[edge.child_node].children.find(*diff.first);
            if (it == nodes_[edge.child_node].children.end())
                return std::nullopt;
            first = diff.first;
            edge.parent_node = std::exchange(edge.child_node, it->second);
        }
    }

    generator<edge_type> depth_first_search(const edge_type& start) const
    {
        for (auto stack = std::stack<edge_type>{{start}}; !stack.empty();) {
            auto edge = stack.top();
            co_yield edge;
            stack.pop();
            if (!leaf(edge.child_node))
                std::ranges::copy(
                    try_reverse(nodes_[edge.child_node].children) |
                        std::views::transform([&](auto&& item) {
                            return edge_type{
                                edge.child_node,
                                item.second,
                                edge.labels_len + length(label(item.second))};
                        }),
                    insert_iterator(stack));
        }
    }

private:
    struct node_type {
        Map children;
        slice_type label;
        Size link;
    };

    inline static auto eq_ = key_equivalence_fn<Map>();
    std::basic_string<Char> str_;
    std::vector<node_type> nodes_;  ///< inner nodes
    Size pos_{}, node_{};           ///< active point

    bool skip(Size node)
    {
        Size len = length(label(node));
        if (size() <= pos_ + len)
            return false;
        pos_ += len;
        node_ = node;
        return true;
    }

    bool split(Size& node)
    {
        auto lbl = label(node);
        Size cut = lbl.first + (size() - pos_) - 1;
        Size back = size() - 1;
        if (eq_(str_[cut], str_[back]))
            return false;
        Size old = std::exchange(node, (Size)nodes_.size());
        nodes_.push_back({{{str_[cut], leaf(old) ? flip(cut) : old},
                           {str_[back], flip(back)}},
                          {lbl.first, cut}});
        if (!leaf(old))
            nodes_[old].label = {cut, lbl.second};
        return true;
    }
};

}  // namespace step20

#endif  // STEP20_SUFFIX_TREE_HPP
