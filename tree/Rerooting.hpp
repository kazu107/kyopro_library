// https://github.com/kazu107/kyopro_library/blob/main/tree/Rerooting.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: Policyで演算を指定し、全頂点を根にした木DPをまとめて求める。省略時は各頂点からの最大距離。
使い方:
  cp::Rerooting<> dp(g); auto answer = dp.build(root);
  cp::Rerooting<MyPolicy> custom(n); custom.add_edge(u, v, cost); auto values = custom.build();
前提: 無向の木・森。PolicyはDP/identity/merge/lift/add_vertexを定義する。mergeは結合則と単位元を持ち、隣接リスト順で結合する。
計算量: 各演算・DPコピーがO(1)なら全頂点分O(V+E)、メモリO(V+E)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/RootedForest.hpp


namespace cp {
struct MaxDistanceRerooting {
    using DP = long long;
    static DP identity() { return 0; }
    static DP merge(const DP& a, const DP& b) { return std::max(a, b); }
    template <class Cost>
    static DP lift(const DP& value, const Edge<Cost>& edge) { return value + edge.cost; }
    static DP add_vertex(const DP& value, int) { return value; }
};

// Rerooting<MyPolicy> dp(n); dp.add_edge(u,v,cost); auto answers=dp.build(root);
// またはRerooting<MyPolicy,Cost> dp(g)。森対応、反復処理。
// Policy: DP, identity(), merge(a,b), lift(dp, incoming_edge), add_vertex(dp,v)。
// mergeは結合則・単位元を持つ。隣接リスト順を保持し、可換でなくても可。
// liftに渡す辺はDPを送る頂点from -> 受け取る頂点toの向き。
// 各演算O(1)なら全頂点分O(V+E)、メモリO(V+E)。数値はDP型に収める。
// 省略時は自分自身までの距離0も含む、各頂点からの最大距離。
template <class Policy = MaxDistanceRerooting, class Cost = long long>
class Rerooting {
    Graph<Cost> graph_;
public:
    using DP = typename Policy::DP;
    std::vector<DP> down, ans;
    explicit Rerooting(int n = 0) : graph_(n, false) {}
    explicit Rerooting(const Graph<Cost>& g) : graph_(g) {}
    int add_edge(int u, int v, Cost cost = Cost{1}) { return graph_.add_edge(u, v, cost); }
    const Graph<Cost>& graph() const { return graph_; }
    std::vector<DP> build(int root = 0) {
        RootedForest<Cost> forest(graph_, root);
        const int n = graph_.size();
        down.assign(n, Policy::identity()); ans.assign(n, Policy::identity());
        std::vector<DP> incoming(n, Policy::identity());
        for (auto it = forest.order.rbegin(); it != forest.order.rend(); ++it) {
            const int v = *it;
            DP acc = Policy::identity();
            for (const auto& edge : graph_[v]) if (forest.parent[edge.to] == v) {
                const Edge<Cost> reverse{edge.to, v, edge.cost, edge.id};
                acc = Policy::merge(acc, Policy::lift(down[edge.to], reverse));
            }
            down[v] = Policy::add_vertex(acc, v);
        }
        for (int v : forest.order) {
            const int degree = static_cast<int>(graph_[v].size());
            std::vector<DP> values(degree, Policy::identity());
            std::vector<DP> prefix(degree + 1, Policy::identity()), suffix(degree + 1, Policy::identity());
            for (int i = 0; i < degree; ++i) {
                const auto& edge = graph_[v][i];
                values[i] = edge.to == forest.parent[v] ? incoming[v]
                    : Policy::lift(down[edge.to], Edge<Cost>{edge.to, v, edge.cost, edge.id});
                prefix[i + 1] = Policy::merge(prefix[i], values[i]);
            }
            for (int i = degree - 1; i >= 0; --i) suffix[i] = Policy::merge(values[i], suffix[i + 1]);
            ans[v] = Policy::add_vertex(prefix[degree], v);
            for (int i = 0; i < degree; ++i) {
                const auto& edge = graph_[v][i];
                if (edge.to == forest.parent[v]) continue;
                const DP without_child = Policy::merge(prefix[i], suffix[i + 1]);
                incoming[edge.to] = Policy::lift(Policy::add_vertex(without_child, v), edge);
            }
        }
        return ans;
    }
};
} // namespace cp
