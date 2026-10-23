// https://github.com/kazu107/kyopro_library/blob/main/tree/Subtreesize.cpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <cassert>
#include <vector>

// tr(root, adjacency, visited): 未訪問の隣接頂点を探索して到達した頂点数を返す。
// 根はvisitedの初期値によらず含める。探索した頂点をvisited=trueにする。
// 木で全visited=falseなら根の部分木サイズ。根の次数1にも対応。
// 0始まり、visited.size()==adjacency.size()。反復DFS、O(V+E)、メモリO(V)。
inline int tr(int root, std::vector<std::vector<int>>& adjacency, std::vector<bool>& visited) {
    assert(0 <= root && root < static_cast<int>(adjacency.size()) && visited.size() == adjacency.size());
    std::vector<int> stack{root};
    visited[root] = true;
    int count = 0;
    while (!stack.empty()) {
        const int v = stack.back(); stack.pop_back(); ++count;
        for (int to : adjacency[v]) {
            assert(0 <= to && to < static_cast<int>(adjacency.size()));
            if (!visited[to]) { visited[to] = true; stack.push_back(to); }
        }
    }
    return count;
}
