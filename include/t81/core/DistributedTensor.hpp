#pragma once

#include "t81/core/T81Tensor.hpp"
#include <vector>
#include <memory>

namespace t81 {

/**
 * @class DistributedT81Tensor
 * @brief A wrapper for T81Tensor that supports distributed computation.
 */
template <typename Element, size_t Rank, size_t... Dims>
class DistributedT81Tensor {
public:
    using LocalTensor = T81Tensor<Element, Rank, Dims...>;

    DistributedT81Tensor(size_t shard_id, size_t num_shards)
        : shard_id_(shard_id), num_shards_(num_shards) {
        local_data_ = std::make_unique<LocalTensor>();
    }

    size_t shard_id() const { return shard_id_; }
    size_t num_shards() const { return num_shards_; }
    LocalTensor& local() { return *local_data_; }
    const LocalTensor& local() const { return *local_data_; }

    void shard(const LocalTensor& global) {
        *local_data_ = global;
    }

    LocalTensor gather() const {
        return *local_data_;
    }

    /**
     * @brief Performs a distributed elementwise addition.
     */
    void add(const DistributedT81Tensor& other) {
        if (num_shards_ != other.num_shards_ || shard_id_ != other.shard_id_) {
            throw std::invalid_argument("Shard configuration mismatch");
        }
        // In a real distributed system, this would trigger network ops.
        // Here we just operate on local shards.
        auto& l = local();
        const auto& r = other.local();
        for (size_t i = 0; i < l.data().size(); ++i) {
            l.data()[i] += r.data()[i];
        }
    }

private:
    size_t shard_id_;
    size_t num_shards_;
    std::unique_ptr<LocalTensor> local_data_;
};

} // namespace t81
