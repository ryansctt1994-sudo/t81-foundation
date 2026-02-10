#include "t81/cli/driver.hpp"
#include "t81/crypto/sha3.hpp"
#include "t81/tisc/encoding.hpp"
#include "t81/vm/vm.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace t81;

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

static void test_compile_twice_is_byte_identical_and_runtime_identical() {
  const std::string source = R"(
    fn main() -> i32 {
      let a: T81BigInt = 12t81;
      let b: T81BigInt = 5t81;
      let c: T81BigInt = a + b;
      print(c);
      print(true);
      return 0;
    }
  )";

  const auto program_a = cli::build_program_from_source(source, "<det-pass-a>");
  const auto program_b = cli::build_program_from_source(source, "<det-pass-b>");
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
  assert(output_a.size() == 2);
  assert(output_a[0] == "17");
  assert(output_a[1] == "true");
}

int main() {
  test_compile_twice_is_byte_identical_and_runtime_identical();
  std::cout << "e2e compile determinism test passed!\n";
  return 0;
}
