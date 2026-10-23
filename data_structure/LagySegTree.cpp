// https://github.com/kazu107/kyopro_library/blob/main/data_structure/LagySegTree.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 区間加算と、和・最小・最大・個数・位置など8種類の区間取得。Policyで独自の演算にも切り替えられる。
使い方:
  LazySegTree<RangeAddSum> seg(vector<long long>{3, 1, 4}); seg.range_apply(0, 2, 5);
  auto answer = seg.prod(0, 3); // 18。RangeAddMin/Max/MinCount/MaxCount/MinIndex/MaxIndex/Statsも選べる。
  seg.set_point(i, value); seg.get(i); seg.all_prod(); // 独自Policyの定義は下の説明を参照。
前提: 0始まり、区間[l,r)。長さ・各数値の途中計算は型の範囲内。空区間の位置は-1、同値の位置は左端。
計算量: 構築O(N)、区間更新・取得・一点操作O(log N)、全体取得O(1)、メモリO(N)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

/*
区間更新と区間取得のルールを型で切り替える遅延セグメント木 (C++17)。
ファイル名は既存の LagySegTree.cpp を維持している。

使い方:
  std::vector<long long> a = {3, 1, 1, 4};
  LazySegTree<RangeAddSum> seg(a);       // 区間加算・区間和
  LazySegTree<RangeAddMin> minimum(a);   // 区間加算・区間最小値
  LazySegTree<RangeAddMax> maximum(a);   // 区間加算・区間最大値
  LazySegTree<RangeAddMinCount> low(a);  // 最小値とその個数
  LazySegTree<RangeAddMaxCount> high(a); // 最大値とその個数
  LazySegTree<RangeAddMinIndex> argmin(a); // 最小値とその位置
  LazySegTree<RangeAddMaxIndex> argmax(a); // 最大値とその位置
  LazySegTree<RangeAddStats> stats(a);   // 和・最小・最大・要素数
  seg.range_apply(0, 3, 5);
  auto sum = seg.prod(0, 3);
  auto result = low.prod(0, 4);          // result.value, result.count
  auto position = argmin.prod(0, 4);     // position.value, position.index
  auto all = stats.prod(0, 4);          // all.sum, minimum, maximum, length

  LazySegTree seg(n);                   // 従来どおり区間加算・区間和、全要素0
  LazySegTree<RangeAddMin> zeros(n);     // 最小値用でも全要素0 (空区間の値とは別)
  LazySegTree<RangeAddMax> filled(n, -5);// 全要素-5

操作:
  init(n, value=0), build(vector<T>) : 配列を作り直す
  range_apply(l, r, f)              : [l, r) を更新
  prod(l, r)                       : [l, r) の集約値 S
  get(i), set_point(i, value)       : 1要素の集約値取得・値の代入
  all_prod(), size()               : 全区間の集約値・要素数

前提:
  添字は0始まり。0 <= l <= r <= size()、0 <= i < size()。
  空配列と空区間に対応。空区間の取得値は Policy::e()。
  標準の8種類は値・加算量に long long を使用する。
  位置は元の配列の0始まりの添字。同値なら左端、空区間ならindex = -1。
  値、和、遅延値の合成など、計算途中も long long の範囲に収めること。
  計算量: 構築 O(n)、更新/区間取得/一点操作 O(log n)、全区間取得 O(1)、
          メモリ O(n) (Policyの各演算が O(1) の場合)。

独自ルール Policy に必要なもの:
  T : 配列の要素型、S : 区間の集約値型、F : 更新の型
  static S leaf(const T& value)                 : 1要素を集約値へ変換
    位置が必要なら代わりに leaf(const T& value, int index) を定義する。
    両方ある場合は位置を受け取る定義を使用する。
  static S e()                                 : 空区間の値
  static S op(const S& a, const S& b)           : 左区間・右区間を結合
  static F id()                                : 何もしない更新
  static S apply(const F& f, const S& x, int len): 非空区間への更新
  static F compose(const F& newer, const F& older)
    : olderを先に、newerを後に適用する更新の合成
  opは結合則を満たし、eはその単位元とする。
  更新は区間の結合と整合し、composeは実際の適用順序と一致させる。
  Fの比較演算子は不要。空の補完ノードにはapplyを呼ばない。
*/


// 以下の8種類はすべて「区間加算」を使う。
struct RangeAddBase {
    using T = long long;
    using F = long long;
    static F id() { return 0; }
    static F compose(const F& newer, const F& older) { return newer + older; }
};

struct RangeAddSum : RangeAddBase {
    using S = long long;
    static S leaf(const T& value) { return value; }
    static S e() { return 0; }
    static S op(const S& a, const S& b) { return a + b; }
    static S apply(const F& f, const S& x, int len) { return x + f * len; }
};

struct RangeAddMin : RangeAddBase {
    using S = long long;
    static S leaf(const T& value) { return value; }
    static S e() { return std::numeric_limits<S>::max(); }
    static S op(const S& a, const S& b) { return std::min(a, b); }
    static S apply(const F& f, const S& x, int) { return x + f; }
};

struct RangeAddMax : RangeAddBase {
    using S = long long;
    static S leaf(const T& value) { return value; }
    static S e() { return std::numeric_limits<S>::lowest(); }
    static S op(const S& a, const S& b) { return std::max(a, b); }
    static S apply(const F& f, const S& x, int) { return x + f; }
};

struct RangeAddMinCount : RangeAddBase {
    struct S {
        long long value;
        int count;
    };
    static S leaf(const T& value) { return {value, 1}; }
    static S e() { return {std::numeric_limits<long long>::max(), 0}; }
    static S op(const S& a, const S& b) {
        if (a.count == 0) return b;
        if (b.count == 0) return a;
        if (a.value == b.value) return {a.value, a.count + b.count};
        return a.value < b.value ? a : b;
    }
    static S apply(const F& f, const S& x, int) { return {x.value + f, x.count}; }
};

struct RangeAddMaxCount : RangeAddBase {
    struct S {
        long long value;
        int count;
    };
    static S leaf(const T& value) { return {value, 1}; }
    static S e() { return {std::numeric_limits<long long>::lowest(), 0}; }
    static S op(const S& a, const S& b) {
        if (a.count == 0) return b;
        if (b.count == 0) return a;
        if (a.value == b.value) return {a.value, a.count + b.count};
        return a.value > b.value ? a : b;
    }
    static S apply(const F& f, const S& x, int) { return {x.value + f, x.count}; }
};

struct RangeAddMinIndex : RangeAddBase {
    struct S {
        long long value;
        int index;
    };
    static S leaf(const T& value, int index) { return {value, index}; }
    static S e() { return {std::numeric_limits<long long>::max(), -1}; }
    static S op(const S& a, const S& b) {
        if (a.index == -1) return b;
        if (b.index == -1) return a;
        if (a.value == b.value) return a.index < b.index ? a : b;
        return a.value < b.value ? a : b;
    }
    static S apply(const F& f, const S& x, int) { return {x.value + f, x.index}; }
};

struct RangeAddMaxIndex : RangeAddBase {
    struct S {
        long long value;
        int index;
    };
    static S leaf(const T& value, int index) { return {value, index}; }
    static S e() { return {std::numeric_limits<long long>::lowest(), -1}; }
    static S op(const S& a, const S& b) {
        if (a.index == -1) return b;
        if (b.index == -1) return a;
        if (a.value == b.value) return a.index < b.index ? a : b;
        return a.value > b.value ? a : b;
    }
    static S apply(const F& f, const S& x, int) { return {x.value + f, x.index}; }
};

struct RangeAddStats : RangeAddBase {
    struct S {
        long long sum;
        long long minimum;
        long long maximum;
        int length;
    };
    static S leaf(const T& value) { return {value, value, value, 1}; }
    static S e() {
        return {0, std::numeric_limits<long long>::max(),
                std::numeric_limits<long long>::lowest(), 0};
    }
    static S op(const S& a, const S& b) {
        return {a.sum + b.sum, std::min(a.minimum, b.minimum),
                std::max(a.maximum, b.maximum), a.length + b.length};
    }
    static S apply(const F& f, const S& x, int len) {
        return {x.sum + f * len, x.minimum + f, x.maximum + f, x.length};
    }
};

template <class Policy = RangeAddSum>
class LazySegTree {
public:
    using T = typename Policy::T;
    using S = typename Policy::S;
    using F = typename Policy::F;

    explicit LazySegTree(int size = 0, const T& value = T{}) { init(size, value); }
    explicit LazySegTree(const std::vector<T>& values) { build(values); }

    void init(int size, const T& value = T{}) {
        init_storage(size);
        for (int i = 0; i < size_; ++i) seg_[n_ + i] = make_leaf(value, i, 0);
        pull_all();
    }

    void build(const std::vector<T>& values) {
        init_storage(static_cast<int>(values.size()));
        for (int i = 0; i < size_; ++i) seg_[n_ + i] = make_leaf(values[i], i, 0);
        pull_all();
    }

    int size() const { return size_; }

    void range_apply(int l, int r, const F& f) {
        assert(0 <= l && l <= r && r <= size_);
        if (l == r) return;
        range_apply(l, r, f, 1, 0, n_);
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size_);
        if (l == r) return Policy::e();
        return prod(l, r, 1, 0, n_);
    }

    S all_prod() const { return seg_[1]; }

    S get(int idx) {
        assert(0 <= idx && idx < size_);
        return prod(idx, idx + 1);
    }

    void set_point(int idx, const T& value) {
        assert(0 <= idx && idx < size_);
        set_point(idx, value, 1, 0, n_);
    }

private:
    int size_ = 0;
    int n_ = 1;
    std::vector<S> seg_;
    std::vector<F> lazy_;
    std::vector<bool> has_lazy_;

    // leaf(value, index) があれば優先し、従来のleaf(value)も利用できる。
    template <class P = Policy>
    static auto make_leaf(const T& value, int index, int)
        -> decltype(P::leaf(value, index)) {
        return P::leaf(value, index);
    }

    template <class P = Policy>
    static S make_leaf(const T& value, int, long) {
        return P::leaf(value);
    }

    void init_storage(int size) {
        assert(size >= 0);
        size_ = size;
        n_ = 1;
        while (n_ < size_) n_ <<= 1;
        seg_.assign(2 * n_, Policy::e());
        lazy_.assign(n_, Policy::id());
        has_lazy_.assign(n_, false);
    }

    void pull_all() {
        for (int k = n_ - 1; k > 0; --k) pull(k);
    }

    void pull(int k) { seg_[k] = Policy::op(seg_[k * 2], seg_[k * 2 + 1]); }

    void apply_node(int k, const F& f, int l, int r) {
        const int len = std::max(0, std::min(r, size_) - l);
        if (len == 0) return;
        seg_[k] = Policy::apply(f, seg_[k], len);
        if (k < n_) {
            lazy_[k] = has_lazy_[k] ? Policy::compose(f, lazy_[k]) : f;
            has_lazy_[k] = true;
        }
    }

    void push(int k, int l, int r) {
        if (k >= n_ || !has_lazy_[k]) return;
        const int m = l + (r - l) / 2;
        apply_node(k * 2, lazy_[k], l, m);
        apply_node(k * 2 + 1, lazy_[k], m, r);
        lazy_[k] = Policy::id();
        has_lazy_[k] = false;
    }

    void range_apply(int a, int b, const F& f, int k, int l, int r) {
        if (r <= a || b <= l) return;
        if (a <= l && r <= b) {
            apply_node(k, f, l, r);
            return;
        }
        push(k, l, r);
        const int m = l + (r - l) / 2;
        range_apply(a, b, f, k * 2, l, m);
        range_apply(a, b, f, k * 2 + 1, m, r);
        pull(k);
    }

    S prod(int a, int b, int k, int l, int r) {
        if (r <= a || b <= l) return Policy::e();
        if (a <= l && r <= b) return seg_[k];
        push(k, l, r);
        const int m = l + (r - l) / 2;
        return Policy::op(prod(a, b, k * 2, l, m), prod(a, b, k * 2 + 1, m, r));
    }

    void set_point(int idx, const T& value, int k, int l, int r) {
        if (r - l == 1) {
            seg_[k] = make_leaf(value, idx, 0);
            return;
        }
        push(k, l, r);
        const int m = l + (r - l) / 2;
        if (idx < m) set_point(idx, value, k * 2, l, m);
        else set_point(idx, value, k * 2 + 1, m, r);
        pull(k);
    }
};
