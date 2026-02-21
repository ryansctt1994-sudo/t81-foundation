#include <cassert>
#include <iostream>

#include "t81/axion/ethics.hpp"
#include "t81/axion/policy_engine.hpp"
#include "t81/config.hpp"
#include "t81/tisc/opcodes.hpp"

void test_recursion_limit() {
  t81::axion::SyscallContext ctx;
  ctx.caller = "test";
  ctx.syscall = "test";
  ctx.recursion_depth = T81_HARD_RECURSION_CEILING;
  ctx.next_opcode = t81::tisc::Opcode::Nop;

  // Check specific principle
  auto verdict = t81::axion::check_ethics(t81::axion::EthicsPrinciple::EntropyContainment, ctx);
  if (verdict.kind != t81::axion::VerdictKind::Deny) {
    std::cerr << "Expected Deny for recursion depth " << T81_HARD_RECURSION_CEILING << ", got "
              << (int)verdict.kind << "\n";
    std::exit(1);
  }
  std::cout << "Recursion limit check passed: " << verdict.reason << "\n";
}

void test_recursion_warning() {
  t81::axion::SyscallContext ctx;
  ctx.caller = "test";
  ctx.recursion_depth = 250;

  // Check specific principle
  auto verdict = t81::axion::check_ethics(t81::axion::EthicsPrinciple::Interpretability, ctx);
  if (verdict.kind != t81::axion::VerdictKind::Warn) {
    std::cerr << "Expected Warn for recursion depth 250, got " << (int)verdict.kind << "\n";
    std::exit(1);
  }
  std::cout << "Recursion warning check passed: " << verdict.reason << "\n";
}

void test_transparent_execution() {
  t81::axion::SyscallContext ctx;
  ctx.caller = "test";
  ctx.next_opcode = t81::tisc::Opcode::MetaWrite;
  ctx.policy = nullptr;  // Ensure no policy

  // Check Theta-9
  auto verdict = t81::axion::check_ethics(t81::axion::EthicsPrinciple::TransparentExecution, ctx);
  if (verdict.kind != t81::axion::VerdictKind::Warn) {
    std::cerr << "Expected Warn for MetaWrite without policy, got " << (int)verdict.kind << "\n";
    std::exit(1);
  }
  std::cout << "Transparent Execution check passed: " << verdict.reason << "\n";
}

void test_all_principles_coverage() {
  t81::axion::SyscallContext ctx;
  ctx.caller = "test";
  ctx.next_opcode = t81::tisc::Opcode::Nop;
  ctx.recursion_depth = 0;

  for (int i = 1; i <= 9; ++i) {
    auto p = static_cast<t81::axion::EthicsPrinciple>(i);
    auto verdict = t81::axion::check_ethics(p, ctx);
    // We expect Allow for most benign cases
    if (verdict.kind == t81::axion::VerdictKind::Deny) {
      std::cerr << "Unexpected Deny for benign context on Principle " << i << "\n";
      std::exit(1);
    }
  }
  std::cout << "All principles coverage check passed (basic allow).\n";
}

void test_policy_engine_integration() {
  auto engine = t81::axion::make_policy_engine(std::nullopt);
  t81::axion::SyscallContext ctx;
  ctx.recursion_depth = 800;

  auto verdict = engine->evaluate(ctx);
  if (verdict.kind != t81::axion::VerdictKind::Deny) {
    std::cerr << "Expected Deny from PolicyEngine for recursion depth 800, got "
              << (int)verdict.kind << "\n";
    std::exit(1);
  }
  std::cout << "PolicyEngine integration (Deny) passed: " << verdict.reason << "\n";
}

void test_policy_engine_integration_warning() {
  auto engine = t81::axion::make_policy_engine(std::nullopt);
  t81::axion::SyscallContext ctx;
  ctx.recursion_depth = 250;

  auto verdict = engine->evaluate(ctx);
  if (verdict.kind != t81::axion::VerdictKind::Warn) {
    std::cerr << "Expected Warn from PolicyEngine for recursion depth 250, got "
              << (int)verdict.kind << "\n";
    std::exit(1);
  }
  std::cout << "PolicyEngine integration (Warn) passed: " << verdict.reason << "\n";
}

int main() {
  test_recursion_limit();
  test_recursion_warning();
  test_transparent_execution();
  test_all_principles_coverage();
  test_policy_engine_integration();
  test_policy_engine_integration_warning();
  return 0;
}
