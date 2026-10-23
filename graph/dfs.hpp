// https://github.com/kazu107/kyopro_library/blob/main/graph/dfs.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 全頂点をDFSの森として探索し、親・深さ・根・訪問順・退出順を取得する。
使い方:
  auto result = cp::dfs_forest(g);
  auto order = result.order; auto postorder = result.postorder; int parent = result.parent[v];
前提: 0始まり、有向・無向とも可。root[v]はDFS木の根で強/弱連結成分番号ではない。tin/toutは2V個のイベント時刻。
計算量: O(V+E)、追加メモリO(V)。明示スタックで探索する。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/Graph.hpp


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
