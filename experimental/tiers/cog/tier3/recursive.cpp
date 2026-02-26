#include "t81/experimental/cog/tier3/recursive.hpp"
#include <cmath>

namespace t81::cog::v3 {

bool ContractionProof::is_valid() const { return verified && (final_entropy <= initial_entropy); }

bool Recursor::can_recurse() const { return current_depth < max_depth; }

bool Recursor::verify_depth_proof(const ContractionProof& proof) const {
  if (!proof.is_valid()) {
    return false;
  }
  if (current_depth >= max_depth) {
    return false;
  }
  if (!proofs.empty()) {
    // Child recursion must start from a state no "hotter" than parent's settled entropy.
    if (proof.initial_entropy > proofs.back().final_entropy) {
      return false;
    }
  }
  return true;
}

bool Recursor::push_frame(const ContractionProof& proof) {
  if (!verify_depth_proof(proof)) {
    return false;
  }
  proofs.push_back(proof);
  ++current_depth;
  return true;
}

bool Recursor::contract_top(double next_entropy) {
  if (proofs.empty()) {
    return false;
  }
  ContractionProof& top = proofs.back();
  if (!top.verified) {
    return false;
  }
  if (next_entropy > top.final_entropy) {
    return false;
  }
  top.final_entropy = next_entropy;
  return true;
}

void Recursor::pop_frame() {
  if (current_depth > 0 && !proofs.empty()) {
    proofs.pop_back();
    current_depth--;
  }
}

bool Recursor::has_active_frame() const { return !proofs.empty() && current_depth > 0; }

double Recursor::current_entropy() const {
  if (proofs.empty()) {
    return 0.0;
  }
  return proofs.back().final_entropy;
}

Recursor::EvalResult Recursor::evaluate_loop(double start_entropy, int max_steps, double epsilon,
                                             const std::function<double(double, int)>& step_fn) {
  EvalResult out;
  if (max_steps <= 0 || !step_fn) {
    out.final_entropy = start_entropy;
    return out;
  }

  double current = start_entropy;
  for (int step = 0; step < max_steps; ++step) {
    const double next = step_fn(current, step);
    out.steps = step + 1;
    if (next > current) {
      out.final_entropy = current;
      return out;
    }
    if (std::fabs(current - next) <= epsilon) {
      out.converged = true;
      out.final_entropy = next;
      return out;
    }
    current = next;
  }

  out.final_entropy = current;
  return out;
}

}  // namespace t81::cog::v3
