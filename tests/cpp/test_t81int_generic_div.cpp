#include "t81/core/T81Int.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>

int main() {
    using BigT = t81::T81Int<50>;
    BigT a;
    // Set a large value: 3^45
    a[45] = t81::Trit::P;

    BigT b;
    b[0] = t81::Trit::P; // 1

    std::cout << "Attempting division of large T81Int..." << std::endl;
    try {
        BigT c = a / b;
        // Verify result is a
        if (c != a) {
            std::cout << "Result incorrect for a / 1" << std::endl;
            return 1;
        }
        std::cout << "Division by 1 succeeded." << std::endl;

        // Try division by 3 (shift right 1 trit)
        BigT three;
        three[1] = t81::Trit::P; // 3^1 = 3
        BigT d = a / three;

        // Expected result: 3^44
        BigT expected;
        expected[44] = t81::Trit::P;

        if (d == expected) {
            std::cout << "Division by 3 succeeded." << std::endl;
        } else {
            std::cout << "Division by 3 incorrect." << std::endl;
            // Print raw data to debug? Can't assume to_int64 works.
            return 1;
        }

        // Test with negative numbers
        BigT neg_three = -three;
        BigT e = a / neg_three;
        if (e == -expected) {
            std::cout << "Division by -3 succeeded." << std::endl;
        } else {
            std::cout << "Division by -3 incorrect." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cout << "Caught unexpected exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
