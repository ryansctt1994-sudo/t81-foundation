#include "t81/cli/driver.hpp"
#include "t81/isa/encoding.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include "test_runtime_check.hpp"

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

static bool same_enum_metadata(const std::vector<tisc::EnumMetadata>& lhs,
                               const std::vector<tisc::EnumMetadata>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i].enum_id != rhs[i].enum_id || lhs[i].name != rhs[i].name ||
        lhs[i].variants.size() != rhs[i].variants.size()) {
      return false;
    }
    for (std::size_t j = 0; j < lhs[i].variants.size(); ++j) {
      if (lhs[i].variants[j].name != rhs[i].variants[j].name ||
          lhs[i].variants[j].variant_id != rhs[i].variants[j].variant_id ||
          lhs[i].variants[j].payload != rhs[i].variants[j].payload) {
        return false;
      }
    }
  }
  return true;
}

static bool sorted_by_enum_id_then_name(const std::vector<tisc::EnumMetadata>& entries) {
  return std::is_sorted(entries.begin(), entries.end(),
                        [](const tisc::EnumMetadata& lhs, const tisc::EnumMetadata& rhs) {
                          if (lhs.enum_id != rhs.enum_id) {
                            return lhs.enum_id < rhs.enum_id;
                          }
                          return lhs.name < rhs.name;
                        });
}

static void test_enum_metadata_is_deterministic_and_canonical() {
  const fs::path source_path = fixture_root() / "13_match_enum_payload_guard_print.t81";
  const std::string source = read_text(source_path);

  const auto program_a = cli::build_program_from_source(source, source_path.string());
  const auto program_b = cli::build_program_from_source(source, source_path.string());
  T81_TEST_CHECK(program_a.has_value());
  T81_TEST_CHECK(program_b.has_value());

  T81_TEST_CHECK(!program_a->enum_metadata.empty());
  T81_TEST_CHECK(!program_b->enum_metadata.empty());
  T81_TEST_CHECK(same_enum_metadata(program_a->enum_metadata, program_b->enum_metadata));
  T81_TEST_CHECK(sorted_by_enum_id_then_name(program_a->enum_metadata));

  const auto bytecode_a = tisc::encode(*program_a);
  const auto bytecode_b = tisc::encode(*program_b);
  T81_TEST_CHECK(bytecode_a == bytecode_b);
}

int main() {
  test_enum_metadata_is_deterministic_and_canonical();
  std::cout << "e2e enum metadata determinism test passed!\n";
  return 0;
}
