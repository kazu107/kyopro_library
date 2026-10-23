// https://github.com/kazu107/kyopro_library/blob/main/graph/bipartite.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 無向グラフを隣接頂点が異なる2色で塗れるか判定し、0/1の色分けを返す。
使い方:
  auto result = cp::bipartite_check(g);
  if (result.is_bipartite) { int color = result.color[v]; }
前提: 0始まり、無向グラフ専用。全成分を判定する。自己ループは二部でない。falseならcolorは途中までの値。
計算量: O(V+E)、追加メモリO(V)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp


namespace cp {
struct BipartiteResult { bool is_bipartite; std::vector<int> color; };
// 無向グラフ専用。全成分を0/1で色分け。自己ループは二部でない。
// 判定falseのときcolorは途中までの値。O(V+E)、メモリO(V)。
template <class Cost>
BipartiteResult bipartite_check(const Graph<Cost>& g) {
    assert(!g.is_directed());
    BipartiteResult result{true, std::vector<int>(g.size(), -1)};
    std::queue<int> queue;
    for (int root = 0; root < g.size(); ++root) {
        if (result.color[root] != -1) continue;
        result.color[root] = 0; queue.push(root);
        while (!queue.empty()) {
            const int v = queue.front(); queue.pop();
            for (const auto& edge : g[v]) {
                if (result.color[edge.to] == -1) {
                    result.color[edge.to] = result.color[v] ^ 1; queue.push(edge.to);
                } else if (result.color[edge.to] == result.color[v]) {
                    result.is_bipartite = false; return result;
                }
            }
        }
    }
    return result;
}
} // namespace cp
