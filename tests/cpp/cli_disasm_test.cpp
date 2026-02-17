#include "t81/cli/driver.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {

fs::path make_temp_path(const std::string& prefix, const std::string& extension) {
  static std::mt19937_64 rng{std::random_device{}()};
  std::uniform_int_distribution<uint64_t> dist;
  return fs::temp_directory_path() / (prefix + "-" + std::to_string(dist(rng)) + extension);
}

void write_source(const fs::path& path, std::string_view contents) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("Failed to write source file: " + path.string());
  }
  out << contents;
  out.flush();
}

}  // namespace

int main() {
  const std::string program = R"(
        fn main() -> i32 {
            return 42;
        }
    )";

  const fs::path src = make_temp_path("t81-disasm", ".t81");
  fs::path out = src;
  out.replace_extension(".tisc");
  write_source(src, program);

  if (t81::cli::compile(src, out) != 0) {
    std::cerr << "compile failed in disasm test\n";
    return 1;
  }
  assert(fs::exists(out));

  std::ostringstream capture;
  auto* old_buf = std::cout.rdbuf(capture.rdbuf());
  const int disasm_rc = t81::cli::disasm_tisc(out);
  std::cout.rdbuf(old_buf);
  if (disasm_rc != 0) {
    std::cerr << "disasm failed in disasm test\n";
    return 1;
  }

  const std::string text = capture.str();
  assert(text.find("; t81 disasm") != std::string::npos);
  assert(text.find("; instructions=") != std::string::npos);
  assert(text.find("0000: ") != std::string::npos);
  assert(text.find("a=") != std::string::npos);
  assert(text.find("b=") != std::string::npos);
  assert(text.find("c=") != std::string::npos);

  fs::remove(src);
  fs::remove(out);

  std::cout << "CliDisasmTest passed!" << std::endl;
  return 0;
}
