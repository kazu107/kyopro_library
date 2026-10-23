// https://github.com/kazu107/kyopro_library/blob/main/graph/zero_one_bfs.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "ShortestPath.hpp"
#include <deque>
#include <stdexcept>

namespace cp {
// 重み0/1専用。zero_one_bfs(g, source)。O(V+E)、メモリO(V+E)。
// 0/1以外の辺があればinvalid_argument。経路も復元できる。
template <class Cost>
ShortestPathResult<Cost> zero_one_bfs(const Graph<Cost>& g, int source) {
    assert(0 <= source && source < g.size());
    for (const auto& edge : g.edges())
        if (edge.cost != 0 && edge.cost != 1) throw std::invalid_argument("0-1 BFS requires weights 0 or 1");
    ShortestPathResult<Cost> result(g.size());
    std::deque<std::pair<Cost, int>> queue;
    result.dist[source] = 0;
    queue.push_front({0, source});
    while (!queue.empty()) {
        const auto [distance, v] = queue.front();
        queue.pop_front();
        if (distance != result.dist[v]) continue;
        for (const auto& edge : g[v]) {
            const Cost candidate = distance + edge.cost;
            if (candidate >= result.dist[edge.to]) continue;
            result.dist[edge.to] = candidate;
            result.parent[edge.to] = v;
            result.parent_edge[edge.to] = edge.id;
            if (edge.cost == 0) queue.push_front({candidate, edge.to});
            else queue.push_back({candidate, edge.to});
        }
    }
    return result;
}
} // namespace cp
