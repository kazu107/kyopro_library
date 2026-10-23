// https://github.com/kazu107/kyopro_library/blob/main/tree/RootedForest.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "../graph/Graph.hpp"
#include <stdexcept>

namespace cp {
// 木/森の共通前処理。rootの成分を最初に、残りは頂点番号順に根付ける。
// 有向グラフや閉路(自己ループ・平行辺を含む)はinvalid_argument。
// 根のparent/parent_edgeは-1。空の森可。反復探索O(V+E)、メモリO(V+E)。
template <class Cost = long long>
struct RootedForest {
    std::vector<int> parent, parent_edge, depth, component, roots, order, subtree_size;
    std::vector<Cost> parent_cost;
    std::vector<std::vector<int>> children;
    explicit RootedForest(const Graph<Cost>& g, int root = 0) {
        if (g.is_directed()) throw std::invalid_argument("Tree algorithms require an undirected forest");
        const int n = g.size();
        parent.assign(n, -1); parent_edge.assign(n, -1); depth.assign(n, 0);
        component.assign(n, -1); subtree_size.assign(n, 1); parent_cost.assign(n, Cost{});
        children.resize(n);
        if (n == 0) return;
        assert(0 <= root && root < n);
        std::vector<int> stack;
        for (int iteration = -1; iteration < n; ++iteration) {
            const int start = iteration == -1 ? root : iteration;
            if (component[start] != -1) continue;
            component[start] = static_cast<int>(roots.size());
            roots.push_back(start); stack.push_back(start);
            while (!stack.empty()) {
                const int v = stack.back(); stack.pop_back(); order.push_back(v);
                for (const auto& edge : g[v]) {
                    if (edge.id == parent_edge[v]) continue;
                    if (component[edge.to] != -1) throw std::invalid_argument("Graph is not a forest");
                    parent[edge.to] = v; parent_edge[edge.to] = edge.id; parent_cost[edge.to] = edge.cost;
                    depth[edge.to] = depth[v] + 1; component[edge.to] = component[v];
                    children[v].push_back(edge.to);
                }
                for (auto it = children[v].rbegin(); it != children[v].rend(); ++it) stack.push_back(*it);
            }
        }
        for (auto it = order.rbegin(); it != order.rend(); ++it)
            if (parent[*it] != -1) subtree_size[parent[*it]] += subtree_size[*it];
    }
};
} // namespace cp
