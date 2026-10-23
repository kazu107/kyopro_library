// https://github.com/kazu107/kyopro_library/blob/main/data_structure/RollbackUnionFind.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 集合の結合を履歴に記録し、1回分または保存した状態まで巻き戻す。
使い方:
  cp::RollbackUnionFind uf(n); auto token = uf.snapshot(); uf.merge(u, v);
  uf.rollback(token); // 保存した状態へ戻す。uf.undo()はmerge呼び出し1回分を取り消す。
前提: 頂点は0始まり。結合しなかったmergeも履歴1回に数える。undoには履歴が必要。tokenは現在の履歴の祖先のみ。
計算量: 結合・検索O(log N)、undo O(1)、rollbackは戻す回数に比例。メモリO(N+履歴数)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし


namespace cp {
// 結合を巻き戻せるUnion-Find。0始まり。経路圧縮を行わない。
// auto token=uf.snapshot(); uf.merge(u,v); uf.rollback(token);
// undo()はmerge呼び出し1回分(結合しなかった場合も含む)を取り消す。
// merge/leader/same/sizeはO(log V)、undoはO(1)、rollbackは戻す回数に比例。
// メモリO(V+保持するmerge呼び出し数)。tokenは現在の履歴の祖先に限る。
class RollbackUnionFind {
    struct Change { int u, pu, v, pv; };
    std::vector<int> parent_;
    std::vector<Change> history_;
    int components_;
public:
    explicit RollbackUnionFind(int n = 0) : components_(n) { assert(n >= 0); parent_.assign(n, -1); }
    int leader(int v) const {
        assert(0 <= v && v < static_cast<int>(parent_.size()));
        while (parent_[v] >= 0) v = parent_[v];
        return v;
    }
    bool merge(int u, int v) {
        u = leader(u); v = leader(v);
        if (u == v) { history_.push_back({-1, 0, -1, 0}); return false; }
        if (parent_[u] > parent_[v]) std::swap(u, v);
        history_.push_back({u, parent_[u], v, parent_[v]});
        parent_[u] += parent_[v]; parent_[v] = u; --components_;
        return true;
    }
    bool same(int u, int v) const { return leader(u) == leader(v); }
    int size(int v) const { return -parent_[leader(v)]; }
    int component_count() const { return components_; }
    std::size_t snapshot() const { return history_.size(); }
    void undo() {
        assert(!history_.empty());
        const auto change = history_.back(); history_.pop_back();
        if (change.u == -1) return;
        parent_[change.u] = change.pu; parent_[change.v] = change.pv; ++components_;
    }
    void rollback(std::size_t token) {
        assert(token <= history_.size());
        while (history_.size() > token) undo();
    }
};
} // namespace cp
