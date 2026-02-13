#include "t81/core/T81BigInt.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

class BigIntAllocationPathologyTest {
public:
    static void test_to_std_chunks_capacity() {
        using namespace t81::v1;

        // Trigger push_back in to_std_chunks
        // We need a negative value that causes a borrow from next, and if next is not there, it pushes back 0 then borrows from it?
        // Wait, if next is not there:
        // if (i + 1 >= chunks.size()) chunks.push_back(0);
        // So valid balanced ternary like { -1 } becomes { B-1, -1 }.
        // Then loop continues to i=1. chunks[1] is -1.
        // chunks[1] += B, push_back 0 (now size 3), chunks[2] -= 1.
        // This looks like it could loop if not careful?
        // But the code has a limit check.

        // Let's create a vector that will trigger push_back.
        // A single -1 at the end of valid chunks?
        // Actually to_std_chunks expects valid balanced chunks (usually from to_chunks).
        // T81BigInt internal representation uses balanced ternary, so it can have negative trits.
        // A value like -1 (int64) is represented as -1 in chunks.
        // to_std_chunks({-1}) -> {-1}.
        // Loop i=0: chunks[0] < 0 (-1).
        // chunks[0] += B.
        // push_back(0). chunks size 2.
        // chunks[1] -= 1. chunks[1] becomes -1.
        // Loop i=1: chunks[1] < 0 (-1).
        // ... infinite loop?
        // The limit check `if (chunks.size() > limit)` breaks it.
        // Ah, so passing negative number to `to_std_chunks` (which expects magnitude?)
        // `to_chunks` calls `to_std_chunks` on `abs()`.
        // So inputs to `to_std_chunks` are always effectively positive.
        // So {-1} is not a valid input for `to_std_chunks` in the context of `T81BigInt` logic, unless it resolves to positive.
        // e.g. {-1, 1} = -1 + B. Positive.
        // i=0: chunks[0]=-1 < 0. chunks[0]+=B. chunks[1]-=1.
        // chunks is {B-1, 0}.
        // i=1: chunks[1]=0. OK.
        // No push_back.

        // To trigger push_back, we need something like {B+1}.
        // i=0: chunks[0] >= B. chunks[0] -= B. push_back(0). chunks[1] += 1.
        // chunks is {1, 1}.
        // No further push_back.

        // What if we have {B-1, B-1}?
        // i=0: ok.
        // i=1: ok.

        // Maybe {B} ?
        // i=0: chunks[0] = B. chunks[0]=0. push_back 0. chunks[1]=1.
        // chunks {0, 1}.
        // One push_back.

        // So {B} triggers one push_back.
        // Let's use that.

        int64_t B = 7625597484987LL;
        std::vector<int64_t> input_chunks(10000, B);
        // This is a huge number.
        // Each element will trigger a carry to the next.
        // chunks[0] -> 0, chunks[1]++
        // chunks[1] became B+1.
        // chunks[1] -> 1, chunks[2]++
        // ...
        // Finally last chunk. chunks[9999] becomes B+1.
        // chunks[9999] -> 1, chunks[10000]++ (push_back).
        // chunks[10000] is 1.
        // So exactly 1 push_back at the end.

        // Because of the ripple carry, we might not get many push_backs.
        // But if we passed {B, B, B...}
        // i=0: c[0]=B -> c[0]=0, c[1]++ (was B, now B+1).
        // i=1: c[1]=B+1 -> c[1]=1, c[2]++ (was B, now B+1).
        // ...
        // Only the last one extends the vector.

        // So we get 1 push_back.
        // If vector capacity was exactly 10000, 1 push_back triggers reallocation of 10000 elements.
        // If we reserve +4, no reallocation.

        auto chunks = T81BigInt::to_std_chunks(input_chunks);

        std::cout << "to_std_chunks(10000 * B): size=" << chunks.size() << ", capacity=" << chunks.capacity() << "\n";

        // If reserve is working, capacity should be roughly size + 3 (since we reserved +4 and used 1).
        // Or at least, capacity shouldn't be 20000.
        // Wait, std::vector growth factor is implementation defined, often 2.
        // If we don't reserve, pushing back on full vector doubles it.
        // So capacity will be ~20000.
        // If we fix it, capacity will be ~10004.

        if (chunks.capacity() > chunks.size() + 100) {
            std::cerr << "PATHOLOGY DETECTED: Capacity exploded (" << chunks.capacity() << " vs " << chunks.size() << ")\n";
            std::abort();
        } else {
             std::cout << "Optimized capacity detected.\n";
        }
    }

    static void test_div_mod_std_normalization() {
        using namespace t81::v1;

        // Test normalization logic in div_mod_std
        // We can pass specific chunk vectors that trigger normalization.

        // Case 1: Simple division
        std::vector<int64_t> u = {100};
        std::vector<int64_t> v = {10};
        auto res = T81BigInt::div_mod_std(u, v);

        if (res.first.size() != 1 || res.first[0] != 10) {
            std::cerr << "Simple division check failed (q)\n";
            std::abort();
        }
        if (res.second.size() != 1 || res.second[0] != 0) {
            std::cerr << "Simple division check failed (r)\n";
            std::abort();
        }

        // Case 2: Division requiring normalization of divisor
        // B is 3^27 = 7625597484987
        int64_t B = 7625597484987LL;

        // u = B, v = 2
        // d = B / (2 + 1) = B / 3
        // v becomes 2 * (B/3) approx 2/3 B.
        // u becomes B * (B/3) approx B^2 / 3.
        // This exercises the scaling logic.

        std::vector<int64_t> u2 = {0, 1}; // Represents B (1 * B + 0)
        std::vector<int64_t> v2 = {2};

        res = T81BigInt::div_mod_std(u2, v2);
        // q should be B / 2
        // B is odd, so q = (B-1)/2, r = 1
        int64_t expected_q = B / 2;
        int64_t expected_r = B % 2;

        if (res.first.size() != 1 || res.first[0] != expected_q) {
            std::cerr << "Normalization check failed (q)\n";
            std::abort();
        }
        if (res.second.size() != 1 || res.second[0] != expected_r) {
            std::cerr << "Normalization check failed (r)\n";
            std::abort();
        }

        std::cout << "div_mod_std normalization test passed.\n";
    }

    static void run_all() {
        test_to_std_chunks_capacity();
        test_div_mod_std_normalization();
    }
};

int main() {
    BigIntAllocationPathologyTest::run_all();
    return 0;
}
