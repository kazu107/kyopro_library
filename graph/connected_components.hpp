// https://github.com/kazu107/kyopro_library/blob/main/graph/connected_components.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"
#include <queue>

namespace cp {
struct ConnectedComponentsResult {
    std::vector<int> component;
    std::vector<std::vector<int>> groups;
};
// 無向グラフ専用。孤立頂点も1成分。O(V+E)、メモリO(V)。
template <class Cost>
ConnectedComponentsResult connected_components(const Graph<Cost>& g) {
    assert(!g.is_directed());
    ConnectedComponentsResult result{std::vector<int>(g.size(), -1), {}};
    std::queue<int> queue;
    for (int root = 0; root < g.size(); ++root) {
        if (result.component[root] != -1) continue;
        const int id = static_cast<int>(result.groups.size());
        result.groups.push_back({});
        result.component[root] = id; queue.push(root);
        while (!queue.empty()) {
            const int v = queue.front(); queue.pop();
            result.groups[id].push_back(v);
            for (const auto& edge : g[v]) if (result.component[edge.to] == -1) {
                result.component[edge.to] = id; queue.push(edge.to);
            }
        }
    }
    return result;
}
} // namespace cp
