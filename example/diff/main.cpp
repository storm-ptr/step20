// Andrew Naplavkov

#include <iostream>
#include <step20/detail/read_file.hpp>
#include <step20/example/diff/diff.hpp>

int main(int argc, char* argv[])
{
    using namespace step20;
    std::cout << "--- " << argv[1] << "\n+++ " << argv[2] << "\n";
    diff::dump(read_file(argv[1]), read_file(argv[2]), std::cout);
}
