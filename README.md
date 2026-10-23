# 競技プログラミング用 C++ ライブラリ

競技プログラミングで使う関数・データ構造を、カテゴリ別にまとめたライブラリです。
既存の18ファイルを分類し、今後コードを追加するカテゴリも用意しています。

## カテゴリ

| フォルダ | 内容 | 既存コード数 |
| --- | --- | ---: |
| [math/number_theory](math/number_theory/) | 素数、約数、素因数分解 | 6 |
| [math/modular](math/modular/) | 剰余演算、繰り返し二乗法 | 2 |
| [math/base_conversion](math/base_conversion/) | 進数変換 | 1 |
| [math/combinatorics](math/combinatorics/) | 組合せ、順列、二項係数（追加用） | 0 |
| [math/convolution](math/convolution/) | FFT、NTT、畳み込み（追加用） | 0 |
| [data_structure](data_structure/) | セグメント木などのデータ構造 | 2 |
| [prefix_sum](prefix_sum/) | 一次元・二次元の累積和 | 2 |
| [two_pointers](two_pointers/) | しゃくとり法、連続部分配列 | 1 |
| [string](string/) | 文字列アルゴリズム | 1 |
| [tree](tree/) | 木の探索、木DP、全方位木DP | 2 |
| [grid](grid/) | グリッド、二次元配列の操作 | 1 |
| [graph](graph/) | 探索、最短路、連結成分（追加用） | 0 |
| [dynamic_programming](dynamic_programming/) | 動的計画法（追加用） | 0 |
| [geometry](geometry/) | 計算幾何（追加用） | 0 |
| [flow](flow/) | 最大流、最小費用流、マッチング（追加用） | 0 |

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
| [Rangesum.cpp](prefix_sum/Rangesum.cpp) | 作成済みの累積和から閉区間 `[l, r]` の和を取得 |
| [Rangesum2D.cpp](prefix_sum/Rangesum2D.cpp) | 作成済みの二次元累積和から両端を含む長方形の和を取得 |
| [longestUniqueSubarray.cpp](two_pointers/longestUniqueSubarray.cpp) | 要素が重複しない連続部分配列の最大長 |

### 文字列・木・グリッド

| コード | 内容 |
| --- | --- |
| [LCS.cpp](string/LCS.cpp) | 最長共通部分列の復元 |
| [Subtreesize.cpp](tree/Subtreesize.cpp) | DFSによる部分木サイズの計算 |
| [rerooting.cpp](tree/rerooting.cpp) | 全方位木DP。現在の演算は各頂点からの最大距離 |
| [rotate90.cpp](grid/rotate90.cpp) | 文字列・整数の二次元配列を90度回転 |

## 使い方

各ライブラリの `.cpp` は提出用プログラムに貼り付けるスニペットです。
既存の多くのコードでは、必要な標準ヘッダ、`using namespace std;`、`using ll = long long;`、`main` を利用側で用意します。
遅延セグメント木は必要な標準ヘッダを含み、名前空間や `ll` の別名を用意せずに利用できます。
リポジトリから直接利用する場合は、次のように `#include` することもできます。

```cpp
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

#include "math/number_theory/prime_factorization.cpp"

int main() {
    for (const auto& [prime, exponent] : prime_factorization(360)) {
        cout << prime << ' ' << exponent << '\n';
    }
}
```

この例は GNU C++17 以降でコンパイルできます。
`main.cpp` としてリポジトリ直下に保存した場合：

```sh
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o main
```

カテゴリ整理では、元のファイル名・関数名・処理内容を維持しました。
その後、遅延セグメント木をテンプレート方式に改修し、8種類の取得ルールと独自ルールの追加に対応しています。
元のコードには個別の前提があり、例えば `Eratosthenes` は `N >= 1`、
`divEnum` は正の整数、`Rotate90` は空でない長方形の配列を前提としています。
`Rotate90` の実装は反時計回りで、元のコメントの「時計回り」とは一致していません。
`Subtreesize.cpp` の `tr` は、根の次数が1でも葉と判定するため、その場合の部分木サイズには修正が必要です。
既存18コードは構文確認済みです。遅延セグメント木は境界条件と51,000回のランダム操作も検証しています。
その他のアルゴリズムの正しさ・境界条件の網羅的な検証は今後の整備対象です。

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
ライブラリ18ファイルとは別に、[遅延セグメント木のテスト](tests/lazy_segtree_test.cpp)を置いています。

## コード追加時のルール

- 内容に合うカテゴリに配置します。複数のカテゴリに関係するコードは主な用途で分類します。
- 各ソースファイルの**最初の行**に、そのファイル自身のGitHub URLをコメントで書きます。
- 2行目に `// SPDX-License-Identifier: CC0-1.0` を書きます。
- 新しく実装・改修するコードには、用途・使い方・計算量・添字や区間の前提をコメントで記載します。
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
