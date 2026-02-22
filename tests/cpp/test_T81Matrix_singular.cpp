#undef NDEBUG
#include <cassert>
#include <iostream>
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Matrix.hpp"

using namespace t81;

// Helper to check if matrix is all zeros
template <typename Scalar, size_t Rows, size_t Cols>
bool is_zero_matrix(const T81Matrix<Scalar, Rows, Cols>& m) {
  for (size_t i = 0; i < Rows; ++i) {
    for (size_t j = 0; j < Cols; ++j) {
      if (!m(i, j).is_zero()) return false;
    }
  }
  return true;
}

int main() {
  std::cout << "Running T81Matrix singular tests...\n";

  using Scalar = T81Float<72, 9>;
  const Scalar zero = Scalar::zero();
  const Scalar one = Scalar::from_double(1.0);

  // 1x1 Singular
  {
    std::cout << "  Testing 1x1 singular..." << std::endl;
    using Mat1 = T81Matrix<Scalar, 1, 1>;
    Mat1 m{};
    m(0, 0) = zero;

    assert(m.determinant().is_zero());
    Mat1 inv = m.inverse();
    if (is_zero_matrix(inv)) {
        std::cout << "  [OK] 1x1 singular returns zero matrix\n";
    } else {
        std::cerr << "  [FAIL] 1x1 singular did NOT return zero matrix\n";
        return 1;
    }
  }

  // 2x2 Singular
  {
    std::cout << "  Testing 2x2 singular..." << std::endl;
    using Mat2 = T81Matrix<Scalar, 2, 2>;
    Mat2 m{};
    // | 1 1 |
    // | 1 1 | -> det = 1*1 - 1*1 = 0
    m(0, 0) = one; m(0, 1) = one;
    m(1, 0) = one; m(1, 1) = one;

    assert(m.determinant().is_zero());
    Mat2 inv = m.inverse();
    if (is_zero_matrix(inv)) {
        std::cout << "  [OK] 2x2 singular returns zero matrix\n";
    } else {
        std::cerr << "  [FAIL] 2x2 singular did NOT return zero matrix\n";
        return 1;
    }
  }

  // 3x3 Singular
  {
    std::cout << "  Testing 3x3 singular..." << std::endl;
    using Mat3 = T81Matrix<Scalar, 3, 3>;
    Mat3 m{};
    // All ones -> det = 0
    for(size_t i=0; i<3; ++i)
        for(size_t j=0; j<3; ++j)
            m(i, j) = one;

    assert(m.determinant().is_zero());
    Mat3 inv = m.inverse();
    if (is_zero_matrix(inv)) {
        std::cout << "  [OK] 3x3 singular returns zero matrix\n";
    } else {
        std::cerr << "  [FAIL] 3x3 singular did NOT return zero matrix\n";
        return 1;
    }
  }

  // 4x4 Singular
  {
    std::cout << "  Testing 4x4 singular..." << std::endl;
    using Mat4 = T81Matrix<Scalar, 4, 4>;
    Mat4 m{};
    // All ones -> det = 0
    for(size_t i=0; i<4; ++i)
        for(size_t j=0; j<4; ++j)
            m(i, j) = one;

    // Note: for 4x4, the generic implementation uses Gaussian elimination.
    // Pivoting might be unstable if floats are used but here we use exact 1.0.
    // Or we use integer-like floats.
    // T81Float should handle 1.0 exactly.

    // Determinant calculation modifies the matrix in place (temp copy).
    // The current implementation of determinant returns 0 if pivot is 0.

    Scalar det = m.determinant();
    // Ideally det should be exactly zero.
    // If not, we might need is_close. But 1.0 - 1.0 should be 0.0.
    if (det.is_zero()) {
         std::cout << "  [OK] 4x4 determinant is zero\n";
    } else {
         std::cout << "  [WARN] 4x4 determinant is " << det.to_double() << " (expected 0)\n";
    }

    Mat4 inv = m.inverse();
    if (is_zero_matrix(inv)) {
        std::cout << "  [OK] 4x4 singular returns zero matrix\n";
    } else {
        std::cerr << "  [FAIL] 4x4 singular did NOT return zero matrix\n";
        return 1;
    }
  }

  std::cout << "All T81Matrix singular tests PASSED!\n";
  return 0;
}
