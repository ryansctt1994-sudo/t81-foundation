/**
 * @file T81NN.hpp
 * @brief Ternary Neural Network Primitives.
 *
 * Implements high-level neural layers like Linear, Conv2D, Activation using T81Tensor.
 */
#pragma once

#include "t81/core/T81Tensor.hpp"
#include "t81/core/T81Float.hpp"

namespace t81 {
namespace nn {

    // Helper alias for common float type
    using TF = T81Float<72, 9>;

    /**
     * @brief Linear Layer (Dense).
     * Y = XW + B
     *
     * @tparam InFeatures Input dimension.
     * @tparam OutFeatures Output dimension.
     */
    template <size_t InFeatures, size_t OutFeatures>
    class Linear {
    public:
        // Weight: (In, Out) for x * W
        // Or (Out, In) for W * x^T?
        // PyTorch uses (Out, In) but does xA^T + b.
        // T81Tensor matmul(a, b) does (M, K) x (K, N) -> (M, N).
        // So if input X is (Batch, In), we need W to be (In, Out).

        T81Tensor<TF, 2, InFeatures, OutFeatures> weight;
        T81Tensor<TF, 1, OutFeatures> bias;

        constexpr Linear() {
            // Initialize with some default (e.g., Xavier/Glorot or just zeros for now)
            // Ideally we'd have a random init, but constexpr random is hard.
            // Leaving zero-init for deterministic default construction.
        }

        // Forward pass for single vector input (1, In) -> (1, Out) effectively
        [[nodiscard]] constexpr auto forward(const T81Tensor<TF, 1, InFeatures>& input) const
            -> T81Tensor<TF, 1, OutFeatures>
        {
            // (1, In) x (In, Out) -> (1, Out)
            // Using T81Tensor matmul for vector: (M, K) x (K) -> (M) ? No.
            // We need Vector * Matrix -> Vector.
            // T81Tensor currently has Matrix * Vector -> Vector.
            // So we can do y = W^T * x + b if W is stored as (In, Out).
            // But transpose is costly.

            // Let's implement Vector-Matrix mult helper here or use loop.
            T81Tensor<TF, 1, OutFeatures> out;
            for (size_t o = 0; o < OutFeatures; ++o) {
                TF sum = bias(o);
                for (size_t i = 0; i < InFeatures; ++i) {
                    sum = sum + input(i) * weight(i, o);
                }
                out(o) = sum;
            }
            return out;
        }

        // Forward pass for batch input (Batch, In) -> (Batch, Out)
        template <size_t Batch>
        [[nodiscard]] constexpr auto forward(const T81Tensor<TF, 2, Batch, InFeatures>& input) const
            -> T81Tensor<TF, 2, Batch, OutFeatures>
        {
             auto y = matmul(input, weight);
             // Broadcast bias
             T81Tensor<TF, 2, Batch, OutFeatures> out;
             for(size_t b=0; b<Batch; ++b) {
                 for(size_t o=0; o<OutFeatures; ++o) {
                     out(b, o) = y(b, o) + bias(o);
                 }
             }
             return out;
        }
    };

    /**
     * @brief 2D Convolution Layer.
     */
    template <size_t InChannels, size_t OutChannels, size_t KernelSize, size_t Stride=1, size_t Padding=0>
    class Conv2d {
    public:
        // Weight: (Out, In, K, K)
        T81Tensor<TF, 4, OutChannels, InChannels, KernelSize, KernelSize> weight;
        // Bias: (Out)
        T81Tensor<TF, 1, OutChannels> bias;

        constexpr Conv2d() {}

        template <size_t Batch, size_t H, size_t W>
        [[nodiscard]] constexpr auto forward(const T81Tensor<TF, 4, Batch, InChannels, H, W>& input) const {
            auto conv_res = t81::conv2d<Stride, Stride, Padding, Padding>(input, weight);

            // Add bias
            // conv_res is (Batch, Out, OH, OW)
            constexpr size_t OH = conv_res.shape()[2];
            constexpr size_t OW = conv_res.shape()[3];

            T81Tensor<TF, 4, Batch, OutChannels, OH, OW> out;
            for(size_t b=0; b<Batch; ++b) {
                for(size_t o=0; o<OutChannels; ++o) {
                    TF bias_val = bias(o);
                    for(size_t h=0; h<OH; ++h) {
                        for(size_t w=0; w<OW; ++w) {
                            out(b, o, h, w) = conv_res(b, o, h, w) + bias_val;
                        }
                    }
                }
            }
            return out;
        }
    };

    /**
     * @brief Activation Functions Module.
     */
    struct Functional {
        template <typename TensorType>
        static constexpr auto relu(const TensorType& t) { return t81::relu(t); }

        template <typename TensorType>
        static constexpr auto gelu(const TensorType& t) { return t81::gelu(t); }

        template <typename TensorType>
        static constexpr auto softmax(const TensorType& t) { return t81::softmax(t); }
    };

} // namespace nn
} // namespace t81
