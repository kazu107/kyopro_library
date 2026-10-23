// https://github.com/kazu107/kyopro_library/blob/main/graph/dijkstra.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "ShortestPath.hpp"
#include <functional>
#include <queue>
#include <stdexcept>

namespace cp {
// 非負整数重み専用。dijkstra(g, source)。負辺はinvalid_argument。
// 重複挿入する二分ヒープ: O(V+(V+E)log(V+E))、メモリO(V+E)。
// 距離・辺重み・単純路の和はdistance_inf<Cost>()未満にする。
template <class Cost>
ShortestPathResult<Cost> dijkstra(const Graph<Cost>& g, int source) {
    assert(0 <= source && source < g.size());
    for (const auto& edge : g.edges())
        if (edge.cost < 0) throw std::invalid_argument("Dijkstra requires nonnegative weights");
    ShortestPathResult<Cost> result(g.size());
    using State = std::pair<Cost, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    result.dist[source] = 0;
    queue.push({0, source});
    while (!queue.empty()) {
        const auto [distance, v] = queue.top();
        queue.pop();
        if (distance != result.dist[v]) continue;
        for (const auto& edge : g[v]) {
            const Cost candidate = saturated_distance_add(distance, edge.cost);
            if (candidate >= result.dist[edge.to]) continue;
            result.dist[edge.to] = candidate;
            result.parent[edge.to] = v;
            result.parent_edge[edge.to] = edge.id;
            queue.push({candidate, edge.to});
        }
    }
    return result;
}
} // namespace cp
