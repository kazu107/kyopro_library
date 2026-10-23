// https://github.com/kazu107/kyopro_library/blob/main/graph/kruskal.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 無向グラフの最小全域木、非連結なら最小全域森を求める。
使い方:
  auto result = cp::kruskal(g);
  auto total_cost = result.cost; auto chosen_ids = result.edge_ids; bool connected = result.connected;
前提: 0始まり、無向グラフ専用。負辺可。空グラフは費用0・成分数0・connected=true。合計費用をCost型に収める。
計算量: O(V+E log(E+1))、メモリO(V+E)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp, data_structure/UnionFind.hpp


namespace cp {
template <class Cost>
struct KruskalResult {
    Cost cost{};
    std::vector<int> edge_ids;
    int components = 0;
    bool connected = false;
};
// 無向グラフ専用。負辺可。非連結なら最小全域「森」と成分数を返す。
// 空グラフはcost=0,components=0,connected=trueと定義。
// O(V+E log(E+1))、メモリO(V+E)。合計コストはCostに収める。
template <class Cost>
KruskalResult<Cost> kruskal(const Graph<Cost>& g) {
    assert(!g.is_directed());
    std::vector<int> ids(g.edge_count());
    std::iota(ids.begin(), ids.end(), 0);
    std::sort(ids.begin(), ids.end(), [&](int a, int b) {
        const auto& x = g.edges()[a]; const auto& y = g.edges()[b];
        return x.cost != y.cost ? x.cost < y.cost : a < b;
    });
    UnionFind uf(g.size());
    KruskalResult<Cost> result;
    for (int id : ids) {
        const auto& edge = g.edges()[id];
        if (uf.merge(edge.from, edge.to)) { result.cost += edge.cost; result.edge_ids.push_back(id); }
    }
    result.components = uf.component_count();
    result.connected = g.size() == 0 || result.components == 1;
    return result;
}
} // namespace cp
