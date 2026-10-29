# 最近のAtCoderから追加したグラフ機能

調査日: 2026-10-29（日本時間）。2026-09-26〜2026-10-24に開催されたABCから、次の5問を確認しました。
ライブラリとして繰り返し使える部分を切り出し、既存のGraph・LCA・Dijkstraと組み合わせる独自実装です。
問題文や解説のコードの転載は行っていません。追加コードのライセンスはCC0です。

## 対象と追加先

| 開催日 | 問題・一次資料 | 読み取った用途 | 追加・既存対応 |
| --- | --- | --- | --- |
| 2026-10-24 | [ABC481 D: Autopilot](https://atcoder.jp/contests/abc481/tasks/abc481_d)、[公式解説](https://atcoder.jp/contests/abc481/editorial/27926) | 0/1重みへの変換後の最短路 | 既存の[zero_one_bfs.hpp](../graph/zero_one_bfs.hpp)で対応できるため追加なし |
| 2026-10-24 | [ABC481 F: Evil Shortest Path](https://atcoder.jp/contests/abc481/tasks/abc481_f) | 辺ラベルを連結した値を、桁数・辞書順で最小化 | [lexicographic_bfs.hpp](../graph/lexicographic_bfs.hpp) |
| 2026-10-03 | [ABC478 F: Min-First Search](https://atcoder.jp/contests/abc478/tasks/abc478_f)、[公式解説](https://atcoder.jp/contests/abc478/editorial/26540) | 最小番号の候補からの探索と、探索順になる木の数 | [min_first_search.hpp](../graph/min_first_search.hpp) |
| 2026-09-26 | [ABC477 E: Wheel Distance](https://atcoder.jp/contests/abc477/tasks/abc477_e) | 輪と中心を持つ重み付きグラフの多数の距離取得 | [WheelGraphDistance.hpp](../graph/WheelGraphDistance.hpp) |
| 2026-09-26 | [ABC477 G: Frequency Query on Tree](https://atcoder.jp/contests/abc477/tasks/abc477_g)、[公式解説](https://atcoder.jp/contests/abc477/editorial/25886) | 木のパス上で出現回数が範囲に入る値の種類数 | [MoOnTree.hpp](../tree/MoOnTree.hpp) と [TreePathFrequencyQueries.hpp](../tree/TreePathFrequencyQueries.hpp) |

ABC479 F・ABC480 Gの問題文も確認しましたが、今回は上表のグラフ・木に直接関係する題材を選びました。
ABC481 FとABC477 Eの個別公式解説は取得できなかったため、問題文から独自に設計し、別の素朴解と比較しています。
ABC478 FとABC477 Gでは、取得できた公式解説を設計上の条件の確認に使いました。

## 一般化と条件

- ラベル付きBFS: 最短辺数、その後ラベル列の辞書順で全頂点への経路を求めます。
  同じprefixの頂点をまとめて次ラベルを比較し、整数の剰余は経路決定後に計算します。
  数字1〜9なら元問題の最小整数経路と一致します。任意の比較可能なラベルにも使えますが、0を含む場合は先頭の0も1辺として数えます。
- 最小優先度の探索: 有向・無向に使え、頂点ごとの優先度と同値時の番号順を指定できます。
  木の数え上げは標準の番号優先だけを対象にし、根を順列の先頭へ一般化しました。
  この一般化は、各頂点の親候補を前方の位置から選ぶ条件から導いたもので、全ラベル木で比較します。
- 車輪距離: 中心を使わない短い輪の経路と、中心までの最短距離の和の小さい方を取る設計です。
  非負辺なので、中心を複数回経由する必要はありません。中心からのDijkstraと累積和で前処理し、取得はO(1)です。
  N=1/2と0重みに拡張し、中心自身の取得も扱います。辺更新や経路復元は対象外です。
- 木上Mo: 2N個の入出イベントでパスを頂点集合に変換し、LCAを一時的に補います。
  任意の追加・削除・記録コールバックを渡せます。森、同じ頂点、反復実行にも対応し、通常終了時に空集合へ戻します。
  経路の順序に依存する集約や処理途中の値更新は対象外です。
- パスの頻度取得: 値を圧縮し、「頻度がk以上の値の数」を管理します。
  頂点の追加・削除と閉区間の頻度回答がO(1)になり、Moの移動回数に沿って処理できます。
  出現0回の値は対象外とし、low>=1を条件にしています。

## 検証

[recent_graph_test.cpp](../tests/recent_graph_test.cpp) を一括ランナーに組み込んでいます。

- ラベル列を直接保持する優先キューの参照解との比較: 1,920始点。
- 車輪を通常の隣接行列で作った全点対最短路との比較: 29,974頂点対。
- setを使う最小優先度探索との比較: 1,885始点。
- Prufer列による6頂点までの全ラベル付き木・全開始頂点: 8,477回の探索。
  得られた分布と873個の順列について、mod=1や非素数を含めて木の数を照合。
- BFSで復元したパス集合との比較: 23,490経路。頻度の直接集計との比較: 22,989クエリ。
- ABC481 Fの3例、ABC478 Fの3例、ABC477 Eの2例、ABC477 Gの2例、計10例の公式サンプル入出力。
- 20万頂点の鎖、空の森、別成分、自己パス、同値ラベル、多重辺、自己ループ、0重み、大きな整数と範囲違反。

上記はローカル比較テストです。AtCoderへの提出によるAC判定や、元問題の時間制限を満たす保証とは区別しています。
通常・デバッグビルドでの一括確認は次のコマンドです。

```sh
python tests/run_tests.py
python tests/run_tests.py --debug
```

全コードはincludeなしで、冒頭に用途・使い方・前提・計算量・依存をコメントに記載しています。
利用例は [グラフの説明](../graph/README.md) と [木の説明](../tree/README.md) を参照してください。
