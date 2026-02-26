#include "test_runtime_check.hpp"

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace {

std::string shell_escape(std::string_view arg) {
  if (arg.empty()) {
    return "''";
  }
  bool needs_quote = false;
  for (char c : arg) {
    if (std::isspace(static_cast<unsigned char>(c)) || c == '"' || c == '\'' || c == '\\' ||
        c == '$' || c == '&' || c == '|' || c == ';' || c == '<' || c == '>' || c == '*' ||
        c == '?' || c == '~' || c == '`' || c == '(' || c == ')' || c == '[' || c == ']' ||
        c == '{' || c == '}') {
      needs_quote = true;
      break;
    }
  }
  if (!needs_quote) {
    return std::string(arg);
  }
  std::string escaped = "'";
  for (char c : arg) {
    if (c == '\'') {
      escaped += "'\\''";
    } else {
      escaped.push_back(c);
    }
  }
  escaped.push_back('\'');
  return escaped;
}

struct CommandResult {
  int exit_code = -1;
  std::string stdout_text;
  std::string stderr_text;
};

std::string read_file(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    return {};
  }
  return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

CommandResult run_cli(const fs::path& bin_path, const std::vector<std::string>& args) {
  const fs::path temp_root = fs::temp_directory_path();
  const fs::path out_path = temp_root / "t81-cli-contract.out";
  const fs::path err_path = temp_root / "t81-cli-contract.err";
  const fs::path code_path = temp_root / "t81-cli-contract.code";

  std::string cmd = shell_escape(bin_path.string());
  for (const auto& arg : args) {
    cmd += " ";
    cmd += shell_escape(arg);
  }
  cmd += " > ";
  cmd += shell_escape(out_path.string());
  cmd += " 2> ";
  cmd += shell_escape(err_path.string());
  cmd += "; echo $? > ";
  cmd += shell_escape(code_path.string());

  const int rc = std::system(cmd.c_str());
  T81_TEST_CHECK(rc == 0);

  CommandResult result;
  result.stdout_text = read_file(out_path);
  result.stderr_text = read_file(err_path);

  std::ifstream code_in(code_path);
  T81_TEST_CHECK(static_cast<bool>(code_in));
  code_in >> result.exit_code;
  T81_TEST_CHECK(static_cast<bool>(code_in));

  std::error_code ignore_ec;
  fs::remove(out_path, ignore_ec);
  fs::remove(err_path, ignore_ec);
  fs::remove(code_path, ignore_ec);
  return result;
}

bool contains(std::string_view haystack, std::string_view needle) {
  return haystack.find(needle) != std::string_view::npos;
}

}  // namespace

int main(int argc, char* argv[]) {
  T81_TEST_CHECK(argc >= 2);
  const fs::path t81_bin = fs::path(argv[1]);
  T81_TEST_CHECK(fs::exists(t81_bin));

  {
    const auto result = run_cli(t81_bin, {"help", "compile"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Usage: t81 compile"));
  }

  {
    const auto result = run_cli(t81_bin, {"--help"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "compile <file.t81|.t81w>"));
    T81_TEST_CHECK(!contains(result.stderr_text, "llama-run <model.gguf>"));
    T81_TEST_CHECK(!contains(result.stderr_text, "weights <subcommand>"));
    T81_TEST_CHECK(contains(result.stderr_text, "t81 help advanced"));
    T81_TEST_CHECK(contains(result.stderr_text, "t81 help labs"));
  }

  {
    const auto result = run_cli(t81_bin, {"help", "advanced"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "weights <subcommand>"));
    T81_TEST_CHECK(contains(result.stderr_text, "policy <subcommand>"));
    T81_TEST_CHECK(contains(result.stderr_text, "trace <subcommand>"));
  }

  {
    const auto result = run_cli(t81_bin, {"help", "labs"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "benchmark"));
    T81_TEST_CHECK(contains(result.stderr_text, "llama-run"));
  }

  {
    const auto result = run_cli(t81_bin, {"compile", "--help"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Usage: t81 compile"));
  }

  {
    const auto result = run_cli(t81_bin, {"help", "weights", "info"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Usage: t81 weights info"));
  }

  {
    const auto result = run_cli(t81_bin, {"weights", "info", "--help"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Usage: t81 weights info"));
  }

  {
    const auto result = run_cli(t81_bin, {"help", "nope"});
    T81_TEST_CHECK(result.exit_code != 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Unknown help topic"));
  }

  {
    const auto result = run_cli(t81_bin, {"-q", "version"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stdout_text, "T81 Foundation 1.0.0-SOVEREIGN"));
  }

  {
    const auto result = run_cli(t81_bin, {"version", "-q"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stdout_text, "T81 Foundation 1.0.0-SOVEREIGN"));
  }

  {
    const auto result = run_cli(t81_bin, {"-q", "nope"});
    T81_TEST_CHECK(result.exit_code != 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Unknown command"));
  }

  {
    const auto result = run_cli(t81_bin, {"init", "bad name"});
    T81_TEST_CHECK(result.exit_code != 0);
    T81_TEST_CHECK(contains(result.stderr_text, "Project name must contain only"));
  }

  {
    const fs::path temp_dir = fs::temp_directory_path() / "t81-cli-contract";
    std::error_code ignore_ec;
    fs::remove_all(temp_dir, ignore_ec);
    fs::create_directories(temp_dir);

    const fs::path source_path = temp_dir / "hello.t81";
    const fs::path program_path = temp_dir / "hello.tisc";
    {
      std::ofstream out(source_path);
      out << "fn main() -> i32 {\n"
             "  print(\"hello-cli-contract\");\n"
             "  return 0;\n"
             "}\n";
    }

    const auto compile_result =
        run_cli(t81_bin, {"compile", source_path.string(), "-o", program_path.string()});
    T81_TEST_CHECK(compile_result.exit_code == 0);

    const auto run_result = run_cli(t81_bin, {"run", program_path.string()});
    T81_TEST_CHECK(run_result.exit_code == 0);
    T81_TEST_CHECK(contains(run_result.stdout_text, "hello-cli-contract"));
    T81_TEST_CHECK(!contains(run_result.stderr_text, "hello-cli-contract"));

    fs::remove_all(temp_dir, ignore_ec);
  }

  {
    const fs::path temp_dir = fs::temp_directory_path() / "t81-cli-contract-trace";
    std::error_code ignore_ec;
    fs::remove_all(temp_dir, ignore_ec);
    fs::create_directories(temp_dir);
    const fs::path trace_a = temp_dir / "a.trace";
    const fs::path trace_b = temp_dir / "b.trace";
    {
      std::ofstream out(trace_a);
      out << "PC=0 NOP\n";
    }
    {
      std::ofstream out(trace_b);
      out << "PC=0 HALT\n";
    }
    const auto result =
        run_cli(t81_bin, {"trace", "diff", trace_a.string(), trace_b.string(), "--no-color"});
    T81_TEST_CHECK(result.exit_code == 0);
    T81_TEST_CHECK(contains(result.stdout_text, "Difference at line 1"));
    T81_TEST_CHECK(!contains(result.stdout_text, "\x1b["));
    fs::remove_all(temp_dir, ignore_ec);
  }

  return 0;
}
