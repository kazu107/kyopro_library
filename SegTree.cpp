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