#include "t81/cli/driver.hpp"
#include "t81/crypto/sha3.hpp"
#include "t81/tisc/encoding.hpp"
#include "t81/vm/vm.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using namespace t81;
namespace fs = std::filesystem;

static std::vector<uint8_t> bytes_to_u8(const std::vector<std::byte>& bytes) {
  std::vector<uint8_t> out;
  out.reserve(bytes.size());
  for (std::byte b : bytes) {
    out.push_back(static_cast<uint8_t>(b));
  }
  return out;
}

static std::vector<std::string> run_and_capture_prints(const tisc::Program& program) {
  auto vm = vm::make_interpreter_vm();
  vm->load_program(program);
  auto run = vm->run_to_halt();
  if (!run.has_value()) {
    throw std::runtime_error("VM execution failed in e2e_compile_determinism_test");
  }
  return vm->state().printed_output;
}

static std::string read_text(const fs::path& path) {
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("Failed to open fixture file: " + path.string());
  }
  return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

static std::vector<std::string> split_lines(const std::string& text) {
  std::vector<std::string> lines;
  std::string current;
  for (char ch : text) {
    if (ch == '\r') {
      continue;
    }
    if (ch == '\n') {
      lines.push_back(current);
      current.clear();
      continue;
    }
    current.push_back(ch);
  }
  if (!current.empty()) {
    lines.push_back(current);
  }
  return lines;
}

static fs::path fixture_root() {
  const fs::path this_file = fs::path(__FILE__);
  return this_file.parent_path().parent_path() / "fixtures" / "t81lang_determinism";
}

static void test_compile_twice_golden_fixture_pack() {
  const fs::path root = fixture_root();
  if (!fs::exists(root)) {
    throw std::runtime_error("Missing fixture root: " + root.string());
  }

  std::vector<fs::path> fixtures;
  for (const auto& entry : fs::directory_iterator(root)) {
    if (entry.is_regular_file() && entry.path().extension() == ".t81") {
      fixtures.push_back(entry.path());
    }
  }
  std::sort(fixtures.begin(), fixtures.end());
  assert(fixtures.size() >= 5);

  for (const auto& fixture : fixtures) {
    const fs::path expected_path = fixture.parent_path() / (fixture.stem().string() + ".out");
    const std::string source = read_text(fixture);
    const std::vector<std::string> expected_output = split_lines(read_text(expected_path));

    const auto program_a = cli::build_program_from_source(source, fixture.string());
    const auto program_b = cli::build_program_from_source(source, fixture.string());
    assert(program_a.has_value());
    assert(program_b.has_value());

    const auto bytecode_a = tisc::encode(*program_a);
    const auto bytecode_b = tisc::encode(*program_b);
    assert(bytecode_a == bytecode_b);

    const auto hash_a = crypto::sha3_512_hex(bytes_to_u8(bytecode_a));
    const auto hash_b = crypto::sha3_512_hex(bytes_to_u8(bytecode_b));
    assert(hash_a == hash_b);

    const auto output_a = run_and_capture_prints(*program_a);
    const auto output_b = run_and_capture_prints(*program_b);
    assert(output_a == output_b);
    assert(output_a == expected_output);
  }
}

int main() {
  test_compile_twice_golden_fixture_pack();
  std::cout << "e2e compile determinism test passed!\n";
  return 0;
}
