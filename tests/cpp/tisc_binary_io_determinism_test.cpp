#include "t81/crypto/sha3.hpp"
#include "t81/tisc/binary_io.hpp"
#include "t81/tisc/program.hpp"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;

static std::vector<uint8_t> read_u8(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  assert(in);
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

static void test_save_program_is_bit_stable_for_same_program() {
  t81::tisc::Program program;
  program.insns.push_back({t81::tisc::Opcode::LoadImm, 1, 42, 0, t81::tisc::LiteralKind::Int});
  program.insns.push_back({t81::tisc::Opcode::LoadImm, 2, 1, 0, t81::tisc::LiteralKind::Bool});
  program.insns.push_back({t81::tisc::Opcode::Print, 1, 0, 0, t81::tisc::LiteralKind::Int});
  program.insns.push_back({t81::tisc::Opcode::Print, 2, 0, 0, t81::tisc::LiteralKind::Bool});
  program.insns.push_back({t81::tisc::Opcode::Halt, 0, 0, 0, t81::tisc::LiteralKind::Int});
  program.float_pool = {1.25, -2.5};
  program.symbol_pool = {"alpha", "beta"};
  program.axion_policy_text = "(policy (tier 1))";
  program.match_metadata_text = "(match-metadata)";

  const fs::path f1 = fs::temp_directory_path() / "t81-binary-io-determinism-1.tisc";
  const fs::path f2 = fs::temp_directory_path() / "t81-binary-io-determinism-2.tisc";

  t81::tisc::save_program(program, f1.string());
  t81::tisc::save_program(program, f2.string());

  const auto b1 = read_u8(f1);
  const auto b2 = read_u8(f2);
  assert(b1 == b2);

  const auto h1 = t81::crypto::sha3_512_hex(b1);
  const auto h2 = t81::crypto::sha3_512_hex(b2);
  assert(h1 == h2);

  const auto loaded = t81::tisc::load_program(f1.string());
  assert(loaded.insns.size() == program.insns.size());
  assert(loaded.insns[1].literal_kind == t81::tisc::LiteralKind::Bool);

  std::error_code ec;
  fs::remove(f1, ec);
  fs::remove(f2, ec);
}

int main() {
  test_save_program_is_bit_stable_for_same_program();
  std::cout << "tisc binary io determinism test passed!\n";
  return 0;
}
