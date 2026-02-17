#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "t81/bigint.hpp"

using namespace t81::v1;

void test_division_features() {
  T81BigInt a(7625597484987LL);
  T81BigInt b(2);
  T81BigInt q = a / b;
  T81BigInt r = a % b;

  assert(q.to_int64() == 3812798742493LL);
  assert(r.to_int64() == 1LL);

  T81BigInt aa = a * a;
  T81BigInt q2 = aa / a;
  assert(q2 == a);
}

void test_gcd_features() {
  T81BigInt a(12);
  T81BigInt b(18);
  T81BigInt g = T81BigInt::gcd(a, b);
  assert(g.to_int64() == 6);
}

void test_string_features() {
  T81BigInt a(81);
  std::string s = a.to_base81_string();
  assert(s == "10");
  T81BigInt b = T81BigInt::from_base81_string("10");
  assert(b == a);
}

void test_compat_api() {
  T81BigInt a = T81BigInt::from_i64(10);
  T81BigInt b = T81BigInt::from_i64(20);

  assert(T81BigInt::add(a, b).to_int64() == 30);
  assert(T81BigInt::sub(b, a).to_int64() == 10);
  assert(T81BigInt::mul(a, b).to_int64() == 200);
  assert(T81BigInt::div(b, a).to_int64() == 2);
  assert(T81BigInt::mod(b, a).to_int64() == 0);
  assert(T81BigInt::cmp(a, b) == -1);
}

int main() {
  test_division_features();
  test_gcd_features();
  test_string_features();
  test_compat_api();
  std::cout << "Robust BigInt Features Tests Passed" << std::endl;
  return 0;
}
