#pragma once

#include <functional>
#include <vector>

namespace t81::cog::v3 {

struct ContractionProof {
  bool verified;
  double initial_entropy;
  double final_entropy;

  bool is_valid() const;
};

struct Recursor {
  int max_depth{81};
  int current_depth{0};
  std::vector<ContractionProof> proofs;

  bool verify_depth_proof(const ContractionProof& proof) const;
  bool can_recurse() const;
  bool push_frame(const ContractionProof& proof);
  bool contract_top(double next_entropy);
  void pop_frame();
  bool has_active_frame() const;
  double current_entropy() const;

  struct EvalResult {
    bool converged{false};
    int steps{0};
    double final_entropy{0.0};
  };
  EvalResult evaluate_loop(double start_entropy, int max_steps, double epsilon,
                           const std::function<double(double, int)>& step_fn);
};

}  // namespace t81::cog::v3
