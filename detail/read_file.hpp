// Andrew Naplavkov

#ifndef STEP20_READ_FILE_HPP
#define STEP20_READ_FILE_HPP

#include <fstream>
#include <iterator>
#include <string>

namespace step20 {

inline std::string read_file(const char* file_name)
{
    using iter_t = std::istreambuf_iterator<char>;
    auto is = std::ifstream{file_name};
    return {(iter_t(is)), iter_t()};
}

}  // namespace step20

#endif  // STEP20_READ_FILE_HPP
