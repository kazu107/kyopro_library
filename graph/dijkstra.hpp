// https://github.com/kazu107/kyopro_library/blob/main/graph/dijkstra.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 非負重みのグラフで、単一始点の最短距離と経路を求める。
使い方:
  auto result = cp::dijkstra(g, source);
  if (result.finite(target)) { auto distance = result.dist[target]; auto path = result.path_to(target); }
前提: 0始まり、有向・無向とも可。重みは非負の符号付き整数。負辺はinvalid_argument。単純路の途中の和もINF未満に収める。
計算量: 重複挿入する二分ヒープでO(V+(V+E)log(V+E))、メモリO(V+E)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/ShortestPath.hpp


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
