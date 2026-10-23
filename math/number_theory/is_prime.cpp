// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/is_prime.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 試し割りで整数が素数か判定する。
使い方:
  bool a = is_prime(97); // true。is_prime(1)はfalse。
前提: x<=1はfalse。正の入力ではi*iがlong long内に収まる範囲(例えばx<=10^18)で使う。
計算量: 正のxに対して時間O(sqrt(x))、追加メモリO(1)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//素数判定
bool is_prime(long long x) {
    if (x <= 1) return false;
    for (long long i = 2; i * i <= x; i ++) if(x % i == 0) return false;
    return true;
}
