// Andrew Naplavkov

#ifndef STEP20_SUFFIX_ARRAY_HPP
#define STEP20_SUFFIX_ARRAY_HPP

#include "to.hpp"
#include <functional>
#include <span>
#include <string>

namespace step20 {

/// Manber's algorithm for constructing suffix array.

/// Time complexity O(N*log(N)*log(N)), space complexity O(N), where:
/// N - text length.
/// @param Char - type of the characters;
/// @param Size - to specify the maximum number / offset of characters;
/// @param Compare - to determine the order of characters.
template <class Char,
          std::unsigned_integral Size = size_t,
          class Compare = std::less<Char>>
class suffix_array {
public:
    using value_type = Char;
    using size_type = Size;
    const Char* data() const { return str_.data(); }
    Size size() const { return str_.size(); }
    const Compare& comp() const { return comp_; }
    virtual ~suffix_array() = default;

    explicit suffix_array(std::basic_string<Char> str, const Compare& comp = {})
        : str_(std::move(str)), idx_(size()), comp_(comp)
    {
        auto sufs = std::vector<suffix>(size());
        auto gen = [i = Size{}]() mutable { return suffix{i++, {}}; };
        auto ch = [&](auto& suf) { return str_[suf.pos]; };
        auto comp_ch = [&](auto& l, auto& r) { return comp_(ch(l), ch(r)); };
        auto comp_rank = [](auto& l, auto& r) { return l.rank < r.rank; };
        std::ranges::generate(sufs, gen);
        fill_first_rank(sufs, comp_ch);
        for (Size offset = 1; !sorted(sufs); offset *= 2) {
            fill_second_rank(sufs, offset);
            fill_first_rank(sufs, comp_rank);
        }
        std::ranges::transform(sufs, idx_.begin(), &suffix::pos);
    }

    explicit suffix_array(std::ranges::input_range auto&& str,
                          const Compare& comp = {})
        : suffix_array(to<std::basic_string>(str), comp)
    {
    }

    /// Starting positions of non-empty suffixes in lexicographic order.
    const Size* sorted_suffixes() const { return idx_.data(); }

    /// @return positions of suffixes starting with substring

    /// Time complexity O(M*log(N)), where:
    /// M - @param str length, N - text length.
    std::span<const Size> equal_range(std::ranges::input_range auto&& str) const
    {
        auto result = std::span{idx_};
        auto first = std::ranges::begin(str);
        auto last = std::ranges::end(str);
        auto i = Size{};
        auto ch = [&](Size n) { return n < str_.size() ? str_[n] : *first; };
        auto comp = [&](Size l, Size r) { return comp_(ch(l + i), ch(r + i)); };
        for (; first != last; ++first, ++i)
            result = std::ranges::equal_range(result, size(), comp);
        return result;
    }

private:
    std::basic_string<Char> str_;
    std::vector<Size> idx_;
    Compare comp_;

    struct suffix {
        Size pos;
        std::pair<Size, Size> rank;

        friend void fill_first_rank(std::vector<suffix>& sufs, auto comp)
        {
            std::ranges::sort(sufs, comp);
            Size acc = 1;
            for (Size i = 1; i < sufs.size(); ++i) {
                bool diff = comp(sufs[i - 1], sufs[i]);
                sufs[i - 1].rank.first = acc;
                acc += diff;
            }
            if (!sufs.empty())
                sufs.back().rank.first = acc;
        }

        friend void fill_second_rank(std::vector<suffix>& sufs, Size offset)
        {
            auto ranks = std::vector<Size>(sufs.size());
            auto at = [&](Size n) { return n < ranks.size() ? ranks[n] : 0; };
            for (auto& suf : sufs)
                ranks[suf.pos] = suf.rank.first;
            for (auto& suf : sufs)
                suf.rank.second = at(suf.pos + offset);
        }

        friend bool sorted(const std::vector<suffix>& sufs)
        {
            return sufs.empty() || sufs.back().rank.first == sufs.size();
        }
    };
};

template <std::ranges::input_range R>
suffix_array(R) -> suffix_array<std::ranges::range_value_t<R>>;

/// Kasai's algorithm for constructing longest common prefix array.

/// Time and space complexity O(N), where: N - text length.
template <class Char,
          std::unsigned_integral Size = size_t,
          class Compare = std::less<Char>>
class enhanced_suffix_array : public suffix_array<Char, Size, Compare> {
public:
    explicit enhanced_suffix_array(suffix_array<Char, Size, Compare>&& array)
        : suffix_array<Char, Size, Compare>(std::move(array))
        , lcp_(this->size())
    {
        auto inverse = std::vector<Size>(this->size());
        auto eq = equivalence_fn(this->comp());
        for (Size i = 0; i < this->size(); ++i)
            inverse[this->sorted_suffixes()[i]] = i;
        for (Size i = 0, lcp = 0; i < this->size(); ++i, lcp -= !!lcp) {
            Size cur = inverse[i];
            Size next = cur + 1;
            if (next < this->size()) {
                auto diff = std::mismatch(
                    this->data() + i + lcp,
                    this->data() + this->size(),
                    this->data() + this->sorted_suffixes()[next] + lcp,
                    this->data() + this->size(),
                    eq);
                lcp = std::distance(this->data() + i, diff.first);
            }
            else
                lcp = 0;
            lcp_[cur] = lcp;
        }
    }

    template <std::ranges::input_range R>
    explicit enhanced_suffix_array(R&& str, const Compare& comp = {})
        : enhanced_suffix_array(
              suffix_array<Char, Size, Compare>(std::forward<R>(str), comp))
    {
    }

    /// Lengths of the longest common prefixes of adjacent sorted suffixes.
    const Size* longest_common_prefixes() const { return lcp_.data(); }

private:
    std::vector<Size> lcp_;
};

template <std::ranges::input_range R>
enhanced_suffix_array(R)
    -> enhanced_suffix_array<std::ranges::range_value_t<R>>;

}  // namespace step20

#endif  // STEP20_SUFFIX_ARRAY_HPP
