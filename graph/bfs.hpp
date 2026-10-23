// https://github.com/kazu107/kyopro_library/blob/main/graph/bfs.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "ShortestPath.hpp"
#include <queue>

namespace cp {
// bfs(g, source) / bfs(g, vector<int>{s1,s2,...})。
// 辺の重みを無視し、辺数の最小値を求める。O(V+E)、メモリO(V)。
// sourcesが空なら全頂点未到達。複数始点からの最短路を同時に求める。
template <class Cost>
ShortestPathResult<long long> bfs(const Graph<Cost>& g, const std::vector<int>& sources) {
    ShortestPathResult<long long> result(g.size());
    std::queue<int> queue;
    for (int source : sources) {
        assert(0 <= source && source < g.size());
        if (result.dist[source] == 0) continue;
        result.dist[source] = 0;
        queue.push(source);
    }
    while (!queue.empty()) {
        const int v = queue.front();
        queue.pop();
        for (const auto& edge : g[v]) {
            if (result.reachable(edge.to)) continue;
            result.dist[edge.to] = result.dist[v] + 1;
            result.parent[edge.to] = v;
            result.parent_edge[edge.to] = edge.id;
            queue.push(edge.to);
        }
    }
    return result;
}
template <class Cost>
ShortestPathResult<long long> bfs(const Graph<Cost>& g, int source) {
    return bfs(g, std::vector<int>{source});
}
} // namespace cp
