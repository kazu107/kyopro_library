// https://github.com/kazu107/kyopro_library/blob/main/graph/lowlink.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 無向グラフの橋と関節点、およびDFSの訪問順・low値を求める。
使い方:
  auto result = cp::lowlink(g);
  auto bridge_ids = result.bridges; auto cut_vertices = result.articulation_points;
前提: 0始まり、無向グラフ専用。橋は辺ID、関節点は頂点番号でともに昇順。非連結・多重辺・自己ループ可。
計算量: O(V+E)、メモリO(V+E)。明示スタックで探索する。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp


namespace cp {
struct LowlinkResult {
    std::vector<int> order, low, bridges, articulation_points;
};
// 無向グラフの橋(辺ID)と関節点(頂点番号)を昇順で取得。
// 非連結・多重辺・自己ループ可。反復DFS。O(V+E)、メモリO(V+E)。
template <class Cost>
LowlinkResult lowlink(const Graph<Cost>& g) {
    assert(!g.is_directed());
    const int n = g.size();
    LowlinkResult result{std::vector<int>(n, -1), std::vector<int>(n), {}, {}};
    std::vector<int> parent(n, -1), parent_edge(n, -1), children(n), next(n), stack;
    std::vector<bool> cut(n, false), bridge(g.edge_count(), false);
    int timer = 0;
    for (int root = 0; root < n; ++root) {
        if (result.order[root] != -1) continue;
        result.order[root] = result.low[root] = timer++; stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back();
            if (next[v] == static_cast<int>(g[v].size())) {
                stack.pop_back();
                const int p = parent[v];
                if (p == -1) cut[v] = children[v] > 1;
                else {
                    result.low[p] = std::min(result.low[p], result.low[v]);
                    if (result.low[v] > result.order[p]) bridge[parent_edge[v]] = true;
                    if (parent[p] != -1 && result.low[v] >= result.order[p]) cut[p] = true;
                }
                continue;
            }
            const auto& edge = g[v][next[v]++];
            if (edge.id == parent_edge[v]) continue;
            if (result.order[edge.to] == -1) {
                parent[edge.to] = v; parent_edge[edge.to] = edge.id; ++children[v];
                result.order[edge.to] = result.low[edge.to] = timer++; stack.push_back(edge.to);
            } else result.low[v] = std::min(result.low[v], result.order[edge.to]);
        }
    }
    for (int id = 0; id < g.edge_count(); ++id) if (bridge[id]) result.bridges.push_back(id);
    for (int v = 0; v < n; ++v) if (cut[v]) result.articulation_points.push_back(v);
    return result;
}
} // namespace cp
