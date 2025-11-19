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