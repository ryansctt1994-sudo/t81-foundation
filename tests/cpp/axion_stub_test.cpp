#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "t81/axion/api.hpp"
#include "t81/axion/policy.hpp"
#include "t81/axion/engine.hpp"
#include "t81/axion/policy_engine.hpp"

int main() {
  using namespace t81::axion;

  // Version and runtime name are updated in the façade.
  [[maybe_unused]] auto v = AxionContext::runtime_version();
  assert(v.major == 1 && v.minor == 2 && v.patch == 0);
  assert(std::string(AxionContext::runtime_name()) == "Axion-Façade");

  AxionContext cx;
  cx.reset_telemetry();

  // Test capabilities
  auto caps = cx.capabilities();
  assert(!caps.empty());
  assert(std::string(caps[0].name) == "DeterministicExecution");

  // Prepare a request
  Signal sig{};
  sig.kind  = 0x42;
  sig.flags = 0xA5A5;
  sig.nonce = 0x1122334455667788ull;

  Buffer in;
  const std::string payload = "ping-axion";
  in.data.assign(payload.begin(), payload.end());

  Buffer out;
  [[maybe_unused]] auto st = cx.submit(sig, in, out);
  assert(st == Status::Ok);

  // Response must contain input + trailer ("AXN\2" + fields LE)
  assert(out.data.size() >= in.data.size() + 4 + 4 + 4);
  // Trailer magic
  [[maybe_unused]] size_t off = payload.size();
  assert(out.data[off+0] == 'A');
  assert(out.data[off+1] == 'X');
  assert(out.data[off+2] == 'N');
  assert(out.data[off+3] == 0x02); // new version

  // Simple telemetry checks
  [[maybe_unused]] const auto& tele = cx.telemetry();
  assert(tele.requests == 1);
  assert(tele.bytes_in  == payload.size());
  assert(tele.bytes_out == out.data.size());

  // Deterministic decision API test
  SyscallContext sctx{};
  sctx.instruction_count = 100;

  // Create context with a real policy engine that denies after 50 instructions
  auto policy_res = parse_policy("(policy (max-instructions 50))");
  assert(policy_res.has_value());
  AxionContext cx_denied(make_policy_engine(std::move(policy_res.value())));

  auto verdict = cx_denied.evaluate(sctx);
  assert(verdict.kind == VerdictKind::Deny);
  assert(cx_denied.telemetry().denies == 1);

  sctx.instruction_count = 10;
  auto verdict_allow = cx_denied.evaluate(sctx);
  assert(verdict_allow.kind == VerdictKind::Allow);

  std::cout << "axion_façade tests ok\n";
  return 0;
}
