// Andrew Naplavkov

#ifndef STEP20_UTILITY_HPP
#define STEP20_UTILITY_HPP

#include <array>
#include <concepts>
#include <iterator>
#include <limits>
#include <ranges>
#include <vector>

namespace step20 {

template <bool B>
void lazy_static_assert()
{
    static_assert(B);
}

template <std::unsigned_integral T>
T flip(T n)
{
    return std::numeric_limits<T>::max() - n;
}

template <class Compare>
constexpr auto equivalence_fn(const Compare& comp)
{
    return [comp](const auto& lhs, const auto& rhs) {
        return !comp(lhs, rhs) && !comp(rhs, lhs);
    };
}

template <class Map>
constexpr auto key_equivalence_fn()
{
    if constexpr (requires { typename Map::key_equal; })
        return typename Map::key_equal{};
    else if constexpr (requires { typename Map::key_compare; })
        return equivalence_fn(typename Map::key_compare{});
    else
        lazy_static_assert<false>();
}

auto try_reverse(std::ranges::borrowed_range auto&& range)
{
    if constexpr (requires { range | std::views::reverse; })
        return range | std::views::reverse;
    else
        return range | std::views::all;
}

template <class Container>
class emplace_iterator {
    Container* c_;

public:
    using iterator_category = std::output_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = void;
    using reference = void;
    explicit constexpr emplace_iterator(Container& c) : c_(std::addressof(c)) {}
    constexpr emplace_iterator& operator*() { return *this; }
    constexpr emplace_iterator& operator++() { return *this; }
    constexpr emplace_iterator operator++(int) { return *this; }

    template <class T>
    constexpr emplace_iterator& operator=(T&& val)
    {
        if constexpr (requires { c_->emplace(std::forward<T>(val)); })
            c_->emplace(std::forward<T>(val));
        else if constexpr (requires { c_->emplace_back(std::forward<T>(val)); })
            c_->emplace_back(std::forward<T>(val));
        else if constexpr (requires { c_->push_back(std::forward<T>(val)); })
            c_->push_back(std::forward<T>(val));  //< std::basic_string
        else
            lazy_static_assert<false>();
        return *this;
    }
};

template <class T, std::size_t N>
class ring_table {
    std::array<std::vector<T>, N> rows_;

public:
    explicit ring_table(std::size_t cols)
    {
        for (auto& row : rows_)
            row.resize(cols);
    }

    auto& operator[](std::size_t row) { return rows_[row % N]; }
};

}  // namespace step20

#endif  // STEP20_UTILITY_HPP
