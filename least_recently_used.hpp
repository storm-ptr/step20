// Andrew Naplavkov

#ifndef STEP20_LEAST_RECENTLY_USED_HPP
#define STEP20_LEAST_RECENTLY_USED_HPP

#include <list>
#include <unordered_map>

namespace step20::least_recently_used {
namespace detail {

template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>>
class linked_hash_map {
public:
    using value_type = std::pair<const Key, T>;
    using list_type = std::list<value_type>;
    using iterator = typename list_type::iterator;

    auto size() const { return list_.size(); }
    iterator begin() { return list_.begin(); }
    iterator end() { return list_.end(); }
    void transfer(iterator from, iterator to) { list_.splice(to, list_, from); }

    iterator find(const Key& key)
    {
        auto it = map_.find(key);
        return it == map_.end() ? list_.end() : it->second;
    }

    iterator erase(iterator it)
    {
        map_.erase(it->first);
        return list_.erase(it);
    }

    template <class M>
    std::pair<iterator, bool> emplace(iterator it, const Key& key, M&& val)
    {
        if (auto pos = find(key); pos != end())
            return {pos, false};
        it = list_.emplace(it, key, std::forward<M>(val));
        try {
            map_.emplace(key, it);
            return {it, true};
        }
        catch (...) {
            list_.erase(it);
            throw;
        }
    }

private:
    list_type list_;
    std::unordered_map<Key, iterator, Hash, KeyEqual> map_;
};

}  // namespace detail

/// An O(1) algorithm for implementing the LRU cache eviction scheme
template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>>
class cache {
    detail::linked_hash_map<Key, T, Hash, KeyEqual> map_;
    size_t capacity_;

public:
    explicit cache(size_t capacity) : capacity_(capacity) {}

    /// @return nullptr if key is not found
    const T* find(const Key& key)
    {
        auto it = map_.find(key);
        if (it == map_.end())
            return nullptr;
        map_.transfer(it, map_.end());
        return std::addressof(it->second);
    }

    /// Basic exception guarantee.
    /// Inserted item remains if an exception occurs.
    void insert_or_assign(const Key& key, const T& val)
    {
        auto [it, success] = map_.emplace(map_.end(), key, val);
        if (!success) {
            map_.transfer(it, map_.end());
            it->second = val;
            return;
        }
        while (map_.size() > capacity_)
            map_.erase(map_.begin());
    }
};

}  // namespace step20::least_recently_used

#endif  // STEP20_LEAST_RECENTLY_USED_HPP
