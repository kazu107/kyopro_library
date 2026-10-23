// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/Factorize.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 篩で得た全素数を使い、元のn以下の各素数と指数を返す旧版の素因数分解。
使い方:
  auto factors = Factorize(6); // {{2,1},{3,1},{5,0}}。指数0の項も含む。
  // 素因数だけが必要な場合はprime_factorization.cppを使う。
前提: n>=1、篩の配列を確保でき、intの積・添字があふれない範囲。llの別名とstdの名前解決を用意する。
計算量: 時間O(n log log n)、メモリO(n)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//nを素因数分解をする。a^bの形でvector<pair<int, int>>に格納.

vector<pair<int, int>> Factorize(ll n){
    vector<bool> prime1(n+1, true);
    for(int i = 2; i*i <= n; i++){
        if(prime1[i]){
            for(int j = 2; i*j <= n; j++){
                prime1[i*j] = false;
            }
        }
    }

    vector<int> prime2;
    for(int i = 2; i <= n; i++){
        if(prime1[i]) prime2.push_back(i);
    }
    vector<pair<int, int>> p;
    for (int x : prime2) {
        int cnt = 0;
        while (n % x == 0) {
            cnt++;
            n /= x;
        }
        p.push_back(make_pair(x, cnt));
    }
    return p;
}
