// https://github.com/kazu107/kyopro_library/blob/main/graph/cycle_detection.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"
#include <algorithm>

namespace cp {
// verticesは始点を末尾でも繰り返す。edge_ids.size()+1==vertices.size()。
// 閉路なしなら両方空。自己ループ・無向の多重辺による2辺の閉路にも対応。
struct CycleResult { std::vector<int> vertices, edge_ids; };
// 有向・無向両対応。明示スタック。O(V+E)、メモリO(V)。
template <class Cost>
CycleResult find_cycle(const Graph<Cost>& g) {
    std::vector<int> color(g.size()), parent(g.size(), -1), parent_edge(g.size(), -1), next(g.size());
    std::vector<int> stack;
    for (int root = 0; root < g.size(); ++root) {
        if (color[root]) continue;
        color[root] = 1; stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back();
            if (next[v] == static_cast<int>(g[v].size())) {
                color[v] = 2; stack.pop_back(); continue;
            }
            const auto& edge = g[v][next[v]++];
            if (!g.is_directed() && edge.id == parent_edge[v]) continue;
            if (color[edge.to] == 0) {
                parent[edge.to] = v; parent_edge[edge.to] = edge.id;
                color[edge.to] = 1; stack.push_back(edge.to);
            } else if (color[edge.to] == 1) {
                CycleResult result;
                int u = v;
                while (u != edge.to) {
                    result.vertices.push_back(u); result.edge_ids.push_back(parent_edge[u]); u = parent[u];
                }
                result.vertices.push_back(edge.to);
                std::reverse(result.vertices.begin(), result.vertices.end());
                std::reverse(result.edge_ids.begin(), result.edge_ids.end());
                result.vertices.push_back(edge.to); result.edge_ids.push_back(edge.id);
                return result;
            }
        }
    }
    return {};
}
} // namespace cp
