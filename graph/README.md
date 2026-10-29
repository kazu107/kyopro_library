# グラフ

新しいライブラリは **GNU C++17以降**、名前空間は `cp` です。
各ファイルにはinclude行を置かず、提出用テンプレートに貼り付けて使います。
標準ヘッダは利用側で用意し、冒頭の `// 依存:` に示した共有コードを依存順に先に貼り付けてください。
[一括ツール](../tests/run_tests.py)の `--bundle graph/dijkstra.hpp` などでも、依存を重複させずにまとめられます。
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
| [lexicographic_bfs.hpp](lexicographic_bfs.hpp) | `lexicographic_bfs(g, source)` | 最短辺数、その中で辺ラベル列を辞書順最小化 | O(V+E log(E+1)) |
| [dfs.hpp](dfs.hpp) | `dfs_forest(g)` | 全頂点の親・深さ・訪問/退出順 | O(V+E) |
| [min_first_search.hpp](min_first_search.hpp) | `min_first_search(g, source)` / `count_min_first_trees(order, mod)` | 最小番号優先探索・その順になる木の個数 | 探索O(E+V log(V+1))、数え上げO(V) |
| [zero_one_bfs.hpp](zero_one_bfs.hpp) | `zero_one_bfs(g, source)` | 重み0/1のみ | O(V+E) |
| [dijkstra.hpp](dijkstra.hpp) | `dijkstra(g, source)` | 非負重み | O(V+(V+E)log(V+E)) |
| [WheelGraphDistance.hpp](WheelGraphDistance.hpp) | `WheelGraphDistance<Cost>(ring, spokes)` | 非負重みの輪と中心。距離のみ取得 | 構築O(V log(V+1))、取得O(1) |
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

標準ヘッダと「使用するコード」の依存先を貼り付けた後に書く利用例です。

```cpp
// 使用するコード: graph/dijkstra.hpp, graph/lowlink.hpp
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

## 最短辺数とラベル列の辞書順

`lexicographic_bfs(g, source)` は `g` の `cost` を辺ラベルとして扱い、まず辺数、次にラベル列の辞書順で経路を最小化します。
ラベルはコピーでき、`<` が厳密弱順序を定める型を使えます。有向・無向、多重辺、自己ループに対応します。
同じラベル列を持つ頂点をまとめて次のラベルを比較するので、各頂点の隣接辺をソートするだけのBFSとは異なります。

- 戻り値は `distance, parent, parent_edge, parent_label, word_rank, discovery_order` を持ちます。未到達の `distance` と `word_rank` は-1、始点の距離は0です。
- `reachable(v)` は到達判定、`path_to(v)` / `edge_path_to(v)` / `labels_to(v)` は頂点・辺ID・ラベル列を返します。未到達なら空、始点の頂点列は始点1個です。
- `word_rank` は最適ラベル列を長さ、その後辞書順で比較するための順位です。同じ列なら同値です。同じラベル列を与える頂点経路の辞書順最小は保証しません。
- `lexicographic_path_values(result, base = 10, mod = 998244353)` は選択済みラベル列を連結した値の剰余を全頂点分返します。未到達は-1、始点は0です。64bit以下の整数ラベル、`base >= 2`、`mod > 0`、各桁が `0 <= label < base` を満たす必要があります。範囲違反は `std::invalid_argument` です。

1〜9の数字ラベルなら、この最適経路は連結した整数そのものを最小にします。0を含む場合は先頭の0も1辺として数えます。
剰余値は経路決定後に計算し、剰余の大小で経路を選びません。比較・コピーがO(1)なら構築O(V+E log(E+1))、メモリO(V+E)、経路復元O(経路長)、剰余計算O(V)です。

```cpp
// 使用するコード: graph/lexicographic_bfs.hpp
int main() {
    cp::Graph<int> g(5, true);
    g.add_edge(0, 1, 1);
    g.add_edge(0, 2, 1);
    g.add_edge(1, 3, 9);
    g.add_edge(2, 3, 2);
    g.add_edge(0, 4, 9);
    g.add_edge(3, 4, 1);
    auto result = cp::lexicographic_bfs(g, 0);
    auto values = cp::lexicographic_path_values(result);
    std::cout << result.distance[3] << ' ' << values[3] << '\n'; // 2 12
    auto path = result.path_to(3);
    for (int i = 0; i < static_cast<int>(path.size()); ++i)
        std::cout << (i ? " " : "") << path[i];
    std::cout << '\n'; // 0 2 3
    std::cout << values[4] << '\n'; // 9: 3辺の121より1辺の9を優先
}
```

## 車輪グラフの距離

`WheelGraphDistance<Cost> wheel(ring, spokes)` は、輪の頂点0〜N-1と中心Nからなる固定グラフを前処理します。
`ring[i]` はiと `(i+1)%N`、`spokes[i]` はiと中心を結ぶ辺の重みです。配列は同じ長さでN>=1、Costは64bit以下の符号付き整数型です。
0重みとN=1/2にも対応します。輪の重み総和と各spokeは `distance_inf<Cost>()` 未満に収めてください。
空配列・長さ違い・負重みは `std::invalid_argument`、距離範囲超過は `std::overflow_error` です。

- `distance(u,v)` は中心を含めた頂点0〜N間の最短距離をCost型で返します。
- `ring_distance(u,v)` は輪の頂点同士について、中心を使わない短い側の距離を返します。
- `distance_to_hub(v)` は中心までの最短距離です。直接のspokeより輪を経由する方が短い場合も含みます。
- `ring_size()`, `hub()`, `circumference()` はN、中心の頂点番号N、輪の重み総和を返します。

中心からのDijkstraと輪の累積和で、構築O(N log(N+1))、各取得O(1)、メモリO(N)です。経路復元と辺の途中更新は扱いません。

```cpp
// 使用するコード: graph/WheelGraphDistance.hpp
int main() {
    cp::WheelGraphDistance<> wheel({4, 4, 4, 4}, {1, 9, 1, 9});
    std::cout << wheel.distance(0, 2) << ' ' << wheel.ring_distance(0, 2) << '\n'; // 2 8
    std::cout << wheel.distance_to_hub(1) << '\n'; // 5: 1 -> 0 -> 中心
}
```

## 最小優先度から選ぶ探索と木の数え上げ

`min_first_search(g, source)` は、発見済みで未処理の候補のうち頂点番号が最小の頂点を取り出します。
`min_first_search(g, source, priorities)` では頂点ごとの優先度を指定でき、同値なら頂点番号順です。
優先度はコピー・比較できる型で、配列の長さをVにそろえます。有向・無向に対応し、辺重みは使いません。最短距離を求める用途には使いません。
戻り値の `order` は処理順、`position[v]` はその逆、`parent[v]` と `parent_edge[v]` は最初に発見したときの親と辺IDです。
未到達のpositionと親は-1、始点の親も-1です。探索O(E+V log(V+1))、追加メモリO(V)です。

`count_min_first_trees(order, mod = 998244353)` は、0〜N-1の順列を標準の頂点番号優先で探索する順としたとき、該当するラベル付き無向木の数をmodで返します。
根は `order.front()` で、空順列は1通りと定義します。順列違反や `mod <= 0` は `std::invalid_argument` です。
辺重みと独自優先度はこの数え上げに使いません。単調スタックで親の候補数を求め、時間O(N)、メモリO(N)です。

```cpp
// 使用するコード: graph/min_first_search.hpp
int main() {
    cp::Graph<> g(5);
    g.add_edge(0, 3);
    g.add_edge(0, 2);
    g.add_edge(3, 1);
    g.add_edge(2, 4);
    auto result = cp::min_first_search(g, 0);
    for (int i = 0; i < static_cast<int>(result.order.size()); ++i)
        std::cout << (i ? " " : "") << result.order[i];
    std::cout << '\n'; // 0 2 3 1 4
    std::cout << cp::count_min_first_trees(result.order) << '\n'; // 8
}
```

## 検証

[graph_test.cpp](../tests/graph_test.cpp) は950個のランダムグラフについて、
経路、最短距離、負閉路、閉路、連結性、SCC、橋・関節点、二部判定、最小全域森を比較します。
辺数を制限したDP、頂点/辺の削除、色分け・全域森の全列挙を正解として使います。
2-SATは1,200個を真偽値の全列挙と照合し、10万頂点の鎖で非再帰の探索も確認します。

[recent_graph_test.cpp](../tests/recent_graph_test.cpp) では、ラベル付き最短路を素朴なラベル列比較、
車輪距離を全点対最短路、探索順の木の数え上げを小さい木の全列挙と照合します。

`python tests/run_tests.py` で他カテゴリと合わせて実行できます。
外部ジャッジへの提出によるAC確認は行っていません。
