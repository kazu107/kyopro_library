// https://github.com/kazu107/kyopro_library/blob/main/tree/TreePathFrequencyQueries.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 木の頂点パス上で、出現回数が指定した閉区間[low,high]に入る値の種類数をまとめて取得する。
使い方:
  cp::TreePathFrequencyQueries<int> queries(g, values);
  int id = queries.add_query(u, v, 1, n); auto answers = queries.solve(); // id番目の答え。
前提: 0始まり、無向の静的な木・森。valuesは各頂点の値で、<と==で比較できる型。値の数を頂点数に一致させる。
  low>=1、high>=low。出現0回の値は対象外。別成分間のパスはinvalid_argument。同じ頂点のクエリ・繰り返しsolve可。
計算量: 構築O(N log(N+1))、クエリ追加O(log(N+1))、solveはO(N sqrt(Q)+N+Q log(Q+1))。
  頻度1増減と答え取得はO(1)。メモリO(N log(N+1)+Q)。Q=0なら空の答え。
参考問題: https://atcoder.jp/contests/abc477/tasks/abc477_g
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/MoOnTree.hpp

namespace cp {
template <class Label = long long>
class TreePathFrequencyQueries {
    MoOnTree mo_;
    std::vector<int> color_;
    std::vector<std::pair<int, int>> bounds_;
    int color_count_;
public:
    template <class Cost>
    TreePathFrequencyQueries(const Graph<Cost>& g, const std::vector<Label>& values, int root = 0)
        : mo_(g, root) {
        if (static_cast<int>(values.size()) != g.size()) throw std::invalid_argument("Vertex label size mismatch");
        auto compressed = values;
        std::sort(compressed.begin(), compressed.end());
        compressed.erase(std::unique(compressed.begin(), compressed.end()), compressed.end());
        color_count_ = static_cast<int>(compressed.size()); color_.resize(g.size());
        for (int v = 0; v < g.size(); ++v)
            color_[v] = static_cast<int>(std::lower_bound(compressed.begin(), compressed.end(), values[v]) - compressed.begin());
    }
    int add_query(int u, int v, int low, int high) {
        if (low < 1 || high < low) throw std::invalid_argument("Expected a positive frequency range");
        const int id = mo_.add_query(u, v);
        bounds_.push_back({low, high});
        return id;
    }
    int query_count() const { return mo_.query_count(); }
    std::vector<int> solve() const {
        const int n = mo_.size();
        std::vector<int> frequency(color_count_, 0), at_least(n + 2, 0), answer(query_count());
        auto add = [&](int v) { ++at_least[++frequency[color_[v]]]; };
        auto remove = [&](int v) { --at_least[frequency[color_[v]]--]; };
        mo_.run(add, remove, [&](int id) {
            const auto [low, high] = bounds_[id];
            answer[id] = low > n ? 0 : at_least[low] - at_least[std::min(high, n) + 1];
        });
        return answer;
    }
};
} // namespace cp
