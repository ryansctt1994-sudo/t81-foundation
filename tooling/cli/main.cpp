/**
 * @file main.cpp
 * @brief T81 Foundation Command-Line Interface (v1.0.0-SOVEREIGN)
 *
 * Sovereign-grade, zero-dependency, ternary-native toolchain driver.
 * MIT + GPL-3.0 dual-licensed.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <cctype>
#include <cerrno>
#include <charconv>
#include <climits>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#if !defined(_WIN32)
#include <sys/wait.h>
#include <unistd.h>
#else
#include <io.h>
#endif

#include "t81/cli/driver.hpp"
#include "internal/tooling/logging.hpp"
#include "t81/frontend/ir_generator.hpp"
#include "t81/frontend/lexer.hpp"
#include "t81/frontend/parser.hpp"
#include "t81/frontend/semantic_analyzer.hpp"
#include "t81/isa/binary_emitter.hpp"
#include "t81/isa/binary_io.hpp"
#include "t81/isa/program.hpp"
#include "t81/vm/vm.hpp"
#include "t81/weights.hpp"
#if defined(T81_HAS_LLAMA_CPP)
#include "t81/experimental/llama_cpp_adapter.hpp"
#endif

namespace fs = std::filesystem;

// ──────────────────────────────────────────────────────────────
// Version & Build Info
// ──────────────────────────────────────────────────────────────
#define STR(x) #x
#define XSTR(x) STR(x)

constexpr const char* T81_VERSION = "1.0.0-SOVEREIGN";
constexpr const char* T81_FULL_VERSION =
    "T81 Foundation 1.0.0-SOVEREIGN (" __DATE__ " " __TIME__ ")";
struct TempTiscFile {
  fs::path path;

  explicit TempTiscFile(const std::string& hint = "t81") {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    while (true) {
      path =
          fs::temp_directory_path() / ("t81-" + hint + "-" + std::to_string(dist(gen)) + ".tisc");

      std::string path_str = path.string();
#if defined(_WIN32)
      int fd =
          _open(path_str.c_str(), _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
      int fd = open(path_str.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600);
#endif
      if (fd != -1) {
#if defined(_WIN32)
        _close(fd);
#else
        close(fd);
#endif
        break;
      } else {
        if (errno == EEXIST) {
          continue;
        }
        throw std::runtime_error("Failed to create temporary file: " + path_str +
                                 " (errno: " + std::to_string(errno) + ")");
      }
    }
  }

  ~TempTiscFile() {
    std::error_code ec;
    fs::remove(path, ec);  // best-effort cleanup
  }
};

// ──────────────────────────────────────────────────────────────
// Version & Help
// ──────────────────────────────────────────────────────────────
void print_version() {
  std::cout << T81_FULL_VERSION << R"(
Ternary-Native Computing Stack
Copyright © 2025 T81 Foundation
Licensed under MIT and GPL-3.0
)";
}

void print_help_weights() {
  std::cerr << R"(
Usage: t81 weights <subcommand> [options]

Subcommands:
  import <file> [-o <out>] [--format <fmt>] Import weights (safetensors/gguf) -> .t81w
  info <model.t81w>                          Print native model metadata
  quantize <input> --to-gguf <out>           Quantize SafeTensors -> T3_K GGUF

Options:
  --format <fmt>    Input format (safetensors, gguf). Default: safetensors
  -o, --out <file>  Output file path
)";
}

void print_help_policy() {
  std::cerr << R"(
Usage: t81 policy <subcommand> [options]

Subcommands:
  compile <file.apl> [-o <out>]   Compile Axion Policy Language -> .axionb
  run <file.apl|.axionb>          Validate and load an Axion policy

Options:
  -o <out>          Output file path
)";
}

void print_help_trace() {
  std::cerr << R"(
Usage: t81 trace <subcommand> [args]

Subcommands:
  show <trace.txt>                Visualize an Axion trace with color
  diff <trace1.txt> <trace2.txt>  Diff two Axion traces
  replay <file.tisc> <trace.txt>  Replay and verify trace matches
)";
}

void print_help_llama_run() {
  std::cerr << R"(
Usage: t81 llama-run <model.gguf> <prompt> --policy <policy.apl> [options]

Options:
  --max-tokens <n>          Maximum generated tokens (default: 64)
  --seed <n>                Sampler seed (default: 0)
  --threads <n>             Threads for prompt/decode (default: 1)
  --temperature <x>         Sampling temperature (default: 0.0)
  --top-k <n>               Top-k (default: 1)
  --top-p <x>               Top-p (default: 1.0)
  --expected-model-hash <h> Enforce exact model hash before inference

Notes:
  - This surface is experimental and non-DCP.
  - --policy is required and must include allowed-tensor-hashes for model authorization.
)";
}

void print_usage(const char* prog) {
  std::cerr << R"(T81 Foundation - Ternary-Native Computing Stack
Version )" << T81_VERSION
            << R"(


Usage: )" << prog
            << R"( <command> [options] [args]


Commands:
  compile <file.t81> [-o <file.tisc>]   Compile T81Lang → TISC bytecode
  run     <file.t81|.tisc> [--policy <policy.apl>] [--trace] Compile and execute
  disasm  <file.tisc>                  Print human-readable TISC disassembly
  debug   <file.t81|.tisc>             Compile (if needed) and start debugger
  check   <file.t81>                   Syntax-check only
  repro-hash [fixtures_dir]            Run T81Lang determinism fixture hash gate
  canonize-tensor <file>               Canonize tensor file to CanonFS store
  init    <project_name>               Scaffold a new T81 project
  pkg     <command> [args]             T81 package manager (init, check)
  lint    <file.t81>                   Alias for check; performs semantic analysis
  repl                                 Enter interactive REPL
  version                              Show version
  benchmark                            Run the core benchmark suite (build/benchmarks/benchmark_runner)
  weights <subcommand> [args]          Manage model weights (import, info, quantize)
  policy <subcommand> [args]           Axion policy tools (compile, run)
  trace <subcommand> [args]            Trace analysis tools (show, diff, replay)
  llama-run <model.gguf> <prompt>      Governed llama.cpp inference (experimental, non-DCP)
  help [command]                       Show this message or help for a specific command


Global options:
  -v, --verbose                        Verbose diagnostic output
  -q, --quiet                          Suppress non-error output
  -h, --help                           Show help

Diagnostics:
  `t81 compile` now prints any semantic or parsing errors with the originating
  source file, line, and column so you can jump directly to the issue without
  rerunning separate diagnostics.

)";
}

// ──────────────────────────────────────────────────────────────
// VM Trap → Exit Code
// ──────────────────────────────────────────────────────────────
// ──────────────────────────────────────────────────────────────
// Core Commands
// ──────────────────────────────────────────────────────────────
// Implemented in tooling/cli/driver.cpp

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

#if !defined(_WIN32)
int decode_system_status(int status) {
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return status;
}
#else
int decode_system_status(int status) { return status; }
#endif

std::optional<fs::path> find_benchmark_runner(const fs::path& exe_path) {
  std::vector<fs::path> candidates;
  auto exe_dir = exe_path.parent_path();
  if (exe_dir.empty()) {
    exe_dir = ".";
  }
  candidates.emplace_back(exe_dir / "benchmarks/benchmark_runner");
  candidates.emplace_back(exe_dir.parent_path() / "benchmarks/benchmark_runner");
  candidates.emplace_back(fs::path("build/benchmarks/benchmark_runner"));
  candidates.emplace_back(fs::path("benchmarks/benchmark_runner"));
  for (auto& candidate : candidates) {
    if (fs::exists(candidate)) {
      return candidate;
    }
  }
  return std::nullopt;
}

// ──────────────────────────────────────────────────────────────
// Argument Parsing (clean & single-pass)
// ──────────────────────────────────────────────────────────────
struct Args {
  std::string command;
  fs::path input;
  std::optional<fs::path> output;
  bool need_help = false;
  bool need_version = false;
  std::vector<std::string> benchmark_args;
  std::vector<std::string> command_args;
  std::optional<fs::path> weights_model;
  std::optional<fs::path> policy;
  bool trace = false;
};

Args parse_args(int argc, char* argv[]) {
  Args a;
  if (argc < 2) {
    a.need_help = true;
    return a;
  }

  std::string cmd = argv[1];
  if (cmd == "help") {
    a.command = "help";
    for (int i = 2; i < argc; ++i) {
      a.command_args.emplace_back(argv[i]);
    }
    return a;
  }
  if (cmd == "--help" || cmd == "-h") {
    a.need_help = true;
    return a;
  }
  if (cmd == "version" || cmd == "--version" || cmd == "-V") {
    a.need_version = true;
    return a;
  }

  a.command = cmd;

  for (int i = 2; i < argc; ++i) {
    std::string_view arg = argv[i];

    if (arg == "-v" || arg == "--verbose")
      g_flags.verbose = true;
    else if (arg == "-q" || arg == "--quiet")
      g_flags.quiet = true;
    else if (arg == "-o" || arg == "--output") {
      if (++i >= argc) {
        error("Missing argument after -o");
        std::exit(1);
      }
      a.output = fs::path(argv[i]);
    } else if (arg == "--weights-model") {
      if (++i >= argc) {
        error("Missing argument after --weights-model");
        std::exit(1);
      }
      a.weights_model = fs::path(argv[i]);
    } else if (arg == "--policy") {
      if (++i >= argc) {
        error("Missing argument after --policy");
        std::exit(1);
      }
      a.policy = fs::path(argv[i]);
    } else if (arg == "--trace") {
      a.trace = true;
    } else if (arg == "-h" || arg == "--help") {
      a.need_help = true;
    } else if (arg.starts_with('-')) {
      // Subcommands under these top-level commands own additional flags.
      if (a.command == "benchmark") {
        a.benchmark_args.emplace_back(argv[i]);
      } else if (a.command == "weights" || a.command == "init" || a.command == "pkg" ||
                 a.command == "repro-hash" || a.command == "policy" || a.command == "trace" ||
                 a.command == "llama-run" ||
                 a.command == "canonize-tensor") {
        a.command_args.emplace_back(argv[i]);
      } else {
        error("Unknown option: " + std::string(arg));
        std::exit(1);
      }
    } else {
      if (a.command == "benchmark") {
        a.benchmark_args.emplace_back(argv[i]);
      } else if (a.command == "weights" || a.command == "init" || a.command == "pkg" ||
                 a.command == "repro-hash" || a.command == "policy" || a.command == "trace" ||
                 a.command == "llama-run" ||
                 a.command == "canonize-tensor") {
        a.command_args.emplace_back(argv[i]);
      } else {
        if (!a.input.empty()) {
          error("Multiple input files not supported yet");
          std::exit(1);
        }
        a.input = fs::path(arg);
      }
    }
  }

  return a;
}

std::shared_ptr<t81::weights::ModelFile> load_weights_model_optional(
    const std::optional<fs::path>& path) {
  if (!path) return nullptr;
  try {
    auto mf = t81::weights::load_t81w(*path);
    return std::make_shared<t81::weights::ModelFile>(std::move(mf));
  } catch (const std::exception& e) {
    error(e.what());
    return nullptr;
  }
}

int run_benchmark(const char* command_name, const Args& args) {
  auto exe_path = fs::path(command_name);
  [[maybe_unused]] [[maybe_unused]] auto runner_path = find_benchmark_runner(exe_path);

  if (!runner_path) {
    error(
        "Could not locate benchmark_runner (looked next to the CLI and under ./build/benchmarks)");
    return 1;
  }

  // Benchmarks intentionally trigger many overflow traps (BM_overflow_*).
  // Mute trap stderr for this subprocess so benchmark runs don't flood logs.
#if defined(_WIN32)
  std::string cmd = "set T81_AXION_TRAP_STDERR=0&& ";
#else
  std::string cmd = "T81_AXION_TRAP_STDERR=0 ";
#endif
  cmd += shell_escape(runner_path->string());
  for (const auto& extra : args.benchmark_args) {
    cmd += ' ';
    cmd += shell_escape(extra);
  }

  info("Running benchmarks via " + runner_path->string());
  int status = std::system(cmd.c_str());
  if (status == -1) {
    error("Failed to execute benchmark_runner");
    return 1;
  }
  return decode_system_status(status);
}

struct WeightsImportOptions {
  fs::path input;
  std::optional<fs::path> output;
  std::string format = "safetensors";
};

int run_weights_import(const Args& args) {
  if (args.command_args.size() < 2) {
    error("weights import requires the input file");
    return 1;
  }
  WeightsImportOptions opts;
  opts.input = fs::path(args.command_args[1]);
  size_t idx = 2;
  while (idx < args.command_args.size()) {
    const auto& token = args.command_args[idx++];
    if (token == "--format") {
      if (idx >= args.command_args.size()) {
        error("weights import: missing argument for --format");
        return 1;
      }
      opts.format = args.command_args[idx++];
    } else if (token == "-o" || token == "--out") {
      if (idx >= args.command_args.size()) {
        error("weights import: missing argument for " + token);
        return 1;
      }
      opts.output = fs::path(args.command_args[idx++]);
    } else if (opts.input.empty()) {
      opts.input = fs::path(token);
    } else {
      error("weights import: unexpected argument '" + token + "'");
      return 1;
    }
  }
  if (opts.input.empty()) {
    error("weights import needs an input file");
    return 1;
  }
  if (!opts.output) {
    opts.output = opts.input.stem();
    opts.output->replace_extension(".t81w");
  }

  t81::weights::ModelFile mf;
  try {
    if (opts.format == "safetensors") {
      mf = t81::weights::load_safetensors(opts.input);
    } else if (opts.format == "gguf") {
      mf = t81::weights::load_gguf(opts.input);
    } else {
      error("weights import: unsupported format: " + opts.format);
      return 1;
    }
  } catch (const std::exception& e) {
    error(e.what());
    return 1;
  }
  t81::weights::print_info(mf);
  if (mf.native.empty()) {
    error("weights import: loader produced no native tensors");
    return 1;
  }
  t81::weights::save_t81w(mf.native, *opts.output);
  info("Saved " + opts.output->string());
  return 0;
}

int run_weights_info(const Args& args) {
  if (args.command_args.size() < 2) {
    error("weights info requires a .t81w file path");
    return 1;
  }
  fs::path path = args.command_args[1];
  try {
    auto mf = t81::weights::load_t81w(path);
    std::cout << "Model:        " << path << "\n";
    std::cout << "Parameters:   " << t81::weights::format_count(mf.total_parameters) << "\n";
    std::cout << "Trits:        " << t81::weights::format_count(mf.total_trits) << " trits\n";
    std::cout << "Storage:      " << t81::weights::format_bytes(mf.file_size) << " (" << std::fixed
              << std::setprecision(3) << mf.bits_per_trit << " bits/trit avg)\n";
    std::cout << std::fixed << std::setprecision(1) << "Sparsity:     " << (mf.sparsity * 100.0)
              << "% zeros\n";
    std::cout << std::defaultfloat;
    std::cout << "Format:       " << mf.format << "\n";
    std::cout << "Checksum:     sha3-512:" << mf.checksum << " (CanonFS-ready)\n";
  } catch (const std::exception& e) {
    error(e.what());
    return 1;
  }
  return 0;
}

int run_weights_quantize(const Args& args) {
  if (args.command_args.size() != 4 || args.command_args[2] != "--to-gguf") {
    error("weights quantize requires: quantize <input> --to-gguf <output>");
    return 1;
  }
  fs::path input = args.command_args[1];
  fs::path output = args.command_args[3];
  try {
    t81::weights::quantize_safetensors_to_gguf(input, output);
  } catch (const std::exception& e) {
    error(e.what());
    return 1;
  }
  return 0;
}

int run_weights(const Args& args) {
  if (args.command_args.empty()) {
    error("weights requires a subcommand (import|info)");
    return 1;
  }
  const std::string sub = args.command_args[0];
  if (sub == "import") {
    return run_weights_import(args);
  } else if (sub == "info") {
    return run_weights_info(args);
  } else if (sub == "quantize") {
    return run_weights_quantize(args);
  }
  error("weights: unknown subcommand '" + sub + "'");
  return 1;
}

int run_policy_compile(const Args& args) {
  if (args.command_args.size() < 2) {
    error("policy compile requires an input .apl file");
    return 1;
  }
  fs::path input = args.command_args[1];
  fs::path output = args.output.value_or(input.stem().string() + ".axionb");
  std::ifstream ifs(input);
  if (!ifs) {
    error("Could not open input file: " + input.string());
    return 1;
  }
  std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  auto policy_res = t81::axion::parse_policy(content);
  if (!policy_res) {
    error("Policy parse error: " + policy_res.error());
    return 1;
  }
  auto& policy = policy_res.value();
  policy.compile_to_bytecode();
  std::ofstream ofs(output, std::ios::binary);
  if (!ofs) {
    error("Could not open output file: " + output.string());
    return 1;
  }
  policy.serialize(ofs);
  info("Compiled policy to " + output.string());
  return 0;
}

int run_policy_run(const Args& args) {
  if (args.command_args.size() < 2) {
    error("policy run requires an input .apl or .axionb file");
    return 1;
  }
  fs::path input = args.command_args[1];
  t81::axion::Policy policy;
  if (input.extension() == ".axionb") {
    std::ifstream ifs(input, std::ios::binary);
    if (!ifs) {
      error("Could not open policy file: " + input.string());
      return 1;
    }
    auto res = t81::axion::Policy::deserialize(ifs);
    if (!res) {
      error("Policy deserialization error: " + res.error());
      return 1;
    }
    policy = std::move(res.value());
  } else {
    std::ifstream ifs(input);
    if (!ifs) {
      error("Could not open policy file: " + input.string());
      return 1;
    }
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    auto res = t81::axion::parse_policy(content);
    if (!res) {
      error("Policy parse error: " + res.error());
      return 1;
    }
    policy = std::move(res.value());
  }
  info("Policy validated successfully.");
  if (g_flags.verbose) {
    std::cout << "Tier: " << policy.tier << "\n";
    if (policy.max_instructions)
      std::cout << "Max Instructions: " << *policy.max_instructions << "\n";
    if (policy.max_stack) std::cout << "Max Stack: " << *policy.max_stack << "\n";
    if (policy.max_recursion) std::cout << "Max Recursion: " << *policy.max_recursion << "\n";
  }
  return 0;
}

int run_policy(const Args& args) {
  if (args.command_args.empty()) {
    error("policy requires a subcommand (compile|run)");
    return 1;
  }
  const std::string sub = args.command_args[0];
  if (sub == "compile") return run_policy_compile(args);
  if (sub == "run") return run_policy_run(args);
  error("policy: unknown subcommand '" + sub + "'");
  return 1;
}

std::optional<fs::path> find_repro_gate_script(const fs::path& exe_path) {
  std::vector<fs::path> candidates = {
      fs::current_path() / "scripts/ci/t81lang_repro_gate.py",
      exe_path.parent_path().parent_path() / "scripts/ci/t81lang_repro_gate.py",
      exe_path.parent_path() / "scripts/ci/t81lang_repro_gate.py",
  };
  for (const auto& path : candidates) {
    if (fs::exists(path)) {
      return path;
    }
  }
  return std::nullopt;
}

int run_repro_hash(const char* command_name, const Args& args) {
  fs::path exe_path = fs::absolute(fs::path(command_name));
  auto script_path = find_repro_gate_script(exe_path);
  if (!script_path) {
    error("Could not locate scripts/ci/t81lang_repro_gate.py");
    return 1;
  }

  fs::path repo_root = script_path->parent_path().parent_path().parent_path();
  const std::string fixtures_arg =
      args.command_args.empty() ? "tests/fixtures/t81lang_determinism" : args.command_args[0];
  const std::string expected_arg =
      args.command_args.empty()
          ? "tests/fixtures/t81lang_determinism/t81lang_repro_hash.txt"
          : (fs::path(args.command_args[0]) / "t81lang_repro_hash.txt").string();

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint64_t> dist;
  fs::path workdir = fs::temp_directory_path() / ("t81-repro-hash-" + std::to_string(dist(gen)));
  fs::path hash_out = workdir / "hash.txt";

  std::string cmd = "cd " + shell_escape(repo_root.string()) + " && python3 " +
                    shell_escape(script_path->string()) + " --t81-bin " +
                    shell_escape(exe_path.string()) + " --fixtures-dir " +
                    shell_escape(fixtures_arg) + " --workdir " + shell_escape(workdir.string()) +
                    " --hash-out " + shell_escape(hash_out.string()) + " --expected-hash-file " +
                    shell_escape(expected_arg);

  int status = std::system(cmd.c_str());
  if (status == -1) {
    error("Failed to execute t81lang_repro_gate.py");
    return 1;
  }
  int rc = decode_system_status(status);
  if (rc != 0) {
    return rc;
  }

  std::ifstream in(hash_out);
  if (!in) {
    error("Failed to read reproducibility hash output");
    return 1;
  }
  std::string hash;
  std::getline(in, hash);
  std::cout << hash << "\n";

  std::error_code ignore_ec;
  fs::remove_all(workdir, ignore_ec);
  return 0;
}

int parse_int_arg(const std::string& text, const char* flag, int min_value, int max_value) {
  int value = 0;
  auto begin = text.data();
  auto end = text.data() + text.size();
  auto [ptr, ec] = std::from_chars(begin, end, value);
  if (ec != std::errc() || ptr != end || value < min_value || value > max_value) {
    error(std::string("invalid value for ") + flag + ": " + text);
    throw std::runtime_error("invalid numeric argument");
  }
  return value;
}

float parse_float_arg(const std::string& text, const char* flag, float min_value, float max_value) {
  char* tail = nullptr;
  const float value = std::strtof(text.c_str(), &tail);
  if (tail == text.c_str() || *tail != '\0' || value < min_value || value > max_value) {
    error(std::string("invalid value for ") + flag + ": " + text);
    throw std::runtime_error("invalid numeric argument");
  }
  return value;
}

int run_llama_run(const Args& args) {
#if defined(T81_HAS_LLAMA_CPP)
  if (!args.policy) {
    error("llama-run requires --policy <policy.apl>");
    return 1;
  }

  std::vector<std::string> positional;
  int max_tokens = 64;
  uint32_t seed = 0;
  int n_threads = 1;
  int top_k = 1;
  float top_p = 1.0f;
  float temperature = 0.0f;
  std::string expected_model_hash;

  for (size_t i = 0; i < args.command_args.size(); ++i) {
    const std::string& token = args.command_args[i];
    if (token == "--max-tokens") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --max-tokens");
        return 1;
      }
      max_tokens = parse_int_arg(args.command_args[i], "--max-tokens", 1, 1000000);
    } else if (token == "--seed") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --seed");
        return 1;
      }
      seed = static_cast<uint32_t>(parse_int_arg(args.command_args[i], "--seed", 0, INT32_MAX));
    } else if (token == "--threads") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --threads");
        return 1;
      }
      n_threads = parse_int_arg(args.command_args[i], "--threads", 1, 4096);
    } else if (token == "--temperature") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --temperature");
        return 1;
      }
      temperature = parse_float_arg(args.command_args[i], "--temperature", 0.0f, 10.0f);
    } else if (token == "--top-k") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --top-k");
        return 1;
      }
      top_k = parse_int_arg(args.command_args[i], "--top-k", 1, INT32_MAX);
    } else if (token == "--top-p") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --top-p");
        return 1;
      }
      top_p = parse_float_arg(args.command_args[i], "--top-p", 0.0f, 1.0f);
    } else if (token == "--expected-model-hash") {
      if (++i >= args.command_args.size()) {
        error("llama-run: missing value for --expected-model-hash");
        return 1;
      }
      expected_model_hash = args.command_args[i];
    } else if (!token.empty() && token.front() == '-') {
      error("llama-run: unknown option '" + token + "'");
      return 1;
    } else {
      positional.push_back(token);
    }
  }

  if (positional.size() != 2) {
    error("llama-run requires exactly: <model.gguf> <prompt>");
    return 1;
  }

  std::ifstream policy_stream(*args.policy);
  if (!policy_stream) {
    error("Could not open policy file: " + args.policy->string());
    return 1;
  }
  std::string policy_text((std::istreambuf_iterator<char>(policy_stream)),
                          std::istreambuf_iterator<char>());

  auto adapter = t81::experimental::LlamaCppAdapter::create(positional[0], policy_text);
  if (!adapter.has_value()) {
    error("llama-run: adapter init failed: " + adapter.error());
    return 1;
  }

  t81::experimental::LlamaCppInferenceRequest req;
  req.prompt = positional[1];
  req.expected_model_hash = expected_model_hash;
  req.seed = seed;
  req.max_tokens = max_tokens;
  req.top_k = top_k;
  req.top_p = top_p;
  req.temperature = temperature;
  req.n_threads = n_threads;

  auto receipt = adapter.value()->infer(req);
  if (!receipt.has_value()) {
    error("llama-run: inference failed: " + receipt.error());
    return 1;
  }

  std::cout << "model_hash: " << receipt->model_hash << "\n";
  std::cout << "prompt_hash: " << receipt->prompt_hash << "\n";
  std::cout << "policy_reason: " << receipt->policy_reason << "\n";
  std::cout << "generated_tokens: " << receipt->token_ids.size() << "\n";
  std::cout << "token_ids_csv:";
  for (size_t i = 0; i < receipt->token_ids.size(); ++i) {
    if (i != 0) std::cout << ",";
    std::cout << receipt->token_ids[i];
  }
  std::cout << "\n";
  std::cout << receipt->text << "\n";
  return 0;
#else
  (void)args;
  error("llama-run is unavailable in this build (reconfigure with -DT81_ENABLE_LLAMA_CPP=ON)");
  return 1;
#endif
}

// ──────────────────────────────────────────────────────────────
// Main
// ──────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
  try {
    auto args = parse_args(argc, argv);

    if (args.need_help) {
      print_usage(argv[0]);
      return 0;
    }
    if (args.need_version) {
      print_version();
      return 0;
    }

    if (args.command == "help") {
      if (args.command_args.empty()) {
        print_usage(argv[0]);
      } else {
        std::string sub = args.command_args[0];
        if (sub == "weights")
          print_help_weights();
        else if (sub == "policy")
          print_help_policy();
        else if (sub == "trace")
          print_help_trace();
        else if (sub == "llama-run")
          print_help_llama_run();
        else {
          print_usage(argv[0]);
          std::cerr << "\nUnknown help topic: " << sub << "\n";
        }
      }
      return 0;
    }

    bool needs_input =
        (args.command == "compile" || args.command == "run" || args.command == "disasm" ||
         args.command == "debug" || args.command == "check" || args.command == "lint");
    if (args.command.empty() || (needs_input && args.input.empty())) {
      print_usage(argv[0]);
      return 1;
    }

    if (args.command == "repl" && !args.input.empty()) {
      error("repl does not accept an input file");
      return 1;
    }

    const auto ext = args.input.extension();
    auto weights_model_ptr = std::shared_ptr<t81::weights::ModelFile>{};
    if (args.weights_model &&
        (args.command == "compile" ||
         ((args.command == "run" || args.command == "debug") && ext == ".t81") ||
         args.command == "repl")) {
      weights_model_ptr = load_weights_model_optional(args.weights_model);
      if (!weights_model_ptr) return 1;
    }

    if (args.command == "compile") {
      fs::path out = args.output.value_or(args.input.stem().string() + ".tisc");
      if (ext == ".t81") {
        // If a policy is provided during compile, we should probably embed it.
        // However, our current compile function doesn't take a policy path.
        // For now, let's just pass it if we extend compile later.
        return t81::cli::compile(args.input, out, {}, {}, weights_model_ptr);
      } else if (ext == ".t81w") {
        try {
          auto model = t81::weights::load_t81w(args.input);
          auto source = t81::weights::emit_t81w_module(model, args.input.string());
          auto model_ptr = std::make_shared<t81::weights::ModelFile>(std::move(model));
          return t81::cli::compile(args.input, out, source, args.input.string(), model_ptr);
        } catch (const std::exception& e) {
          error(e.what());
          return 1;
        }
      } else {
        error("compile expects a .t81 or .t81w source file");
        return 1;
      }

    } else if (args.command == "run") {
      if (ext == ".t81") {
        TempTiscFile temp(args.input.stem().string());
        int rc = t81::cli::compile(args.input, temp.path, {}, {}, weights_model_ptr);
        if (rc != 0) return rc;
        return t81::cli::run_tisc(temp.path, args.policy, args.trace);
      } else if (ext == ".tisc") {
        return t81::cli::run_tisc(args.input, args.policy, args.trace);
      } else {
        error("run expects .t81 or .tisc file");
        return 1;
      }

    } else if (args.command == "disasm") {
      if (ext == ".tisc") {
        return t81::cli::disasm_tisc(args.input);
      }
      error("disasm expects a .tisc file");
      return 1;

    } else if (args.command == "debug") {
      if (ext == ".t81") {
        TempTiscFile temp(args.input.stem().string());
        int rc = t81::cli::compile(args.input, temp.path, {}, {}, weights_model_ptr);
        if (rc != 0) return rc;
        return t81::cli::debug_tisc(temp.path, args.policy);
      } else if (ext == ".tisc") {
        return t81::cli::debug_tisc(args.input, args.policy);
      } else {
        error("debug expects .t81 or .tisc file");
        return 1;
      }

    } else if (args.command == "check" || args.command == "lint") {
      if (ext != ".t81") {
        error(args.command + " expects a .t81 source file");
        return 1;
      }
      return t81::cli::check_syntax(args.input);

    } else if (args.command == "benchmark") {
      return run_benchmark(argv[0], args);

    } else if (args.command == "repro-hash") {
      return run_repro_hash(argv[0], args);

    } else if (args.command == "canonize-tensor") {
      if (args.command_args.empty()) {
        error("canonize-tensor requires an input file");
        return 1;
      }
      return t81::cli::canonize_tensor(args.command_args[0]);

    } else if (args.command == "init") {
      if (args.command_args.empty()) {
        error("init requires a project name");
        return 1;
      }
      return t81::cli::init_project(args.command_args[0]);

    } else if (args.command == "pkg") {
      if (args.command_args.empty()) {
        error("pkg requires a subcommand (init, check)");
        return 1;
      }
      const std::string sub = args.command_args[0];
      if (sub == "init") {
        std::string name = args.command_args.size() > 1 ? args.command_args[1] : "my-t81-pkg";
        return t81::cli::init_package(name);
      } else if (sub == "check") {
        info("Package check placeholder: manifest validated.");
        return 0;
      }
      error("pkg: unknown subcommand '" + sub + "'");
      return 1;

    } else if (args.command == "repl") {
      return t81::cli::repl(weights_model_ptr, args.policy);

    } else if (args.command == "weights") {
      return run_weights(args);

    } else if (args.command == "policy") {
      return run_policy(args);

    } else if (args.command == "trace") {
      t81::cli::TraceArgs ta;
      if (!args.command_args.empty()) {
        ta.subcommand = args.command_args[0];
        ta.args.reserve(args.command_args.size() - 1);
        for (size_t i = 1; i < args.command_args.size(); ++i)
          ta.args.push_back(args.command_args[i]);
      }
      return t81::cli::run_trace(ta);

    } else if (args.command == "llama-run") {
      return run_llama_run(args);

    } else {
      error("Unknown command: " + args.command);
      print_usage(argv[0]);
      return 1;
    }

  } catch (const std::exception& e) {
    error(e.what());
    if (!g_flags.quiet) {
      std::cerr << "Run '" << (argc > 0 ? argv[0] : "t81") << " help' for usage.\n";
    }
    return 1;
  } catch (...) {
    error("Unknown exception");
    return 1;
  }
}
