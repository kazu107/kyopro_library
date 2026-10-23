// https://github.com/kazu107/kyopro_library/blob/main/graph/Graph.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <cassert>
#include <limits>
#include <type_traits>
#include <vector>

namespace cp {
// 0始まり。無向辺もedges()では1本、隣接リストでは両方向に保持する。
// Graph<long long> g(n, false); int id = g.add_edge(u, v, cost);
// 構築O(V)、辺追加は償却O(1)、メモリO(V+E)。多重辺・自己ループ可。
template <class Cost = long long>
struct Edge {
    int from, to;
    Cost cost;
    int id;
};

template <class Cost = long long>
class Graph {
    bool directed_;
    std::vector<std::vector<Edge<Cost>>> adjacency_;
    std::vector<Edge<Cost>> edges_;
public:
    explicit Graph(int n = 0, bool directed = false) : directed_(directed) {
        assert(n >= 0);
        adjacency_.resize(n);
    }
    int size() const { return static_cast<int>(adjacency_.size()); }
    int edge_count() const { return static_cast<int>(edges_.size()); }
    bool is_directed() const { return directed_; }
    int add_edge(int from, int to, Cost cost = Cost{1}) {
        assert(0 <= from && from < size() && 0 <= to && to < size());
        const int id = edge_count();
        edges_.push_back({from, to, cost, id});
        adjacency_[from].push_back({from, to, cost, id});
        if (!directed_) adjacency_[to].push_back({to, from, cost, id});
        return id;
    }
    const std::vector<Edge<Cost>>& operator[](int v) const {
        assert(0 <= v && v < size());
        return adjacency_[v];
    }
    const std::vector<Edge<Cost>>& edges() const { return edges_; }
};

// 最短路は64bit以下の符号付き整数に対応。有限距離・単純路の途中の和は
// (-distance_inf<Cost>(), distance_inf<Cost>()) に収める。
template <class Cost>
constexpr Cost distance_inf() {
    static_assert(std::is_integral_v<Cost> && std::is_signed_v<Cost> && sizeof(Cost) <= 8);
    return std::numeric_limits<Cost>::max() / 4;
}
template <class Cost>
Cost saturated_distance_add(Cost a, Cost b) {
    const __int128 sum = static_cast<__int128>(a) + b;
    const Cost inf = distance_inf<Cost>();
    if (sum >= inf) return inf;
    if (sum <= -static_cast<__int128>(inf)) return -inf;
    return static_cast<Cost>(sum);
}
} // namespace cp
