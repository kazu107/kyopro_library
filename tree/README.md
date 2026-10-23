# 木

新規コードはGNU C++17以降、名前空間 `cp` です。
入力には [Graph](../graph/Graph.hpp) の無向グラフを使います。

## 一覧

| ファイル | 機能 | 計算量 |
| --- | --- | --- |
| [RootedForest.hpp](RootedForest.hpp) | 親・辺・深さ・成分・DFS順・部分木サイズの共通前処理 | O(V+E) |
| [Lca.hpp](Lca.hpp) | ダブリングLCA、祖先ジャンプ、距離、経路上k番目の頂点 | 構築O(V log V)、取得O(log V) |
| [EulerTour.hpp](EulerTour.hpp) | 部分木を連続する半開区間に変換 | 構築O(V)、取得O(1) |
| [HeavyLightDecomposition.hpp](HeavyLightDecomposition.hpp) | HLD、順序付きの経路分解、部分木区間、LCA | 構築O(V)、経路分解/LCA O(log V) |
| [diameter.hpp](diameter.hpp) | 非負重みの木の直径・端点・経路・辺ID | O(V) |
| [Rerooting.hpp](Rerooting.hpp) | Policyで演算を指定する全方位木DP | 演算O(1)なら全頂点分O(V+E) |
| [rerooting.cpp](rerooting.cpp) | 既存のグローバル名 `Rerooting` の互換入口。最大距離用 | O(V+E) |
| [Subtreesize.cpp](Subtreesize.cpp) | 既存の `tr(root, adjacency, visited)`。根の次数1も正しく計算 | O(V+E) |

すべての深い探索は反復処理です。空の森に対応します。
入力が有向グラフ、または閉路・平行辺・自己ループを持つ場合は `std::invalid_argument` です。
直径以外は非連結の森にも対応し、指定したrootの成分を最初に根付け、残りは頂点番号順に根を選びます。
空でない場合、rootは有効な頂点番号である必要があります。
数値の途中計算は指定した型に収めてください。

## LCA

`Lca<Cost> lca(g, root = 0)` を作成します。

- `lca(u,v)`: 最近共通祖先。別成分なら-1。
- `kth_ancestor(v,k)`: k個上の祖先。k=0ならv。負数や深さより大きいkなら-1。
- `distance(u,v)`: 重み付きの唯一の経路の長さ。別成分ならnullopt。
- `kth_on_path(u,v,k)`: uを0番目とした経路上の頂点。別成分や範囲外なら-1。
- `forest()`: 前処理した親、深さ、成分などを参照。

負辺を持つ木でもLCA・距離・全方位木DPは使えます。
2回の探索で求める `tree_diameter` は非負重みの連結な木に限定し、条件違反なら例外にします。
空の木の直径は長さ0・端点-1・空の経路、1頂点なら長さ0・その頂点のみです。

## Euler Tour

`EulerTour<Cost> tour(g, root = 0)` は各頂点を1回だけ配列に並べます。
部分木vの区間は `[tin[v], tout[v])`、`vertex_at[i]` は元の頂点番号です。
`subtree_range(v)` は区間の組、`is_ancestor(u,v)` は祖先判定を返します。
別成分の祖先判定はfalseです。

## HLDと遅延セグメント木

`HeavyLightDecomposition<Cost> hld(g, root = 0)` を作成します。

- `pos[v]`: 元の頂点vに対応する配列位置。
- `vertex_at[i]`: 配列位置iに対応する元の頂点。
- `path_segments(u,v)`: uからvへ進む順の区間リスト。
  各区間は `{l,r,reversed}` です。
  reversed=falseならlからr-1、trueならr-1からlへ読みます。
- `path_segments(u,v,true)`: 辺の値用。各辺の値を深い側の頂点のposに保存し、LCAの頂点を含めません。
- `subtree_range(v)`: 頂点の部分木区間。第2引数trueならv自身を除いた辺用区間。
- `lca(u,v)`: 別成分なら-1。別成分のpath_segmentsは空です。

和・最小・最大など可換な取得では区間の向きを無視できます。
文字列結合などではreversedを考慮したデータの保持・取得が必要です。
通常のセグメント木と組み合わせると経路への更新・取得はO(log^2 V)、部分木はO(log V)になります。

`RangeAddMinIndex` / `RangeAddMaxIndex` が返すindexは **HLDで並べ替えた配列の位置**です。
元の頂点番号へは `hld.vertex_at[index]` で戻せます。
同値のときに選ばれるのは配列位置が最小の頂点で、元の頂点番号が最小とは限りません。

標準ヘッダと「使用するコード」の依存先をテンプレートに貼り付けた後の利用例です。

```cpp
// 使用するコード: tree/Lca.hpp, tree/HeavyLightDecomposition.hpp, data_structure/LagySegTree.cpp
int main() {
    cp::Graph<> g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);

    cp::HeavyLightDecomposition<> hld(g);
    std::vector<long long> value = {1, 2, 3, 4, 5}, flattened(5);
    for (int v = 0; v < 5; ++v) flattened[hld.pos[v]] = value[v];
    LazySegTree<RangeAddSum> seg(flattened);

    auto path_sum = [&](int u, int v) {
        long long sum = 0;
        for (auto part : hld.path_segments(u, v)) sum += seg.prod(part.l, part.r);
        return sum;
    };
    std::cout << path_sum(2, 4) << '\n'; // 14
    for (auto part : hld.path_segments(2, 4)) seg.range_apply(part.l, part.r, 10);
    std::cout << path_sum(2, 4) << '\n'; // 54

    cp::Lca<> lca(g);
    std::cout << lca.lca(2, 4) << ' ' << *lca.distance(2, 4) << '\n'; // 1 3
}
```

## 汎用の全方位木DP

`Rerooting<Policy, Cost = long long>` に演算を渡します。
省略時は `MaxDistanceRerooting` で、自分自身までの距離0も含む各頂点からの最大距離です。

Policyには次を用意します。

| 定義 | 意味 |
| --- | --- |
| `DP` | 各方向から集める情報の型 |
| `identity()` | mergeの単位元 |
| `merge(left,right)` | 隣接リスト順に情報を結合。結合則が必要 |
| `lift(dp,edge)` | 隣から辺を越えて情報を受け取る。edgeは送信元fromから受信先toの向き |
| `add_vertex(dp,v)` | 頂点v自身の効果を追加 |

隣接リスト順を維持するため、mergeが可換でない場合にも対応します。
各演算・DPコピーがO(1)なら全頂点分をO(V+E)で求めます。
次は各頂点から全頂点への距離の和を取得する例です。

```cpp
// 使用するコード: tree/Rerooting.hpp
struct SumDistances {
    struct DP { long long sum; int count; };
    static DP identity() { return {0, 0}; }
    static DP merge(const DP& a, const DP& b) { return {a.sum + b.sum, a.count + b.count}; }
    static DP lift(const DP& x, const cp::Edge<long long>& e) {
        return {x.sum + x.count * e.cost, x.count};
    }
    static DP add_vertex(const DP& x, int) { return {x.sum, x.count + 1}; }
};

int main() {
    cp::Rerooting<SumDistances> dp(3);
    dp.add_edge(0, 1);
    dp.add_edge(1, 2);
    auto answer = dp.build();
    std::cout << answer[0].sum << ' ' << answer[1].sum << ' ' << answer[2].sum << '\n'; // 3 2 3
}
```

既存の `Rerooting r(n); r.add_edge(...); r.build();` は
`rerooting.cpp` の互換名で引き続き使えます。従来の公開フィールドN/gへの直接アクセスは互換対象外です。
部分木サイズのtrは渡されたvisitedを更新します。根はvisitedの初期値によらず含み、
それ以外は未訪問の隣接頂点だけ探索します。

## 検証

[tree_test.cpp](../tests/tree_test.cpp) は180個の木・森を使い、BFSで求めた親・経路・距離と、
全頂点を根にした素朴なDPと照合します。負辺、空の森、別成分、順序を持つ全方位木DPも含みます。
HLDの区間順序と辺用区間を確認し、既存の遅延セグメント木との組み合わせをランダム操作で検証します。
10万頂点の鎖でLCA・HLD・直径・全方位木DP・既存trの非再帰動作も確認します。
