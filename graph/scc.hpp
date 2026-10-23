// https://github.com/kazu107/kyopro_library/blob/main/graph/scc.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "dfs.hpp"
#include <algorithm>

namespace cp {
struct SccResult {
    std::vector<int> component;
    std::vector<std::vector<int>> groups, dag;
};
// 有向グラフの反復版Kosaraju法。O(V+E)、メモリO(V+E)。
// 成分番号はトポロジカル順。dagは成分間の辺で、多重辺を除去しない。
template <class Cost>
SccResult strongly_connected_components(const Graph<Cost>& g) {
    assert(g.is_directed());
    const auto traversal = dfs_forest(g);
    std::vector<std::vector<int>> reverse(g.size());
    for (const auto& edge : g.edges()) reverse[edge.to].push_back(edge.from);
    SccResult result{std::vector<int>(g.size(), -1), {}, {}};
    std::vector<int> stack;
    for (auto it = traversal.postorder.rbegin(); it != traversal.postorder.rend(); ++it) {
        const int root = *it;
        if (result.component[root] != -1) continue;
        const int id = static_cast<int>(result.groups.size());
        result.groups.push_back({});
        result.component[root] = id; stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back(); stack.pop_back();
            result.groups[id].push_back(v);
            for (int to : reverse[v]) if (result.component[to] == -1) {
                result.component[to] = id; stack.push_back(to);
            }
        }
    }
    result.dag.resize(result.groups.size());
    for (const auto& edge : g.edges()) {
        const int from = result.component[edge.from], to = result.component[edge.to];
        if (from != to) result.dag[from].push_back(to);
    }
    return result;
}
} // namespace cp
