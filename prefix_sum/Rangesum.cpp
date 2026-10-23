// https://github.com/kazu107/kyopro_library/blob/main/prefix_sum/Rangesum.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 作成済みの一次元累積和から、両端を含む区間和を取得する。
使い方:
  vector<ll> prefix = {2,5,9}; ll sum = Rangesum(prefix, 1, 2); // 7。
前提: prefix[i]は元配列の0〜i番目の和。区間[l,r]、0始まり。l>rなら交換、不正な端点は0。llの別名とstdの名前解決を用意する。
計算量: 時間O(1)、追加メモリO(1)。累積和の構築は呼び出し側で行う。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

/*
Rangesum(累積和をとった配列, 範囲のはじめ, 終わり)
0 <= l, r <= len - 1
制約を守ってない場合は0を返す
*/
ll Rangesum(vector<ll> &vec, int l, int r) {
    ll ansL, ansR;
    if (l > r) swap(l, r);
    if (l < 0) return 0;
    if (r >= (int)vec.size()) return 0;
    if (l == 0) ansL = 0;
    else ansL = vec[l - 1];
    ansR = vec[r];
    return ansR - ansL;
}
