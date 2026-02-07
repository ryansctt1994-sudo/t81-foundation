#include "t81/cog/tier4.hpp"
#include "t81/axion/context.hpp"
#include <iostream>

namespace t81::cog {

Tier4Loop::Tier4Loop(const TierStatus& status, t81::axion::Engine& engine)
    : status_(status), engine_(engine) {
    update_self_model();
}

void Tier4Loop::reflect() {
    t81::axion::SyscallContext ctx{{}, "cog", "reflect", nullptr, {}, 0, t81::tisc::Opcode::Nop};
    auto verdict = engine_.evaluate(ctx);

    if (verdict.kind == t81::axion::VerdictKind::Allow) {
        self_model_.push_back("Refinement: observed " + status_.label + " consistency.");
        if (self_model_.size() > 81) {
            self_model_.erase(self_model_.begin());
        }
    }
}

void Tier4Loop::cycle() {
    reflect();
}

void Tier4Loop::update_self_model() {
    self_model_.clear();
    self_model_.push_back("Initial state: " + status_.label);
}

} // namespace t81::cog
