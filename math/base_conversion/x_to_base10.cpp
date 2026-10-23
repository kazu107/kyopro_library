// https://github.com/kazu107/kyopro_library/blob/main/math/base_conversion/x_to_base10.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 2〜36進数の文字列を10進数のintへ変換する。
使い方:
  int value = base_x("FF", 16); // 255。数字は0〜9、大文字A〜Z。
前提: 2<=基数<=36で各桁が基数未満、結果はint内。powを使う旧実装なので大きな値の精度は要注意。stdの名前解決を用意する。
計算量: 桁数Dに対してpow呼び出しD回、追加メモリO(1)。文字列は値渡し。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//x進数の整数Nを10進数に変換する
int base_x(string N, int x) {
    int ans = 0;
    int d = 0;
    for (int i = N.size() - 1; i >= 0; i--) {
        if ('0' <= N[i] && N[i] <= '9') {
            d = N[i] - '0';
        }
        else {
            d = N[i] - 'A' + 10;
        }
        ans += d * pow(x, N.size() - i - 1);
    }
    return ans;
}
