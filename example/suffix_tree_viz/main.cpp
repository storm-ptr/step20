// Andrew Naplavkov

#include <iostream>
#include <step20/example/suffix_tree_viz/suffix_tree_viz.hpp>
#include <step20/to.hpp>

int main(int argc, char* argv[])
{
    using namespace step20;
    auto str = std::string_view{argv[1]};
    auto tree = to<suffix_tree_viz>(str);
    std::cout << tree;
}
