#include "t81/cli/driver.hpp"

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

std::string read_file(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}
}  // namespace

int main() {
  auto expect = [](bool cond, const char* msg) -> bool {
    if (!cond) {
      std::cerr << "cli_trace_export_test failure: " << msg << "\n";
      return false;
    }
    return true;
  };

  const fs::path trace_path = make_temp_path("t81-trace-export", ".txt");
  const fs::path json_path = make_temp_path("t81-trace-export", ".json");
  const fs::path csv_path = make_temp_path("t81-trace-export", ".csv");

  {
    std::ofstream out(trace_path, std::ios::binary | std::ios::trunc);
    out << "PC=0 Load\n";
    out << "PC=1 Add trap=BoundsFault\n";
    out << "opaque trace line\n";
  }

  t81::cli::TraceArgs json_args;
  json_args.subcommand = "export";
  json_args.args = {trace_path.string(), "--format", "json", "--out", json_path.string()};
  int json_rc = t81::cli::run_trace(json_args);
  if (!expect(json_rc == 0, "json export returned non-zero")) return 1;

  std::string json = read_file(json_path);
  if (!expect(json.find("\"index\":1") != std::string::npos, "json missing first entry index"))
    return 1;
  if (!expect(json.find("\"pc\":0") != std::string::npos, "json missing parsed pc")) return 1;
  if (!expect(json.find("\"opcode\":\"Load\"") != std::string::npos, "json missing opcode"))
    return 1;
  if (!expect(json.find("\"trap\":\"BoundsFault\"") != std::string::npos, "json missing trap"))
    return 1;
  if (!expect(json.find("\"pc\":null") != std::string::npos, "json missing null pc for raw line"))
    return 1;
  if (!expect(json.find("\"raw\":\"opaque trace line\"") != std::string::npos,
              "json missing raw fallback"))
    return 1;

  t81::cli::TraceArgs csv_args;
  csv_args.subcommand = "export";
  csv_args.args = {trace_path.string(), "--format", "csv", "-o", csv_path.string()};
  int csv_rc = t81::cli::run_trace(csv_args);
  if (!expect(csv_rc == 0, "csv export returned non-zero")) return 1;

  std::string csv = read_file(csv_path);
  if (!expect(csv.find("index,pc,opcode,trap,raw") != std::string::npos, "csv missing header"))
    return 1;
  if (!expect(csv.find("2,1,\"Add\",\"BoundsFault\"") != std::string::npos,
              "csv missing parsed entry row"))
    return 1;
  if (!expect(csv.find("3,,\"\",\"\",\"opaque trace line\"") != std::string::npos,
              "csv missing raw fallback row"))
    return 1;

  t81::cli::TraceArgs bad_args;
  bad_args.subcommand = "export";
  bad_args.args = {trace_path.string(), "--format", "xml"};
  int bad_rc = t81::cli::run_trace(bad_args);
  if (!expect(bad_rc != 0, "invalid format unexpectedly succeeded")) return 1;

  std::error_code ec;
  fs::remove(trace_path, ec);
  fs::remove(json_path, ec);
  fs::remove(csv_path, ec);
  return 0;
}
