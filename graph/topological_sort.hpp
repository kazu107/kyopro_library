// https://github.com/kazu107/kyopro_library/blob/main/graph/topological_sort.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"
#include <queue>

namespace cp {
struct TopologicalSortResult { bool is_dag; std::vector<int> order; };
// 有向グラフのKahn法。閉路があればis_dag=falseでorderは部分的な順序。
// 辞書順最小の保証はない。O(V+E)、メモリO(V)。
template <class Cost>
TopologicalSortResult topological_sort(const Graph<Cost>& g) {
    assert(g.is_directed());
    std::vector<int> indegree(g.size());
    for (const auto& edge : g.edges()) ++indegree[edge.to];
    std::queue<int> queue;
    for (int v = 0; v < g.size(); ++v) if (indegree[v] == 0) queue.push(v);
    TopologicalSortResult result{false, {}};
    while (!queue.empty()) {
        const int v = queue.front(); queue.pop();
        result.order.push_back(v);
        for (const auto& edge : g[v]) if (--indegree[edge.to] == 0) queue.push(edge.to);
    }
    result.is_dag = static_cast<int>(result.order.size()) == g.size();
    return result;
}
} // namespace cp
