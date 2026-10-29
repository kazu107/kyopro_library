// https://github.com/kazu107/kyopro_library/blob/main/tree/MoOnTree.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 静的な木・森の頂点パスに対するオフラインクエリを、頂点の追加/削除と答え記録のコールバックで処理する。
使い方:
  cp::MoOnTree mo(g, root); int id = mo.add_query(u, v);
  mo.run(add_vertex, remove_vertex, record_answer); // record_answer(id)へクエリ番号を渡す。
前提: 0始まり、無向の森。別成分間のクエリはinvalid_argument。重みは使わず、LCAも単位重みのコピーで前処理する。
  入出イベント列は各頂点2回。通常のEulerTourの1回ずつの配列とは異なる。
  状態は空集合から開始し、runの最後に空へ戻す。同じインスタンスで再実行可。コールバックは例外を投げず、集合の追加/削除として扱う。
  頂点/辺の値の途中更新や経路上の順序依存の集約は対象外。手動ブロック幅Bはrunの第4引数、0なら自動。
計算量: 構築O(N log(N+1))、クエリ追加O(log(N+1))、runはO(Q log(Q+1)+(N^2/B+QB+N)U+QA)。
  Uは追加/削除、Aは答え取得の計算量。自動BならLCA補完も含む追加削除回数O(N sqrt(Q)+N+Q)。
  メモリO(N log(N+1)+Q)。
参考問題: https://atcoder.jp/contests/abc477/tasks/abc477_g
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/Lca.hpp

namespace cp {
class MoOnTree {
    struct Query { int l, r, extra, id; };
    Lca<int> lca_;
    std::vector<int> tin_, tout_, tour_;
    std::vector<Query> queries_;
    template <class Cost>
    static Graph<int> unweighted(const Graph<Cost>& g) {
        if (g.size() > std::numeric_limits<int>::max() / 2) throw std::length_error("Euler event array too large");
        Graph<int> result(g.size(), g.is_directed());
        for (const auto& edge : g.edges()) result.add_edge(edge.from, edge.to);
        return result;
    }
public:
    template <class Cost>
    explicit MoOnTree(const Graph<Cost>& g, int root = 0) : lca_(unweighted(g), root) {
        const auto& forest = lca_.forest();
        tin_.resize(g.size()); tout_.resize(g.size());
        std::vector<int> next(g.size(), 0), stack;
        tour_.reserve(2 * g.size());
        for (int start : forest.roots) {
            tin_[start] = static_cast<int>(tour_.size()); tour_.push_back(start); stack.push_back(start);
            while (!stack.empty()) {
                const int v = stack.back();
                if (next[v] == static_cast<int>(forest.children[v].size())) {
                    tout_[v] = static_cast<int>(tour_.size()); tour_.push_back(v); stack.pop_back();
                } else {
                    const int child = forest.children[v][next[v]++];
                    tin_[child] = static_cast<int>(tour_.size()); tour_.push_back(child); stack.push_back(child);
                }
            }
        }
    }
    int size() const { return static_cast<int>(tin_.size()); }
    int query_count() const { return static_cast<int>(queries_.size()); }
    const std::vector<int>& tour() const { return tour_; }
    int add_query(int u, int v) {
        assert(0 <= u && u < size() && 0 <= v && v < size());
        const int ancestor = lca_.lca(u, v);
        if (ancestor == -1) throw std::invalid_argument("Path query crosses forest components");
        if (tin_[u] > tin_[v]) std::swap(u, v);
        const int id = query_count();
        queries_.push_back({tin_[u] + 1, tin_[v] + 1, ancestor, id});
        return id;
    }
    template <class Add, class Remove, class Answer>
    void run(Add add, Remove remove, Answer answer, int block_size = 0) const {
        if (block_size < 0) throw std::invalid_argument("Block size must be nonnegative");
        if (queries_.empty()) return;
        if (block_size == 0) block_size = std::max(1, static_cast<int>(
            tour_.size() / std::sqrt(static_cast<double>(queries_.size()))));
        auto queries = queries_;
        std::sort(queries.begin(), queries.end(), [block_size](const Query& a, const Query& b) {
            const int left_block = a.l / block_size, right_block = b.l / block_size;
            if (left_block != right_block) return left_block < right_block;
            if (a.r != b.r) return left_block & 1 ? a.r > b.r : a.r < b.r;
            return a.id < b.id;
        });
        std::vector<bool> active(size(), false);
        auto toggle = [&](int v) {
            if (active[v]) remove(v); else add(v);
            active[v] = !active[v];
        };
        int left = 0, right = 0;
        for (const auto& query : queries) {
            while (left > query.l) toggle(tour_[--left]);
            while (right < query.r) toggle(tour_[right++]);
            while (left < query.l) toggle(tour_[left++]);
            while (right > query.r) toggle(tour_[--right]);
            assert(!active[query.extra]);
            toggle(query.extra); answer(query.id); toggle(query.extra);
        }
        for (int v = 0; v < size(); ++v) if (active[v]) remove(v);
    }
};
} // namespace cp
