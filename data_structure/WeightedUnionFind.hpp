// https://github.com/kazu107/kyopro_library/blob/main/data_structure/WeightedUnionFind.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: value[v]-value[u]=wという差の制約を管理し、矛盾の有無や2頂点間の差を取得する。
使い方:
  cp::WeightedUnionFind<long long> uf(n); bool consistent = uf.merge(u, v, w);
  auto difference = uf.diff(u, v); // 非連結ならnullopt。連結ならvalue[v]-value[u]。
前提: 頂点は0始まり。mergeのtrueは整合性を示し、既に同じ成分の場合も検査する。差と途中計算はT型の範囲内。
計算量: 初期化O(N)、通常操作は償却O(alpha(N))、メモリO(N)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし


namespace cp {
// value[v]-value[u]=wという差の制約を管理。0始まり。
// merge(u,v,w)は矛盾しなければtrue。同じ成分でも整合性を検査する。
// diff(u,v)は非連結ならnullopt。差・合成中の値はTに収める。
// 初期化O(V)、操作は償却O(alpha(V))、メモリO(V)。
template <class T = long long>
class WeightedUnionFind {
    std::vector<int> parent_;
    std::vector<T> weight_; // value[v]-value[parent[v]]
    int components_;
public:
    explicit WeightedUnionFind(int n = 0) : components_(n) {
        assert(n >= 0); parent_.assign(n, -1); weight_.assign(n, T{});
    }
    int leader(int v) {
        assert(0 <= v && v < static_cast<int>(parent_.size()));
        if (parent_[v] < 0) return v;
        const int p = parent_[v];
        parent_[v] = leader(p); // union by sizeなので圧縮前の再帰もO(log V)
        weight_[v] += weight_[p];
        return parent_[v];
    }
    T potential(int v) { leader(v); return weight_[v]; }
    bool merge(int u, int v, T difference) {
        difference += potential(u);
        difference -= potential(v);
        u = leader(u); v = leader(v);
        if (u == v) return difference == T{};
        if (parent_[u] > parent_[v]) { std::swap(u, v); difference = -difference; }
        parent_[u] += parent_[v];
        parent_[v] = u;
        weight_[v] = difference;
        --components_;
        return true;
    }
    bool same(int u, int v) { return leader(u) == leader(v); }
    std::optional<T> diff(int u, int v) {
        if (!same(u, v)) return std::nullopt;
        return potential(v) - potential(u);
    }
    int size(int v) { return -parent_[leader(v)]; }
    int component_count() const { return components_; }
};
} // namespace cp
