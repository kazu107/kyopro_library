// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/divEnum.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 正の整数の約数を昇順に列挙する。
使い方:
  vector<ll> divisors = divEnum(12); // {1,2,3,4,6,12}。
前提: N>0、i*iの計算がlong long内。using ll = long longとstdの名前解決を用意する。
計算量: 時間O(sqrt(N))、約数の個数をDとしてメモリO(D)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

/*
Nの約数をvectorに格納
戻り値はvector<ll>/
*/
vector<ll> divEnum(ll N) {
    vector<ll> num1, num2;
    for (ll i = 1; i * i <= N; i ++) {
        if (N % i == 0) {
            num1.push_back(i);
            num2.push_back(N / i);
        }
    }
    if (num1[(int)num1.size() - 1] == num2[(int)num2.size() - 1]) num2.pop_back();
    for (int i = (int)num2.size() - 1; i >= 0; i --) {
        num1.push_back(num2[i]);
    }
    return num1;
}
