//遅延セグメントツリーの構造体
struct SegTree {
    int tr[300000], lz[300000], trsize = 1;// trはtreeの配列、lzは遅延配列、trsizeはtreeのサイズ
    void init(int n) {
        while (trsize < n) trsize *= 2;// trsizeをn以上の最小の2のべき乗にする
        for (int i = 1; i < 2 * trsize; i++) {
            tr[i] = 0; 
            lz[i] = 0;
        }
    }
    // 遅延配列の伝搬
    void lz_spr(int cur, int l, int r) {
        if (lz[cur] == 0) return;// 遅延配列が0の場合は何もしない
        tr[cur] += lz[cur];// 遅延配列をtrに反映
        if (r - l > 1) {// 葉でない場合は子に遅延配列を伝搬
            lz[cur * 2] += lz[cur] / 2;
            lz[cur * 2 + 1] += lz[cur] / 2;
        }
        lz[cur] = 0;// 現在の遅延配列を0に戻す
    }
    // [l, r)番目の要素をnumに変更(1 <= l, r - 1 <= n)
    // a = 1, b = trsize + 1, cur = 1で呼び出す
    void update (int l, int r, int a, int b, int cur, int num) {
        lz_spr(cur, a, b);// 遅延配列の伝搬
        if (l >= b || r <= a) return;// [l, r)に[a, b)が含まれない場合
        if (l <= a && b <= r) {// [l, r)が[a, b)に完全に含まれる場合
            lz[cur] += (b - a) * num;// 遅延配列にnumを加算
            lz_spr(cur, a, b);// 遅延配列の伝搬
            return;
        }
        // [l, r)が[a, b)に一部含まれる場合
        int mid = (a + b) / 2;
        update(l, r, a, mid, cur * 2, num);// 左の子に更新
        update(l, r, mid, b, cur * 2 + 1, num);// 右の子に更新
        tr[cur] = tr[cur * 2] + tr[cur * 2 + 1];// 左右の子の和をtrに反映
    }
    // [l, r)の和を求める
    // a = 1, b = trsize + 1, cur = 1で呼び出す
    int query (int l, int r, int a, int b, int cur) {
        if (l >= b || r <= a) return 0;// [l, r)に[a, b)が含まれない場合
        lz_spr(cur, a, b);// 遅延配列の伝搬
        if (l <= a && b <= r) return tr[cur];// [l, r)が[a, b)に完全に含まれる場合
        // [l, r)が[a, b)に一部含まれる場合
        int mid = (a + b) / 2;
        int ansL = query(l, r, a, mid, cur * 2);
        int ansR = query(l, r, mid, b, cur * 2 + 1);
        return ansL + ansR;// 左右の子の和を返す
    }
};
