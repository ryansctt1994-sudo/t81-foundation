/**
 * @file T81Network.hpp
 * @brief Defines the T81Network class for ternary-native, reflective networking.
 */
#pragma once

#include "t81/core/T81Thread.hpp"
#include "t81/core/T81Bytes.hpp"
#include "t81/core/T81String.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Time.hpp"
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Result.hpp"
#include <asio.hpp>
#include <memory>

namespace t81 {
using asio::ip::tcp;

struct T81Endpoint {
    std::string host;
    uint16_t  port;

    T81Endpoint(std::string h, uint16_t p) : host(std::move(h)), port(p) {}
    [[nodiscard]] std::string to_string() const { return host + ":" + std::to_string(port); }

    bool operator<(const T81Endpoint& o) const {
        if (host != o.host) return host < o.host;
        return port < o.port;
    }
};

class T81Network {
    asio::io_context ioc_;
    std::unique_ptr<std::thread> runner_;
    std::atomic<bool> alive_{true};

    T81Network() {
        runner_ = std::make_unique<std::thread>([this]() {
            while (alive_) {
                ioc_.run_one();
                std::this_thread::yield();
            }
        });
    }

public:
    static inline T81Network& universe() {
        static T81Network instance;
        return instance;
    }

    ~T81Network() {
        alive_ = false;
        if (runner_ && runner_->joinable()) runner_->join();
    }

    asio::io_context& context() { return ioc_; }

    [[nodiscard]] static T81Result<tcp::socket> connect(
        const T81Endpoint& remote,
        T81Agent& self
    ) {
        auto fuel = self.consume_entropy();
        if (!fuel) {
            return T81Result<tcp::socket>::failure(errors::OUT_OF_ENTROPY,
                T81String("CANNOT REACH ACROSS THE VOID WITHOUT FUEL"));
        }

        tcp::socket sock(universe().ioc_);
        asio::error_code ec;
        auto addr = asio::ip::make_address(remote.host, ec);
        if (ec) {
             return T81Result<tcp::socket>::failure(T81Symbol::intern("INVALID_ADDRESS"),
                T81String("INVALID ADDRESS: ") + T81String(ec.message().c_str()));
        }

        sock.connect(tcp::endpoint(addr, remote.port), ec);

        if (ec) {
            return T81Result<tcp::socket>::failure(T81Symbol::intern("CANNOT_CONNECT"),
                T81String("FAILED TO REACH ") + T81String(remote.to_string().c_str()) + T81String(": ") + T81String(ec.message().c_str()));
        }

        self.observe(symbols::CONNECTION_MADE);
        return T81Result<tcp::socket>::success(std::move(sock));
    }

    static T81Result<bool> send(tcp::socket& sock, const T81Bytes& message, T81Agent& self) {
        auto fuel = self.consume_entropy();
        if (!fuel) {
            return T81Result<bool>::failure(errors::OUT_OF_ENTROPY, T81String("SPEECH REQUIRES ENERGY"));
        }

        asio::error_code ec;
        asio::write(sock, asio::buffer(message.data(), message.size()), ec);

        if (ec) {
            return T81Result<bool>::failure(T81Symbol::intern("TRANSMISSION_FAILED"),
                T81String("THE VOID SWALLOWED THE WORDS: ") + T81String(ec.message().c_str()));
        }

        return T81Result<bool>::success(true);
    }

    static T81Result<T81Bytes> receive(tcp::socket& sock, size_t max_bytes, T81Agent& self) {
        auto fuel = self.consume_entropy();
        if (!fuel) {
            return T81Result<T81Bytes>::failure(errors::OUT_OF_ENTROPY, T81String("LISTENING REQUIRES ATTENTION"));
        }

        T81Bytes buffer(max_bytes);
        asio::error_code ec;
        size_t received = sock.read_some(asio::buffer(buffer.data(), max_bytes), ec);

        if (ec && ec != asio::error::eof) {
            return T81Result<T81Bytes>::failure(T81Symbol::intern("RECEPTION_FAILED"),
                T81String("THE VOICE WAS LOST IN THE VOID: ") + T81String(ec.message().c_str()));
        }

        return T81Result<T81Bytes>::success(buffer.slice(0, received));
    }
};

} // namespace t81
