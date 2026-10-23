# フロー・マッチング

新規コードはGNU C++17以降、名前空間 `cp`、頂点番号は0始まりです。
通常のGraphとは異なる残余辺や左右の頂点集合を持つため、専用の構造体を使います。
必要なヘッダは各ファイルがincludeします。

| ファイル | 機能 | 計算量 |
| --- | --- | --- |
| [Dinic.hpp](Dinic.hpp) | 最大流、最小カット、各辺の流量 | 一般容量O(V^2 E) |
| [MinCostFlow.hpp](MinCostFlow.hpp) | 最小費用最大流、指定した上限までの最小費用流 | O(VE + A(V+E)log(V+E)) |
| [BipartiteMatching.hpp](BipartiteMatching.hpp) | 最大二部マッチング、最小頂点被覆 | O((V+E)sqrt(V+1)) |

メモリはいずれもO(V+E)です。Aは最小費用流で使う増加路の本数です。
2-SATはSCCを使うため [graph/TwoSat.hpp](../graph/TwoSat.hpp) にあります。

## 最大流

`Dinic<Cap = long long> flow(n)` を作成します。

- `add_edge(u,v,capacity)` は有向の容量辺を追加し、辺IDを返します。
- `flow(s,t,limit = Capの最大値)` は追加で流せた量を返します。sとtは異なる頂点です。
- 同じネットワークに対してflowを繰り返し呼び出せます。戻り値は毎回の追加分です。
- `min_cut(s)` は残余グラフでsから到達できる頂点のbool配列です。
  流量を制限せず最大まで流した後なら、最小カットのs側の集合になります。
- `get_edge(id)` / `edges()` は `{from,to,capacity,flow}` を返します。
  capacityは元の容量です。

Capは符号付き整数、容量は非負です。返す流量やその合計はCapに収めてください。
多重辺・自己ループ・0容量を扱えます。増加路の探索は反復処理です。

## 最小費用流

`MinCostFlow<Cap = long long, Cost = long long> flow(n)` を作成します。

- `add_edge(u,v,capacity,cost)` は辺IDを返します。
- `flow(s,t,limit)` は上限まで送れるだけ送り、追加の `{流量, 最小費用}` を返します。
- limit省略時は送れるだけ送ります。送れない場合はその時点までの流量と費用を返します。
- 再度flowを呼び出せます。返すのは毎回の追加分で、初期のBellman-Fordも毎回実行します。
- `get_edge(id)` / `edges()` は `{from,to,capacity,flow,cost}` を返します。

負費用も扱えますが、**始点から残余辺で到達できる負閉路は対象外**です。
検出した場合は `std::domain_error` にします。この検出で新しく流量を変更することはありません。
負閉路を含む循環流の最適化は実装していません。始点から到達できない負閉路は影響しません。
初期Bellman-Fordとポテンシャル付きDijkstraで最短増加路を求めます。

Cap/Costは64bit以下の符号付き整数、容量は非負です。sとtは異なる頂点です。
逆辺の費用を表すため、costにCost型の最小値は指定できません。
単価・経路費用・返す総費用をCost型、流量をCap型に収めてください。
内部計算はGCC/Clangの `__int128` 拡張です。limit=0なら流量・費用とも0を返します。

## 二部マッチング

`BipartiteMatching matching(left_n,right_n)` を作成します。
左右の頂点番号は **それぞれ独立した0始まり** です。

- `add_edge(left,right)` で対応可能な組を追加します。多重辺可。
- `solve()` は最大マッチング数を返します。毎回最初から計算し直します。
- `pairs()` は対応する `{左頂点,右頂点}` の一覧です。
- `left_match` / `right_match` は相手側の頂点番号です。未対応なら-1です。
- `minimum_vertex_cover()` は `{左側の頂点一覧,右側の頂点一覧}` を返します。
  頂点数の合計は最大マッチング数と等しくなります。

pairsとminimum_vertex_coverはsolve後に使ってください。
辺を追加した後は、もう一度solveする必要があります。
実装は再帰を使わないHopcroft-Karpです。左右0頂点の場合も利用できます。

## 使用例

リポジトリ直下からコンパイルできる例です。

```cpp
#include <iostream>
#include "flow/Dinic.hpp"
#include "flow/MinCostFlow.hpp"
#include "flow/BipartiteMatching.hpp"

int main() {
    cp::Dinic<> maximum(3);
    maximum.add_edge(0, 1, 3);
    maximum.add_edge(1, 2, 2);
    maximum.add_edge(0, 2, 1);
    std::cout << maximum.flow(0, 2) << '\n'; // 3

    cp::MinCostFlow<> cheapest(3);
    cheapest.add_edge(0, 1, 3, -2);
    cheapest.add_edge(1, 2, 2, 3);
    cheapest.add_edge(0, 2, 1, 5);
    auto [sent, cost] = cheapest.flow(0, 2);
    std::cout << sent << ' ' << cost << '\n'; // 3 7

    cp::BipartiteMatching matching(2, 2);
    matching.add_edge(0, 0);
    matching.add_edge(0, 1);
    matching.add_edge(1, 0);
    std::cout << matching.solve() << '\n'; // 2
    auto cover = matching.minimum_vertex_cover();
    std::cout << cover.first.size() + cover.second.size() << '\n'; // 2
}
```

## 検証

[flow_test.cpp](../tests/flow_test.cpp) で次を比較します。

- 最大流550ケース: 全カットの列挙、流量保存則、容量制約、最小カット
- 最小費用流420ケース: 各辺の流量の全列挙、負費用のDAG、残余辺での流し直し、分割実行
- 二部マッチング1,000ケース: 部分集合DP、最小頂点被覆、辺追加後の再計算
- 自己ループ・平行辺・0容量・大きな流量・到達可能/不可能な負閉路
- 10万頂点の最大流の増加路、3万頂点ずつの二部グラフの長い交互路

`python tests/run_tests.py` で一括実行できます。外部ジャッジへの提出は行っていません。
