// https://github.com/kazu107/kyopro_library/blob/main/tree/Subtreesize.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 未訪問の隣接頂点をたどって到達した頂点数を数える。木では根の次数1にも対応する部分木サイズになる。
使い方:
  vector<bool> visited(adjacency.size(), false); int count = tr(root, adjacency, visited);
前提: 0始まり、visitedと隣接リストの長さを一致させる。根はvisitedの初期値によらず含め、他は未訪問の頂点のみ。visitedを書き換える。
計算量: O(V+E)、メモリO(V)。明示スタックを使用。stdの名前解決を利用側で用意する。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし


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
