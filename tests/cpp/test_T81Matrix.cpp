// tests/cpp/test_T81Matrix.cpp
// Matrix tests that are robust to T81Float<72,9> internal representation.

#undef NDEBUG
#include <cassert>
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Matrix.hpp"

#include <iostream>
#include <cmath>

using namespace t81;

template <typename Scalar>
bool is_close(const Scalar& a, const Scalar& b, double tol = 1e-9) {
    return std::abs(a.to_double() - b.to_double()) < tol;
}

#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_TRUE(cond) assert(cond)

int main() {
  std::cout << "Running T81Matrix tests...\n";

  using Scalar = T81Float<72, 9>;
  using Mat = T81Matrix<Scalar, 3, 3>;

  // Pre-constructed scalars so equality is purely "same ternary value"
  const Scalar one = Scalar::from_double(1.0);
  const Scalar two = Scalar::from_double(2.0);
  const Scalar three = Scalar::from_double(3.0);
  const Scalar four = Scalar::from_double(4.0);
  const Scalar five = Scalar::from_double(5.0);
  const Scalar six = Scalar::from_double(6.0);
  const Scalar seven = Scalar::from_double(7.0);
  const Scalar eight = Scalar::from_double(8.0);
  const Scalar nine = Scalar::from_double(9.0);

  // 1) Default construction → all zero
  {
    Mat zero{};
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j) ASSERT_TRUE(zero(i, j).is_zero());
    std::cout << "  [OK] Default construction (zero matrix)\n";
  }

  // 2) Manual fill
  Mat m{};
  m(0, 0) = one;
  m(0, 1) = two;
  m(0, 2) = three;
  m(1, 0) = four;
  m(1, 1) = five;
  m(1, 2) = six;
  m(2, 0) = seven;
  m(2, 1) = eight;
  m(2, 2) = nine;

  // Check that the assigned values are exactly what we stored
  ASSERT_EQ(m(0, 0), one);
  ASSERT_EQ(m(0, 1), two);
  ASSERT_EQ(m(0, 2), three);
  ASSERT_EQ(m(1, 0), four);
  ASSERT_EQ(m(1, 1), five);
  ASSERT_EQ(m(1, 2), six);
  ASSERT_EQ(m(2, 0), seven);
  ASSERT_EQ(m(2, 1), eight);
  ASSERT_EQ(m(2, 2), nine);
  std::cout << "  [OK] Manual fill and element access\n";

  // 3) Transpose – purely structural check
  [[maybe_unused]] Mat mt = m.transpose();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) ASSERT_EQ(mt(i, j), m(j, i));
  std::cout << "  [OK] Transpose\n";

  // 4) Addition and subtraction – check with algebraic identities
  [[maybe_unused]] Mat m2 = m;
  [[maybe_unused]] Mat sum = m + m2;
  [[maybe_unused]] Mat diff = m - m2;

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      // sum should be element-wise double of m
      [[maybe_unused]] Scalar expected_sum = m(i, j) + m2(i, j);
      ASSERT_EQ(sum(i, j), expected_sum);

      // diff should be zero everywhere
      ASSERT_TRUE(diff(i, j).is_zero());
    }
  }
  std::cout << "  [OK] Addition and subtraction\n";

  // 5) Copy construction and assignment
  {
    Mat copy1 = m;  // copy constructor
    Mat copy2;
    copy2 = m;  // copy assignment

    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j) {
        ASSERT_EQ(copy1(i, j), m(i, j));
        ASSERT_EQ(copy2(i, j), m(i, j));
      }
    std::cout << "  [OK] Copy construction and assignment\n";
  }

  // 6) Determinant and Inverse (3x3)
  {
    // Identity
    Mat I = t81::identity<Scalar, 3>();
    ASSERT_EQ(I.determinant(), one);
    Mat I_inv = I.inverse();
    ASSERT_EQ(I_inv, I);

    // Simple diagonal
    Mat D{};
    D(0,0) = two; D(1,1) = two; D(2,2) = two;
    ASSERT_EQ(D.determinant(), Scalar::from_double(8.0));
    Mat D_inv = D.inverse();
    ASSERT_EQ(D_inv(0,0), Scalar::from_double(0.5));

    std::cout << "  [OK] 3x3 Determinant and Inverse (simple)\n";
  }

  // 7) 4x4 Determinant and Inverse (Generic Implementation)
  {
      using Mat4 = T81Matrix<Scalar, 4, 4>;
      Mat4 I = t81::identity<Scalar, 4>();
      ASSERT_EQ(I.determinant(), one);
      Mat4 I_inv = I.inverse();
      ASSERT_EQ(I_inv, I);

      // Permutation matrix (det = -1)
      // 0 1 0 0
      // 0 0 1 0
      // 0 0 0 1
      // 1 0 0 0
      Mat4 P{};
      P(0,1) = one; P(1,2) = one; P(2,3) = one; P(3,0) = one;

      Scalar det = P.determinant();
      Scalar neg_one = Scalar::from_double(-1.0);
      ASSERT_EQ(det, neg_one);

      Mat4 P_inv = P.inverse();
      Mat4 P_T = P.transpose();
      ASSERT_EQ(P_inv, P_T);

      std::cout << "  [OK] 4x4 Determinant and Inverse (permutation)\n";
  }

  // 8) 5x5 Determinant and Inverse (Larger Generic)
  {
      using Mat5 = T81Matrix<Scalar, 5, 5>;
      Mat5 D{};
      // Diagonal 1, 2, 1, 2, 1
      D(0,0) = one;
      D(1,1) = two;
      D(2,2) = one;
      D(3,3) = two;
      D(4,4) = one;

      ASSERT_EQ(D.determinant(), four); // 1*2*1*2*1 = 4

      Mat5 D_inv = D.inverse();
      ASSERT_EQ(D_inv(1,1), Scalar::from_double(0.5));
      ASSERT_EQ(D_inv(3,3), Scalar::from_double(0.5));
      ASSERT_EQ(D_inv(0,0), one);

      std::cout << "  [OK] 5x5 Determinant and Inverse (diagonal)\n";
  }

  std::cout << "All T81Matrix tests PASSED!\n";
  return 0;
}
