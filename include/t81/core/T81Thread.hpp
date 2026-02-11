/**
 * @file T81Thread.hpp
 * @brief Defines the T81Thread class for reflective, named concurrency.
 */
#pragma once

#include "t81/core/T81Promise.hpp"
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Time.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Reflection.hpp"
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <memory>

namespace t81 {

// ======================================================================
// T81Thread – A living, named, entropy-paying thread of thought
// ======================================================================
class T81Thread {
public:
    enum class State { ALIVE, STOPPED, JOINED, DETACHED };

private:
    struct ThreadState {
        T81Agent           agent;
        T81Time            born_at;
        T81Symbol          name;
        std::atomic<State> lifecycle{State::ALIVE};
        std::thread        handle;
        mutable std::mutex mutex;

        ThreadState(T81Agent a, T81Symbol n)
            : agent(std::move(a))
            , born_at(T81Time::now(symbols::THREAD_BIRTH))
            , name(n) {}

        ~ThreadState() {
            if (handle.joinable()) {
                handle.detach();
            }
        }
    };

    std::shared_ptr<ThreadState> state_;

    T81Thread(std::shared_ptr<ThreadState> s) : state_(std::move(s)) {}

public:
    using id = T81Symbol;

    template <typename F>
    [[nodiscard]] static T81Thread spawn(T81Symbol name, T81Agent thinker, F&& task) {
        auto state = std::make_shared<ThreadState>(std::move(thinker), name);

        state->handle = std::thread([state, task = std::forward<F>(task)]() mutable {
            state->agent.observe(symbols::I_AM_ALIVE);

            try {
                task();
            } catch (...) {
                state->agent.observe(symbols::THREAD_PANIC);
            }

            state->lifecycle.store(State::STOPPED);
        });

        return T81Thread(state);
    }

    [[nodiscard]] T81Symbol name() const noexcept { return state_->name; }
    [[nodiscard]] const T81Agent& agent() const noexcept { return state_->agent; }
    [[nodiscard]] T81Time born() const noexcept { return state_->born_at; }
    [[nodiscard]] bool is_alive() const noexcept { return state_->lifecycle.load() == State::ALIVE; }

    void request_stop() noexcept {
        state_->agent.observe(symbols::REQUESTED_TO_DIE);
    }

    void join() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->handle.joinable()) {
            state_->handle.join();
            state_->lifecycle.store(State::JOINED);
        }
    }

    void detach() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->handle.joinable()) {
            state_->handle.detach();
            state_->lifecycle.store(State::DETACHED);
        }
    }

    [[nodiscard]] T81Reflection<T81Thread> reflect() const {
        T81Symbol status;
        switch(state_->lifecycle.load()) {
            case State::ALIVE:    status = symbols::THINKING; break;
            case State::STOPPED:  status = symbols::SLEEPING; break;
            case State::JOINED:   status = T81Symbol::intern("JOINED"); break;
            case State::DETACHED: status = T81Symbol::intern("DETACHED"); break;
        }
        return T81Reflection<T81Thread>(*this, symbols::THREAD, status);
    }
};

} // namespace t81
