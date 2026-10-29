# 競技プログラミング用 C++ ライブラリ

競技プログラミングで使う関数・データ構造を、カテゴリ別にまとめたライブラリです。
現在51個のライブラリファイルがあります。グラフ・木・フロー・Union-Find関連の33ヘッダと、比較テストを追加しています。
最近のAtCoderを基に、ラベル付き最短路、車輪グラフの距離、最小番号優先探索、木上Mo・経路の値頻度を追加しました。[対象問題と一般化の記録](docs/atcoder_graph_research_20261029.md)も参照できます。

## カテゴリ

| フォルダ | 内容 | コード数 |
| --- | --- | ---: |
| [math/number_theory](math/number_theory/) | 素数、約数、素因数分解 | 6 |
| [math/modular](math/modular/) | 剰余演算、繰り返し二乗法 | 2 |
| [math/base_conversion](math/base_conversion/) | 進数変換 | 1 |
| [math/combinatorics](math/combinatorics/) | 組合せ、順列、二項係数（追加用） | 0 |
| [math/convolution](math/convolution/) | FFT、NTT、畳み込み（追加用） | 0 |
| [data_structure](data_structure/) | セグメント木、3種類のUnion-Find | 5 |
| [prefix_sum](prefix_sum/) | 一次元・二次元の累積和 | 2 |
| [two_pointers](two_pointers/) | しゃくとり法、連続部分配列 | 1 |
| [string](string/) | 文字列アルゴリズム | 1 |
| [tree](tree/) | LCA、Euler Tour、HLD、直径、汎用全方位木DP、木上Mo・経路頻度 | 10 |
| [grid](grid/) | グリッド、二次元配列の操作 | 1 |
| [graph](graph/) | 探索、ラベル付き最短路、車輪距離、連結性、最小全域森、2-SAT | 19 |
| [dynamic_programming](dynamic_programming/) | 動的計画法（追加用） | 0 |
| [geometry](geometry/) | 計算幾何（追加用） | 0 |
| [flow](flow/) | 最大流、最小費用流、二部マッチング・最小頂点被覆 | 3 |

「追加用」のフォルダにはカテゴリの説明だけを置いています。

## コード一覧

### 数学

| コード | 内容 |
| --- | --- |
| [divEnum.cpp](math/number_theory/divEnum.cpp) | 約数を昇順に列挙 |
| [Eratosthenes.cpp](math/number_theory/Eratosthenes.cpp) | エラトステネスの篩による素数列挙 |
| [vecprime.cpp](math/number_theory/vecprime.cpp) | `int` 型の素数列挙 |
| [is_prime.cpp](math/number_theory/is_prime.cpp) | 試し割りによる素数判定 |
| [prime_factorization.cpp](math/number_theory/prime_factorization.cpp) | 試し割りによる素因数分解 |
| [Factorize.cpp](math/number_theory/Factorize.cpp) | 篩を使う素因数分解の旧実装。指数0の項も含む |
| [modint.cpp](math/modular/modint.cpp) | `x^y mod z` の計算。modintクラスではなく累乗関数 |
| [largeInt_mod.cpp](math/modular/largeInt_mod.cpp) | 文字列で表した大きい非負整数の剰余 |
| [x_to_base10.cpp](math/base_conversion/x_to_base10.cpp) | x進数の文字列を10進数に変換 |

### データ構造・区間処理

| コード | 内容 |
| --- | --- |
| [SegTree.cpp](data_structure/SegTree.cpp) | 一点更新・区間最大値。添字は1始まり |
| [LagySegTree.cpp](data_structure/LagySegTree.cpp) | `LazySegTree<Policy>`：区間加算の取得方法を和・最小・最大・最小/最大の個数・最小/最大の位置・まとめ取得の8種類から選択。添字は0始まり、区間は `[l, r)`。詳細は[使い方](data_structure/README.md) |
| [UnionFind.hpp](data_structure/UnionFind.hpp) | 通常のUnion-Find、成分サイズ・成分一覧 |
| [WeightedUnionFind.hpp](data_structure/WeightedUnionFind.hpp) | 重み付きUnion-Find、差の制約・矛盾判定 |
| [RollbackUnionFind.hpp](data_structure/RollbackUnionFind.hpp) | snapshot・undo・rollbackを持つUnion-Find |
| [Rangesum.cpp](prefix_sum/Rangesum.cpp) | 作成済みの累積和から閉区間 `[l, r]` の和を取得 |
| [Rangesum2D.cpp](prefix_sum/Rangesum2D.cpp) | 作成済みの二次元累積和から両端を含む長方形の和を取得 |
| [longestUniqueSubarray.cpp](two_pointers/longestUniqueSubarray.cpp) | 要素が重複しない連続部分配列の最大長 |

### 文字列・木・グリッド

| コード | 内容 |
| --- | --- |
| [LCS.cpp](string/LCS.cpp) | 最長共通部分列の復元 |
| [Subtreesize.cpp](tree/Subtreesize.cpp) | DFSによる部分木サイズの計算 |
| [rerooting.cpp](tree/rerooting.cpp) | 既存のRerooting名の互換入口。各頂点からの最大距離 |
| [RootedForest.hpp](tree/RootedForest.hpp) | 木・森の親、深さ、成分、部分木サイズの共通前処理 |
| [Lca.hpp](tree/Lca.hpp) | ダブリングLCA、距離、祖先・経路上の頂点 |
| [EulerTour.hpp](tree/EulerTour.hpp) | 部分木の区間化 |
| [HeavyLightDecomposition.hpp](tree/HeavyLightDecomposition.hpp) | 順序付きの経路分解と部分木区間 |
| [diameter.hpp](tree/diameter.hpp) | 非負重みの木の直径と経路 |
| [Rerooting.hpp](tree/Rerooting.hpp) | Policyを指定する汎用全方位木DP |
| [MoOnTree.hpp](tree/MoOnTree.hpp) | 木・森の静的な頂点パスを追加/削除コールバックで処理する木上Mo |
| [TreePathFrequencyQueries.hpp](tree/TreePathFrequencyQueries.hpp) | 頂点パス上で出現回数が閉区間に入る値の種類数 |
| [rotate90.cpp](grid/rotate90.cpp) | 文字列・整数の二次元配列を90度回転 |


### グラフ・フロー

| コード | 内容 |
| --- | --- |
| [Graph.hpp](graph/Graph.hpp) | 有向/無向、重み、辺IDを持つ共通グラフ |
| [ShortestPath.hpp](graph/ShortestPath.hpp) | 最短路の距離・親・頂点列/辺列の復元 |
| [bfs.hpp](graph/bfs.hpp) | BFS、複数始点BFS |
| [lexicographic_bfs.hpp](graph/lexicographic_bfs.hpp) | 最短辺数の中で辺ラベル列が辞書順最小になる経路・連結値の剰余 |
| [dfs.hpp](graph/dfs.hpp) | 非再帰DFSの森、親・深さ・訪問/退出順 |
| [min_first_search.hpp](graph/min_first_search.hpp) | 最小優先度の候補から選ぶ探索・指定探索順になる木の数え上げ |
| [zero_one_bfs.hpp](graph/zero_one_bfs.hpp) | 0-1 BFS |
| [dijkstra.hpp](graph/dijkstra.hpp) | 非負重みの最短路 |
| [WheelGraphDistance.hpp](graph/WheelGraphDistance.hpp) | 輪と中心を持つ重み付き車輪グラフのO(1)距離取得 |
| [bellman_ford.hpp](graph/bellman_ford.hpp) | 負辺のある最短路、負閉路の影響判定 |
| [floyd_warshall.hpp](graph/floyd_warshall.hpp) | 全頂点間の最短路と経路復元 |
| [connected_components.hpp](graph/connected_components.hpp) | 無向グラフの連結成分 |
| [bipartite.hpp](graph/bipartite.hpp) | 二部グラフ判定と色分け |
| [cycle_detection.hpp](graph/cycle_detection.hpp) | 有向・無向の閉路検出と復元 |
| [topological_sort.hpp](graph/topological_sort.hpp) | トポロジカルソート、DAG判定 |
| [scc.hpp](graph/scc.hpp) | 強連結成分と縮約DAG |
| [lowlink.hpp](graph/lowlink.hpp) | 多重辺対応の橋・関節点 |
| [kruskal.hpp](graph/kruskal.hpp) | 最小全域木/森 |
| [TwoSat.hpp](graph/TwoSat.hpp) | 2-SAT、条件追加、充足割り当て |
| [Dinic.hpp](flow/Dinic.hpp) | 最大流、最小カット、辺ごとの流量 |
| [MinCostFlow.hpp](flow/MinCostFlow.hpp) | 負費用対応の最小費用流（到達可能な負閉路は対象外） |
| [BipartiteMatching.hpp](flow/BipartiteMatching.hpp) | 最大二部マッチング、最小頂点被覆 |

使い方・入力条件・計算量は [グラフ](graph/README.md)、[木](tree/README.md)、[フロー](flow/README.md)、[データ構造](data_structure/README.md) を参照してください。

## 使い方

各 `.cpp` / `.hpp` は、提出用テンプレートに貼り付けるコードです。
すべてのC++ファイルから `#include` と `#pragma once` を除き、冒頭に「できること・使い方・前提・計算量」をコメントで記載しています。
標準ヘッダ（例: `bits/stdc++.h`）、`using namespace std;`、`using ll = long long;`、`main` は利用側で用意してください。
新規グラフ関連の名前空間は `cp` です。最短路・最小費用流などにはGCC/Clangの `__int128` 拡張を使用します。

共有コードが必要な場合は、各ファイルの `// 依存:` に書かれたファイルを先に貼り付けます。
依存先にも依存があれば、そのコードを先に置き、同じ本体は重複させないでください。
`依存: なし` は、このリポジトリ内の共有コードが不要という意味で、標準ヘッダは別途必要です。
次は「使用するコード」とその依存先をテンプレートに貼り付けた後の利用例です。

```cpp
// 使用するコード: math/number_theory/prime_factorization.cpp
using namespace std;
using ll = long long;


int main() {
    for (const auto& [prime, exponent] : prime_factorization(360)) {
        cout << prime << ' ' << exponent << '\n';
    }
}
```

標準ヘッダ・依存コード・利用例をまとめて `main.cpp` にすれば、GNU C++17以降でコンパイルできます：

```sh
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o main
```

カテゴリ整理では、元のファイル名・関数名・処理内容を維持しました。
その後、遅延セグメント木をテンプレート方式に改修し、8種類の取得ルールと独自ルールの追加に対応しています。
元のコードには個別の前提があり、例えば `Eratosthenes` は `N >= 1`、
`divEnum` は正の整数、`Rotate90` は空でない長方形の配列を前提としています。
`Rotate90` は反時計回りに90度回転します。ファイル内の説明も実装に合わせています。
`Subtreesize.cpp` は根の次数1の誤判定を修正し、全方位木DPとともに再帰を使わない処理へ改修しました。
遅延セグメント木と新規グラフ関連は境界条件・全探索・ランダム比較テストで検証しています。
外部ジャッジへの提出によるAC確認は行っていません。その他の既存コードは構文確認が中心です。

## 遅延セグメント木の切り替え

```cpp
LazySegTree<RangeAddSum> seg(a);      // 区間加算・区間和
LazySegTree<RangeAddMin> minimum(a);  // 区間加算・区間最小値
LazySegTree<RangeAddMax> maximum(a);  // 区間加算・区間最大値
LazySegTree<RangeAddMinCount> low(a); // 最小値とその個数
LazySegTree<RangeAddMaxCount> high(a);// 最大値とその個数
LazySegTree<RangeAddMinIndex> argmin(a); // 最小値とその位置（同値なら左端）
LazySegTree<RangeAddMaxIndex> argmax(a); // 最大値とその位置（同値なら左端）
LazySegTree<RangeAddStats> stats(a);  // 和・最小値・最大値・要素数
```

型を省略した `LazySegTree seg(n)` は、従来どおり区間加算・区間和として使えます。
詳しい利用方法、独自ルールの定義、テストの実行方法は[データ構造の説明](data_structure/README.md)を参照してください。
[遅延セグメント木のテスト](tests/lazy_segtree_test.cpp)も一括テストに含めています。


## 一括テスト

Python 3.10以降とGNU C++17対応のg++があれば、外部Pythonパッケージなしで実行できます。

```sh
python tests/run_tests.py
python tests/run_tests.py --debug
```

コンパイラを変える場合は `--cxx clang++` などを指定できます。
生成物は追跡対象外の `build/` に保存します。assertを無効にしないでください。

| テスト | 比較内容 |
| --- | --- |
| [graph_test.cpp](tests/graph_test.cpp) | 950グラフと独立した最短路DP・削除/全列挙、1,200個の2-SAT、10万頂点の鎖 |
| [recent_graph_test.cpp](tests/recent_graph_test.cpp) | ラベル列の素朴解、車輪の全点対最短路、木の全列挙、経路の直接集計と新5ヘッダを照合 |
| [union_find_test.cpp](tests/union_find_test.cpp) | 20,000操作とBFSの連結性・既知の差・履歴の削除 |
| [tree_test.cpp](tests/tree_test.cpp) | 180個の木/森の全頂点対、順序付き経路、素朴な全方位DP、HLDと遅延木、10万頂点の鎖 |
| [flow_test.cpp](tests/flow_test.cpp) | 550個の全カット、420個の流量全列挙、1,000個の二部マッチングDP、深い増加路 |
| [lazy_segtree_test.cpp](tests/lazy_segtree_test.cpp) | 8種類・独自ルールの境界条件と51,000回のランダム操作 |

[run_tests.py](tests/run_tests.py) は各コードの `依存:` コメントをたどり、標準ヘッダを検証時だけ補ってコンパイルします。
51コードそれぞれの依存展開と全コードのまとめ読み込み、README利用例、先頭のURL/SPDX、説明コメント、コード数、リンク、include行がないことも検査します。
既存の `largeInt_mod.cpp` の符号付き/符号なし比較警告は、処理を変更せず維持しています。

## 必要なコードをまとめる

依存先を手作業で並べる代わりに、次のようにまとめることもできます。
生成されるコードにもinclude行は入りません。標準ヘッダを用意した提出用テンプレートへ貼り付けてください。

```sh
python tests/run_tests.py --bundle graph/dijkstra.hpp --output build/dijkstra_snippet.cpp
```

複数の入口も指定でき、共有する依存先は1回だけ出力します。
既存の出力ファイルは保護し、置き換える場合だけ `--overwrite` を指定します。
`--output` を省略すると標準出力へ出します。束ねる操作だけならC++コンパイラは不要です。

## コード追加時のルール

- 内容に合うカテゴリに配置します。複数のカテゴリに関係するコードは主な用途で分類します。
- 各ソースファイルの**最初の行**に、そのファイル自身のGitHub URLをコメントで書きます。
- 2行目に `// SPDX-License-Identifier: CC0-1.0` を書きます。
- 全コードに、できること・使い方・計算量・添字や区間などの前提をコメントで記載します。
- C++コードに `#include` / `#pragma once` は書かず、共有コードは `// 依存: graph/Graph.hpp` のようにリポジトリ相対パスで示します。複数ならカンマ区切り、不要なら `なし` です。
- コードを追加・移動したら、この一覧と先頭のURLも更新します。

先頭2行の例：

```cpp
// https://github.com/kazu107/kyopro_library/blob/main/data_structure/SegTree.cpp
// SPDX-License-Identifier: CC0-1.0
```

URLは整理後の `main` ブランチ上の配置先を指します。
ローカルで追加・移動したファイルのURLは、その変更をGitHubの `main` に反映した後に閲覧できます。

## ライセンス

このリポジトリのコードとドキュメントには **CC0 1.0 Universal** を適用します。
全文は [LICENSE](LICENSE)、公式の原文は [Creative Commons CC0](https://creativecommons.org/publicdomain/zero/1.0/legalcode.txt) を参照してください。
