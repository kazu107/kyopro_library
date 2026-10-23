// https://github.com/kazu107/kyopro_library/blob/main/math/modular/largeInt_mod.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 10進数の非負整数を文字列で受け取り、その剰余を求める。
使い方:
  ll remainder = mod_of_large_int("12345678901234567890", 1000000007LL);
前提: 符号なしの数字列、mod>0。remainder*10+digitがlong long内に収まること。using ll = long longとstdの名前解決を用意する。
計算量: D桁に対して時間O(D)、文字列のコピーを除く追加メモリO(1)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//文字列で受け取った大きい整数を任意のmodであれできる
ll mod_of_large_int(string large_int, ll mod) {
    ll remainder = 0;
    for (int i = 0; i < large_int.length(); i++) {
        remainder = (remainder * 10 + (large_int[i] - '0')) % mod;
    }
    return remainder;
}
