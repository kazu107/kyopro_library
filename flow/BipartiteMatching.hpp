// https://github.com/kazu107/kyopro_library/blob/main/flow/BipartiteMatching.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace cp {
// 左右別々に0始まり。BipartiteMatching m(left_n,right_n); m.add_edge(l,r);
// solve()で最大マッチング数、pairs()で対応。solveは毎回最初から計算。
// solve後のminimum_vertex_cover()は左右の頂点番号リストを返す(Konig)。
// 多重辺可。反復Hopcroft-Karp O((V+E)sqrt(V+1))、メモリO(V+E)。
class BipartiteMatching {
    int left_size_, right_size_;
    std::vector<std::vector<int>> graph_;
    std::vector<int> distance_, next_;
    bool solved_ = false;
    bool augment(int start, int shortest) {
        std::vector<int> stack{start}, right_path;
        while (!stack.empty()) {
            const int u = stack.back();
            bool advanced = false;
            while (next_[u] < static_cast<int>(graph_[u].size())) {
                const int v = graph_[u][next_[u]++];
                const int partner = right_match[v];
                if (partner == -1 && distance_[u] + 1 == shortest) {
                    left_match[u] = v; right_match[v] = u;
                    for (int i = static_cast<int>(right_path.size()) - 1; i >= 0; --i) {
                        left_match[stack[i]] = right_path[i]; right_match[right_path[i]] = stack[i];
                    }
                    return true;
                }
                if (partner != -1 && distance_[partner] == distance_[u] + 1 && distance_[partner] < shortest) {
                    stack.push_back(partner); right_path.push_back(v); advanced = true; break;
                }
            }
            if (!advanced) {
                distance_[u] = -1; stack.pop_back();
                if (!right_path.empty()) right_path.pop_back();
            }
        }
        return false;
    }
public:
    std::vector<int> left_match, right_match;
    explicit BipartiteMatching(int left_n = 0, int right_n = 0) : left_size_(left_n), right_size_(right_n) {
        assert(left_n >= 0 && right_n >= 0); graph_.resize(left_n);
    }
    void add_edge(int left, int right) {
        assert(0 <= left && left < left_size_ && 0 <= right && right < right_size_);
        graph_[left].push_back(right); solved_ = false;
    }
    int solve() {
        left_match.assign(left_size_, -1); right_match.assign(right_size_, -1);
        int matching = 0;
        while (true) {
            distance_.assign(left_size_, -1);
            std::queue<int> queue;
            for (int u = 0; u < left_size_; ++u) if (left_match[u] == -1) { distance_[u] = 0; queue.push(u); }
            int shortest = std::numeric_limits<int>::max();
            while (!queue.empty()) {
                const int u = queue.front(); queue.pop();
                if (distance_[u] >= shortest) continue;
                for (int v : graph_[u]) {
                    const int partner = right_match[v];
                    if (partner == -1) shortest = std::min(shortest, distance_[u] + 1);
                    else if (distance_[partner] == -1) { distance_[partner] = distance_[u] + 1; queue.push(partner); }
                }
            }
            if (shortest == std::numeric_limits<int>::max()) break;
            next_.assign(left_size_, 0);
            for (int u = 0; u < left_size_; ++u)
                if (left_match[u] == -1 && distance_[u] == 0 && augment(u, shortest)) ++matching;
        }
        solved_ = true;
        return matching;
    }
    std::vector<std::pair<int, int>> pairs() const {
        assert(solved_);
        std::vector<std::pair<int, int>> result;
        for (int u = 0; u < left_size_; ++u) if (left_match[u] != -1) result.push_back({u, left_match[u]});
        return result;
    }
    std::pair<std::vector<int>, std::vector<int>> minimum_vertex_cover() const {
        assert(solved_);
        std::vector<bool> left_seen(left_size_, false), right_seen(right_size_, false);
        std::queue<int> queue;
        for (int u = 0; u < left_size_; ++u) if (left_match[u] == -1) { left_seen[u] = true; queue.push(u); }
        while (!queue.empty()) {
            const int u = queue.front(); queue.pop();
            for (int v : graph_[u]) if (left_match[u] != v && !right_seen[v]) {
                right_seen[v] = true;
                if (right_match[v] != -1 && !left_seen[right_match[v]]) {
                    left_seen[right_match[v]] = true; queue.push(right_match[v]);
                }
            }
        }
        std::pair<std::vector<int>, std::vector<int>> result;
        for (int u = 0; u < left_size_; ++u) if (!left_seen[u]) result.first.push_back(u);
        for (int v = 0; v < right_size_; ++v) if (right_seen[v]) result.second.push_back(v);
        return result;
    }
};
} // namespace cp
