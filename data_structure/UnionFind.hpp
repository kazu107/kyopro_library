// https://github.com/kazu107/kyopro_library/blob/main/data_structure/UnionFind.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 集合の結合、連結判定、成分のサイズ・個数・頂点一覧を管理する。
使い方:
  cp::UnionFind uf(n); bool joined = uf.merge(u, v);
  bool connected = uf.same(u, v); int count = uf.size(u); auto groups = uf.groups();
前提: 頂点は0始まり。mergeは実際に結合したらtrue。辺の削除は対象外。
計算量: 初期化O(N)、通常操作は償却O(alpha(N))、groupsはO(N alpha(N))、メモリO(N)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし


namespace cp {
// UnionFind uf(n); uf.merge(u,v); uf.same(u,v); uf.size(v);
// 0始まり。初期化O(V)、merge/leader/same/sizeは償却O(alpha(V))。
// 辺の削除は不可。groups()はO(V alpha(V))、メモリO(V)。
class UnionFind {
    std::vector<int> parent_;
    int components_;
public:
    explicit UnionFind(int n = 0) : components_(n) { assert(n >= 0); parent_.assign(n, -1); }
    int leader(int v) {
        assert(0 <= v && v < static_cast<int>(parent_.size()));
        int root = v;
        while (parent_[root] >= 0) root = parent_[root];
        while (v != root) {
            const int next = parent_[v];
            parent_[v] = root;
            v = next;
        }
        return root;
    }
    bool merge(int u, int v) {
        u = leader(u); v = leader(v);
        if (u == v) return false;
        if (parent_[u] > parent_[v]) std::swap(u, v);
        parent_[u] += parent_[v];
        parent_[v] = u;
        --components_;
        return true;
    }
    bool same(int u, int v) { return leader(u) == leader(v); }
    int size(int v) { return -parent_[leader(v)]; }
    int component_count() const { return components_; }
    std::vector<std::vector<int>> groups() {
        std::vector<std::vector<int>> buckets(parent_.size()), result;
        for (int v = 0; v < static_cast<int>(parent_.size()); ++v) buckets[leader(v)].push_back(v);
        for (auto& group : buckets) if (!group.empty()) result.push_back(std::move(group));
        return result;
    }
};
} // namespace cp
