// https://github.com/kazu107/kyopro_library/blob/main/tree/diameter.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "RootedForest.hpp"
#include <algorithm>

namespace cp {
template <class Cost>
struct TreeDiameterResult {
    Cost length{};
    int from = -1, to = -1;
    std::vector<int> vertices, edge_ids;
};
// 非負重みの連結な木。空なら端点-1、1頂点なら長さ0でその頂点。
// 負辺/非連結/閉路はinvalid_argument。2回の反復探索O(V)、メモリO(V)。
// 重み・距離の合計はCostに収める。
template <class Cost>
TreeDiameterResult<Cost> tree_diameter(const Graph<Cost>& g) {
    RootedForest<Cost> validation(g);
    if (g.size() == 0) return {};
    if (validation.roots.size() != 1) throw std::invalid_argument("Diameter requires a connected tree");
    for (const auto& edge : g.edges()) if (edge.cost < 0)
        throw std::invalid_argument("Two-sweep diameter requires nonnegative weights");
    auto farthest = [&](int start, std::vector<int>& parent, std::vector<int>& parent_edge,
                        std::vector<Cost>& distance) {
        parent.assign(g.size(), -1); parent_edge.assign(g.size(), -1); distance.assign(g.size(), Cost{});
        std::vector<int> stack{start};
        while (!stack.empty()) {
            const int v = stack.back(); stack.pop_back();
            for (const auto& edge : g[v]) if (edge.to != parent[v]) {
                parent[edge.to] = v; parent_edge[edge.to] = edge.id;
                distance[edge.to] = distance[v] + edge.cost; stack.push_back(edge.to);
            }
        }
        int best = 0;
        for (int v = 1; v < g.size(); ++v) if (distance[v] > distance[best]) best = v;
        return best;
    };
    std::vector<int> parent, parent_edge;
    std::vector<Cost> distance;
    const int from = farthest(0, parent, parent_edge, distance);
    const int to = farthest(from, parent, parent_edge, distance);
    TreeDiameterResult<Cost> result;
    result.from = from; result.to = to; result.length = distance[to];
    for (int v = to; v != -1; v = parent[v]) {
        result.vertices.push_back(v);
        if (parent_edge[v] != -1) result.edge_ids.push_back(parent_edge[v]);
    }
    std::reverse(result.vertices.begin(), result.vertices.end());
    std::reverse(result.edge_ids.begin(), result.edge_ids.end());
    return result;
}
} // namespace cp
