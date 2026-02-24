#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "t81/axion/context.hpp"
#include "t81/axion/verdict.hpp"
#include "t81/cog/tier4/self_model.hpp"
#include "t81/vm/state.hpp"

namespace t81::cog::v1 {

/**
 * @struct ReflectionTrace
 * @brief Captures cognitive state for auditing and promotion decisions.
 */
struct ReflectionTrace {
  std::string goal;
  float confidence;
  std::string reason;
  std::vector<std::string> history_snapshot;
};

/**
 * @class Tier4Loop
 * @brief Implements the Tier 4 observe-reflect-refine cycle.
 */
class Tier4Loop {
public:
  using AxionEvaluator = std::function<t81::axion::Verdict(const t81::axion::SyscallContext&)>;

  Tier4Loop(AxionEvaluator evaluator);

  void observe(const std::string& observation);
  ReflectionTrace reflect();
  void refine();

  /**
   * @brief Consumes a reflection snapshot from the VM to update the internal model.
   */
  void consume_snapshot(const t81::vm::ReflectionSnapshot& snapshot);

  const SelfModel& get_model() const { return model_; }

  /**
   * @brief Updates the agent's self-model based on internal reflection.
   */
  void update_self_model(const std::string& belief_key, const std::string& belief_val);

private:
  AxionEvaluator evaluator_;
  SelfModel model_;

  void log_reflection(const std::string& reason);
};

}  // namespace t81::cog::v1
