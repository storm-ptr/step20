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

inline pair_t split(range_t r, std::string_view pattern)
{
    auto it = std::ranges::find(r, pattern);
    return {{r.begin(), it}, {std::next(it), r.end()}};
}

inline generator<pair_t> zip_split(range_t r1, range_t r2, range_t patterns)
{
    for (auto pattern : patterns) {
        auto [head1, tail1] = split(r1, pattern);
        auto [head2, tail2] = split(r2, pattern);
        co_yield {head1, head2};
        r1 = tail1;
        r2 = tail2;
    }
    co_yield {r1, r2};
}

}  // namespace detail

/// @see https://en.wikipedia.org/wiki/Diff#Unified_format
inline void dump(std::string_view txt1, std::string_view txt2, std::ostream& os)
{
    auto lines1 = detail::to_lines(txt1);
    auto lines2 = detail::to_lines(txt2);
    auto patterns = detail::lines_t{};
    longest_common_subsequence::copy(
        lines1, lines2, std::back_inserter(patterns));
    for (auto [r1, r2] : detail::zip_split(lines1, lines2, patterns)) {
        if (r1.empty() && r2.empty())
            continue;
        os << "@@ -" << (r1.begin() - lines1.begin() + 1) << "," << r1.size()
           << " +" << (r2.begin() - lines2.begin() + 1) << "," << r2.size()
           << " @@\n";
        for (auto line : r1)
            os << "-" << line << "\n";
        for (auto line : r2)
            os << "+" << line << "\n";
    }
}

}  // namespace step20::diff

#endif  // STEP20_EXAMPLE_DIFF_HPP
