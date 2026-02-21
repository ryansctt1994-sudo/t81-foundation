#include <cstdint>
#include <cstddef>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <random>
#include <algorithm>
#include "t81/core/T81BigInt.hpp"

using namespace t81::v1;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < 2) return 0;

    // Split data into two parts
    size_t split = Size / 2;

    auto bytes_to_bigint = [](const uint8_t* data, size_t len) -> T81BigInt {
        if (len == 0) return T81BigInt::zero();
        T81BigInt res(0);
        T81BigInt mult(1);
        T81BigInt base(256);
        for (size_t i = 0; i < len; ++i) {
            // Little-endian construction
            res = res + mult * T81BigInt(data[i]);
            mult = mult * base;
        }
        return res;
    };

    // Fuzz inputs
    T81BigInt a = bytes_to_bigint(Data, split);
    T81BigInt b = bytes_to_bigint(Data + split, Size - split);

    try {
        // Addition
        {
          volatile T81BigInt sum = a + b;
          (void)sum;
        }

        // Subtraction
        {
          volatile T81BigInt diff = a - b;
          (void)diff;
        }

        // Multiplication
        {
          volatile T81BigInt prod = a * b;
          (void)prod;
        }

        // Division & Modulo
        if (!b.is_zero()) {
            T81BigInt quot = a / b;
            T81BigInt rem = a % b;

            // Invariant check: a = b * q + r
            T81BigInt check = quot * b + rem;
            if (check != a) {
                std::cerr << "Division invariant failed!\n";
                std::cerr << "a: " << a.to_string() << "\n";
                std::cerr << "b: " << b.to_string() << "\n";
                std::cerr << "q: " << quot.to_string() << "\n";
                std::cerr << "r: " << rem.to_string() << "\n";
                std::cerr << "check: " << check.to_string() << "\n";
                std::abort();
            }

            // Invariant check: 0 <= r < |b|
            if (rem.is_negative()) {
                 std::cerr << "Remainder negative!\n";
                 std::abort();
            }
            if (rem >= b.abs()) {
                 std::cerr << "Remainder too large!\n";
                 std::abort();
            }
        }

        // Modular Inverse (Stein)
        if (!b.is_zero() && b.abs() > T81BigInt::one()) {
             try {
                 T81BigInt inv = T81BigInt::modular_inverse_stein(a, b);
                 // If successful, check property (a * inv) % |b| == 1?
                 // modular_inverse_stein expects coprime inputs.
                 // If it returns, check:
                 T81BigInt prod = (a * inv) % b.abs();
                 // If result is negative, make positive
                 if (prod.is_negative()) prod = prod + b.abs();

                 if (!T81BigInt::is_one(prod)) {
                      std::cerr << "Modular Inverse check failed!\n";
                      std::cerr << "a=" << a.to_string() << " b=" << b.to_string() << "\n";
                      std::cerr << "inv=" << inv.to_string() << "\n";
                      std::cerr << "prod=" << prod.to_string() << "\n";
                      std::abort();
                 }
             } catch (const std::domain_error&) {
                 // GCD != 1 or other valid domain error
             }
        }

    } catch (const std::overflow_error&) {
        // Expected for large values
    } catch (const std::domain_error&) {
        // Expected for division by zero (though we check) or other domain errors
    } catch (const std::exception& e) {
        // Other exceptions?
        // std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

#ifndef T81_FUZZ_TARGET
// Standalone main for testing without libFuzzer
int main() {
    // Generate some random data and feed it to the fuzzer function
    std::mt19937 rng(12345);
    std::uniform_int_distribution<size_t> len_dist(2, 64); // Keep smallish for quick run
    std::uniform_int_distribution<uint16_t> byte_dist(0, 255);

    int iterations = 1000;
    std::cout << "Running standalone fuzz (" << iterations << " iterations)...\n";

    for (int i = 0; i < iterations; ++i) {
        size_t len = len_dist(rng);
        std::vector<uint8_t> data(len);
        for (size_t j = 0; j < len; ++j) {
            data[j] = static_cast<uint8_t>(byte_dist(rng));
        }
        LLVMFuzzerTestOneInput(data.data(), data.size());
    }
    std::cout << "Standalone fuzz run completed.\n";
    return 0;
}
#endif
