#include "t81/core/T81Graph.hpp"
#include <iostream>
#include <memory>

using namespace t81;

// Define a graph with 1000 nodes.
// pagerank will create a 1000x1000 tensor of Weight81 (32 bytes).
// Size = 1000 * 1000 * 32 = 32 MB.
// This should segfault on stack.

void test_large_graph() {
    std::cout << "Creating large graph on heap..." << std::endl;
    // Graph itself is large too (1000 * 10 * 34 = 340KB), so safer on heap.
    auto g_ptr = std::make_unique<T81Graph<1000, 10>>();
    T81Graph<1000, 10>& g = *g_ptr;

    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    std::cout << "Running pagerank..." << std::endl;
    // This call should crash due to stack overflow if internal tensor is on stack.
    auto pr = pagerank(g, 1);

    std::cout << "Pagerank completed successfully." << std::endl;
}

int main() {
    test_large_graph();
    return 0;
}
