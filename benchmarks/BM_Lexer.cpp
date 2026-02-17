#include <benchmark/benchmark.h>
#include <t81/frontend/lexer.hpp>
#include <cctype>
#include <string>

static std::string BuildLexerSource() {
  std::string source;
  source.reserve(1000 * 50);
  for (int i = 0; i < 1000; ++i) {
    source += "let x: T81Int = 12345; // Comment\n";
    source += "fn foo() { return 42; }\n";
  }
  return source;
}

static void BM_Lexer_AllTokens_T81(benchmark::State& state) {
  // Setup: Create a large source string
  // 1000 lines of repeating patterns
  const std::string source = BuildLexerSource();
  const double work_per_iter = static_cast<double>(source.size());
  state.counters["work_per_iter"] = work_per_iter;

  // The code we are benchmarking
  for (auto _ : state) {
    t81::frontend::Lexer lexer(source);
    // This calls all_tokens which builds the vector
    auto tokens = lexer.all_tokens();
    benchmark::DoNotOptimize(tokens.size());
  }
  state.SetItemsProcessed(state.iterations() * source.size());
  state.SetLabel("work: ops/iter=" + std::to_string(source.size()));
}
BENCHMARK(BM_Lexer_AllTokens_T81);

static void BM_Lexer_AllTokens_Binary(benchmark::State& state) {
  const std::string source = BuildLexerSource();
  const double work_per_iter = static_cast<double>(source.size());
  state.counters["work_per_iter"] = work_per_iter;
  for (auto _ : state) {
    std::size_t tokens = 0;
    bool in_token = false;
    for (const unsigned char c : source) {
      const bool is_token = std::isalnum(c) || c == '_';
      if (is_token && !in_token) {
        ++tokens;
      }
      in_token = is_token;
    }
    benchmark::DoNotOptimize(tokens);
  }
  state.SetItemsProcessed(state.iterations() * source.size());
  state.SetLabel("work: ops/iter=" + std::to_string(source.size()));
}
BENCHMARK(BM_Lexer_AllTokens_Binary);
