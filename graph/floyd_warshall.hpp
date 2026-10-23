// https://github.com/kazu107/kyopro_library/blob/main/graph/floyd_warshall.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 全頂点対の最短距離、負閉路の影響、頂点列・辺ID列の復元を求める。
使い方:
  auto result = cp::floyd_warshall(g);
  if (result.finite(s, t)) { auto distance = result.dist[s][t]; auto path = result.path(s, t); }
前提: 0始まり、符号付き整数重み。負辺可。sから負閉路を経由してtへ行ける場合はnegative[s][t]=true、距離-INF。
計算量: O(V^3)、メモリO(V^2)。有限の単純路の途中の和は(-INF,INF)内。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp


namespace cp {
template <class Cost>
struct AllPairsShortestPathResult {
    std::vector<std::vector<Cost>> dist;
    std::vector<std::vector<int>> next, next_edge;
    std::vector<std::vector<bool>> negative;
    bool finite(int s, int t) const {
        assert(0 <= s && s < static_cast<int>(dist.size()) && 0 <= t && t < static_cast<int>(dist.size()));
        return dist[s][t] != distance_inf<Cost>() && !negative[s][t];
    }
    std::vector<int> path(int s, int t) const {
        if (!finite(s, t)) return {};
        std::vector<int> result{s};
        while (s != t) {
            s = next[s][t];
            result.push_back(s);
        }
        return result;
    }
    std::vector<int> edge_path(int s, int t) const {
        if (!finite(s, t)) return {};
        std::vector<int> result;
        while (s != t) {
            result.push_back(next_edge[s][t]);
            s = next[s][t];
        }
        return result;
    }
};
// 全頂点対の最短路。負辺可。O(V^3)、メモリO(V^2)。
// 負閉路を経由できる(s,t)をnegative[s][t]=true、距離を-INFとする。
// 有限の単純路の途中の和も(-INF,INF)内に収める。自己ループ・多重辺可。
template <class Cost>
AllPairsShortestPathResult<Cost> floyd_warshall(const Graph<Cost>& g) {
    const int n = g.size();
    const Cost inf = distance_inf<Cost>();
    AllPairsShortestPathResult<Cost> result{
        std::vector<std::vector<Cost>>(n, std::vector<Cost>(n, inf)),
        std::vector<std::vector<int>>(n, std::vector<int>(n, -1)),
        std::vector<std::vector<int>>(n, std::vector<int>(n, -1)),
        std::vector<std::vector<bool>>(n, std::vector<bool>(n, false))};
    for (int v = 0; v < n; ++v) {
        result.dist[v][v] = 0;
        result.next[v][v] = v;
        for (const auto& edge : g[v]) if (edge.cost < result.dist[v][edge.to]) {
            result.dist[v][edge.to] = edge.cost;
            result.next[v][edge.to] = edge.to;
            result.next_edge[v][edge.to] = edge.id;
        }
    }
    for (int k = 0; k < n; ++k) for (int i = 0; i < n; ++i) {
        if (result.dist[i][k] == inf) continue;
        for (int j = 0; j < n; ++j) {
            if (result.dist[k][j] == inf) continue;
            const Cost candidate = saturated_distance_add(result.dist[i][k], result.dist[k][j]);
            if (candidate >= result.dist[i][j]) continue;
            result.dist[i][j] = candidate;
            result.next[i][j] = result.next[i][k];
            result.next_edge[i][j] = result.next_edge[i][k];
        }
    }
    for (int k = 0; k < n; ++k) if (result.dist[k][k] < 0) {
        for (int i = 0; i < n; ++i) if (result.dist[i][k] != inf)
            for (int j = 0; j < n; ++j) if (result.dist[k][j] != inf)
                result.negative[i][j] = true;
    }
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j)
        if (result.negative[i][j]) result.dist[i][j] = -inf;
    return result;
}
} // namespace cp
