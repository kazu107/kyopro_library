// https://github.com/kazu107/kyopro_library/blob/main/graph/lexicographic_bfs.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 最短の辺数を優先し、その中で辺ラベル列が辞書順最小になる経路を全頂点分求める。
使い方:
  auto result = cp::lexicographic_bfs(g, source); // gのcostをラベルとして扱う。
  auto labels = result.labels_to(target); auto vertices = result.path_to(target);
  auto values = cp::lexicographic_path_values(result, 10, 998244353); // 整数ラベルの連結値をmodで取得。
前提: 0始まり、有向・無向・多重辺・自己ループ可。ラベルはコピーと<比較が可能な型。
  distance=-1は未到達。word_rankは同じラベル列なら同値で、到達した最適列の長さ・辞書順に並ぶ。
  ラベル1〜9なら最小の連結整数と一致する。0や負ラベルでは「辺数、その後辞書順」という定義で使う。
  mod値だけを比較して経路を選ばない。valuesは未到達-1、始点0。base>=2、mod>0、各桁0以上base未満。
計算量: 比較・コピーO(1)ならO(V+E log(E+1))、メモリO(V+E)。経路復元O(経路長)、mod値取得O(V)。
参考問題: https://atcoder.jp/contests/abc481/tasks/abc481_f
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp

namespace cp {
template <class Label>
struct LexicographicBfsResult {
    std::vector<int> distance, parent, parent_edge, word_rank, discovery_order;
    std::vector<std::optional<Label>> parent_label;
    explicit LexicographicBfsResult(int n)
        : distance(n, -1), parent(n, -1), parent_edge(n, -1), word_rank(n, -1), parent_label(n) {}
    bool reachable(int v) const {
        assert(0 <= v && v < static_cast<int>(distance.size()));
        return distance[v] != -1;
    }
    std::vector<int> path_to(int v) const {
        if (!reachable(v)) return {};
        std::vector<int> result;
        for (; v != -1; v = parent[v]) result.push_back(v);
        std::reverse(result.begin(), result.end());
        return result;
    }
    std::vector<int> edge_path_to(int v) const {
        if (!reachable(v)) return {};
        std::vector<int> result;
        for (; parent[v] != -1; v = parent[v]) result.push_back(parent_edge[v]);
        std::reverse(result.begin(), result.end());
        return result;
    }
    std::vector<Label> labels_to(int v) const {
        if (!reachable(v)) return {};
        std::vector<Label> result;
        for (; parent[v] != -1; v = parent[v]) result.push_back(*parent_label[v]);
        std::reverse(result.begin(), result.end());
        return result;
    }
};

template <class Label>
LexicographicBfsResult<Label> lexicographic_bfs(const Graph<Label>& g, int source) {
    assert(0 <= source && source < g.size());
    LexicographicBfsResult<Label> result(g.size());
    result.distance[source] = 0; result.word_rank[source] = 0;
    result.discovery_order.push_back(source);
    std::vector<std::vector<int>> groups{{source}};
    struct Candidate { Label label; int from, to, id; };
    for (std::size_t index = 0; index < groups.size(); ++index) {
        std::vector<Candidate> candidates;
        for (int v : groups[index]) for (const auto& edge : g[v])
            if (result.distance[edge.to] == -1) candidates.push_back({edge.cost, v, edge.to, edge.id});
        std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
            if (a.label < b.label) return true;
            if (b.label < a.label) return false;
            return std::tie(a.to, a.from, a.id) < std::tie(b.to, b.from, b.id);
        });
        for (std::size_t first = 0; first < candidates.size();) {
            std::size_t last = first + 1;
            while (last < candidates.size() && !(candidates[first].label < candidates[last].label)
                   && !(candidates[last].label < candidates[first].label)) ++last;
            std::vector<int> next;
            const int rank = static_cast<int>(groups.size());
            for (std::size_t i = first; i < last; ++i) {
                const auto& edge = candidates[i];
                if (result.distance[edge.to] != -1) continue;
                result.distance[edge.to] = result.distance[edge.from] + 1;
                result.parent[edge.to] = edge.from; result.parent_edge[edge.to] = edge.id;
                result.parent_label[edge.to] = edge.label; result.word_rank[edge.to] = rank;
                result.discovery_order.push_back(edge.to); next.push_back(edge.to);
            }
            if (!next.empty()) groups.push_back(std::move(next));
            first = last;
        }
    }
    return result;
}

template <class Label>
std::vector<long long> lexicographic_path_values(const LexicographicBfsResult<Label>& result,
                                               long long base = 10, long long mod = 998244353) {
    static_assert(std::is_integral_v<Label> && sizeof(Label) <= 8);
    if (base < 2 || mod <= 0) throw std::invalid_argument("Invalid base or modulus");
    std::vector<long long> values(result.distance.size(), -1);
    for (int v : result.discovery_order) {
        if (result.parent[v] == -1) { values[v] = 0; continue; }
        const __int128 digit = static_cast<__int128>(*result.parent_label[v]);
        if (digit < 0 || digit >= base) throw std::invalid_argument("Label is not a base digit");
        values[v] = static_cast<long long>((static_cast<__int128>(values[result.parent[v]]) * base + digit) % mod);
    }
    return values;
}
} // namespace cp
