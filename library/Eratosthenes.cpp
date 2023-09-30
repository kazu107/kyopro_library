/*
１からNまでの素数をvectorに入れる。
戻り値はvector<ll>
*/
vector<long long> Eratosthenes(long long N) {
    vector<bool> numprime(N + 1, true);
    numprime[0] = numprime[1] = false;
    for (long long i = 2; i <= N; i ++) {
        if (!numprime[i]) continue;
        for (long long j = i * 2; j <= N; j += i) {
            numprime[j] = false;
        }
    }
    vector<long long> ansprime;
    for (long long i = 0; i <= N; i ++) {
        if (numprime[i]) ansprime.push_back(i);
    }
    return ansprime;
}