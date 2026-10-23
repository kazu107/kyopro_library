// https://github.com/kazu107/kyopro_library/blob/main/graph/dfs.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"

namespace cp {
struct DfsForest {
    std::vector<int> parent, parent_edge, depth, root, tin, tout, order, postorder;
};
// 全頂点を0から順に探索。明示スタックなので長いパスも再帰しない。
// root[v]はDFS木の根で、有向グラフの強/弱連結成分番号ではない。
// tin/toutは訪問/退出イベントの時刻。O(V+E)、メモリO(V)。
template <class Cost>
DfsForest dfs_forest(const Graph<Cost>& g) {
    const int n = g.size();
    DfsForest result;
    result.parent.assign(n, -1);
    result.parent_edge.assign(n, -1);
    result.depth.assign(n, 0);
    result.root.assign(n, -1);
    result.tin.assign(n, -1);
    result.tout.assign(n, -1);
    std::vector<int> next(n, 0), stack;
    int timer = 0;
    for (int root = 0; root < n; ++root) {
        if (result.tin[root] != -1) continue;
        result.root[root] = root;
        result.tin[root] = timer++;
        result.order.push_back(root);
        stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back();
            if (next[v] == static_cast<int>(g[v].size())) {
                result.tout[v] = timer++;
                result.postorder.push_back(v);
                stack.pop_back();
                continue;
            }
            const auto& edge = g[v][next[v]++];
            if (result.tin[edge.to] != -1) continue;
            result.parent[edge.to] = v;
            result.parent_edge[edge.to] = edge.id;
            result.depth[edge.to] = result.depth[v] + 1;
            result.root[edge.to] = root;
            result.tin[edge.to] = timer++;
            result.order.push_back(edge.to);
            stack.push_back(edge.to);
        }
    }
    return result;
}
} // namespace cp
