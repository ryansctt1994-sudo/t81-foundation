#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <algorithm>
#include "t81/codec/trit_packing.hpp"
#include "t81/core/Result.hpp"
#include "t81/core/T81Int.hpp"

namespace t81::experimental {

class PackedTritVector {
public:
  // Factory methods
  static Result<PackedTritVector> from_trits(const std::vector<int8_t>& trits) {
    std::vector<Trit> t_vec;
    t_vec.reserve(trits.size());
    for (auto t : trits) {
      if (t < -1 || t > 1) {
        return Result<PackedTritVector>::failure(
            T81Symbol::intern("INVALID_TRIT"),
            T81String("Input trit value must be -1, 0, or 1"),
            T81Symbol::intern("PackedTritVector"));
      }
      t_vec.push_back(static_cast<Trit>(t));
    }
    auto res = t81::codec::trit_packing::pack_pt5(t_vec);
    if (res.is_err()) {
        return Result<PackedTritVector>(res.error());
    }
    return Result<PackedTritVector>::success(PackedTritVector(res.value(), trits.size()));
  }

  static Result<PackedTritVector> from_packed(const std::vector<uint8_t>& packed, size_t trit_count) {
    auto res = t81::codec::trit_packing::unpack_pt5(packed, trit_count);
    if (res.is_err()) {
        return Result<PackedTritVector>(res.error());
    }
    return Result<PackedTritVector>::success(PackedTritVector(packed, trit_count));
  }

  // Accessors
  size_t size() const { return count_; }
  const std::vector<uint8_t>& packed_data() const { return packed_; }

  Result<std::vector<int8_t>> to_trits() const {
    auto res = t81::codec::trit_packing::unpack_pt5(packed_, count_);
    if (res.is_err()) {
        return Result<std::vector<int8_t>>(res.error());
    }
    std::vector<int8_t> out;
    out.reserve(count_);
    for (auto t : res.value()) {
      out.push_back(static_cast<int8_t>(t));
    }
    return Result<std::vector<int8_t>>::success(out);
  }

  // Scalar logic (source of truth)
  static int8_t scalar_not(int8_t t) {
    return -t;
  }

  static int8_t scalar_and(int8_t a, int8_t b) {
    return std::min(a, b);
  }

  static int8_t scalar_or(int8_t a, int8_t b) {
    return std::max(a, b);
  }

  // TXor semantics: Defined as (a - b) wrapped, per src/vm/vm.cpp implementation of Opcode::TXor.
  // Note: This operation is non-commutative (Difference), despite the name "Xor".
  static int8_t scalar_xor(int8_t a, int8_t b) {
    int result = a - b;
    if (result > 1) return -1;
    if (result < -1) return 1;
    return static_cast<int8_t>(result);
  }

  // Packed operations (unpack-operate-repack)
  Result<PackedTritVector> t_not() const {
    auto trits_res = to_trits();
    if (trits_res.is_err()) return Result<PackedTritVector>(trits_res.error());

    std::vector<int8_t> result_trits;
    result_trits.reserve(count_);
    for (auto t : trits_res.value()) {
      result_trits.push_back(scalar_not(t));
    }
    return from_trits(result_trits);
  }

  Result<PackedTritVector> t_and(const PackedTritVector& other) const {
    if (count_ != other.count_) {
       return Result<PackedTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("PackedTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<PackedTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<PackedTritVector>(rhs_res.error());

    std::vector<int8_t> result_trits;
    result_trits.reserve(count_);
    for (size_t i = 0; i < count_; ++i) {
      result_trits.push_back(scalar_and(lhs_res.value()[i], rhs_res.value()[i]));
    }
    return from_trits(result_trits);
  }

  Result<PackedTritVector> t_or(const PackedTritVector& other) const {
    if (count_ != other.count_) {
       return Result<PackedTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("PackedTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<PackedTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<PackedTritVector>(rhs_res.error());

    std::vector<int8_t> result_trits;
    result_trits.reserve(count_);
    for (size_t i = 0; i < count_; ++i) {
      result_trits.push_back(scalar_or(lhs_res.value()[i], rhs_res.value()[i]));
    }
    return from_trits(result_trits);
  }

  Result<PackedTritVector> t_xor(const PackedTritVector& other) const {
    if (count_ != other.count_) {
       return Result<PackedTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("PackedTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<PackedTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<PackedTritVector>(rhs_res.error());

    std::vector<int8_t> result_trits;
    result_trits.reserve(count_);
    for (size_t i = 0; i < count_; ++i) {
      result_trits.push_back(scalar_xor(lhs_res.value()[i], rhs_res.value()[i]));
    }
    return from_trits(result_trits);
  }

private:
  PackedTritVector(std::vector<uint8_t> packed, size_t count)
      : packed_(std::move(packed)), count_(count) {}

  std::vector<uint8_t> packed_;
  size_t count_;
};

// Phase 2A: Compute-Friendly Representation Prototype
// Uses 2-bit packing (4 trits per byte) for faster access than PT-5.
// Mapping: 0->00, 1->01, -1->11 (Invalid: 10)
class ComputeTritVector {
public:
  static Result<ComputeTritVector> from_trits(const std::vector<int8_t>& trits) {
    std::vector<uint8_t> data;
    size_t packed_len = (trits.size() + 3) / 4;
    data.reserve(packed_len);

    for (size_t i = 0; i < trits.size(); i += 4) {
      uint8_t byte = 0;
      for (size_t j = 0; j < 4; ++j) {
        if (i + j < trits.size()) {
          int8_t t = trits[i + j];
          if (t < -1 || t > 1) {
             return Result<ComputeTritVector>::failure(
                T81Symbol::intern("INVALID_TRIT"),
                T81String("Input trit value must be -1, 0, or 1"),
                T81Symbol::intern("ComputeTritVector"));
          }
          // Map: 0->00, 1->01, -1->11.
          uint8_t val = 0;
          if (t == 0) val = 0; // 00
          else if (t == 1) val = 1; // 01
          else if (t == -1) val = 3; // 11

          byte |= (val << (j * 2));
        }
      }
      data.push_back(byte);
    }
    return Result<ComputeTritVector>::success(ComputeTritVector(std::move(data), trits.size()));
  }

  static Result<ComputeTritVector> from_phase1(const PackedTritVector& other) {
    auto trits_res = other.to_trits();
    if (trits_res.is_err()) return Result<ComputeTritVector>(trits_res.error());
    return from_trits(trits_res.value());
  }

  size_t size() const { return count_; }
  const std::vector<uint8_t>& data() const { return data_; }

  Result<std::vector<int8_t>> to_trits() const {
    std::vector<int8_t> out;
    out.reserve(count_);
    for (size_t i = 0; i < count_; ++i) {
      size_t byte_idx = i / 4;
      size_t bit_idx = (i % 4) * 2;
      uint8_t byte = data_[byte_idx];
      uint8_t val = (byte >> bit_idx) & 0x03;

      int8_t t = 0;
      if (val == 0) t = 0;
      else if (val == 1) t = 1;
      else if (val == 3) t = -1;
      else {
         return Result<std::vector<int8_t>>::failure(
            T81Symbol::intern("INVALID_PACKED_DATA"),
            T81String("Encountered invalid 2-bit pattern"),
            T81Symbol::intern("ComputeTritVector"));
      }
      out.push_back(t);
    }
    return Result<std::vector<int8_t>>::success(out);
  }

  Result<ComputeTritVector> t_not() const {
    auto trits_res = to_trits();
    if (trits_res.is_err()) return Result<ComputeTritVector>(trits_res.error());
    std::vector<int8_t> trits = trits_res.value();
    for (auto& t : trits) t = PackedTritVector::scalar_not(t);
    return from_trits(trits);
  }

  Result<ComputeTritVector> t_and(const ComputeTritVector& other) const {
    if (count_ != other.count_) {
       return Result<ComputeTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("ComputeTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<ComputeTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<ComputeTritVector>(rhs_res.error());

    auto& lhs = lhs_res.value();
    auto& rhs = rhs_res.value();
    std::vector<int8_t> res;
    res.reserve(count_);
    for(size_t i=0; i<count_; ++i) {
        res.push_back(PackedTritVector::scalar_and(lhs[i], rhs[i]));
    }
    return from_trits(res);
  }

  Result<ComputeTritVector> t_or(const ComputeTritVector& other) const {
    if (count_ != other.count_) {
       return Result<ComputeTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("ComputeTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<ComputeTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<ComputeTritVector>(rhs_res.error());

    auto& lhs = lhs_res.value();
    auto& rhs = rhs_res.value();
    std::vector<int8_t> res;
    res.reserve(count_);
    for(size_t i=0; i<count_; ++i) {
        res.push_back(PackedTritVector::scalar_or(lhs[i], rhs[i]));
    }
    return from_trits(res);
  }

  Result<ComputeTritVector> t_xor(const ComputeTritVector& other) const {
    if (count_ != other.count_) {
       return Result<ComputeTritVector>::failure(
            T81Symbol::intern("LENGTH_MISMATCH"),
            T81String("Vectors must have same length for binary operation"),
            T81Symbol::intern("ComputeTritVector"));
    }
    auto lhs_res = to_trits();
    if (lhs_res.is_err()) return Result<ComputeTritVector>(lhs_res.error());
    auto rhs_res = other.to_trits();
    if (rhs_res.is_err()) return Result<ComputeTritVector>(rhs_res.error());

    auto& lhs = lhs_res.value();
    auto& rhs = rhs_res.value();
    std::vector<int8_t> res;
    res.reserve(count_);
    for(size_t i=0; i<count_; ++i) {
        res.push_back(PackedTritVector::scalar_xor(lhs[i], rhs[i]));
    }
    return from_trits(res);
  }

private:
  ComputeTritVector(std::vector<uint8_t> data, size_t count)
    : data_(std::move(data)), count_(count) {}

  std::vector<uint8_t> data_;
  size_t count_;
};

} // namespace t81::experimental
