// https://github.com/kazu107/kyopro_library/blob/main/graph/kruskal.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include "Graph.hpp"
#include "../data_structure/UnionFind.hpp"
#include <algorithm>
#include <numeric>

namespace cp {
template <class Cost>
struct KruskalResult {
    Cost cost{};
    std::vector<int> edge_ids;
    int components = 0;
    bool connected = false;
};
// 無向グラフ専用。負辺可。非連結なら最小全域「森」と成分数を返す。
// 空グラフはcost=0,components=0,connected=trueと定義。
// O(V+E log(E+1))、メモリO(V+E)。合計コストはCostに収める。
template <class Cost>
KruskalResult<Cost> kruskal(const Graph<Cost>& g) {
    assert(!g.is_directed());
    std::vector<int> ids(g.edge_count());
    std::iota(ids.begin(), ids.end(), 0);
    std::sort(ids.begin(), ids.end(), [&](int a, int b) {
        const auto& x = g.edges()[a]; const auto& y = g.edges()[b];
        return x.cost != y.cost ? x.cost < y.cost : a < b;
    });
    UnionFind uf(g.size());
    KruskalResult<Cost> result;
    for (int id : ids) {
        const auto& edge = g.edges()[id];
        if (uf.merge(edge.from, edge.to)) { result.cost += edge.cost; result.edge_ids.push_back(id); }
    }
    result.components = uf.component_count();
    result.connected = g.size() == 0 || result.components == 1;
    return result;
}
} // namespace cp
