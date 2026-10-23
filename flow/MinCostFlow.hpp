// https://github.com/kazu107/kyopro_library/blob/main/flow/MinCostFlow.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {
// MinCostFlow<> f(n); f.add_edge(u,v,capacity,cost); auto [sent,cost]=f.flow(s,t,limit);
// 整数容量・整数費用、0始まり。負費用可。ただし始点から残余辺で到達できる
// 負閉路がある入力はdomain_error(負閉路を含む循環流の最適化は対象外)。
// 毎回初期Bellman-Fordを行うのでflowの再呼び出しも可。返すのは追加分。
// O(VE + A(V+E)log(V+E))、Aは増加路の本数。メモリO(V+E)。
// 容量はCap、単価/経路費用/返す総費用はCostに収める。内部計算は__int128。
// 多重辺・自己ループ可。s!=t。0容量の負費用辺は負閉路判定に影響しない。
template <class Cap = long long, class Cost = long long>
class MinCostFlow {
    using Wide = __int128;
    struct Arc { int to, reverse; Cap capacity; Cost cost; };
    std::vector<std::vector<Arc>> graph_;
    std::vector<std::pair<int, int>> positions_;
    void check(int v) const { assert(0 <= v && v < static_cast<int>(graph_.size())); }
public:
    struct Edge { int from, to; Cap capacity, flow; Cost cost; };
    explicit MinCostFlow(int n = 0) {
        static_assert(std::is_integral_v<Cap> && std::is_signed_v<Cap> && sizeof(Cap) <= 8);
        static_assert(std::is_integral_v<Cost> && std::is_signed_v<Cost> && sizeof(Cost) <= 8);
        assert(n >= 0); graph_.resize(n);
    }
    int add_edge(int from, int to, Cap capacity, Cost cost) {
        check(from); check(to); assert(capacity >= 0);
        assert(cost != std::numeric_limits<Cost>::lowest());
        const int id = static_cast<int>(positions_.size());
        const int forward = static_cast<int>(graph_[from].size());
        int reverse = static_cast<int>(graph_[to].size());
        if (from == to) ++reverse;
        positions_.push_back({from, forward});
        graph_[from].push_back({to, reverse, capacity, cost});
        graph_[to].push_back({from, forward, 0, static_cast<Cost>(-cost)});
        return id;
    }
    std::pair<Cap, Cost> flow(int s, int t, Cap limit = std::numeric_limits<Cap>::max()) {
        check(s); check(t); assert(s != t && limit >= 0);
        if (limit == 0) return {0, 0};
        const int n = static_cast<int>(graph_.size());
        const Wide inf = static_cast<Wide>(1) << 120;
        std::vector<Wide> potential(n, inf); potential[s] = 0;
        for (int iteration = 0; iteration < n; ++iteration) {
            bool changed = false;
            for (int v = 0; v < n; ++v) if (potential[v] != inf)
                for (const auto& edge : graph_[v]) if (edge.capacity > 0 && potential[edge.to] > potential[v] + edge.cost) {
                    potential[edge.to] = potential[v] + edge.cost; changed = true;
                    if (iteration == n - 1) throw std::domain_error("Reachable negative residual cycle");
                }
            if (!changed) break;
        }
        for (auto& p : potential) if (p == inf) p = 0;
        Cap total_flow = 0;
        Wide total_cost = 0;
        while (total_flow < limit) {
            std::vector<Wide> distance(n, inf);
            std::vector<int> parent(n, -1), parent_edge(n, -1);
            using State = std::pair<Wide, int>;
            std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
            distance[s] = 0; queue.push({0, s});
            while (!queue.empty()) {
                const auto [d, v] = queue.top(); queue.pop();
                if (d != distance[v]) continue;
                for (int i = 0; i < static_cast<int>(graph_[v].size()); ++i) {
                    const auto& edge = graph_[v][i];
                    if (edge.capacity <= 0) continue;
                    const Wide reduced = static_cast<Wide>(edge.cost) + potential[v] - potential[edge.to];
                    assert(reduced >= 0);
                    if (d + reduced >= distance[edge.to]) continue;
                    distance[edge.to] = d + reduced; parent[edge.to] = v; parent_edge[edge.to] = i;
                    queue.push({distance[edge.to], edge.to});
                }
            }
            if (distance[t] == inf) break;
            for (int v = 0; v < n; ++v) if (distance[v] != inf) potential[v] += distance[v];
            Cap sent = limit - total_flow;
            Wide path_cost = 0;
            for (int v = t; v != s; v = parent[v]) {
                const auto& edge = graph_[parent[v]][parent_edge[v]];
                sent = std::min(sent, edge.capacity); path_cost += edge.cost;
            }
            assert(std::numeric_limits<Cost>::lowest() <= path_cost && path_cost <= std::numeric_limits<Cost>::max());
            total_cost += static_cast<Wide>(sent) * path_cost;
            assert(std::numeric_limits<Cost>::lowest() <= total_cost && total_cost <= std::numeric_limits<Cost>::max());
            for (int v = t; v != s; v = parent[v]) {
                auto& edge = graph_[parent[v]][parent_edge[v]];
                edge.capacity -= sent; graph_[edge.to][edge.reverse].capacity += sent;
            }
            total_flow += sent;
        }
        return {total_flow, static_cast<Cost>(total_cost)};
    }
    Edge get_edge(int id) const {
        assert(0 <= id && id < static_cast<int>(positions_.size()));
        const auto [from, index] = positions_[id];
        const auto& edge = graph_[from][index];
        const auto& reverse = graph_[edge.to][edge.reverse];
        return {from, edge.to, edge.capacity + reverse.capacity, reverse.capacity, edge.cost};
    }
    std::vector<Edge> edges() const {
        std::vector<Edge> result;
        for (int id = 0; id < static_cast<int>(positions_.size()); ++id) result.push_back(get_edge(id));
        return result;
    }
};
} // namespace cp
