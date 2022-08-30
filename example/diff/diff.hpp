// Andrew Naplavkov

#ifndef STEP20_EXAMPLE_DIFF_HPP
#define STEP20_EXAMPLE_DIFF_HPP

#include "../../generator.hpp"
#include "../../longest_common_subsequence.hpp"
#include "../../to.hpp"
#include <ostream>
#include <string_view>

namespace step20::diff {
namespace detail {

using lines_t = std::vector<std::string_view>;
using range_t = std::ranges::subrange<lines_t::iterator>;
using pair_t = std::pair<range_t, range_t>;

inline lines_t to_lines(std::string_view txt)
{
    return to<lines_t>(
        txt | std::views::split('\n') | std::views::transform([](auto&& line) {
            return std::string_view(std::addressof(*line.begin()),
                                    std::ranges::distance(line));
        }));
}

inline pair_t partition(range_t range, std::string_view separator)
{
    auto it = std::ranges::find(range, separator);
    return {{range.begin(), it}, {std::next(it), range.end()}};
}

inline generator<pair_t> zip_partition(pair_t pair, range_t separators)
{
    for (auto separator : separators) {
        auto [head1, tail1] = partition(pair.first, separator);
        auto [head2, tail2] = partition(pair.second, separator);
        co_yield {head1, head2};
        pair = {tail1, tail2};
    }
    co_yield pair;
}

}  // namespace detail

/// @see https://en.wikipedia.org/wiki/Diff#Unified_format
inline void dump(std::string_view txt1, std::string_view txt2, std::ostream& os)
{
    auto lines1 = detail::to_lines(txt1);
    auto lines2 = detail::to_lines(txt2);
    auto separators = detail::lines_t{};
    longest_common_subsequence::copy(
        lines1, lines2, std::back_inserter(separators));
    for (auto hunk : detail::zip_partition({lines1, lines2}, separators)) {
        if (hunk.first.empty() && hunk.second.empty())
            continue;
        os << "@@ -" << (hunk.first.begin() - lines1.begin() + 1) << ","
           << hunk.first.size() << " +"
           << (hunk.second.begin() - lines2.begin() + 1) << ","
           << hunk.second.size() << " @@\n";
        for (auto line : hunk.first)
            os << "-" << line << "\n";
        for (auto line : hunk.second)
            os << "+" << line << "\n";
    }
}

}  // namespace step20::diff

#endif  // STEP20_EXAMPLE_DIFF_HPP
