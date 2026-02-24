#include <cassert>
#include <cmath>
#include <iostream>
#include "t81/types/T81Float.hpp"

using namespace t81;

int main() {
  T81Float<72, 9> v1(1.2);
  T81Float<72, 9> v2(1.8);
  T81Float<72, 9> v3(-1.2);
  T81Float<72, 9> v4(-1.8);

  assert(v1.floor().to_double() == 1.0);
  assert(v1.ceil().to_double() == 2.0);
  assert(v1.round().to_double() == 1.0);

  assert(v2.floor().to_double() == 1.0);
  assert(v2.ceil().to_double() == 2.0);
  assert(v2.round().to_double() == 2.0);

  assert(v3.floor().to_double() == -2.0);
  assert(v3.ceil().to_double() == -1.0);
  assert(v3.round().to_double() == -1.0);

  assert(v4.floor().to_double() == -2.0);
  assert(v4.ceil().to_double() == -1.0);
  assert(v4.round().to_double() == -2.0);

  std::cout << "float rounding test passed" << std::endl;
  return 0;
}
