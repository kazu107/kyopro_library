// https://github.com/kazu107/kyopro_library/blob/main/data_structure/SegTree.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 一点代入と、区間の最大値の取得を行う固定長配列のセグメント木。
使い方:
  SegTree seg; seg.init(n); seg.update(pos, value);
  int maximum = seg.query(l, r, 1, seg.trsize + 1, 1);
前提: 1始まり、区間[l,r)。1<=n<=131072、値は-2000000000以上のint。新しいオブジェクトをinitして使う。stdの名前解決を用意する。
計算量: 初期化O(N)、一点代入・区間取得O(log N)。固定配列300000個のintを保持。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//セグメントツリーの構造体
struct SegTree {
    int tr[300000], trsize = 1;// trはtreeの配列、trsizeはtreeのサイズ
    void init(int n) {
        while (trsize < n) trsize *= 2;// trsizeをn以上の最小の2のべき乗にする
        for (int i = 1; i < 2 * trsize; i++) tr[i] = 0; 
    }
    void update (int pos, int num) {// pos番目の要素をnumに変更(1 <= pos <= n)
        pos += trsize - 1;// posをtreeの中での位置に変換
        tr[pos] = num;// pos番目の要素をnumに変更
        while (pos > 1) {
            pos /= 2;// posの親の位置に変更
            tr[pos] = max(tr[pos * 2], tr[pos * 2 + 1]); // posの親の値を更新
        }
    }
    // [l, r)の最大値を求める
    // a = 1, b = trsize + 1, cur = 1で呼び出す
    int query (int l, int r, int a, int b, int cur) {
        if (l >= b || r <= a) return -2000000000;// [l, r)に[a, b)が含まれない場合
        if (l <= a && b <= r) return tr[cur];// [l, r)が[a, b)に完全に含まれる場合
        // [l, r)が[a, b)に一部含まれる場合
        int mid = (a + b) / 2;
        int ansL = query(l, r, a, mid, cur * 2);// 左の子の最大値
        int ansR = query(l, r, mid, b, cur * 2 + 1);// 右の子の最大値
        return max(ansL, ansR);// 左右の子の最大値の大きい方
    }
};
