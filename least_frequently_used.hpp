// Andrew Naplavkov

#ifndef STEP20_LEAST_FREQUENTLY_USED_HPP
#define STEP20_LEAST_FREQUENTLY_USED_HPP

#include <algorithm>
#include <list>
#include <unordered_map>

namespace step20::least_frequently_used {

/// An O(1) algorithm for implementing the LFU cache eviction scheme
template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>>
class cache {
    struct item_type;
    using item_list = std::list<item_type>;
    using item_iterator = typename item_list::iterator;
    struct freq_type;
    using freq_list = std::list<freq_type>;
    using freq_iterator = typename freq_list::iterator;

    struct item_type {
        freq_iterator parent;
        Key key;
        T val;
    };

    struct freq_type {
        std::size_t n;
        item_list items;
    };

    freq_list list_;
    std::unordered_map<Key, item_iterator, Hash, KeyEqual> map_;
    std::size_t capacity_;

    bool equal(freq_iterator it, std::size_t n) const
    {
        return it != list_.end() && it->n == n;
    }

public:
    explicit cache(std::size_t capacity) : capacity_(capacity) {}
    cache(cache&&) = default;
    cache& operator=(cache&&) = default;
    cache(const cache&) = delete;
    cache& operator=(const cache&) = delete;
    virtual ~cache() = default;

    /// @return nullptr if key is not found
    const T* find(const Key& key)
    {
        auto it = map_.find(key);
        if (it == map_.end())
            return nullptr;
        auto item = it->second;
        auto freq = item->parent;
        auto next = std::next(freq);
        if (!equal(next, freq->n + 1)) {
            if (freq->items.size() == 1) {
                ++freq->n;
                return std::addressof(item->val);
            }
            next = list_.emplace(next, freq->n + 1);
        }
        try {
            next->items.splice(next->items.end(), freq->items, item);
        }
        catch (...) {
            if (next->items.empty())
                list_.erase(next);
            throw;
        }
        item->parent = next;
        if (freq->items.empty())
            list_.erase(freq);
        return std::addressof(item->val);
    }

    /// Basic exception guarantee.
    /// Evicted item is not restored if an exception occurs.
    void insert_or_assign(const Key& key, const T& val)
    {
        if (auto ptr = find(key)) {
            *const_cast<T*>(ptr) = val;
            return;
        }
        if (map_.size() >= std::max<std::size_t>(1, capacity_)) {
            auto freq = list_.begin();
            auto item = freq->items.begin();
            map_.erase(item->key);
            freq->items.erase(item);
            if (freq->items.empty())
                list_.erase(freq);
        }
        auto freq = list_.begin();
        auto exists = equal(freq, 1);
        freq = exists ? freq : list_.emplace(freq, 1);
        auto item = freq->items.end();
        try {
            item = freq->items.emplace(item, freq, key, val);
            map_.emplace(key, item);
        }
        catch (...) {
            if (!exists)
                list_.erase(freq);
            else if (item != freq->items.end())
                freq->items.erase(item);
            throw;
        }
    }
};

}  // namespace step20::least_frequently_used

#endif  // STEP20_LEAST_FREQUENTLY_USED_HPP
