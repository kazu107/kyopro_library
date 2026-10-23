// https://github.com/kazu107/kyopro_library/blob/main/tests/lazy_segtree_test.cpp
// SPDX-License-Identifier: CC0-1.0
// 配列の直接更新・走査を正解として照合するC++17テスト。
// 空区間、非2冪長、負数、同値の個数と左端の位置、一点代入、再構築、更新の合成順序を確認。
#include "../data_structure/LagySegTree.cpp"

#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>

#ifdef NDEBUG
#error "Run these tests with assertions enabled."
#endif

using ll = long long;
constexpr ll HI = std::numeric_limits<ll>::max();
constexpr ll LO = std::numeric_limits<ll>::lowest();

struct AddSuite {
    std::vector<ll> a;
    LazySegTree<RangeAddSum> sum;
    LazySegTree<RangeAddMin> minimum;
    LazySegTree<RangeAddMax> maximum;
    LazySegTree<RangeAddMinCount> min_count;
    LazySegTree<RangeAddMaxCount> max_count;
    LazySegTree<RangeAddMinIndex> min_index;
    LazySegTree<RangeAddMaxIndex> max_index;
    LazySegTree<RangeAddStats> stats;

    explicit AddSuite(const std::vector<ll>& values)
        : a(values), sum(a), minimum(a), maximum(a),
          min_count(a), max_count(a), min_index(a), max_index(a), stats(a) {}

    void add(int l, int r, ll delta) {
        for (int i = l; i < r; ++i) a[i] += delta;
        sum.range_apply(l, r, delta);
        minimum.range_apply(l, r, delta);
        maximum.range_apply(l, r, delta);
        min_count.range_apply(l, r, delta);
        max_count.range_apply(l, r, delta);
        min_index.range_apply(l, r, delta);
        max_index.range_apply(l, r, delta);
        stats.range_apply(l, r, delta);
    }

    void set(int idx, ll value) {
        a[idx] = value;
        sum.set_point(idx, value);
        minimum.set_point(idx, value);
        maximum.set_point(idx, value);
        min_count.set_point(idx, value);
        max_count.set_point(idx, value);
        min_index.set_point(idx, value);
        max_index.set_point(idx, value);
        stats.set_point(idx, value);
    }

    void init(int n, ll value = 0) {
        a.assign(n, value);
        sum.init(n, value);
        minimum.init(n, value);
        maximum.init(n, value);
        min_count.init(n, value);
        max_count.init(n, value);
        min_index.init(n, value);
        max_index.init(n, value);
        stats.init(n, value);
    }

    void build(const std::vector<ll>& values) {
        a = values;
        sum.build(a);
        minimum.build(a);
        maximum.build(a);
        min_count.build(a);
        max_count.build(a);
        min_index.build(a);
        max_index.build(a);
        stats.build(a);
    }

    void check_values(int l, int r, ll s, ll low, ll high,
                      RangeAddMinCount::S lc, RangeAddMaxCount::S hc,
                      RangeAddMinIndex::S li, RangeAddMaxIndex::S hi,
                      RangeAddStats::S st) const {
        ll expected_sum = 0, expected_min = HI, expected_max = LO;
        for (int i = l; i < r; ++i) {
            expected_sum += a[i];
            expected_min = std::min(expected_min, a[i]);
            expected_max = std::max(expected_max, a[i]);
        }
        int lows = 0, highs = 0;
        int leftmost_min = -1, leftmost_max = -1;
        for (int i = l; i < r; ++i) {
            lows += a[i] == expected_min;
            highs += a[i] == expected_max;
            if (a[i] == expected_min && leftmost_min == -1) leftmost_min = i;
            if (a[i] == expected_max && leftmost_max == -1) leftmost_max = i;
        }
        assert(s == expected_sum && low == expected_min && high == expected_max);
        assert(lc.value == expected_min && lc.count == lows);
        assert(hc.value == expected_max && hc.count == highs);
        assert(li.value == expected_min && li.index == leftmost_min);
        assert(hi.value == expected_max && hi.index == leftmost_max);
        assert(st.sum == expected_sum && st.minimum == expected_min);
        assert(st.maximum == expected_max && st.length == r - l);
    }

    void check(int l, int r) {
        check_values(l, r, sum.prod(l, r), minimum.prod(l, r), maximum.prod(l, r),
                     min_count.prod(l, r), max_count.prod(l, r),
                     min_index.prod(l, r), max_index.prod(l, r), stats.prod(l, r));
    }

    void check_all() const {
        const int n = static_cast<int>(a.size());
        assert(sum.size() == n && minimum.size() == n && maximum.size() == n);
        assert(min_count.size() == n && max_count.size() == n && stats.size() == n);
        assert(min_index.size() == n && max_index.size() == n);
        check_values(0, n, sum.all_prod(), minimum.all_prod(), maximum.all_prod(),
                     min_count.all_prod(), max_count.all_prod(),
                     min_index.all_prod(), max_index.all_prod(), stats.all_prod());
    }

    void check_points() {
        for (int i = 0; i < static_cast<int>(a.size()); ++i) {
            check_values(i, i + 1, sum.get(i), minimum.get(i), maximum.get(i),
                         min_count.get(i), max_count.get(i),
                         min_index.get(i), max_index.get(i), stats.get(i));
        }
    }
};

// 加算だけでは気づけないcomposeの順序と、Fにoperator==が不要なことを確認。
struct AffineSum {
    using T = ll;
    using S = ll;
    struct F { ll mul, add; };
    static S leaf(const T& x) { return x; }
    static S e() { return 0; }
    static S op(const S& a, const S& b) { return a + b; }
    static F id() { return {1, 0}; }
    static S apply(const F& f, const S& x, int len) {
        return f.mul * x + f.add * len;
    }
    static F compose(const F& newer, const F& older) {
        return {newer.mul * older.mul, newer.mul * older.add + newer.add};
    }
};

// 結合が可換でなくても左右の順序が保存されることを確認。
// 文字列の演算はO(1)ではないため、これは汎用性の確認用。
struct AssignString {
    using T = char;
    using S = std::string;
    struct F { bool active; char value; };
    static S leaf(const T& x) { return S(1, x); }
    static S e() { return ""; }
    static S op(const S& a, const S& b) { return a + b; }
    static F id() { return {false, '\0'}; }
    static S apply(const F& f, const S& x, int len) {
        return f.active ? S(len, f.value) : x;
    }
    static F compose(const F& newer, const F& older) {
        return newer.active ? newer : older;
    }
};

void deterministic_tests() {
    LazySegTree old_style(5);
    old_style.range_apply(0, 5, 3);
    old_style.set_point(2, -4);
    assert(old_style.prod(0, 5) == 8);
    LazySegTree old_vector(std::vector<ll>{2, -1, 3});
    assert(old_vector.prod(0, 3) == 4);
    static_assert(std::is_same_v<decltype(old_style), LazySegTree<RangeAddSum>>);

    LazySegTree<RangeAddMin> zeros(5);
    LazySegTree<RangeAddMax> filled(5, -7);
    assert(zeros.all_prod() == 0 && zeros.prod(2, 2) == HI);
    assert(filled.all_prod() == -7 && filled.prod(2, 2) == LO);

    for (int n : {0, 1, 2, 3, 5, 7, 8, 9, 17, 31, 32, 33, 64}) {
        AddSuite t(std::vector<ll>(n, -3));
        t.check_all();
        t.add(0, n, 8);
        t.add(0, n, -2);
        if (n > 0) t.set(n / 2, -11); // 遅延を残したまま一点を代入
        t.check_points();
        for (int l = 0; l <= n; ++l) {
            for (int r = l; r <= n; ++r) t.check(l, r);
        }
        t.add(0, 0, HI); // 空更新は実データにも遅延値にも影響しない
        t.check_all();
        t.add(0, n, 17);
        t.init(n, 0); // 遅延が残っていても初期化で消える
        t.check_all();
        t.check_points();
        t.add(0, n, -13);
        t.build({4, -2, -2, 4, 4}); // サイズも変えて再構築
        t.check_all();
        t.check_points();
    }

    LazySegTree<RangeAddMin> real_max(std::vector<ll>{HI, HI, HI});
    real_max.range_apply(0, 3, -1);
    assert(real_max.all_prod() == HI - 1 && real_max.get(2) == HI - 1);
    LazySegTree<RangeAddMax> real_min(std::vector<ll>{LO, LO, LO});
    real_min.range_apply(0, 3, 1);
    assert(real_min.all_prod() == LO + 1 && real_min.get(2) == LO + 1);
    LazySegTree<RangeAddMinCount> count_max(std::vector<ll>{HI, HI, HI});
    count_max.range_apply(0, 3, -1);
    assert(count_max.prod(1, 3).value == HI - 1 && count_max.prod(1, 3).count == 2);
    LazySegTree<RangeAddMaxCount> count_min(std::vector<ll>{LO, LO, LO});
    count_min.range_apply(0, 3, 1);
    assert(count_min.prod(0, 2).value == LO + 1 && count_min.prod(0, 2).count == 2);

    LazySegTree<AffineSum> affine(8, 2);
    affine.range_apply(0, 8, {1, 3});
    affine.range_apply(0, 8, {-1, 0});
    assert(affine.get(3) == -5);
    affine.range_apply(0, 8, {0, 0}); // 0への代入も更新
    affine.range_apply(0, 8, {1, 7});
    affine.set_point(3, -9);
    assert(affine.prod(2, 5) == 5);

    LazySegTree<AssignString> text(std::vector<char>{'a', 'b', 'c', 'd', 'e'});
    assert(text.prod(1, 4) == "bcd");
    text.range_apply(0, 5, {true, 'x'});
    text.range_apply(1, 4, {true, 'y'});
    text.range_apply(0, 5, AssignString::id());
    text.set_point(2, 'z');
    assert(text.all_prod() == "xyzyx" && text.prod(1, 4) == "yzy");
}

void position_tests() {
    AddSuite t({5, 1, 1, 9, 9, 1, 3});
    t.check(2, 6); // 部分区間でも返すのは元の配列の添字
    assert(t.min_index.prod(2, 6).index == 2);
    assert(t.max_index.prod(2, 6).index == 3);
    t.add(0, 2, 8);
    t.check_all();
    t.set(2, 9);
    assert(t.min_index.all_prod().index == 5);
    t.add(5, 7, 8);
    t.check_all();
    t.check_points();
    assert(t.min_index.all_prod().index == 1);
    assert(t.max_index.all_prod().index == 0);

    LazySegTree<RangeAddMinIndex> zeros(5);
    LazySegTree<RangeAddMaxIndex> filled(5, -7);
    assert(zeros.all_prod().value == 0 && zeros.all_prod().index == 0);
    assert(zeros.prod(2, 5).index == 2);
    assert(filled.prod(2, 5).value == -7 && filled.prod(2, 5).index == 2);
    assert(zeros.prod(3, 3).index == -1 && filled.prod(3, 3).index == -1);
    zeros.range_apply(0, 5, 4);
    zeros.set_point(2, 0);
    assert(zeros.all_prod().index == 2 && zeros.get(2).index == 2);
    zeros.init(0);
    assert(zeros.all_prod().value == HI && zeros.all_prod().index == -1);
    zeros.build({9, 9, 8});
    assert(zeros.all_prod().index == 2);

    // 実データが番兵値と同じでも空ノードと区別し、位置を返す。
    LazySegTree<RangeAddMinIndex> upper(std::vector<ll>{HI, HI, HI});
    LazySegTree<RangeAddMaxIndex> lower(std::vector<ll>{LO, LO, LO});
    assert(upper.prod(1, 3).index == 1 && lower.prod(1, 3).index == 1);
    upper.range_apply(0, 3, -1);
    lower.range_apply(0, 3, 1);
    assert(upper.get(2).value == HI - 1 && upper.get(2).index == 2);
    assert(lower.get(2).value == LO + 1 && lower.get(2).index == 2);

    // 2引数版を優先し、構築・初期化・一点代入のすべてで添字を渡す。
    struct IndexedSum : RangeAddSum {
        using RangeAddSum::leaf;
        static S leaf(const T& value, int index) { return value + index; }
    };
    LazySegTree<IndexedSum> indexed(3);
    assert(indexed.all_prod() == 3);
    indexed.set_point(1, 10);
    assert(indexed.all_prod() == 13);
    indexed.build({1, 2});
    assert(indexed.all_prod() == 4);
    indexed.init(4);
    assert(indexed.all_prod() == 6);
}

int randomized_tests() {
    std::mt19937 random(20261023);
    int operations = 0;
    auto value = [&]() { return static_cast<ll>(random() % 41) - 20; };
    for (int n : {0, 1, 2, 3, 5, 7, 8, 9, 17, 31, 32, 33, 64}) {
        std::vector<ll> values(n);
        for (ll& x : values) x = value();
        AddSuite t(values);
        for (int step = 0; step < 3000; ++step) {
            int l = static_cast<int>(random() % (n + 1));
            int r = static_cast<int>(random() % (n + 1));
            if (l > r) std::swap(l, r);
            const int action = static_cast<int>(random() % 8);
            if (action < 3) {
                t.add(l, r, value());
            } else if (action == 3 && n > 0) {
                t.set(static_cast<int>(random() % n), value());
            } else if (action == 4 && step % 17 == 0) {
                t.init(n, value());
            } else if (action == 5 && step % 19 == 0) {
                for (ll& x : values) x = value();
                t.build(values);
            } else {
                t.check(l, r);
            }
            t.check_all();
            if (step % 73 == 0) t.check_points();
            ++operations;
        }
        t.check_points();
    }

    for (int n : {0, 1, 3, 8, 17, 33}) {
        std::vector<ll> a(n, 2);
        LazySegTree<AffineSum> tree(a);
        for (int step = 0; step < 2000; ++step) {
            int l = static_cast<int>(random() % (n + 1));
            int r = static_cast<int>(random() % (n + 1));
            if (l > r) std::swap(l, r);
            if (random() % 3 != 0) {
                const ll mul = static_cast<ll>(random() % 3) - 1;
                const ll add = value();
                tree.range_apply(l, r, {mul, add});
                for (int i = l; i < r; ++i) a[i] = mul * a[i] + add;
            } else if (n > 0) {
                const int idx = static_cast<int>(random() % n);
                const ll x = value();
                tree.set_point(idx, x);
                a[idx] = x;
            }
            assert(tree.prod(l, r) == std::accumulate(a.begin() + l, a.begin() + r, 0LL));
            assert(tree.all_prod() == std::accumulate(a.begin(), a.end(), 0LL));
            ++operations;
        }
    }
    return operations;
}

int main() {
    deterministic_tests();
    position_tests();
    const int operations = randomized_tests();
    std::cout << "PASS: 8 range-add presets, custom affine/assignment policies, "
              << operations << " randomized operations plus deterministic boundary cases\n";
}
