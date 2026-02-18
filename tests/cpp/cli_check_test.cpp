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
  [[maybe_unused]] std::uniform_int_distribution<uint64_t> dist;
  return fs::temp_directory_path() / (prefix + "-" + std::to_string(dist(rng)) + extension);
}

void write_source(const fs::path& path, std::string_view contents) {
  std::ofstream out(path, std::ios::binary);
  if (!out) {
    throw std::runtime_error("Failed to open source file: " + path.string());
  }
  out << contents;
  out.flush();
}
}  // namespace

int main() {
  const std::string minimal_program = R"(
        fn main() -> i32 {
            return 0;
        }
    )";

  [[maybe_unused]] auto source_path = make_temp_path("t81-check", ".t81");
  write_source(source_path, minimal_program);
  assert(t81::cli::check_syntax(source_path) == 0);
  fs::remove(source_path);

  const std::string broken_program = R"(
        fn main() -> i32 {
            let bad: i8 = 1.5;
            return 0;
        }
    )";

  [[maybe_unused]] auto broken_path = make_temp_path("t81-check-fail", ".t81");
  write_source(broken_path, broken_program);

  [[maybe_unused]] std::ostringstream captured;
  auto* old_buf = std::cerr.rdbuf(captured.rdbuf());
  [[maybe_unused]] int rc = t81::cli::check_syntax(broken_path);
  std::cerr.rdbuf(old_buf);

  if (rc == 0) {
    std::cerr << "Expected `t81 check` to fail on invalid input\n";
    return 1;
  }
  [[maybe_unused]] std::string output = captured.str();
  assert(output.find(broken_path.string()) != std::string::npos);
  assert(output.find("Cannot assign initializer") != std::string::npos);

  fs::remove(broken_path);

  const std::string split_program = R"(
        fn main() -> i32 {
            let parts: Vector[T81String] = std.text.split("alpha", ",");
            let _ = parts;
            return 0;
        }
    )";

  [[maybe_unused]] auto split_path = make_temp_path("t81-check-split-fail", ".t81");
  write_source(split_path, split_program);

  [[maybe_unused]] std::ostringstream split_captured;
  old_buf = std::cerr.rdbuf(split_captured.rdbuf());
  [[maybe_unused]] int split_rc = t81::cli::check_syntax(split_path);
  std::cerr.rdbuf(old_buf);

  if (split_rc == 0) {
    std::cerr << "Expected `t81 check` to fail on split feature-gate input\n";
    return 1;
  }
  [[maybe_unused]] std::string split_output = split_captured.str();
  assert(split_output.find(split_path.string()) != std::string::npos);
  assert(split_output.find("std.text.split is not implemented yet") != std::string::npos);

  fs::remove(split_path);

  const std::string join_program = R"(
        fn main() -> i32 {
            let joined: T81String = std.text.join(["a", "b"], ",");
            let _ = joined;
            return 0;
        }
    )";

  [[maybe_unused]] auto join_path = make_temp_path("t81-check-join-fail", ".t81");
  write_source(join_path, join_program);

  [[maybe_unused]] std::ostringstream join_captured;
  old_buf = std::cerr.rdbuf(join_captured.rdbuf());
  [[maybe_unused]] int join_rc = t81::cli::check_syntax(join_path);
  std::cerr.rdbuf(old_buf);

  if (join_rc == 0) {
    std::cerr << "Expected `t81 check` to fail on join feature-gate input\n";
    return 1;
  }
  [[maybe_unused]] std::string join_output = join_captured.str();
  assert(join_output.find(join_path.string()) != std::string::npos);
  assert(join_output.find("std.text.join is not implemented yet") != std::string::npos);

  fs::remove(join_path);

  std::cout << "CliCheckTest passed!" << std::endl;
  return 0;
}
