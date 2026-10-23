# グラフ

新しいライブラリは **GNU C++17以降**、名前空間は `cp` です。
必要な標準ヘッダと依存ファイルを各ヘッダから読み込むため、使う機能の `.hpp` を直接includeできます。
外部ライブラリは不要です。最短路の内部計算ではGCC/Clangの `__int128` 拡張を使用します。

## 共通のグラフ

`Graph<Cost = long long> g(n, directed = false)` で作成します。
頂点番号は0始まり、`g.add_edge(u, v, cost = 1)` は追加した辺のIDを返します。

- `g[v]` は頂点vから出る辺のリスト。各辺に `from, to, cost, id` が入っています。
- `g.edges()` は追加した元の辺の一覧です。無向辺もここでは1本だけ保持します。
- 無向辺は隣接リストに両方向を登録し、同じIDを割り当てます。無向自己ループも2つの隣接要素になります。
- 多重辺・自己ループ・孤立頂点・空グラフを保存できます。
- `size()` は頂点数、`edge_count()` は元の辺数、`is_directed()` は有向かどうかです。
- グラフ自体のメモリはO(V+E)、辺追加は償却O(1)です。頂点数は作成時に固定します。

## 一覧

Vは頂点数、Eは元の辺数です。メモリ欄は入力グラフに追加して使う領域の目安です。

| ファイル | 呼び出し・戻り値 | 条件 | 計算量 |
| --- | --- | --- | --- |
| [Graph.hpp](Graph.hpp) | `Graph<Cost>`, `Edge<Cost>` | 共通の隣接リスト・辺ID | メモリO(V+E) |
| [ShortestPath.hpp](ShortestPath.hpp) | `ShortestPathResult<Cost>` | 距離・親・経路復元の共通結果 | 復元O(経路長) |
| [bfs.hpp](bfs.hpp) | `bfs(g, source)` / `bfs(g, sources)` | 辺の重みを無視し辺数を最小化。複数始点可 | O(V+E) |
| [dfs.hpp](dfs.hpp) | `dfs_forest(g)` | 全頂点の親・深さ・訪問/退出順 | O(V+E) |
| [zero_one_bfs.hpp](zero_one_bfs.hpp) | `zero_one_bfs(g, source)` | 重み0/1のみ | O(V+E) |
| [dijkstra.hpp](dijkstra.hpp) | `dijkstra(g, source)` | 非負重み | O(V+(V+E)log(V+E)) |
| [bellman_ford.hpp](bellman_ford.hpp) | `bellman_ford(g, source)` | 負辺可、負閉路の影響も判定 | O(VE+V+E) |
| [floyd_warshall.hpp](floyd_warshall.hpp) | `floyd_warshall(g)` | 全頂点対、負辺可 | O(V^3)、メモリO(V^2) |
| [connected_components.hpp](connected_components.hpp) | `connected_components(g)` | 無向グラフの成分番号・頂点群 | O(V+E) |
| [bipartite.hpp](bipartite.hpp) | `bipartite_check(g)` | 無向グラフの二部判定と0/1色分け | O(V+E) |
| [cycle_detection.hpp](cycle_detection.hpp) | `find_cycle(g)` | 有向・無向、閉路の頂点列と辺ID列 | O(V+E) |
| [topological_sort.hpp](topological_sort.hpp) | `topological_sort(g)` | 有向グラフ | O(V+E) |
| [scc.hpp](scc.hpp) | `strongly_connected_components(g)` | 有向グラフの強連結成分・縮約DAG | O(V+E) |
| [lowlink.hpp](lowlink.hpp) | `lowlink(g)` | 無向グラフの橋・関節点 | O(V+E) |
| [kruskal.hpp](kruskal.hpp) | `kruskal(g)` | 無向グラフ、負辺可。非連結なら最小全域森 | O(V+E log(E+1)) |
| [TwoSat.hpp](TwoSat.hpp) | `TwoSat sat(n)` | 2-SATの節追加・充足判定・割り当て | O(変数数+節数) |

Dijkstraは重複挿入を許す二分ヒープを使用するため、表では多重辺を含めた計算量を記載しています。
BFS/DFS、Lowlink、SCCなどの深い探索に再帰は使用していません。
Union-Find系は [data_structure](../data_structure/README.md)、木は [tree](../tree/README.md)、フローは [flow](../flow/README.md) にあります。

## 最短路と経路復元

単一始点の結果は `dist, parent, parent_edge, negative` を持ちます。
BFSの距離型は `long long`、その他はグラフのCost型です。
重み付き最短路は64bit以下の符号付き整数型に対応します。

- `distance_inf<Cost>()` はCostの最大値の1/4です。未到達の距離はこのINFになります。
- 辺重み・有限の単純路の途中の和・有限距離を `(-INF, INF)` に収めてください。
- `reachable(v)` は負閉路の影響を受ける場合もtrueです。
- `finite(v)` は有限の最短距離が求まるときだけtrueです。
- `path_to(v)` は始点からvまでの頂点列、`edge_path_to(v)` は対応する辺ID列です。
- 未到達・負閉路の影響を受ける場合、どちらの復元結果も空です。
- 始点自身への有限な経路は頂点1個・辺0個です。
- 複数始点BFSは、到達したいずれかの始点まで復元します。空の始点リストなら全頂点未到達です。

Bellman-Fordは始点から到達できる負閉路と、その先に到達できる頂点を `negative[v] = true`、
距離を `-INF` にします。始点から到達できない負閉路の影響は受けません。

Floyd-Warshallの結果は `dist[s][t], negative[s][t]` と、
`finite(s,t), path(s,t), edge_path(s,t)` を持ちます。
sから負閉路を経由してtへ到達できる場合のみ `negative[s][t] = true`、距離は `-INF` です。
有限の最短路がない場合は経路復元結果も空です。

## その他の戻り値

- DFS: `parent, parent_edge, depth, root, tin, tout, order, postorder`。
  `root` はDFS木の根で、有向グラフの強/弱連結成分番号ではありません。
  `tin/tout` は2V個の訪問・退出イベントの時刻です。部分木を配列化する用途にはEulerTourを使ってください。
- 連結成分: `component[v]` と `groups`。孤立頂点も1成分です。
- 二部判定: `is_bipartite, color`。判定falseなら色配列は途中までの値です。
- 閉路: `vertices, edge_ids`。頂点列は末尾に始点を繰り返します。閉路なしなら両方空です。
- トポロジカルソート: `is_dag, order`。閉路ありならorderは部分的な順序です。辞書順最小は保証しません。
- SCC: `component, groups, dag`。成分番号はトポロジカル順です。dagの多重辺は除去しません。
- Lowlink: `order, low, bridges, articulation_points`。bridgesは辺ID、articulation_pointsは頂点番号で、両方昇順です。
- Kruskal: `cost, edge_ids, components, connected`。非連結なら最小全域森です。
  空グラフは費用0・成分数0・connected=trueと定義します。総費用はCost型に収めてください。

有向・無向の条件、頂点番号などの前提はassertで検査します。
Dijkstraの負辺、0-1 BFSの0/1以外の重みは `std::invalid_argument` にします。

## 使用例

リポジトリ直下に置くプログラムの例です。

```cpp
#include <iostream>
#include "graph/dijkstra.hpp"
#include "graph/lowlink.hpp"

int main() {
    cp::Graph<> g(4);
    g.add_edge(0, 1, 2);
    g.add_edge(1, 2, 3);
    g.add_edge(0, 2, 9);
    g.add_edge(2, 3, 1);

    auto result = cp::dijkstra(g, 0);
    std::cout << result.dist[3] << '\n'; // 6
    auto path = result.path_to(3);
    for (int i = 0; i < static_cast<int>(path.size()); ++i)
        std::cout << (i ? " " : "") << path[i];
    std::cout << '\n'; // 0 1 2 3
    std::cout << cp::lowlink(g).bridges.front() << '\n'; // 辺ID 3
}
```

## 検証

[graph_test.cpp](../tests/graph_test.cpp) は950個のランダムグラフについて、
経路、最短距離、負閉路、閉路、連結性、SCC、橋・関節点、二部判定、最小全域森を比較します。
辺数を制限したDP、頂点/辺の削除、色分け・全域森の全列挙を正解として使います。
2-SATは1,200個を真偽値の全列挙と照合し、10万頂点の鎖で非再帰の探索も確認します。

`python tests/run_tests.py` で他カテゴリと合わせて実行できます。
外部ジャッジへの提出によるAC確認は行っていません。
