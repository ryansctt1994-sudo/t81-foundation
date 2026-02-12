/**
 * @file T81Stream.hpp
 * @brief Defines the T81Stream class for infinite, lazy sequences.
 */
#pragma once

#include "t81/core/T81Int.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81List.hpp"
#include <coroutine>
#include <optional>
#include <concepts>
#include <utility>

namespace t81 {

template <typename T>
class T81Stream {
public:
    struct promise_type {
        T current_value{};
        auto get_return_object() { return T81Stream{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        static std::suspend_always initial_suspend() noexcept { return {}; }
        static std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        auto yield_value(T value) noexcept {
            current_value = std::move(value);
            return std::suspend_always{};
        }
        void return_void() noexcept {}
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro_;

    explicit T81Stream(handle_type h) noexcept : coro_(h) {}

public:
    using value_type = T;

    T81Stream() noexcept : coro_(nullptr) {}
    ~T81Stream() { if (coro_) coro_.destroy(); }

    T81Stream(const T81Stream&) = delete;
    T81Stream& operator=(const T81Stream&) = delete;

    T81Stream(T81Stream&& other) noexcept : coro_(other.coro_) {
        other.coro_ = nullptr;
    }

    T81Stream& operator=(T81Stream&& other) noexcept {
        if (this != &other) {
            if (coro_) coro_.destroy();
            coro_ = other.coro_;
            other.coro_ = nullptr;
        }
        return *this;
    }

    class iterator {
        handle_type coro_;
        bool done_ = false;

    public:
        explicit iterator(handle_type h = nullptr) noexcept : coro_(h) {
            if (coro_) {
                advance();
            } else {
                done_ = true;
            }
        }

        void advance() {
            if (coro_ && !coro_.done()) {
                coro_.resume();
                done_ = coro_.done();
            } else {
                done_ = true;
            }
        }

        bool operator==(const iterator& o) const noexcept {
            return done_ == o.done_;
        }

        bool operator!=(const iterator& o) const noexcept {
            return !(*this == o);
        }

        iterator& operator++() { advance(); return *this; }
        const T& operator*() const noexcept { return coro_.promise().current_value; }
    };

    [[nodiscard]] iterator begin() { return iterator(coro_); }
    [[nodiscard]] iterator end() const noexcept { return iterator(nullptr); }
};

template <typename F>
[[nodiscard]] T81Stream<decltype(std::declval<F>()())> stream_from(F generator) {
    while (true) co_yield generator();
}

template <typename T>
[[nodiscard]] T81Stream<T> constant(T value) {
    while (true) co_yield value;
}

[[nodiscard]] inline T81Stream<T81Int<81>> naturals() {
    T81Int<81> state(0);
    while(true) {
        co_yield state;
        state += T81Int<81>(1);
    }
}

[[nodiscard]] inline T81Stream<T81Int<81>> fibonacci() {
    T81Int<81> a(0), b(1);
    while(true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

} // namespace t81
