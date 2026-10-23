// https://github.com/kazu107/kyopro_library/blob/main/graph/bellman_ford.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 負辺を含むグラフの最短路と、始点から到達可能な負閉路の影響を求める。
使い方:
  auto result = cp::bellman_ford(g, source);
  bool unbounded = result.negative[target]; auto path = result.path_to(target); // 影響ありなら経路は空。
前提: 0始まり、64bit以下の符号付き整数重み。未到達はINF、負閉路を経由できる頂点はnegative=trueで距離-INF。有限距離は(-INF,INF)内。
計算量: O(VE+V+E)、メモリO(V)。内部の__int128はGNU/Clang拡張。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/ShortestPath.hpp


namespace cp {
// 負辺可。bellman_ford(g,s)。O(VE+V+E)、メモリO(V)。
// 始点から到達可能な負閉路と、その先の頂点をnegative=trueにする。
// その距離は-INF、未到達はINF。有限距離は(-INF,INF)内。
// 負閉路による飽和で検出を見失わないよう、緩和は内部で__int128を使用。
template <class Cost>
ShortestPathResult<Cost> bellman_ford(const Graph<Cost>& g, int source) {
    assert(0 <= source && source < g.size());
    const int n = g.size();
    const __int128 wide_inf = static_cast<__int128>(1) << 120;
    std::vector<__int128> distance(n, wide_inf);
    ShortestPathResult<Cost> result(n);
    distance[source] = 0;
    std::queue<int> queue;
    for (int iteration = 0; iteration < n; ++iteration) {
        bool changed = false;
        for (int v = 0; v < n; ++v) {
            if (distance[v] == wide_inf) continue;
            for (const auto& edge : g[v]) {
                const __int128 candidate = distance[v] + edge.cost;
                if (candidate >= distance[edge.to]) continue;
                distance[edge.to] = candidate;
                result.parent[edge.to] = v;
                result.parent_edge[edge.to] = edge.id;
                changed = true;
                if (iteration == n - 1 && !result.negative[edge.to]) {
                    result.negative[edge.to] = true;
                    queue.push(edge.to);
                }
            }
        }
        if (!changed) break;
    }
    while (!queue.empty()) {
        const int v = queue.front();
        queue.pop();
        for (const auto& edge : g[v]) if (!result.negative[edge.to]) {
            result.negative[edge.to] = true;
            queue.push(edge.to);
        }
    }
    const Cost inf = distance_inf<Cost>();
    for (int v = 0; v < n; ++v) {
        if (result.negative[v]) {
            result.dist[v] = -inf;
            result.parent[v] = result.parent_edge[v] = -1;
        } else if (distance[v] != wide_inf) {
            assert(-static_cast<__int128>(inf) < distance[v] && distance[v] < inf);
            result.dist[v] = static_cast<Cost>(distance[v]);
        }
    }
    return result;
}
} // namespace cp
