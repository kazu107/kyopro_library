/*
使い方
区間に一様加算、区間和取得ができる遅延伝搬セグメント木
LazySegTree seg(n); // 要素数nで初期化
seg.range_apply(l, r, x); // 区間[l, r)にxを一様加算
seg.prod(l, r); // 区間[l, r)の和を取得
seg.get(i); // i番目の要素を取得
seg.set_point(i, x); // i番目の要素をxに更新
計算量: 構築O(n), 各クエリO(log n)
*/

struct LazySegTree {
    using S = long long; // データ型
    using F = long long; // 遅延値の型

    int sz;
    int n;
    vector<S> seg;
    vector<F> lazy;

    LazySegTree(int n_) { init(n_); }
    LazySegTree(const vector<S>& v) { build(v); }

    void init(int n_) {
        sz = n_;
        n = 1;
        while (n < sz) n <<= 1;
        seg.assign(2 * n, e());
        lazy.assign(2 * n, id());
    }

    void build(const vector<S>& v) {
        init((int)v.size());
        build(1, 0, n, v);
    }

    void range_apply(int l, int r, const F& f) {
        range_apply(l, r, f, 1, 0, n);
    }

    S prod(int l, int r) {
        return prod(l, r, 1, 0, n);
    }

    S get(int idx) {
        return prod(idx, idx + 1);
    }

    void set_point(int idx, const S& x) {
        set_point(idx, x, 1, 0, n);
    }

private:
    S e() const { return 0; } // データ型の単位元
    S op(const S& a, const S& b) const { return a + b; } // 区間のマージ
    F id() const { return 0; } // 遅延値の単位元
    S apply(const F& f, const S& x, int len) const { return x + f * len; } // 遅延値を区間に適用
    F compose(const F& f, const F& g) const { return f + g; } // 遅延値どうしの合成 (新しいfを既存gの上に重ねる)

    void build(int k, int l, int r, const vector<S>& v) {
        if (r - l == 1) {
            if (l < (int)v.size()) seg[k] = v[l];
            return;
        }
        int m = (l + r) / 2;
        build(k * 2, l, m, v);
        build(k * 2 + 1, m, r, v);
        seg[k] = op(seg[k * 2], seg[k * 2 + 1]);
    }

    void apply_node(int k, const F& f, int l, int r) {
        seg[k] = apply(f, seg[k], min(r, sz) - l);
        lazy[k] = compose(f, lazy[k]);
    }

    void push(int k, int l, int r) {
        if (lazy[k] == id() || r - l <= 1) return;
        int m = (l + r) / 2;
        apply_node(k * 2,     lazy[k], l, m);
        apply_node(k * 2 + 1, lazy[k], m, r);
        lazy[k] = id();
    }

    void range_apply(int a, int b, const F& f, int k, int l, int r) {
        if (r <= a || b <= l) return;
        if (a <= l && r <= b) {
            apply_node(k, f, l, r);
            return;
        }
        push(k, l, r);
        int m = (l + r) / 2;
        range_apply(a, b, f, k * 2,     l, m);
        range_apply(a, b, f, k * 2 + 1, m, r);
        seg[k] = op(seg[k * 2], seg[k * 2 + 1]);
    }

    S prod(int a, int b, int k, int l, int r) {
        if (r <= a || b <= l) return e();
        if (a <= l && r <= b) return seg[k];
        push(k, l, r);
        int m = (l + r) / 2;
        return op(prod(a, b, k * 2,     l, m),
                  prod(a, b, k * 2 + 1, m, r));
    }

    void set_point(int a, const S& x, int k, int l, int r) {
        if (r <= a || a < l) return;
        if (r - l == 1) {
            seg[k] = x;
            lazy[k] = id();
            return;
        }
        push(k, l, r);
        int m = (l + r) / 2;
        if (a < m) set_point(a, x, k * 2,     l, m);
        else       set_point(a, x, k * 2 + 1, m, r);
        seg[k] = op(seg[k * 2], seg[k * 2 + 1]);
    }
};