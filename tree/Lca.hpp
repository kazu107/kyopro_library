// https://github.com/kazu107/kyopro_library/blob/main/tree/Lca.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 最近共通祖先、k個上の祖先、2頂点間の距離、経路上k番目の頂点を求める。
使い方:
  cp::Lca<long long> index(g, root); int ancestor = index.lca(u, v);
  auto distance = index.distance(u, v); int node = index.kth_on_path(u, v, k); // uが0番目。
前提: 0始まり、無向の木・森。別成分のlca/kth_on_pathは-1、distanceはnullopt。kth_ancestorの範囲外も-1。途中計算をCostに収める。
計算量: 構築O(V log V)、取得O(log V)、メモリO(V log V)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/RootedForest.hpp


namespace cp {
// Lca<> lca(g,root); lca.lca(u,v); lca.kth_ancestor(v,k); lca.distance(u,v);
// 森にも対応。別成分のlca/kth_on_pathは-1、distanceはnullopt。
// 構築O(V log V)、クエリO(log V)、メモリO(V log V)。再帰しない。
// kth_on_pathはuを0番目とする。重みの和・途中計算はCostに収める。
template <class Cost = long long>
class Lca {
    RootedForest<Cost> forest_;
    std::vector<std::vector<int>> up_;
    std::vector<Cost> distance_;
    void check(int v) const { assert(0 <= v && v < static_cast<int>(forest_.parent.size())); }
public:
    explicit Lca(const Graph<Cost>& g, int root = 0) : forest_(g, root), distance_(g.size(), Cost{}) {
        int log = 1;
        while ((1LL << log) <= g.size()) ++log;
        up_.assign(log, std::vector<int>(g.size(), -1));
        up_[0] = forest_.parent;
        for (int v : forest_.order) if (forest_.parent[v] != -1)
            distance_[v] = distance_[forest_.parent[v]] + forest_.parent_cost[v];
        for (int k = 1; k < log; ++k) for (int v = 0; v < g.size(); ++v)
            if (up_[k - 1][v] != -1) up_[k][v] = up_[k - 1][up_[k - 1][v]];
    }
    const RootedForest<Cost>& forest() const { return forest_; }
    int kth_ancestor(int v, long long k) const {
        check(v);
        if (k < 0 || k > forest_.depth[v]) return -1;
        for (int bit = 0; k != 0; ++bit, k >>= 1) if (k & 1) v = up_[bit][v];
        return v;
    }
    int lca(int u, int v) const {
        check(u); check(v);
        if (forest_.component[u] != forest_.component[v]) return -1;
        if (forest_.depth[u] < forest_.depth[v]) std::swap(u, v);
        u = kth_ancestor(u, forest_.depth[u] - forest_.depth[v]);
        if (u == v) return u;
        for (int k = static_cast<int>(up_.size()) - 1; k >= 0; --k)
            if (up_[k][u] != up_[k][v]) { u = up_[k][u]; v = up_[k][v]; }
        return forest_.parent[u];
    }
    std::optional<Cost> distance(int u, int v) const {
        const int w = lca(u, v);
        if (w == -1) return std::nullopt;
        return (distance_[u] - distance_[w]) + (distance_[v] - distance_[w]);
    }
    int kth_on_path(int u, int v, long long k) const {
        const int w = lca(u, v);
        if (w == -1 || k < 0) return -1;
        const int left = forest_.depth[u] - forest_.depth[w];
        const int right = forest_.depth[v] - forest_.depth[w];
        if (k > static_cast<long long>(left) + right) return -1;
        return k <= left ? kth_ancestor(u, k) : kth_ancestor(v, left + right - k);
    }
};
} // namespace cp
