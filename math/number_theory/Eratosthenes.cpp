// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/Eratosthenes.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: エラトステネスの篩でN以下の素数を昇順に列挙する。
使い方:
  vector<long long> primes = Eratosthenes(10); // {2,3,5,7}。
前提: N>=1で配列を確保できる範囲。N=0は対象外。stdの名前解決を利用側で用意する。
計算量: 時間O(N log log N)、メモリO(N)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

/*
１からNまでの素数をvectorに入れる.
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
