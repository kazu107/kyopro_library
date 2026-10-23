# データ構造

## 遅延セグメント木

[LagySegTree.cpp](LagySegTree.cpp) は、取得・更新のルールをテンプレート引数で指定します。
ファイル名は従来の名前を維持しています。C++17以降で利用できます。

### 区間加算のプリセット

| 型名 | `prod(l, r)` の戻り値 | 用途 |
| --- | --- | --- |
| `RangeAddSum` | `long long` | 区間和 |
| `RangeAddMin` | `long long` | 区間最小値 |
| `RangeAddMax` | `long long` | 区間最大値 |
| `RangeAddMinCount` | `{value, count}` | 最小値と、その値を持つ要素の個数 |
| `RangeAddMaxCount` | `{value, count}` | 最大値と、その値を持つ要素の個数 |
| `RangeAddMinIndex` | `{value, index}` | 最小値と、それを持つ左端の位置 |
| `RangeAddMaxIndex` | `{value, index}` | 最大値と、それを持つ左端の位置 |
| `RangeAddStats` | `{sum, minimum, maximum, length}` | 和・最小値・最大値を一度に取得 |

いずれも要素型・加算量は `long long` です。
最小値・最大値の個数は、例えば「最小の負荷になる箇所がいくつあるか」を扱うときに使えます。
位置は元の配列の0始まりの添字です。例えば `prod(2, 6)` が返す位置も、区間内の相対位置ではなく元の添字です。
同じ最小値・最大値が複数ある場合は、問い合わせ区間内で一番小さい添字を返します。
`RangeAddStats` は取得ごとに使う値を選びたいときに便利です。
非空区間の平均は `sum / length` から計算できます。小数が必要なら除算前に浮動小数点型へ変換してください。

### 使用例

標準ヘッダを用意した提出用テンプレートへ「使用するコード」を貼り付けた後の利用例です。

```cpp
// 使用するコード: data_structure/LagySegTree.cpp
int main() {
    std::vector<long long> a = {3, 1, 1, 4};

    // この型名を RangeAddSum / RangeAddMax に変えれば取得方法を変更できる。
    LazySegTree<RangeAddMin> seg(a);
    seg.range_apply(0, 3, 5); // [3,1,1,4] -> [8,6,6,4]
    std::cout << seg.prod(0, 4) << '\n'; // 4

    LazySegTree<RangeAddMinCount> count(a);
    auto low = count.prod(0, 4);
    std::cout << low.value << ' ' << low.count << '\n'; // 1 2

    LazySegTree<RangeAddStats> stats(a);
    stats.range_apply(0, 3, 5);
    auto result = stats.prod(0, 4);
    std::cout << result.sum << ' ' << result.minimum << ' '
              << result.maximum << ' ' << result.length << '\n'; // 24 4 8 4

    LazySegTree<RangeAddMinIndex> argmin(a);
    LazySegTree<RangeAddMaxIndex> argmax(a);
    auto min_at = argmin.prod(1, 4);
    auto max_at = argmax.prod(0, 3);
    std::cout << min_at.value << ' ' << min_at.index << '\n'; // 1 1
    std::cout << max_at.value << ' ' << max_at.index << '\n'; // 3 0
    argmin.range_apply(1, 3, 5);
    std::cout << argmin.prod(1, 4).index << '\n'; // 3 (最小値は4)
}
```

各インスタンスは独立した配列を持ちます。
`seg` への更新が `count` や `stats` に反映されることはありません。

### 操作・初期化

```cpp
LazySegTree<RangeAddMin> seg(n);     // 実際のn要素をすべて0で初期化
LazySegTree<RangeAddMax> filled(n, -5); // 実際のn要素をすべて-5で初期化
LazySegTree legacy(n);              // 省略時はRangeAddSum。従来の書き方に対応
```

添字は0始まり、区間は半開区間 `[l, r)` です。

| 操作 | 意味 | 計算量 |
| --- | --- | --- |
| `init(n, value = 0)` | 同じ値を持つ配列へ作り直す | O(n) |
| `build(a)` | 配列 `a` から作り直す | O(n) |
| `range_apply(l, r, f)` | 区間を更新。上記8種類では `f` を加算 | O(log n) |
| `prod(l, r)` | 区間の集約値を取得 | O(log n) |
| `get(i)` | 1要素の集約値を取得 | O(log n) |
| `set_point(i, value)` | 1要素の値を代入 | O(log n) |
| `all_prod()` | 全区間の集約値を取得 | O(1) |
| `size()` | 要素数を取得 | O(1) |

計算量はルール内の各演算がO(1)の場合です。メモリはO(n)です。
`get(i)` の戻り値も `prod` と同じ型です。
例えば `RangeAddMinCount` では `{value, 1}`、`RangeAddStats` では `length == 1` の集約値を返します。
`RangeAddMinIndex`・`RangeAddMaxIndex` の `get(i)` は `{value, i}` を返します。
`init`・`build` は位置も新しい配列の0始まりの添字で作り直し、`set_point` は変更した位置を維持します。

空配列と空区間も利用できます。空区間への更新は何もしません。
空区間の取得値は次のとおりです。

| 型 | 空区間の値 |
| --- | --- |
| `RangeAddSum` | `0` |
| `RangeAddMin` | `LLONG_MAX` |
| `RangeAddMax` | `LLONG_MIN` |
| `RangeAddMinCount` | `{LLONG_MAX, 0}` |
| `RangeAddMaxCount` | `{LLONG_MIN, 0}` |
| `RangeAddMinIndex` | `{LLONG_MAX, -1}` |
| `RangeAddMaxIndex` | `{LLONG_MIN, -1}` |
| `RangeAddStats` | `{0, LLONG_MAX, LLONG_MIN, 0}` |

空区間の値と、実際の配列を0で初期化することは別です。
補完用の空ノードには更新を適用しないため、最小・最大用の番兵値には加算しません。
値、区間和、更新値の合成など、計算途中も `long long` の範囲に収めてください。

### 独自ルールの追加

`LazySegTree<MyPolicy>` として使うには、`MyPolicy` に次を定義します。

| 定義 | 内容 |
| --- | --- |
| `T` | 元の配列の1要素の型 |
| `S` | 区間の集約値の型 |
| `F` | 区間更新の型 |
| `leaf(value)` または `leaf(value, index)` | 1要素を集約値に変換。後者は元の配列の添字も受け取る |
| `e()` | 空区間を表す集約値 |
| `op(left, right)` | 左右の区間を順序どおりに結合 |
| `id()` | 何もしない更新 |
| `apply(f, x, length)` | 非空区間の集約値に更新を反映 |
| `compose(newer, older)` | `older` の後に `newer` を実行する更新を生成 |

`op` は結合則を満たし、`e()` は単位元とします。
更新は、区間全体に適用しても左右それぞれに適用して結合しても同じ結果になる必要があります。
更新値の比較演算子 `operator==` は不要です。
長さは本体から渡すため、区間和の更新のためだけに `S` に長さを保存する必要はありません。
`leaf` は1引数版・2引数版のどちらかを定義してください。両方がある場合は2引数版を優先します。
従来の1引数版だけの独自ルールも、そのまま利用できます。

取得方法と更新方法は、対応する組み合わせで定義します。
単に `op` をGCDなどへ置き換えても、区間加算をそのまま使えるわけではありません。
この条件は [AtCoder Libraryの遅延セグメント木の仕様](https://atcoder.github.io/ac-library/production/document_ja/lazysegtree.html)も参考になります。
この実装は独自実装で、ACLを依存ライブラリとして必要としません。

今後追加する候補は次のとおりです。現時点のプリセットには含めていません。

| 取得する情報 | 対応する更新の例・必要な情報 |
| --- | --- |
| GCD | 区間代入。区間加算ではGCDだけを保存しても更新できず、別の情報が必要 |
| XOR | 区間XOR。更新対象の要素数の偶奇も利用 |
| ビットAND・OR | 区間への同種のビット演算など、結合と整合する更新 |
| 総和と二乗和 | 区間加算。元の総和と要素数も利用し、分散などを計算 |

### テスト

[tests/lazy_segtree_test.cpp](../tests/lazy_segtree_test.cpp) は、
直接更新した配列と結果を照合します。

- 8種類のプリセットに同じ操作を与えて検証
- 最小・最大の左端位置、部分区間での元の添字、空区間の位置-1、同値の並びと更新後の位置変化
- 空配列・空区間、非2冪長、負数、同値の個数、極値と番兵値の区別
- 遅延が残った状態での一点代入、初期化、サイズを変えた再構築
- 独自のアフィン更新による合成順序の検証（0への代入を含む）
- 結合が可換でない文字列の独自ルールによる左右順序の検証
- 独自ルールの `leaf` の1引数版との互換性、2引数版の優先と添字の受け渡し
- seed固定の51,000回のランダム操作

リポジトリ直下で実行します。`build/` はGitの追跡対象外です。

```sh
mkdir build
g++ -std=c++17 -O2 -Wall -Wextra -Werror tests/lazy_segtree_test.cpp -o build/lazy_segtree_test
./build/lazy_segtree_test
```

## Union-Find

新規のUnion-FindはGNU C++17以降、名前空間 `cp` です。
頂点番号は0始まりです。標準ヘッダは利用側で用意し、必要なコードをテンプレートに貼り付けます。

| ファイル | 主な操作 | 計算量 |
| --- | --- | --- |
| [UnionFind.hpp](UnionFind.hpp) | `merge, leader, same, size, component_count, groups` | 初期化O(V)、通常の操作は償却O(alpha(V)) |
| [WeightedUnionFind.hpp](WeightedUnionFind.hpp) | 差の制約の追加、矛盾判定、差の取得 | 初期化O(V)、償却O(alpha(V)) |
| [RollbackUnionFind.hpp](RollbackUnionFind.hpp) | 結合、undo、snapshot、rollback | 結合/検索O(log V)、undo O(1) |

`UnionFind::merge(u,v)` は実際に結合したときtrue、既に連結ならfalseです。
`groups()` は成分ごとの頂点一覧で、O(V alpha(V))です。辺の削除には対応しません。

`WeightedUnionFind<T = long long>::merge(u,v,w)` は `value[v]-value[u]=w` を追加します。
戻り値は「矛盾しないか」で、同じ成分でも差が整合すればtrueです。
`diff(u,v)` は非連結ならnullopt、連結なら `value[v]-value[u]` を返します。
矛盾した制約は取り込みません。差や途中計算はT型に収めてください。

`RollbackUnionFind` は経路圧縮を行いません。
`snapshot()` のトークンを保存して `rollback(token)` で戻せます。
`undo()` はmerge呼び出し1回分を取り消し、結合しなかった呼び出しも1回に数えます。
トークンは現在の履歴の祖先に限り、巻き戻して破棄した未来のトークンは再利用しないでください。
rollbackは戻す呼び出し数に比例し、メモリはO(V+保持する呼び出し数)です。

```cpp
// 使用するコード: data_structure/UnionFind.hpp, data_structure/WeightedUnionFind.hpp, data_structure/RollbackUnionFind.hpp
int main() {
    cp::UnionFind uf(3);
    uf.merge(0, 1);
    std::cout << uf.size(0) << '\n'; // 2

    cp::WeightedUnionFind<> weighted(3);
    weighted.merge(0, 1, 4);
    weighted.merge(1, 2, -1);
    std::cout << *weighted.diff(0, 2) << '\n'; // 3

    cp::RollbackUnionFind rollback(3);
    auto token = rollback.snapshot();
    rollback.merge(0, 1);
    rollback.rollback(token);
    std::cout << rollback.same(0, 1) << '\n'; // 0
}
```

[union_find_test.cpp](../tests/union_find_test.cpp) で、合計20,000操作を
BFSによる連結性、既知のポテンシャル差、履歴の削除と照合します。
