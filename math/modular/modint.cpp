// https://github.com/kazu107/kyopro_library/blob/main/math/modular/modint.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 繰り返し二乗法でxのy乗をzで割った余りを求める。modintクラスではなく関数。
使い方:
  long long value = modint(2, 10, 1000000007); // 1024。
前提: 0<=x<z、y>=0、z>=2。積がlong longに収まる範囲で使う(例えばz<=1000000007)。
計算量: 時間O(log(y+1))、再帰スタックO(log(y+1))。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//x^y%z 
long long modint(long long x, long long y, long long z) {
    if (y == 0) return 1;
    else if (y % 2 == 0) {
        long long t = modint(x, y / 2, z);
        return t * t % z;
    }
    else return modint(x, y - 1, z) * x % z;
}
