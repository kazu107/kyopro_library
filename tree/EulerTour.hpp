// https://github.com/kazu107/kyopro_library/blob/main/tree/EulerTour.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 部分木を配列上の連続区間へ変換し、祖先判定や位置からの頂点復元を行う。
使い方:
  cp::EulerTour<long long> tour(g, root); auto [l, r] = tour.subtree_range(v);
  bool ancestor = tour.is_ancestor(u, v); int node = tour.vertex_at[position];
前提: 0始まり、無向の木・森。部分木は[tin[v],tout[v])。各頂点を1回だけ配列に並べる。別成分の祖先判定はfalse。
計算量: 構築O(V+E)、取得・祖先判定O(1)、メモリO(V+E)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/RootedForest.hpp


namespace cp {
// 部分木を配列上の半開区間[tin[v],tout[v])へ変換する(各頂点1回)。
// EulerTour<> tour(g,root); tour.subtree_range(v); tour.vertex_at[index];
// 森可、別成分のis_ancestorはfalse。構築O(V)、問い合わせO(1)、メモリO(V)。
template <class Cost = long long>
struct EulerTour {
    std::vector<int> tin, tout, vertex_at, parent, depth, component, subtree_size;
    explicit EulerTour(const Graph<Cost>& g, int root = 0) {
        RootedForest<Cost> forest(g, root);
        parent = forest.parent; depth = forest.depth; component = forest.component;
        subtree_size = forest.subtree_size; vertex_at = forest.order;
        tin.resize(g.size()); tout.resize(g.size());
        for (int i = 0; i < g.size(); ++i) tin[vertex_at[i]] = i;
        for (int v = 0; v < g.size(); ++v) tout[v] = tin[v] + subtree_size[v];
    }
    std::pair<int, int> subtree_range(int v) const {
        assert(0 <= v && v < static_cast<int>(tin.size()));
        return {tin[v], tout[v]};
    }
    bool is_ancestor(int u, int v) const {
        assert(0 <= u && u < static_cast<int>(tin.size()) && 0 <= v && v < static_cast<int>(tin.size()));
        return component[u] == component[v] && tin[u] <= tin[v] && tin[v] < tout[u];
    }
};
} // namespace cp
