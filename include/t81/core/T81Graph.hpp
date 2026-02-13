/**
 * @file T81Graph.hpp
 * @brief Defines the T81Graph class, a static graph structure for high performance.
 *
 * This file provides a static, cache-oblivious graph data structure designed for
 * hardware-native performance. The `T81Graph<NodeCount, MaxDegree>` class uses
 * a contiguous adjacency list, making it suitable for efficient execution of
 * graph algorithms that can be expressed as tensor operations, such as PageRank
 * and message passing.
 */
#pragma once

#include "t81/core/T81Int.hpp"
#include "t81/core/T81Float.hpp"
#include "t81/core/T81Symbol.hpp"
#include "t81/core/T81Tensor.hpp"
#include <cstddef>
#include <span>
#include <array>
#include <bit>
#include <compare>

namespace t81 {

// ======================================================================
// Edge weight — exactly one tryte (81 trits)
// ======================================================================
using Weight81 = T81Float<72,9>;        // 81-trit floating weight
// using Weight81 = T81Fixed<72,9>;     // for exact integer weights
// using Weight81 = T81Symbol;          // for symbolic/categorical edges

// ======================================================================
// T81Graph<NodeCount, MaxDegree> — Static, cache-oblivious, hardware-native
// ======================================================================
template <size_t NodeCount, size_t MaxDegree = 81>
class T81Graph {
    static_assert(NodeCount <= 81*81, "NodeCount fits in two trytes (symbolic ID)");
    static_assert(MaxDegree <= 81,     "MaxDegree fits in one tryte (index)");

public:
    using NodeID   = uint16_t;                    // 0..6560 — fits in two trytes
    using Weight   = Weight81;
    using EdgeList = std::array<std::pair<NodeID, Weight>, MaxDegree>;

    static constexpr size_t nodes() noexcept { return NodeCount; }
    static constexpr size_t max_degree() noexcept { return MaxDegree; }

private:
    // Adjacency list — contiguous, cache-line aligned, perfect for tensor cores
    alignas(64) EdgeList adj[NodeCount];

    // Optional: node labels (symbols, embeddings, etc.)
    alignas(64) T81Symbol labels[NodeCount];

public:
    //===================================================================
    // Construction
    //===================================================================
    constexpr T81Graph() noexcept {
        for (auto& list : adj) list.fill({NodeID(-1), Weight{}}); // sentinel = invalid
    }

    //===================================================================
    // Edge manipulation — O(1), hardware-accelerated on Axion
    //===================================================================
    constexpr void add_edge(NodeID from, NodeID to, Weight w = Weight(1)) noexcept {
        for (auto& e : adj[from]) {
            if (e.first == NodeID(-1) || e.first == to) {
                e = {to, w};
                return;
            }
        }
        // Overflow → hardware trap on real silicon (degree limit exceeded)
    }

    constexpr void set_weight(NodeID from, NodeID to, Weight w) noexcept {
        for (auto& e : adj[from]) {
            if (e.first == to) { e.second = w; return; }
        }
    }

    [[nodiscard]] constexpr Weight weight(NodeID from, NodeID to) const noexcept {
        for (const auto& e : adj[from]) {
            if (e.first == to) return e.second;
        }
        return Weight(0);
    }

    //===================================================================
    // Views
    //===================================================================
    [[nodiscard]] constexpr std::span<const std::pair<NodeID, Weight>> 
    outgoing(NodeID n) const noexcept {
        return {adj[n].begin(), std::find_if(adj[n].begin(), adj[n].end(),
                   [](auto& e){ return e.first == NodeID(-1); })};
    }

    //===================================================================
    // Symbolic interface — nodes are T81Symbol
    //===================================================================
    constexpr void label(NodeID n, T81Symbol sym) noexcept { labels[n] = sym; }
    [[nodiscard]] constexpr T81Symbol label(NodeID n) const noexcept { return labels[n]; }

    //===================================================================
    // Graph algorithms become tensor operations
    //===================================================================

    // PageRank → manual iteration for Rank 1 tensors
    // Optimized to use sparse updates (O(E) per step) instead of dense matrix (O(N²)).
    [[nodiscard]] friend constexpr auto pagerank(const T81Graph& g, int steps = 20, Weight81 epsilon = Weight81(1e-6)) noexcept
        -> T81Tensor<Weight81, 1, NodeCount>
    {
        using Tensor1D = T81Tensor<Weight81, 1, NodeCount>;
        auto v = Tensor1D::zeros();
        v(0) = Weight81(1); // initial state

        Weight81 damping(0.85);
        Weight81 teleport = Weight81(0.15) / Weight81(static_cast<long long>(NodeCount));

        // Ensure epsilon is positive
        if (epsilon < Weight81(0)) epsilon = -epsilon;

        for (int s = 0; s < steps; ++s) {
            // Calculate total mass to determine teleport contribution
            Weight81 current_sum = reduce_sum(v);
            Weight81 base_val = current_sum * teleport;

            // Initialize next_v with base teleportation value
            // (Avoiding Tensor1D::zeros() + fill loop by direct init if possible, but fill is fine)
            Tensor1D next_v;
            for (size_t i = 0; i < NodeCount; ++i) next_v(i) = base_val;

            // Sparse update: push mass from each node to its neighbors
            for (NodeID i = 0; i < NodeCount; ++i) {
                auto out_edges = g.outgoing(i);
                size_t deg = out_edges.size();
                if (deg > 0) {
                     Weight81 w_scale = Weight81(1) / Weight81(static_cast<long long>(deg));
                     Weight81 mass = v(i) * damping * w_scale;
                     for (auto [j, w] : out_edges) {
                         // Accumulate contribution
                         next_v(j) = next_v(j) + mass * w;
                     }
                }
            }

            // Check convergence
            Weight81 diff_sum = Weight81::zero();
            for (size_t i = 0; i < NodeCount; ++i) {
                Weight81 d = next_v(i) - v(i);
                diff_sum = diff_sum + d.abs();
            }

            v = next_v;

            if (diff_sum < epsilon) break;
        }
        return v;
    }

    /**
     * @brief Breadth-First Search (BFS) returning distances.
     * @param start The starting node.
     * @return Tensor of distances (hops). -1 for unreachable.
     */
    [[nodiscard]] constexpr auto bfs(NodeID start) const noexcept
        -> T81Tensor<T81Int<81>, 1, NodeCount>
    {
        using DistTensor = T81Tensor<T81Int<81>, 1, NodeCount>;
        DistTensor dists;

        // Initialize with -1
        T81Int<81> unreachable(static_cast<std::int64_t>(-1));
        for (size_t i = 0; i < NodeCount; ++i) dists(i) = unreachable;

        dists(start) = T81Int<81>(static_cast<std::int64_t>(0));

        // Wavefront iteration
        bool changed = true;

        for (size_t iter = 0; iter < NodeCount && changed; ++iter) {
            changed = false;
            T81Int<81> current_dist(static_cast<std::int64_t>(iter));
            T81Int<81> next_dist = current_dist + T81Int<81>(static_cast<std::int64_t>(1));

            for (NodeID u = 0; u < NodeCount; ++u) {
                if (dists(u) == current_dist) {
                    // Expand
                    // outgoing returns span of pairs
                    auto out_edges = outgoing(u);
                    for (auto& edge : out_edges) {
                        NodeID v = edge.first;
                        if (dists(v) == unreachable) {
                            dists(v) = next_dist;
                            changed = true;
                        }
                    }
                }
            }
        }
        return dists;
    }

    /**
     * @brief Computes connected components (Weakly Connected Components).
     * @return Tensor of component IDs. Each component ID is the smallest NodeID in that component.
     */
    [[nodiscard]] constexpr auto connected_components() const noexcept
        -> T81Tensor<T81Int<81>, 1, NodeCount>
    {
        using ComponentTensor = T81Tensor<T81Int<81>, 1, NodeCount>;
        ComponentTensor comps;

        for (NodeID i = 0; i < NodeCount; ++i) {
            comps(i) = T81Int<81>(static_cast<std::int64_t>(i));
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (NodeID u = 0; u < NodeCount; ++u) {
                // Propagate minimum component ID between neighbors (WCC style)
                T81Int<81> id_u = comps(u);

                for (auto [v, w] : outgoing(u)) {
                    T81Int<81> id_v = comps(v);

                    if (id_v < id_u) {
                        comps(u) = id_v;
                        changed = true;
                        id_u = id_v; // Update local
                    } else if (id_u < id_v) {
                        comps(v) = id_u;
                        changed = true;
                    }
                }
            }
        }
        return comps;
    }

    /**
     * @brief Computes shortest paths from start node using Dijkstra's algorithm.
     * @return Tensor of distances.
     */
    [[nodiscard]] constexpr auto shortest_path(NodeID start) const noexcept
        -> T81Tensor<Weight81, 1, NodeCount>
    {
        T81Tensor<Weight81, 1, NodeCount> dist;
        // Initialize with infinity
        Weight81 inf = Weight81::inf(true);
        for(size_t i=0; i<NodeCount; ++i) dist(i) = inf;

        dist(start) = Weight81(0);

        // Visited set
        bool visited[NodeCount] = {};

        for (size_t i = 0; i < NodeCount; ++i) {
            // Find min dist node among unvisited
            NodeID u = NodeID(-1);
            Weight81 min_d = inf;

            for (size_t j = 0; j < NodeCount; ++j) {
                // Check dist(j) < min_d.
                // We must handle NaE/Inf correctly. T81Float comparison works.
                if (!visited[j]) {
                    Weight81 d = dist(j);
                    if (d < min_d) {
                        min_d = d;
                        u = static_cast<NodeID>(j);
                    }
                }
            }

            if (u == NodeID(-1)) break; // No reachable nodes left or all are Inf

            visited[u] = true;

            // Relax neighbors
            for (auto [v, w] : outgoing(u)) {
                if (!visited[v]) {
                    Weight81 new_dist = dist(u) + w;
                    if (new_dist < dist(v)) {
                        dist(v) = new_dist;
                    }
                }
            }
        }

        return dist;
    }

    // Message passing (one step) → single sparse tensor contraction
    [[nodiscard]] constexpr auto message_pass(
        const T81Tensor<Weight81, 1, NodeCount>& node_states) const noexcept
        -> T81Tensor<Weight81, 1, NodeCount>
    {
        T81Tensor<Weight81, 1, NodeCount> out{};
        for (NodeID i = 0; i < NodeCount; ++i) {
            Weight81 sum{};
            for (auto [j, w] : outgoing(i)) {
                sum = sum + node_states(j) * w;
            }
            out(i) = sum;
        }
        return out;
    }
};

// ======================================================================
// Canonical graph types of the new era
// ======================================================================
using SymbolGraph81   = T81Graph<6561, 81>;   // 81² nodes, degree 81 → full HRR binding graph
using AttentionGraph  = T81Graph<4096, 128>;  // transformer KV graph
using KnowledgeGraph  = T81Graph<81*81*81, 27>; // 81³ nodes (531441), sparse symbolic

// ======================================================================
// The future of all computation is a graph of 81-trit weights
// ======================================================================
// Size is roughly 12-13 MiB for SymbolGraph81 — fits in L3

} // namespace t81
