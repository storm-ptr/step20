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
/// @param Less - to determine the order of characters.
template <class Char,
          std::unsigned_integral Size = size_t,
          class Less = std::less<Char>>
class suffix_array {
public:
    using value_type = Char;
    using size_type = Size;
    const Char* data() const { return str_.data(); }
    Size size() const { return str_.size(); }
    virtual ~suffix_array() = default;

    explicit suffix_array(std::basic_string<Char> str, const Less& less = {})
        : less_(less), str_(std::move(str)), idx_(size())
    {
        auto sufs = std::vector<suffix>(size());
        auto gen = [i = Size{}]() mutable { return suffix{i++, {}}; };
        auto ch = [&](auto& suf) { return str_[suf.pos]; };
        auto less_ch = [&](auto& l, auto& r) { return less_(ch(l), ch(r)); };
        auto less_rank = [](auto& l, auto& r) { return l.rank < r.rank; };
        std::ranges::generate(sufs, gen);
        fill_first_rank(sufs, less_ch);
        for (Size offset = 1; !sorted(sufs); offset *= 2) {
            fill_second_rank(sufs, offset);
            fill_first_rank(sufs, less_rank);
        }
        std::ranges::transform(sufs, idx_.begin(), &suffix::pos);
    }

    explicit suffix_array(std::ranges::input_range auto&& str,
                          const Less& less = {})
        : suffix_array(to<std::basic_string>(str), less)
    {
    }

    /// @return starting positions of non-empty suffixes in lexicographic order
    const Size* sorted_suffixes() const { return idx_.data(); }

    /// @return starting positions of non-empty suffixes starting with substring

    /// Time complexity O(M*log(N)), where:
    /// M - @param str length, N - text length.
    std::span<const Size> equal_range(std::ranges::input_range auto&& str) const
    {
        auto result = std::span{idx_};
        auto first = std::ranges::begin(str);
        auto last = std::ranges::end(str);
        auto i = Size{};
        auto ch = [&](Size n) { return n < str_.size() ? str_[n] : *first; };
        auto less = [&](Size l, Size r) { return less_(ch(l + i), ch(r + i)); };
        for (; first != last; ++first, ++i)
            result = std::ranges::equal_range(result, size(), less);
        return result;
    }

private:
    Less less_;
    std::basic_string<Char> str_;
    std::vector<Size> idx_;

    struct suffix {
        Size pos;
        std::pair<Size, Size> rank;

        friend void fill_first_rank(std::vector<suffix>& sufs, auto less)
        {
            std::ranges::sort(sufs, less);
            Size acc = 1;
            for (Size i = 1; i < sufs.size(); ++i) {
                bool diff = less(sufs[i - 1], sufs[i]);
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
          class Less = std::less<Char>>
class enhanced_suffix_array : public suffix_array<Char, Size, Less> {
public:
    explicit enhanced_suffix_array(suffix_array<Char, Size, Less>&& array)
        : suffix_array<Char, Size, Less>(std::move(array)), lcp_(this->size())
    {
        auto inverse = std::vector<Size>(this->size());
        auto eq = equivalence<Less>;
        for (Size i = 0; i < this->size(); ++i)
            inverse[this->sorted_suffixes()[i]] = i;
        for (Size pos = 0, lcp = 0; pos < this->size(); ++pos, lcp -= !!lcp) {
            Size cur = inverse[pos];
            Size next = cur + 1;
            if (next < this->size()) {
                auto diff = std::mismatch(
                    this->data() + pos + lcp,
                    this->data() + this->size(),
                    this->data() + this->sorted_suffixes()[next] + lcp,
                    this->data() + this->size(),
                    eq);
                lcp = std::distance(this->data() + pos, diff.first);
            }
            else
                lcp = 0;
            lcp_[cur] = lcp;
        }
    }

    template <std::ranges::input_range R>
    explicit enhanced_suffix_array(R&& str, const Less& less = {})
        : enhanced_suffix_array(
              suffix_array<Char, Size, Less>(std::forward<R>(str), less))
    {
    }

    /// Lengths of the longest common prefixes between all pairs of
    /// consecutive non-empty suffixes, in lexicographic order.
    const Size* longest_common_prefixes() const { return lcp_.data(); }

private:
    std::vector<Size> lcp_;
};

template <std::ranges::input_range R>
enhanced_suffix_array(R)
    -> enhanced_suffix_array<std::ranges::range_value_t<R>>;

}  // namespace step20

#endif  // STEP20_SUFFIX_ARRAY_HPP
