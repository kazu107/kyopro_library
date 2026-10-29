// https://github.com/kazu107/kyopro_library/blob/main/graph/min_first_search.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 到達可能な未処理の候補から最小優先度の頂点を選ぶ探索と、指定した番号優先探索順になるラベル付き木の数え上げ。
使い方:
  auto result = cp::min_first_search(g, source); // 標準優先度は頂点番号。
  auto custom = cp::min_first_search(g, source, priorities); // 同値なら頂点番号が小さい方。
  auto count = cp::count_min_first_trees(permutation, 998244353);
前提: 探索は有向・無向とも可、0始まりで辺重みは無視する。orderは処理順、positionはその逆、parent/parent_edgeは発見時の探索木。
  最短距離を求める探索ではない。未到達のposition/parent/parent_edgeは-1。
  数え上げは0..N-1の順列とmod>0。根は順列の先頭。空順列は1通りと定義。重みや独自優先度は数え上げに使わない。
計算量: 探索O(E+V log(V+1))、数え上げO(N)、ともにメモリO(V)またはO(N)。剰余の積は__int128を使用。
参考問題: https://atcoder.jp/contests/abc478/tasks/abc478_f
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp

namespace cp {
struct MinFirstSearchResult {
    std::vector<int> order, position, parent, parent_edge;
};

template <class Cost, class Priority>
MinFirstSearchResult min_first_search(const Graph<Cost>& g, int source,
                                     const std::vector<Priority>& priority) {
    assert(0 <= source && source < g.size());
    if (static_cast<int>(priority.size()) != g.size()) throw std::invalid_argument("Priority size mismatch");
    MinFirstSearchResult result{{}, std::vector<int>(g.size(), -1),
                               std::vector<int>(g.size(), -1), std::vector<int>(g.size(), -1)};
    using State = std::pair<Priority, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> frontier;
    std::vector<bool> queued(g.size(), false);
    queued[source] = true; frontier.push({priority[source], source});
    while (!frontier.empty()) {
        const int v = frontier.top().second; frontier.pop();
        result.position[v] = static_cast<int>(result.order.size()); result.order.push_back(v);
        for (const auto& edge : g[v]) if (!queued[edge.to]) {
            queued[edge.to] = true; result.parent[edge.to] = v; result.parent_edge[edge.to] = edge.id;
            frontier.push({priority[edge.to], edge.to});
        }
    }
    return result;
}
template <class Cost>
MinFirstSearchResult min_first_search(const Graph<Cost>& g, int source) {
    std::vector<int> priority(g.size());
    std::iota(priority.begin(), priority.end(), 0);
    return min_first_search(g, source, priority);
}

inline long long count_min_first_trees(const std::vector<int>& order, long long mod = 998244353) {
    if (mod <= 0) throw std::invalid_argument("Modulus must be positive");
    const int n = static_cast<int>(order.size());
    std::vector<bool> seen(n, false);
    for (int v : order) {
        if (v < 0 || v >= n || seen[v]) throw std::invalid_argument("Expected a permutation of 0..N-1");
        seen[v] = true;
    }
    long long answer = 1 % mod;
    std::vector<int> stack;
    if (n != 0) stack.push_back(0); // 先頭は親候補の下限として残す。
    for (int i = 1; i < n; ++i) {
        while (stack.size() > 1 && order[stack.back()] < order[i]) stack.pop_back();
        answer = static_cast<long long>(static_cast<__int128>(answer) * (i - stack.back()) % mod);
        stack.push_back(i);
    }
    return answer;
}
} // namespace cp
