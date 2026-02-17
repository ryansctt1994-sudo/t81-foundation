/**
 * @file T81Discovery.hpp
 * @brief Defines a zero-configuration peer discovery protocol for T81 agents.
 */
#pragma once

#include <asio.hpp>
#include <mutex>
#include <set>
#include <thread>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Bytes.hpp"
#include "t81/core/T81Entropy.hpp"
#include "t81/core/T81Network.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Time.hpp"

namespace t81 {
using asio::ip::udp;

struct T81Beacon {
  T81Symbol identity;
  T81String name;
  uint16_t port;

  [[nodiscard]] T81Bytes serialize() const {
    T81Bytes data;
    auto raw_id = identity.raw();
    data += T81Bytes(reinterpret_cast<const uint8_t*>(&raw_id), sizeof(raw_id));

    // Serialize name with null terminator
    std::string s = name.str();
    data += T81Bytes(reinterpret_cast<const uint8_t*>(s.c_str()), s.length() + 1);

    data += T81Bytes(reinterpret_cast<const uint8_t*>(&port), sizeof(port));
    return data;
  }

  static T81Beacon deserialize(const T81Bytes& raw) {
    T81Beacon b;
    size_t off = 0;
    T81Int<81> raw_id;
    std::memcpy(&raw_id, raw.data() + off, sizeof(raw_id));
    off += sizeof(raw_id);
    b.identity = T81Symbol::from_raw(raw_id);

    std::string name_str;
    while (off < raw.size() && raw.data()[off] != 0 && off < 256) {
      name_str.push_back(static_cast<char>(raw.data()[off++]));
    }
    b.name = T81String(name_str);
    if (off < raw.size() && raw.data()[off] == 0) off++;

    std::memcpy(&b.port, raw.data() + off, sizeof(b.port));
    return b;
  }
};

class T81Discovery {
  static constexpr uint16_t DISCOVERY_PORT = 8181;

  udp::socket socket_;
  udp::endpoint broadcast_ep_;
  std::thread listener_;
  std::thread beacon_thread_;
  std::atomic<bool> alive_{true};
  std::mutex cv_mutex_;
  std::condition_variable cv_;

  mutable std::mutex peers_mutex_;
  std::set<T81Endpoint> known_minds_;

  T81Agent& self_;
  uint16_t listen_port_;

  void beacon_loop() {
    while (alive_) {
      auto fuel = self_.consume_entropy();
      if (fuel) {
        T81Beacon beacon{self_.identity(), T81String(self_.identity().to_string().c_str()),
                         listen_port_};
        auto packet = beacon.serialize();
        try {
          socket_.send_to(asio::buffer(packet.data(), packet.size()), broadcast_ep_);
        } catch (...) {
        }
      }

      std::unique_lock<std::mutex> lock(cv_mutex_);
      cv_.wait_for(lock, std::chrono::seconds(3), [this] { return !alive_; });
    }
  }

  void listen_loop() {
    T81Bytes buffer(1024);
    udp::endpoint sender;

    while (alive_) {
      asio::error_code ec;
      size_t len = socket_.receive_from(asio::buffer(buffer.data(), buffer.size()), sender, 0, ec);
      if (ec || len == 0) continue;

      try {
        auto incoming = T81Beacon::deserialize(buffer.slice(0, len));
        T81Endpoint peer(incoming.name, incoming.port);

        {
          std::lock_guard<std::mutex> lock(peers_mutex_);
          if (known_minds_.insert(peer).second) {
            self_.observe(symbols::NEW_MIND_DISCOVERED);
          }
        }
      } catch (...) {
      }
    }
  }

public:
  static T81Discovery& join(T81Agent& me, uint16_t port = 0) {
    static T81Discovery instance(me, port);
    return instance;
  }

private:
  T81Discovery(T81Agent& me, uint16_t port)
      : socket_(T81Network::universe().context(), udp::endpoint(udp::v4(), 0)),
        broadcast_ep_(asio::ip::make_address_v4("255.255.255.255"), DISCOVERY_PORT),
        self_(me),
        listen_port_(port == 0 ? 8181 : port) {
    socket_.set_option(asio::socket_base::broadcast(true));
    listener_ = std::thread([this]() { listen_loop(); });
    beacon_thread_ = std::thread([this]() { beacon_loop(); });
  }

public:
  ~T81Discovery() {
    {
      std::lock_guard<std::mutex> lock(cv_mutex_);
      alive_ = false;
      cv_.notify_all();
    }

    // Close socket to unblock listener
    asio::error_code ec;
    socket_.close(ec);

    if (listener_.joinable()) listener_.join();
    if (beacon_thread_.joinable()) beacon_thread_.join();
  }

  [[nodiscard]] std::vector<T81Endpoint> peers() const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return {known_minds_.begin(), known_minds_.end()};
  }
};

}  // namespace t81
