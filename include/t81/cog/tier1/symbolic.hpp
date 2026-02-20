#pragma once

#include <optional>
#include <string>
#include <vector>
#include "t81/core/T81Symbol.hpp"

namespace t81::cog::v1 {

struct SymbolicAtom {
  T81Symbol id;
  std::string label;

  static SymbolicAtom create(const std::string& label);
  bool operator==(const SymbolicAtom& other) const;
  bool operator<(const SymbolicAtom& other) const;
};

struct SymbolicEdge {
  T81Symbol from;
  T81Symbol to;
  std::string label;

  bool operator==(const SymbolicEdge& other) const;
  bool operator<(const SymbolicEdge& other) const;
};

struct RewriteRule {
  T81Symbol match_node;
  T81Symbol replace_node;
};

struct SymbolicGraph {
  std::vector<SymbolicAtom> nodes;
  std::vector<SymbolicEdge> edges;

  void add_node(const SymbolicAtom& node);
  void add_edge(const T81Symbol& from, const T81Symbol& to, const std::string& label = "");

  void apply_rewrite(const RewriteRule& rule);
  bool is_confluent() const;
  void canonicalize();
};

}  // namespace t81::cog::v1
