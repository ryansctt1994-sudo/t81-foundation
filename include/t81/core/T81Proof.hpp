/**
 * @file T81Proof.hpp
 * @brief Defines the T81Proof class for representing formal, verifiable proofs.
 */
#pragma once

#include <functional>
#include <variant>
#include "t81/core/T81Agent.hpp"
#include "t81/core/T81Map.hpp"
#include "t81/core/T81Reflection.hpp"
#include "t81/core/T81String.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Tree.hpp"

namespace t81 {

struct T81Theorem {
  T81Symbol name;
  T81String statement;
  T81Tree<T81Symbol>::ptr logical_form;

  T81Theorem(T81Symbol n, T81String s, T81Tree<T81Symbol>::ptr lf)
      : name(n), statement(std::move(s)), logical_form(std::move(lf)) {}
};

struct T81InferenceRule {
  T81Symbol name;
  T81Tree<T81Symbol>::ptr conclusion_pattern;
  std::function<bool(const T81Tree<T81Symbol>::ptr&)> validator;
};

class T81Proof {
  T81Symbol theorem_name_;
  T81Tree<T81Symbol>::ptr goal_;
  T81List<T81Tree<T81Symbol>::ptr> steps_;
  T81List<T81Reflection<T81Symbol>> audit_trail_;

public:
  explicit T81Proof(const T81Theorem& th) : theorem_name_(th.name), goal_(th.logical_form) {}

  bool apply_rule(const T81InferenceRule& rule, T81Agent& prover) {
    auto fuel = prover.consume_entropy();
    if (!fuel) return false;

    if (!rule.validator || rule.validator(steps_.empty() ? nullptr : steps_.back())) {
      steps_.push_back(rule.conclusion_pattern);
      audit_trail_.push_back(
          T81Reflection<T81Symbol>(rule.name, symbols::PROOF, T81Symbol::intern("STEP")));
      return true;
    }
    return false;
  }

  [[nodiscard]] bool is_complete() const noexcept {
    if (steps_.empty() || !goal_) return false;
    return *steps_.back() == *goal_;
  }

  [[nodiscard]] T81String natural_language() const {
    return T81String("PROVEN: ") + T81String(theorem_name_.to_string().c_str());
  }
};

}  // namespace t81
