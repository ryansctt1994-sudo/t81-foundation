#include <benchmark/benchmark.h>
#include <t81/frontend/lexer.hpp>
#include <string>

static void BM_Lexer_AllTokens(benchmark::State& state) {
  // Setup: Create a large source string
  // 1000 lines of repeating patterns
  std::string source;
  source.reserve(1000 * 50);
  for (int i = 0; i < 1000; ++i) {
    source += "let x: T81Int = 12345; // Comment\n";
    source += "fn foo() { return 42; }\n";
  }

  // The code we are benchmarking
  for (auto _ : state) {
    t81::frontend::Lexer lexer(source);
    // This calls all_tokens which builds the vector
    auto tokens = lexer.all_tokens();
    benchmark::DoNotOptimize(tokens.size());
  }
}
BENCHMARK(BM_Lexer_AllTokens);
