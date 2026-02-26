#include "t81/experimental/cog/tier1/symbolic.hpp"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

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
  if (!rule.match_node.is_valid()) {
    return;
  }
  if (rule.match_node == rule.replace_node) {
    return;
  }

  bool touched = false;
  for (auto& node : nodes) {
    if (node.id == rule.match_node) {
      node.id = rule.replace_node;
      node.label = rule.replace_node.to_string();
      touched = true;
    }
  }
  for (auto& edge : edges) {
    if (edge.from == rule.match_node) {
      edge.from = rule.replace_node;
      touched = true;
    }
    if (edge.to == rule.match_node) {
      edge.to = rule.replace_node;
      touched = true;
    }
  }

  if (touched) {
    canonicalize();
  }
}

bool SymbolicGraph::is_confluent() const {
  // Tier-1 check: graph is confluent when rewrite traversal is deterministic.
  // We enforce:
  // 1) unique node identities
  // 2) all edges refer to existing nodes
  // 3) for any (from,label), there is at most one destination.
  std::unordered_set<T81Symbol> node_ids;
  node_ids.reserve(nodes.size());
  for (const auto& node : nodes) {
    if (!node.id.is_valid()) {
      return false;
    }
    if (!node_ids.insert(node.id).second) {
      return false;
    }
  }

  struct TransitionKey {
    T81Symbol from;
    std::string label;

    bool operator==(const TransitionKey& other) const {
      return from == other.from && label == other.label;
    }
  };

  struct TransitionKeyHash {
    std::size_t operator()(const TransitionKey& k) const noexcept {
      std::size_t h1 = std::hash<T81Symbol>{}(k.from);
      std::size_t h2 = std::hash<std::string>{}(k.label);
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
  };

  std::unordered_map<TransitionKey, T81Symbol, TransitionKeyHash> transition_targets;
  transition_targets.reserve(edges.size());
  for (const auto& edge : edges) {
    if (!edge.from.is_valid() || !edge.to.is_valid()) {
      return false;
    }
    if (!node_ids.contains(edge.from) || !node_ids.contains(edge.to)) {
      return false;
    }

    TransitionKey key{edge.from, edge.label};
    auto it = transition_targets.find(key);
    if (it == transition_targets.end()) {
      transition_targets.emplace(std::move(key), edge.to);
      continue;
    }
    if (it->second != edge.to) {
      return false;
    }
  }

  return true;
}

void SymbolicGraph::canonicalize() {
  std::sort(nodes.begin(), nodes.end());
  std::sort(edges.begin(), edges.end());
  nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
  edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
}

}  // namespace t81::cog::v1
