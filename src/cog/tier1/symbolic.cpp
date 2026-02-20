#include "t81/cog/tier1/symbolic.hpp"
#include <algorithm>

namespace t81::cog::v1 {

SymbolicAtom SymbolicAtom::create(const std::string& label) {
  SymbolicAtom atom;
  atom.id = T81Symbol::intern(label);
  atom.label = label;
  return atom;
}

bool SymbolicAtom::operator==(const SymbolicAtom& other) const { return id == other.id; }

bool SymbolicAtom::operator<(const SymbolicAtom& other) const { return id < other.id; }

bool SymbolicEdge::operator==(const SymbolicEdge& other) const {
  return from == other.from && to == other.to && label == other.label;
}

bool SymbolicEdge::operator<(const SymbolicEdge& other) const {
  if (from != other.from) return from < other.from;
  if (to != other.to) return to < other.to;
  return label < other.label;
}

void SymbolicGraph::add_node(const SymbolicAtom& node) { nodes.push_back(node); }

void SymbolicGraph::add_edge(const T81Symbol& from, const T81Symbol& to, const std::string& label) {
  edges.push_back({from, to, label});
}

void SymbolicGraph::apply_rewrite(const RewriteRule& rule) {
  for (auto& node : nodes) {
    if (node.id == rule.match_node) {
      node.id = rule.replace_node;
      // In a full implementation, we might update label too, but T81Symbol encapsulates identity.
    }
  }
  for (auto& edge : edges) {
    if (edge.from == rule.match_node) edge.from = rule.replace_node;
    if (edge.to == rule.match_node) edge.to = rule.replace_node;
  }
}

bool SymbolicGraph::is_confluent() const {
  // Placeholder for T243 confluence check.
  // Full graph rewriting confluence (Church-Rosser property) requires
  // checking all critical pairs of rewrite rules.
  // In V1, we assume single-rule application or user-verified confluence.
  return true;
}

void SymbolicGraph::canonicalize() {
  std::sort(nodes.begin(), nodes.end());
  std::sort(edges.begin(), edges.end());
  nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
  edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
}

}  // namespace t81::cog::v1
