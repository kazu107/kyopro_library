// https://github.com/kazu107/kyopro_library/blob/main/graph/ShortestPath.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"
#include <algorithm>

namespace cp {
// dist == INFは未到達。negative[v]は到達可能な負閉路からvへ到達可能。
// path_to(t), edge_path_to(t) は有限の最短路がなければ空。復元O(経路長)。
template <class Cost = long long>
struct ShortestPathResult {
    std::vector<Cost> dist;
    std::vector<int> parent, parent_edge;
    std::vector<bool> negative;
    explicit ShortestPathResult(int n = 0)
        : dist(n, distance_inf<Cost>()), parent(n, -1), parent_edge(n, -1), negative(n, false) {}
    bool reachable(int v) const {
        assert(0 <= v && v < static_cast<int>(dist.size()));
        return dist[v] != distance_inf<Cost>();
    }
    bool finite(int v) const { return reachable(v) && !negative[v]; }
    std::vector<int> path_to(int v) const {
        if (!finite(v)) return {};
        std::vector<int> path;
        for (int u = v; u != -1; u = parent[u]) path.push_back(u);
        std::reverse(path.begin(), path.end());
        return path;
    }
    std::vector<int> edge_path_to(int v) const {
        if (!finite(v)) return {};
        std::vector<int> path;
        for (int u = v; parent[u] != -1; u = parent[u]) path.push_back(parent_edge[u]);
        std::reverse(path.begin(), path.end());
        return path;
    }
};
} // namespace cp
