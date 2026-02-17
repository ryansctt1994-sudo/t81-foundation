#include "t81/tisc/base81_view.hpp"
#include <sstream>
#include <string_view>
#include "t81/codec/trit_packing.hpp"
#include "t81/core/T81Int.hpp"

namespace t81::tisc::base81_view {

using namespace t81::codec::trit_packing;

std::string render_insn(const Insn& insn) {
  std::vector<Trit> trits;
  trits.reserve(68);

  // Opcode: 8 trits (2 digits)
  T81Int<8> op(static_cast<int64_t>(insn.opcode));
  for (int i = 0; i < 8; ++i) trits.push_back(op[i]);

  auto add_32 = [&](int32_t val) {
    T81Int<20> v(static_cast<int64_t>(val));
    for (int i = 0; i < 20; ++i) trits.push_back(v[i]);
  };

  add_32(insn.a);
  add_32(insn.b);
  add_32(insn.c);

  auto b81_res = pack_base81(trits);
  if (!b81_res) return "???";
  return b81_digits_to_string(b81_res.value());
}

std::string render(const Program& program) {
  std::string out;
  for (const auto& insn : program.insns) {
    if (!out.empty()) out += " ";
    out += render_insn(insn);
  }
  return out;
}

Result<Program> parse(std::string_view s) {
  Program program;
  std::stringstream ss;
  ss << std::string(s);
  std::string word;
  while (ss >> word) {
    auto digits_res = string_to_b81_digits(word);
    if (!digits_res) return digits_res.error();

    auto trits_res = unpack_base81(digits_res.value(), 68);
    if (!trits_res) return trits_res.error();
    const auto& trits = trits_res.value();

    Insn insn;
    T81Int<8> op;
    for (int i = 0; i < 8; ++i) op[i] = trits[i];
    insn.opcode = static_cast<Opcode>(op.to_int64());

    auto read_32 = [&](int offset) {
      T81Int<20> v;
      for (int i = 0; i < 20; ++i) v[i] = trits[offset + i];
      return static_cast<int32_t>(v.to_int64());
    };

    insn.a = read_32(8);
    insn.b = read_32(28);
    insn.c = read_32(48);

    program.insns.push_back(insn);
  }
  return program;
}

}  // namespace t81::tisc::base81_view
