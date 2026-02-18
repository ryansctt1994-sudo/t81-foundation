#include "t81/cli/driver.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

namespace {

struct CerrRedirect {
  CerrRedirect() : old_buf(std::cerr.rdbuf(buffer.rdbuf())) {}
  ~CerrRedirect() { std::cerr.rdbuf(old_buf); }
  std::string str() const { return buffer.str(); }

  [[maybe_unused]] std::ostringstream buffer;
  std::streambuf* old_buf = nullptr;
};

std::string capture_diagnostics(const std::string& source,
                                const std::string& diag_label = "<diagnostic>") {
  [[maybe_unused]] CerrRedirect redirect;
  [[maybe_unused]] auto program = t81::cli::build_program_from_source(source, diag_label);
  assert(!program);
  return redirect.str();
}

void assert_contains(const std::string& output, const std::string& pattern, const char* label) {
  if (output.find(pattern) == std::string::npos) {
    std::cerr << "[" << label << "] diagnostic output missing '" << pattern << "'\n";
    std::cerr << output << '\n';
    assert(false);
  }
}

}  // namespace

int main() {
  const std::string option_source = R"(
fn main() -> i32 {
    let maybe: Option[i32] = Some(true);
    return match (maybe) {
        Some(v) => v;
        None => 0;
    };
    return 0;
}
)";

  const std::string loop_source = R"(
fn main() -> i32 {
    loop {
        break;
    }
    return 0;
}
)";

  const std::string generic_source = R"(
fn main() -> i32 {
    let missing: Option[] = Some(1);
    return 0;
}
)";

  const std::string match_source = R"(
fn main() -> i32 {
    let maybe: Option[i32] = Some(1);
    return match (maybe) {
        Some(v) => v;
    };
}
)";

  const std::string split_source = R"(
fn main() -> i32 {
    let parts: Vector[T81String] = std.text.split("alpha", "");
    let _ = parts;
    return 0;
}
)";

  const std::string join_source = R"(
fn main() -> i32 {
    let joined: T81String = std.text.join(["a", "b"], 7);
    let _ = joined;
    return 0;
}
)";

  const std::string bytes_split_source = R"(
fn main() -> i32 {
    let parts: Vector[T81Bytes] = std.bytes.split(T81Bytes("alpha"), T81Bytes(""));
    let _ = parts;
    return 0;
}
)";

  const std::string bytes_join_source = R"(
fn main() -> i32 {
    let joined: T81Bytes = std.bytes.join([T81Bytes("a"), T81Bytes("b")], 7);
    let _ = joined;
    return 0;
}
)";

  const std::string symbol_source = R"(
fn main() -> i32 {
    let sym: T81String = std.symbol.intern(7);
    let _ = sym;
    return 0;
}
)";

  const std::string symbol_eq_source = R"(
fn main() -> i32 {
    let same: bool = std.symbol.eq("a", 7);
    let _ = same;
    return 0;
}
)";

  {
    [[maybe_unused]] auto output = capture_diagnostics(option_source, "option");
    assert_contains(output, "Some(true);", "option");
    assert_contains(output, "Option payload", "option");
    assert_contains(output, "error:", "option");
    assert_contains(output, "^", "option");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(loop_source, "loop");
    assert_contains(output, "loop {", "loop");
    assert_contains(output, "Loops must be annotated with '@bounded(...)'.", "loop");
    assert_contains(output, "^", "loop");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(generic_source, "generic");
    assert_contains(output, "Option[]", "generic");
    assert_contains(output, "Generic type requires at least one parameter.", "generic");
    assert_contains(output, "^", "generic");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(match_source, "match");
    assert_contains(output, "Some(v) => v;", "match");
    assert_contains(output, "requires 'None' arm", "match");
    assert_contains(output, "^", "match");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(split_source, "split");
    assert_contains(output, "std.text.split(\"alpha\", \"\")", "split");
    assert_contains(output, "str_split separator must not be empty.", "split");
    assert_contains(output, "^", "split");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(join_source, "join");
    assert_contains(output, "std.text.join([\"a\", \"b\"], 7)", "join");
    assert_contains(output, "str_join expects a T81String separator argument.", "join");
    assert_contains(output, "^", "join");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(bytes_split_source, "bytes_split");
    assert_contains(output, "std.bytes.split(T81Bytes(\"alpha\"), T81Bytes(\"\"))", "bytes_split");
    assert_contains(output, "bytes_split separator must not be empty.", "bytes_split");
    assert_contains(output, "^", "bytes_split");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(bytes_join_source, "bytes_join");
    assert_contains(output, "std.bytes.join([T81Bytes(\"a\"), T81Bytes(\"b\")], 7)", "bytes_join");
    assert_contains(output, "bytes_join expects a T81Bytes separator argument.", "bytes_join");
    assert_contains(output, "^", "bytes_join");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(symbol_source, "symbol");
    assert_contains(output, "std.symbol.intern(7)", "symbol");
    assert_contains(output, "symbol_intern expects a T81String argument.", "symbol");
    assert_contains(output, "^", "symbol");
  }

  {
    [[maybe_unused]] auto output = capture_diagnostics(symbol_eq_source, "symbol_eq");
    assert_contains(output, "std.symbol.eq(\"a\", 7)", "symbol_eq");
    assert_contains(output, "symbol_eq expects T81String arguments.", "symbol_eq");
    assert_contains(output, "^", "symbol_eq");
  }

  return 0;
}
