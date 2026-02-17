#include <benchmark/benchmark.h>
#include <t81/frontend/ir_generator.hpp>
#include <t81/frontend/lexer.hpp>
#include <t81/frontend/parser.hpp>
#include <t81/frontend/semantic_analyzer.hpp>
#include <t81/tisc/binary_emitter.hpp>

#include <cstdint>
#include <string_view>
#include <string>

namespace {
constexpr std::string_view kBenchmarkSource = R"(
fn main() -> T81Int {
  let x: T81Int = 42;
  return x;
}
)";
constexpr std::size_t kSourceBytesPerIter = kBenchmarkSource.size();
constexpr std::size_t kBinaryBatchScansPerIter = 16384;
}

static void BM_T81LangCompile_T81(benchmark::State& state) {
  std::int64_t failures = 0;
  const std::size_t bytes_per_iter = kSourceBytesPerIter * kBinaryBatchScansPerIter;
  const std::string work_label = "T81Lang frontend compile; work: bytes/iter=" +
                                 std::to_string(bytes_per_iter) +
                                 ", compiles/iter=" + std::to_string(kBinaryBatchScansPerIter);
  state.counters["work_per_iter"] = static_cast<double>(bytes_per_iter);
  for (auto _ : state) {
    for (std::size_t scan = 0; scan < kBinaryBatchScansPerIter; ++scan) {
      t81::frontend::Lexer lexer(kBenchmarkSource);
      t81::frontend::Parser parser(lexer, "t81lang_benchmark");
      auto statements = parser.parse();
      if (parser.had_error()) {
        ++failures;
        continue;
      }

      t81::frontend::SemanticAnalyzer analyzer(statements, "t81lang_benchmark");
      analyzer.analyze();
      if (analyzer.had_error()) {
        ++failures;
        continue;
      }

      t81::frontend::IRGenerator ir_generator;
      ir_generator.attach_semantic_analyzer(&analyzer);
      auto ir = ir_generator.generate(statements);

      t81::tisc::BinaryEmitter emitter;
      auto program = emitter.emit(ir);
      benchmark::DoNotOptimize(program.insns.size());
    }
  }
  state.SetLabel(work_label);
  state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(bytes_per_iter));
  state.counters["Failures"] = static_cast<double>(failures);
}
BENCHMARK(BM_T81LangCompile_T81);

static void BM_T81LangCompile_Binary(benchmark::State& state) {
  const std::size_t bytes_per_iter = kSourceBytesPerIter * kBinaryBatchScansPerIter;
  const std::string work_label = "Binary source-byte scan; work: bytes/iter=" +
                                 std::to_string(bytes_per_iter) +
                                 ", scans/iter=" + std::to_string(kBinaryBatchScansPerIter);
  state.counters["work_per_iter"] = static_cast<double>(bytes_per_iter);
  const auto* data = reinterpret_cast<const unsigned char*>(kBenchmarkSource.data());
  for (auto _ : state) {
    std::uint64_t checksum = 1469598103934665603ULL;  // FNV offset basis
    for (std::size_t scan = 0; scan < kBinaryBatchScansPerIter; ++scan) {
      for (std::size_t i = 0; i < kSourceBytesPerIter; ++i) {
        checksum ^= static_cast<std::uint64_t>(data[i]);
        checksum *= 1099511628211ULL;  // FNV prime
      }
    }
    benchmark::DoNotOptimize(checksum);
  }
  state.SetLabel(work_label);
  state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(bytes_per_iter));
}
BENCHMARK(BM_T81LangCompile_Binary);
