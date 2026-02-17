#include "t81/cog/tier4/tier4_loop.hpp"
#include <cmath>
#include <sstream>
#include "t81/axion/context.hpp"

namespace t81::cog::v1 {

Tier4Loop::Tier4Loop(t81::axion::Engine& engine) : engine_(engine) {
  model_.current_goal = "initialize";
  model_.confidence = 1.0f;
}

void Tier4Loop::observe(const std::string& observation) {
  model_.add_history("Observation: " + observation);
  log_reflection("observed state change");
}

ReflectionTrace Tier4Loop::reflect() {
  std::string reason;
  // Determine if refinement is needed based on history and confidence
  if (model_.confidence < 0.81f) {
    model_.current_goal = "recalibrate";
    reason = "confidence below threshold, seeking refinement";
    log_reflection(reason);
  } else {
    reason = "reflection complete: state stable";
    log_reflection(reason);
  }

  ReflectionTrace trace;
  trace.goal = model_.current_goal;
  trace.confidence = model_.confidence;
  trace.reason = reason;
  trace.history_snapshot = model_.history;
  return trace;
}

void Tier4Loop::refine() {
  if (model_.current_goal == "recalibrate") {
    model_.confidence = 1.0f;
    model_.current_goal = "execute";
    log_reflection("refined model: confidence restored");
  }
}

void Tier4Loop::consume_snapshot(const t81::vm::ReflectionSnapshot& snapshot) {
  model_.add_history("Consuming VM snapshot at PC=" + std::to_string(snapshot.pc));
  model_.add_history("CODE hash: " + std::to_string(snapshot.code_hash));

  // Heuristic: decrease confidence if we see many traps in recent trace
  int traps = 0;
  for (const auto& entry : snapshot.recent_trace) {
    if (entry.trap.has_value()) traps++;
  }
  if (traps > 0) {
    model_.confidence *= std::pow(0.9f, static_cast<float>(traps));
    log_reflection("detected " + std::to_string(traps) +
                   " traps in recent trace, confidence decreased");
  }
}

void Tier4Loop::update_self_model(const std::string& belief_key, const std::string& belief_val) {
  model_.beliefs[belief_key] = belief_val;
  log_reflection("updated self-model belief: " + belief_key);
}

void Tier4Loop::log_reflection(const std::string& reason) {
  std::ostringstream ss;
  ss << "cog:tier4:reflect: " << reason;

  // Create a dummy context for the engine to evaluate.
  // In a real VM integration, this would be part of a syscall.
  t81::axion::SyscallContext ctx;
  ctx.trace_reasons.push_back(ss.str());
  ctx.next_opcode = t81::tisc::Opcode::Nop;

  engine_.evaluate(ctx);
}

}  // namespace t81::cog::v1
