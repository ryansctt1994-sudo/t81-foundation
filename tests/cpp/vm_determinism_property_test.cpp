#include <array>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "t81/tisc/encoding.hpp"
#include "t81/tisc/program.hpp"
#include "t81/vm/vm.hpp"

namespace {

using t81::tisc::Insn;
using t81::tisc::Opcode;
using t81::tisc::Program;

bool expect(bool cond, const std::string& msg) {
  if (!cond) {
    std::cerr << "vm_determinism_property_test failure: " << msg << "\n";
    return false;
  }
  return true;
}

struct Snapshot {
  std::array<std::int64_t, 16> regs{};
  std::array<t81::vm::ValueTag, 16> reg_tags{};
  t81::vm::Flags flags{};
  bool completed{true};
  t81::vm::Trap trap{t81::vm::Trap::None};
  std::size_t pc{0};
  bool halted{false};
  std::size_t gc_cycles{0};
  std::size_t call_depth{0};
  std::vector<std::string> printed;
  std::vector<t81::vm::TraceEntry> trace;
  std::vector<t81::vm::AxionEvent> axion_log;
};

bool run_and_capture(const Program& program, Snapshot* out) {
  if (out == nullptr) return false;
  auto vm = t81::vm::make_interpreter_vm();
  vm->load_program(program);
  auto result = vm->run_to_halt(5000);
  if (result.has_value()) {
    out->completed = true;
    out->trap = t81::vm::Trap::None;
  } else {
    out->completed = false;
    out->trap = result.error();
  }
  const auto& st = vm->state();

  for (std::size_t i = 0; i < out->regs.size(); ++i) {
    out->regs[i] = st.registers[i];
    out->reg_tags[i] = st.register_tags[i];
  }
  out->flags = st.flags;
  out->pc = st.pc;
  out->halted = st.halted;
  out->gc_cycles = st.gc_cycles;
  out->call_depth = st.call_depth;
  out->printed = st.printed_output;
  out->trace = st.trace;
  out->axion_log = st.axion_log;
  return true;
}

bool equal_snapshots(const Snapshot& a, const Snapshot& b) {
  if (a.completed != b.completed || a.trap != b.trap) return false;
  if (a.regs != b.regs) return false;
  if (a.reg_tags != b.reg_tags) return false;
  if (a.flags.zero != b.flags.zero || a.flags.negative != b.flags.negative ||
      a.flags.positive != b.flags.positive) {
    return false;
  }
  if (a.pc != b.pc || a.halted != b.halted || a.gc_cycles != b.gc_cycles ||
      a.call_depth != b.call_depth) {
    return false;
  }
  if (a.printed != b.printed) return false;
  if (a.trace.size() != b.trace.size()) return false;
  for (std::size_t i = 0; i < a.trace.size(); ++i) {
    if (a.trace[i].pc != b.trace[i].pc) return false;
    if (a.trace[i].opcode != b.trace[i].opcode) return false;
    if (a.trace[i].trap != b.trace[i].trap) return false;
  }
  if (a.axion_log.size() != b.axion_log.size()) return false;
  for (std::size_t i = 0; i < a.axion_log.size(); ++i) {
    const auto& ea = a.axion_log[i];
    const auto& eb = b.axion_log[i];
    if (ea.opcode != eb.opcode || ea.tag != eb.tag || ea.value != eb.value) return false;
    if (ea.verdict.kind != eb.verdict.kind || ea.verdict.reason != eb.verdict.reason) return false;
  }
  return true;
}

Program make_random_program(std::mt19937_64& rng, int body_len) {
  Program p;

  auto reg = [&]() -> std::int32_t { return static_cast<std::int32_t>(rng() % 8); };
  auto nz_reg = [&]() -> std::int32_t { return static_cast<std::int32_t>(1 + (rng() % 7)); };
  auto imm = [&]() -> std::int64_t {
    return static_cast<std::int64_t>(static_cast<std::int64_t>(rng() % 97) - 48);
  };

  // Seed registers deterministically for each generated program.
  for (int r = 0; r < 8; ++r) {
    p.insns.push_back({Opcode::LoadImm, r, imm(), 0});
  }
  // Ensure at least one non-zero divisor source.
  p.insns.push_back({Opcode::LoadImm, 7, static_cast<std::int64_t>((rng() % 9) + 1), 0});

  const std::vector<Opcode> ops = {
      Opcode::Add,           Opcode::Sub,        Opcode::Mul,
      Opcode::Div,           Opcode::Mod,        Opcode::Mov,
      Opcode::Inc,           Opcode::Dec,        Opcode::Neg,
      Opcode::Cmp,           Opcode::Less,       Opcode::Greater,
      Opcode::Equal,         Opcode::JumpIfZero, Opcode::JumpIfNotZero,
      Opcode::TNot,          Opcode::TAnd,       Opcode::TOr,
      Opcode::TXor,          Opcode::SetF,       Opcode::JumpIfNegative,
      Opcode::JumpIfPositive};

  for (int i = 0; i < body_len; ++i) {
    Opcode op = ops[rng() % ops.size()];
    switch (op) {
      case Opcode::Add:
      case Opcode::Sub:
      case Opcode::Mul:
      case Opcode::Less:
      case Opcode::Greater:
      case Opcode::Equal:
        p.insns.push_back({op, reg(), reg(), reg()});
        break;
      case Opcode::Div:
      case Opcode::Mod:
        // Force non-zero divisor using reg 7 (initialized to non-zero).
        p.insns.push_back({op, reg(), reg(), 7});
        break;
      case Opcode::Mov:
        p.insns.push_back({op, reg(), reg(), 0});
        break;
      case Opcode::Inc:
      case Opcode::Dec:
        p.insns.push_back({op, reg(), 0, 0});
        break;
      case Opcode::Neg:
        p.insns.push_back({op, reg(), reg(), 0});
        break;
      case Opcode::TNot:
        p.insns.push_back({op, reg(), reg(), 0});
        break;
      case Opcode::TAnd:
      case Opcode::TOr:
      case Opcode::TXor:
        p.insns.push_back({op, reg(), reg(), reg()});
        break;
      case Opcode::Cmp:
        p.insns.push_back({op, reg(), reg(), 0});
        break;
      case Opcode::SetF:
        p.insns.push_back({op, reg(), 0, 0});
        break;
      case Opcode::JumpIfZero:
      case Opcode::JumpIfNotZero: {
        // Forward-only bounded jumps to guarantee termination.
        const int current = static_cast<int>(p.insns.size());
        const int target = current + 1 + static_cast<int>(rng() % 3);
        p.insns.push_back({op, target, nz_reg(), 0});
        break;
      }
      case Opcode::JumpIfNegative:
      case Opcode::JumpIfPositive: {
        const int current = static_cast<int>(p.insns.size());
        const int target = current + 1 + static_cast<int>(rng() % 3);
        p.insns.push_back({op, target, 0, 0});
        break;
      }
      default:
        break;
    }
  }

  // Fixed deterministic extension: memory and option/result boundaries.
  constexpr std::int64_t kMemAddr = 120;
  p.insns.push_back({Opcode::Store, kMemAddr, 0, 0});
  p.insns.push_back({Opcode::Load, 8, kMemAddr, 0});
  p.insns.push_back({Opcode::Push, 8, 0, 0});
  p.insns.push_back({Opcode::Pop, 9, 0, 0});
  p.insns.push_back({Opcode::MakeOptionSome, 10, 9, 0});
  p.insns.push_back({Opcode::OptionIsSome, 11, 10, 0});
  p.insns.push_back({Opcode::OptionUnwrap, 12, 10, 0});
  p.insns.push_back({Opcode::MakeResultOk, 13, 12, 0});
  p.insns.push_back({Opcode::ResultIsOk, 14, 13, 0});
  p.insns.push_back({Opcode::ResultUnwrapOk, 15, 13, 0});

  // Force a deterministic observable output and halt.
  p.insns.push_back({Opcode::Print, 0, 0, 0});
  p.insns.push_back({Opcode::Halt, 0, 0, 0});
  return p;
}

}  // namespace

int main() {
  std::mt19937_64 rng(0xC0FFEE81ULL);

  // Property-style sweep over randomized programs.
  for (int i = 0; i < 64; ++i) {
    Program p = make_random_program(rng, 40);

    // Binary encoding must be deterministic for the same in-memory program.
    const auto enc_a = t81::tisc::encode(p);
    const auto enc_b = t81::tisc::encode(p);
    if (!expect(enc_a == enc_b, "encoding drift for identical in-memory program")) return 1;

    // VM execution must be deterministic across runs.
    Snapshot s1;
    Snapshot s2;
    if (!run_and_capture(p, &s1)) return 1;
    if (!run_and_capture(p, &s2)) return 1;
    if (!expect(equal_snapshots(s1, s2), "snapshot drift across identical runs")) return 1;
  }

  return 0;
}
