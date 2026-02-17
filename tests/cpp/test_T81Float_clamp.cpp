#include <cassert>
#include <iostream>
#include "t81/core/T81Float.hpp"

using namespace t81;

int main() {
  T81Float<72, 9> v(5.0);
  T81Float<72, 9> min(1.0);
  T81Float<72, 9> max(4.0);

  // Clamp 5 to [1, 4] -> 4
  assert(v.clamp(min, max).to_double() == 4.0);

  // Clamp 5 to [1, 6] -> 5
  assert(v.clamp(min, T81Float<72, 9>(6.0)).to_double() == 5.0);

  // Clamp 5 to [6, 10] -> 6
  assert(v.clamp(T81Float<72, 9>(6.0), T81Float<72, 9>(10.0)).to_double() == 6.0);

  std::cout << "clamp test passed" << std::endl;
  return 0;
}
