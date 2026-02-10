#include "t81/cli/driver.hpp"
#include "t81/vm/vm.hpp"

#include <algorithm>
#include <cassert>
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

static fs::path fixture_root() {
  return fs::path(__FILE__).parent_path().parent_path() / "fixtures" / "t81lang_determinism";
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

struct RunResult {
  std::vector<std::string> printed;
  std::vector<std::string> match_reasons;
};

static RunResult run_and_collect(const tisc::Program& program) {
  auto vm = vm::make_interpreter_vm();
  vm->load_program(program);
  auto run = vm->run_to_halt();
  if (!run.has_value()) {
    throw std::runtime_error("VM execution failed in e2e_match_metadata_determinism_test");
  }

  RunResult result;
  result.printed = vm->state().printed_output;
  for (const auto& entry : vm->state().axion_log) {
    const std::string& reason = entry.verdict.reason;
    if (reason.find("match metadata:") != std::string::npos ||
        reason.find("guard-expr") != std::string::npos ||
        reason.find("variant=") != std::string::npos ||
        reason.find("payload=") != std::string::npos) {
      result.match_reasons.push_back(reason);
    }
  }
  return result;
}

static void test_guarded_match_metadata_is_deterministic() {
  const fs::path root = fixture_root();
  const std::vector<std::string> fixture_names{
      "12_match_option_guard_print",
      "13_match_enum_payload_guard_print",
  };

  for (const std::string& name : fixture_names) {
    const fs::path source_path = root / (name + ".t81");
    const fs::path output_path = root / (name + ".out");

    const std::string source = read_text(source_path);
    const std::vector<std::string> expected = split_lines(read_text(output_path));

    const auto program_a = cli::build_program_from_source(source, source_path.string());
    const auto program_b = cli::build_program_from_source(source, source_path.string());
    assert(program_a.has_value());
    assert(program_b.has_value());

    assert(program_a->match_metadata_text == program_b->match_metadata_text);
    if (program_a->match_metadata_text.find("guard-expr") == std::string::npos) {
      throw std::runtime_error("Missing guard-expr metadata for fixture: " + name);
    }

    const auto run_a = run_and_collect(*program_a);
    const auto run_b = run_and_collect(*program_b);
    assert(run_a.printed == expected);
    assert(run_b.printed == expected);
    assert(run_a.match_reasons == run_b.match_reasons);
    assert(!run_a.match_reasons.empty());
  }
}

int main() {
  test_guarded_match_metadata_is_deterministic();
  std::cout << "e2e match metadata determinism test passed!\n";
  return 0;
}
