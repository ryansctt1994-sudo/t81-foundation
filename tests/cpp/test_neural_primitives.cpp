#include <cmath>
#include <cstdlib>
#include <iostream>
#include "t81/core/T81NN.hpp"

using namespace t81;
using namespace t81::nn;

void check(bool cond, const char* msg) {
  if (!cond) {
    std::cerr << "Check failed: " << msg << "\n";
    std::exit(1);
  }
}

bool approx_eq(double a, double b, double eps = 1e-4) { return std::abs(a - b) < eps; }

void test_linear_layer() {
  using TF = T81Float<72, 9>;

  // Linear 2 -> 1
  // Y = XW + B
  // X: (Batch=1, In=2)
  // W: (In=2, Out=1)
  // B: (Out=1)

  Linear<2, 1> layer;

  // Set weights: W = [ [2], [3] ]
  layer.weight(0, 0) = TF(2.0);
  layer.weight(1, 0) = TF(3.0);

  // Set bias: B = [1]
  layer.bias(0) = TF(1.0);

  // Input: [ [1, 2] ]
  // Y = (1*2 + 2*3) + 1 = 2 + 6 + 1 = 9
  T81Tensor<TF, 2, 1, 2> input;
  input(0, 0) = TF(1.0);
  input(0, 1) = TF(2.0);

  auto out = layer.forward(input);

  check(approx_eq(out(0, 0).to_double(), 9.0), "Linear forward check");

  std::cout << "test_linear_layer passed\n";
}

void test_conv2d_layer() {
  using TF = T81Float<72, 9>;

  // Conv2d 1 -> 1, 2x2 Kernel
  // Input: 3x3
  // [[1, 2, 3],
  //  [4, 5, 6],
  //  [7, 8, 9]]
  // Weight: 2x2 = [[1, 0], [0, 1]] (Identity-ish)
  // Bias: 0.5

  Conv2d<1, 1, 2> layer;

  // Set weights
  layer.weight(0, 0, 0, 0) = TF(1.0);
  layer.weight(0, 0, 0, 1) = TF(0.0);
  layer.weight(0, 0, 1, 0) = TF(0.0);
  layer.weight(0, 0, 1, 1) = TF(1.0);

  layer.bias(0) = TF(0.5);

  T81Tensor<TF, 4, 1, 1, 3, 3> input;
  // Fill input
  input(0, 0, 0, 0) = TF(1);
  input(0, 0, 0, 1) = TF(2);
  input(0, 0, 0, 2) = TF(3);
  input(0, 0, 1, 0) = TF(4);
  input(0, 0, 1, 1) = TF(5);
  input(0, 0, 1, 2) = TF(6);
  input(0, 0, 2, 0) = TF(7);
  input(0, 0, 2, 1) = TF(8);
  input(0, 0, 2, 2) = TF(9);

  // Output should be 2x2
  // (0,0): 1*1 + 2*0 + 4*0 + 5*1 = 6 + 0.5 = 6.5
  // (0,1): 2*1 + 3*0 + 5*0 + 6*1 = 8 + 0.5 = 8.5
  // (1,0): 4*1 + 5*0 + 7*0 + 8*1 = 12 + 0.5 = 12.5
  // (1,1): 5*1 + 6*0 + 8*0 + 9*1 = 14 + 0.5 = 14.5

  auto out = layer.forward<1, 3, 3>(input);

  check(approx_eq(out(0, 0, 0, 0).to_double(), 6.5), "Conv 0,0");
  check(approx_eq(out(0, 0, 0, 1).to_double(), 8.5), "Conv 0,1");
  check(approx_eq(out(0, 0, 1, 0).to_double(), 12.5), "Conv 1,0");
  check(approx_eq(out(0, 0, 1, 1).to_double(), 14.5), "Conv 1,1");

  std::cout << "test_conv2d_layer passed\n";
}

void test_activation() {
  using TF = T81Float<72, 9>;
  T81Tensor<TF, 1, 3> t;
  t(0) = TF(-1.0);
  t(1) = TF(0.0);
  t(2) = TF(1.0);

  auto r = Functional::relu(t);
  check(approx_eq(r(0).to_double(), 0.0), "ReLU -1");
  check(approx_eq(r(1).to_double(), 0.0), "ReLU 0");
  check(approx_eq(r(2).to_double(), 1.0), "ReLU 1");

  std::cout << "test_activation passed\n";
}

int main() {
  test_linear_layer();
  test_conv2d_layer();
  test_activation();
  return 0;
}
