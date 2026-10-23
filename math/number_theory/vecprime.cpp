// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/vecprime.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: intの篩を使ってn以下の素数を昇順に列挙する。
使い方:
  vector<int> primes = vecprime(10); // {2,3,5,7}。n=0/1なら空。
前提: n>=0、篩の配列を確保でき、i*i・i*jがint内に収まる範囲。stdの名前解決を用意する。
計算量: 時間O(n log log n)、メモリO(n)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//素数列挙...
vector<int> vecprime(int n){
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
    return prime2;
}
