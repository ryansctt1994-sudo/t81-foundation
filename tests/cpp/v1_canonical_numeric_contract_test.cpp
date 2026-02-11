#include <iostream>

#include "t81/bigint.hpp"
#include "t81/fraction.hpp"

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "v1_canonical_numeric_contract_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  const auto a = t81::v1::CanonicalBigInt::from_i64(7);
  const auto b = t81::v1::CanonicalBigInt::from_i64(5);
  const auto c = t81::v1::CanonicalBigInt::add(a, b);
  if (!expect(c.to_string() == "12", "canonical bigint add contract mismatch")) return 1;

  const t81::v1::CanonicalFraction f1(
      t81::v1::CanonicalBigInt::from_i64(2),
      t81::v1::CanonicalBigInt::from_i64(6));
  const t81::v1::CanonicalFraction f2(
      t81::v1::CanonicalBigInt::from_i64(1),
      t81::v1::CanonicalBigInt::from_i64(3));
  if (!expect(t81::v1::CanonicalFraction::cmp(f1, f2) == 0, "canonical fraction cmp contract mismatch")) return 1;

  return 0;
}
