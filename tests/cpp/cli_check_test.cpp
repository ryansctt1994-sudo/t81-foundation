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

  if (split_rc != 0) {
    std::cerr << "Expected `t81 check` to succeed on valid split input\n";
    std::cerr << split_captured.str() << "\n";
    return 1;
  }

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

  if (join_rc != 0) {
    std::cerr << "Expected `t81 check` to succeed on valid join input\n";
    std::cerr << join_captured.str() << "\n";
    return 1;
  }

  fs::remove(join_path);

  const std::string bytes_split_join_program = R"(
        fn main() -> i32 {
            let parts: Vector[T81Bytes] = std.bytes.split(T81Bytes("a,,b"), T81Bytes(","));
            let joined: T81Bytes = std.bytes.join(parts, T81Bytes(","));
            let _ = joined;
            return 0;
        }
    )";

  [[maybe_unused]] auto bytes_split_join_path =
      make_temp_path("t81-check-bytes-split-join", ".t81");
  write_source(bytes_split_join_path, bytes_split_join_program);

  [[maybe_unused]] std::ostringstream bytes_split_join_captured;
  old_buf = std::cerr.rdbuf(bytes_split_join_captured.rdbuf());
  [[maybe_unused]] int bytes_split_join_rc = t81::cli::check_syntax(bytes_split_join_path);
  std::cerr.rdbuf(old_buf);

  if (bytes_split_join_rc != 0) {
    std::cerr << "Expected `t81 check` to succeed on valid bytes split/join input\n";
    std::cerr << bytes_split_join_captured.str() << "\n";
    return 1;
  }

  fs::remove(bytes_split_join_path);

  const std::string symbol_program = R"(
        fn main() -> i32 {
            let sym: T81String = std.symbol.intern("omega");
            let rendered: T81String = std.symbol.to_string(sym);
            let same: bool = std.symbol.eq(sym, "omega");
            let diff: bool = std.symbol.ne(sym, "alpha");
            let _ = same;
            let _d = diff;
            let _r = rendered;
            return 0;
        }
    )";

  [[maybe_unused]] auto symbol_path = make_temp_path("t81-check-symbol", ".t81");
  write_source(symbol_path, symbol_program);

  [[maybe_unused]] std::ostringstream symbol_captured;
  old_buf = std::cerr.rdbuf(symbol_captured.rdbuf());
  [[maybe_unused]] int symbol_rc = t81::cli::check_syntax(symbol_path);
  std::cerr.rdbuf(old_buf);

  if (symbol_rc != 0) {
    std::cerr << "Expected `t81 check` to succeed on valid symbol input\n";
    std::cerr << symbol_captured.str() << "\n";
    return 1;
  }

  fs::remove(symbol_path);

  std::cout << "CliCheckTest passed!" << std::endl;
  return 0;
}
