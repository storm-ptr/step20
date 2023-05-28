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

/// @see https://en.cppreference.com/w/cpp/header/generator
template <std::movable T>
struct generator : private std::unique_ptr<void, decltype(co_destroy)> {
    struct promise_type : std::optional<T> {
        std::suspend_never initial_suspend() { return {}; }
        void await_transform() = delete;
        void return_void() {}
        void unhandled_exception() { throw; }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value)
        {
            this->emplace(std::move(value));
            return {};
        }

        generator get_return_object()
        {
            auto result = generator{};
            result.reset(handle::from_promise(*this).address());
            return result;
        }
    };

    using handle = std::coroutine_handle<promise_type>;

    struct iterator : private handle {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = const T&;
        explicit iterator(handle coro) : handle{coro} {}
        reference operator*() const { return *this->promise(); }
        bool operator==(std::default_sentinel_t) const { return this->done(); }
        void operator++(int) { this->resume(); }

        iterator& operator++()
        {
            this->resume();
            return *this;
        }
    };

    auto begin() const { return iterator{handle::from_address(get())}; }
    auto end() const { return std::default_sentinel; }
};

}  // namespace step20

#endif  // STEP20_GENERATOR_HPP
