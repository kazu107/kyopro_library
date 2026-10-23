// https://github.com/kazu107/kyopro_library/blob/main/tree/HeavyLightDecomposition.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 木の経路を順序付きの配列区間に分ける。部分木区間・LCAも取得でき、遅延セグメント木と組み合わせられる。
使い方:
  cp::HeavyLightDecomposition<long long> hld(g, root); auto parts = hld.path_segments(u, v);
  for (auto part : parts) { seg.range_apply(part.l, part.r, delta); } // 和・最小・最大など。
  auto edges = hld.path_segments(u, v, true); // 辺の値は子のposに置く。LCAを除く。
前提: 0始まり、無向の森。区間[l,r)、reversed=trueなら逆順に読む。別成分の経路は空。配列位置iの元頂点はvertex_at[i]。
計算量: 構築O(V+E)、経路分解・LCA O(log V)、通常のセグメント木と組み合わせた経路操作O(log^2 V)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/RootedForest.hpp


namespace cp {
struct PathSegment { int l, r; bool reversed; };
// HLD: 頂点uからvへの経路を、順序付きの半開区間[l,r)へ分解する。
// reversed=trueならr-1,r-2,...,lの順に読む。位置posと元の頂点vertex_atを対応。
// path_segments(u,v,true)は辺用(辺の値を子のposに置き、LCAを含めない)。
// 森可、別成分の経路は空でlcaは-1。構築O(V)、分解/LCA O(log V)。
// セグメント木と組み合わせた経路更新/取得は通常O(log^2 V)。メモリO(V)。
template <class Cost = long long>
class HeavyLightDecomposition {
    void check(int v) const { assert(0 <= v && v < static_cast<int>(parent.size())); }
public:
    std::vector<int> parent, depth, head, pos, vertex_at, subtree_size, component;
    explicit HeavyLightDecomposition(const Graph<Cost>& g, int root = 0) {
        RootedForest<Cost> forest(g, root);
        parent = forest.parent; depth = forest.depth; component = forest.component;
        subtree_size = forest.subtree_size;
        const int n = g.size();
        head.resize(n); pos.resize(n); vertex_at.resize(n);
        std::vector<int> heavy(n, -1);
        for (int v = 0; v < n; ++v) for (int child : forest.children[v])
            if (heavy[v] == -1 || subtree_size[child] > subtree_size[heavy[v]]) heavy[v] = child;
        std::vector<std::pair<int, int>> stack;
        for (auto it = forest.roots.rbegin(); it != forest.roots.rend(); ++it) stack.push_back({*it, *it});
        int timer = 0;
        while (!stack.empty()) {
            const auto [v, h] = stack.back(); stack.pop_back();
            head[v] = h; pos[v] = timer; vertex_at[timer++] = v;
            for (auto it = forest.children[v].rbegin(); it != forest.children[v].rend(); ++it)
                if (*it != heavy[v]) stack.push_back({*it, *it});
            if (heavy[v] != -1) stack.push_back({heavy[v], h});
        }
    }
    int lca(int u, int v) const {
        check(u); check(v);
        if (component[u] != component[v]) return -1;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) std::swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }
    std::vector<PathSegment> path_segments(int u, int v, bool edge_values = false) const {
        check(u); check(v);
        if (component[u] != component[v]) return {};
        std::vector<PathSegment> up, down;
        while (head[u] != head[v]) {
            if (depth[head[u]] >= depth[head[v]]) {
                up.push_back({pos[head[u]], pos[u] + 1, true}); u = parent[head[u]];
            } else {
                down.push_back({pos[head[v]], pos[v] + 1, false}); v = parent[head[v]];
            }
        }
        const int skip = edge_values ? 1 : 0;
        if (depth[u] >= depth[v]) {
            if (pos[v] + skip < pos[u] + 1) up.push_back({pos[v] + skip, pos[u] + 1, true});
        } else if (pos[u] + skip < pos[v] + 1) down.push_back({pos[u] + skip, pos[v] + 1, false});
        for (auto it = down.rbegin(); it != down.rend(); ++it) up.push_back(*it);
        return up;
    }
    std::pair<int, int> subtree_range(int v, bool edge_values = false) const {
        check(v);
        return {pos[v] + (edge_values ? 1 : 0), pos[v] + subtree_size[v]};
    }
};
} // namespace cp
