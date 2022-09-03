// Andrew Naplavkov

#ifndef STEP20_EXAMPLE_SUFFIX_TREE_VIZ_HPP
#define STEP20_EXAMPLE_SUFFIX_TREE_VIZ_HPP

#include "../../suffix_tree.hpp"
#include <cstdint>
#include <map>
#include <ostream>
#include <string_view>

namespace step20 {

// @see https://graphviz.org/
struct suffix_tree_viz : suffix_tree<char, uint32_t, std::map<char, uint32_t>> {
    friend std::ostream& operator<<(std::ostream& os, const suffix_tree_viz& me)
    {
        auto inner_node_viz = [&](const auto& node) { os << "_" << node; };
        auto child_node_viz = [&](const auto& edge) {
            me.leaf(edge.child_node) ? void(os << me.labels(edge).first)
                                     : inner_node_viz(edge.child_node);
        };
        os << "digraph \"" << std::string_view(me.data(), me.size())
           << "\" {\nrankdir=LR\n";
        if (auto start = me.branch(std::string_view{}))
            for (auto edge : me.depth_first_search(*start)) {
                child_node_viz(edge);
                os << " [shape="
                   << (me.leaf(edge.child_node) ? "plaintext" : "point")
                   << "]\n";
                if (edge.parent_node == edge.child_node)  ///< aux root edge
                    continue;
                inner_node_viz(edge.parent_node);
                os << "->";
                child_node_viz(edge);
                auto lbl = me.label(edge.child_node);
                os << " [label=\""
                   << std::string_view(me.data() + lbl.first, lbl.length())
                   << "\"]\n";
            }
        return os << "}\n";
    }
};

}  // namespace step20

#endif  // STEP20_EXAMPLE_SUFFIX_TREE_VIZ_HPP
