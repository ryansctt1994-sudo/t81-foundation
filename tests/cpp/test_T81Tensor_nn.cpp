#include <cmath>
#include <cstdlib>
#include <iostream>
#include "t81/core/T81Tensor.hpp"

using namespace t81;

// Helper to check float equality
bool approx_eq(double a, double b, double eps = 1e-3) { return std::abs(a - b) < eps; }

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

void test_layer_norm() {
  using TF = T81Float<72, 9>;
  // Input: 2x3
  // [[1, 2, 3],
  //  [4, 5, 6]]
  T81Tensor<TF, 2, 2, 3> input;
  input(0, 0) = TF(1.0);
  input(0, 1) = TF(2.0);
  input(0, 2) = TF(3.0);
  input(1, 0) = TF(4.0);
  input(1, 1) = TF(5.0);
  input(1, 2) = TF(6.0);

  // Mean for row 0: 2. Var: ((1-2)^2 + (2-2)^2 + (3-2)^2)/3 = 2/3 = 0.666...
  // Std = sqrt(0.666...) = 0.8165
  // Out[0,0] = (1-2)/0.8165 = -1.2247
  // Out[0,1] = (2-2)/0.8165 = 0
  // Out[0,2] = (3-2)/0.8165 = 1.2247

  auto out = layer_norm(input);

  double v00 = out(0, 0).to_double();
  double v01 = out(0, 1).to_double();
  double v02 = out(0, 2).to_double();

  check(approx_eq(v00, -1.2247), "layer_norm 0,0");
  check(approx_eq(v01, 0.0), "layer_norm 0,1");
  check(approx_eq(v02, 1.2247), "layer_norm 0,2");

  std::cout << "test_layer_norm passed\n";
}

void test_embedding() {
  using TF = T81Float<72, 9>;
  using TI = T81Int<81>;

  // Weights: 4x2
  // 0: [0.1, 0.2]
  // 1: [0.3, 0.4]
  // 2: [0.5, 0.6]
  // 3: [0.7, 0.8]
  T81Tensor<TF, 2, 4, 2> weights;
  weights(0, 0) = TF(0.1);
  weights(0, 1) = TF(0.2);
  weights(1, 0) = TF(0.3);
  weights(1, 1) = TF(0.4);
  weights(2, 0) = TF(0.5);
  weights(2, 1) = TF(0.6);
  weights(3, 0) = TF(0.7);
  weights(3, 1) = TF(0.8);

  // Indices: 2x2
  // [[1, 2], [3, 0]]
  T81Tensor<TI, 2, 2, 2> indices;
  indices(0, 0) = TI(1);
  indices(0, 1) = TI(2);
  indices(1, 0) = TI(3);
  indices(1, 1) = TI(0);

  auto out = embedding(indices, weights);
  // Out: 2x2x2
  // [[ [0.3, 0.4], [0.5, 0.6] ],
  //  [ [0.7, 0.8], [0.1, 0.2] ]]

  check(approx_eq(out(0, 0, 0).to_double(), 0.3), "emb 0,0,0");
  check(approx_eq(out(0, 0, 1).to_double(), 0.4), "emb 0,0,1");
  check(approx_eq(out(0, 1, 0).to_double(), 0.5), "emb 0,1,0");
  check(approx_eq(out(1, 0, 0).to_double(), 0.7), "emb 1,0,0");
  check(approx_eq(out(1, 1, 1).to_double(), 0.2), "emb 1,1,1");

  std::cout << "test_embedding passed\n";
}

int main() {
  test_layer_norm();
  test_embedding();
  return 0;
}
