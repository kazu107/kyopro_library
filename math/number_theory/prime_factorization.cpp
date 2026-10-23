// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/prime_factorization.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 正の整数を素因数分解し、素数と指数の組を昇順に返す。
使い方:
  auto factors = prime_factorization(360); // {{2,3},{3,2},{5,1}}。1なら空。
前提: n>=1、i*iがlong long内に収まる範囲。using ll = long longとstdの名前解決を用意する。
計算量: 時間O(sqrt(n))、異なる素因数の個数をKとしてメモリO(K)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

// nの素因数分解を行う。O(√n)
vector<pair<ll, ll>> prime_factorization(ll n) {
    vector<pair<ll, ll>> res;
    for (ll i = 2; i * i <= n; i ++) {
        if (n % i != 0) continue;
        ll cnt = 0;
        while (n % i == 0) {
            n /= i;
            cnt ++;
        }
        res.push_back({i, cnt});
    }
    if (n != 1) res.push_back({n, 1});
    return res;
}
