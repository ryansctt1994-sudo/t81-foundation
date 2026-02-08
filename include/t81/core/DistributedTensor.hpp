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
     * @brief Performs a distributed reduction (sum).
     * In a mock environment, we sum all local data and return.
     */
    Element reduce_sum() const {
        return t81::reduce_sum(*local_data_);
    }

    /**
     * @brief Performs a distributed matrix multiplication C = A * B.
     * This mock implementation handles Rank 2 tensors.
     * A is sharded by rows, B is replicated.
     * Returns a sharded result C (also sharded by rows).
     */
    template <size_t InnerDim, size_t Cols, size_t OutRows>
    void matmul(const T81Tensor<Element, 2, InnerDim, Cols>& B,
                DistributedT81Tensor<Element, 2, OutRows, Cols>& C) {
        static constexpr auto shape_A = LocalTensor::shape();
        if (shape_A[1] != InnerDim || shape_A[0] != OutRows) {
            throw std::invalid_argument("Matrix dimension mismatch");
        }

        // Local computation: each shard multiplies its rows of A with B.
        auto& A_local = local();
        auto& C_local = C.local();

        for (size_t i = 0; i < shape_A[0]; ++i) {
            for (size_t j = 0; j < Cols; ++j) {
                Element sum{};
                for (size_t k = 0; k < InnerDim; ++k) {
                    sum = sum + A_local(i, k) * B(k, j);
                }
                C_local(i, j) = sum;
            }
        }
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
        for (size_t i = 0; i < l.size(); ++i) {
            l.data[i] += r.data[i];
        }
    }

private:
    size_t shard_id_;
    size_t num_shards_;
    std::unique_ptr<LocalTensor> local_data_;
};

} // namespace t81
