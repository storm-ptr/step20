// Andrew Naplavkov

#ifndef STEP20_GENERATOR_HPP
#define STEP20_GENERATOR_HPP

#include <coroutine>
#include <iterator>
#include <memory>
#include <optional>

namespace step20 {

inline auto co_destroy = [](void* address) {
    std::coroutine_handle<>::from_address(address).destroy();
};

/// @see wg21.link/p2168
template <std::movable T>
struct generator : private std::unique_ptr<void, decltype(co_destroy)> {
    struct promise_type : std::optional<T> {
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { throw; }
        void return_void() {}
        void await_transform() = delete;

        std::suspend_always yield_value(T value) noexcept
        {
            this->emplace(std::move(value));
            return {};
        }

        generator get_return_object()
        {
            auto result = generator{};
            result.reset(co_handle::from_promise(*this).address());
            return result;
        }
    };

    using co_handle = std::coroutine_handle<promise_type>;

    struct iterator : private co_handle {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = const T&;
        explicit iterator(const co_handle& fn) : co_handle{fn} {}
        reference operator*() const { return *this->promise(); }
        bool operator==(std::default_sentinel_t) const { return this->done(); }

        iterator& operator++()
        {
            this->resume();
            return *this;
        }

        iterator operator++(int)
        {
            auto unchanged_value = std::move<std::optional<T>>(this->promise());
            this->resume();
            this->promise() = std::move(unchanged_value);
            return *this;
        }
    };

    auto begin() const { return iterator{co_handle::from_address(get())}; }
    auto end() const { return std::default_sentinel; }
};

}  // namespace step20

#endif  // STEP20_GENERATOR_HPP
