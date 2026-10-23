// https://github.com/kazu107/kyopro_library/blob/main/flow/Dinic.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {
// Dinic<> flow(n); id=flow.add_edge(u,v,capacity); flow.flow(s,t,limit);
// 0始まり、整数の非負容量。flowは追加で流れた量を返し、繰り返し呼べる。
// min_cut(s)は残余到達可能集合。最大まで流した後なら最小カットのs側。
// 多重辺・自己ループ可。再帰なし。一般容量O(V^2 E)、メモリO(V+E)。
// 流量・容量の合計はCapに収める。s!=t。
template <class Cap = long long>
class Dinic {
    struct Arc { int to, reverse; Cap capacity; };
    std::vector<std::vector<Arc>> graph_;
    std::vector<std::pair<int, int>> positions_;
    std::vector<int> level_, next_;
    void check(int v) const { assert(0 <= v && v < static_cast<int>(graph_.size())); }
    bool bfs(int s, int t) {
        level_.assign(graph_.size(), -1);
        std::queue<int> queue; queue.push(s); level_[s] = 0;
        while (!queue.empty()) {
            const int v = queue.front(); queue.pop();
            for (const auto& edge : graph_[v]) if (edge.capacity > 0 && level_[edge.to] == -1) {
                level_[edge.to] = level_[v] + 1; queue.push(edge.to);
            }
        }
        return level_[t] != -1;
    }
    Cap augment(int s, int t, Cap limit) {
        std::vector<int> vertices{s}, path;
        std::vector<Cap> bottleneck{limit};
        while (!vertices.empty()) {
            const int v = vertices.back();
            if (v == t) {
                const Cap sent = bottleneck.back();
                for (int i = 0; i < static_cast<int>(path.size()); ++i) {
                    auto& edge = graph_[vertices[i]][path[i]];
                    edge.capacity -= sent; graph_[edge.to][edge.reverse].capacity += sent;
                }
                return sent;
            }
            bool advanced = false;
            for (int& i = next_[v]; i < static_cast<int>(graph_[v].size()); ++i) {
                const auto& edge = graph_[v][i];
                if (edge.capacity <= 0 || level_[edge.to] != level_[v] + 1) continue;
                path.push_back(i); vertices.push_back(edge.to);
                bottleneck.push_back(std::min(bottleneck.back(), edge.capacity));
                advanced = true; break;
            }
            if (!advanced) {
                level_[v] = -1; vertices.pop_back(); bottleneck.pop_back();
                if (!path.empty()) { path.pop_back(); ++next_[vertices.back()]; }
            }
        }
        return 0;
    }
public:
    struct Edge { int from, to; Cap capacity, flow; };
    explicit Dinic(int n = 0) {
        static_assert(std::is_integral_v<Cap> && std::is_signed_v<Cap>);
        assert(n >= 0); graph_.resize(n);
    }
    int add_edge(int from, int to, Cap capacity) {
        check(from); check(to); assert(capacity >= 0);
        const int id = static_cast<int>(positions_.size());
        const int forward = static_cast<int>(graph_[from].size());
        int reverse = static_cast<int>(graph_[to].size());
        if (from == to) ++reverse;
        positions_.push_back({from, forward});
        graph_[from].push_back({to, reverse, capacity});
        graph_[to].push_back({from, forward, 0});
        return id;
    }
    Cap flow(int s, int t, Cap limit = std::numeric_limits<Cap>::max()) {
        check(s); check(t); assert(s != t && limit >= 0);
        Cap result = 0;
        while (result < limit && bfs(s, t)) {
            next_.assign(graph_.size(), 0);
            while (result < limit) {
                const Cap sent = augment(s, t, limit - result);
                if (sent == 0) break;
                result += sent;
            }
        }
        return result;
    }
    std::vector<bool> min_cut(int s) const {
        check(s);
        std::vector<bool> visited(graph_.size(), false);
        std::queue<int> queue; queue.push(s); visited[s] = true;
        while (!queue.empty()) {
            const int v = queue.front(); queue.pop();
            for (const auto& edge : graph_[v]) if (edge.capacity > 0 && !visited[edge.to]) {
                visited[edge.to] = true; queue.push(edge.to);
            }
        }
        return visited;
    }
    Edge get_edge(int id) const {
        assert(0 <= id && id < static_cast<int>(positions_.size()));
        const auto [from, index] = positions_[id];
        const auto& edge = graph_[from][index];
        const auto& reverse = graph_[edge.to][edge.reverse];
        return {from, edge.to, edge.capacity + reverse.capacity, reverse.capacity};
    }
    std::vector<Edge> edges() const {
        std::vector<Edge> result;
        for (int id = 0; id < static_cast<int>(positions_.size()); ++id) result.push_back(get_edge(id));
        return result;
    }
};
} // namespace cp
