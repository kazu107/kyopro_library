// https://github.com/kazu107/kyopro_library/blob/main/prefix_sum/Rangesum2D.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 作成済みの二次元累積和から、両端を含む長方形の和を取得する。
使い方:
  vector<vector<int>> prefix = {{1,3},{4,10}}; int sum = Rangesum2D(prefix, 1, 0, 1, 1); // 7。
前提: prefix[i][j]は元配列の左上から(i,j)までの和。0<=a<=c<H、0<=b<=d<W。途中の和をintに収め、stdの名前解決を用意する。
計算量: 時間O(1)、追加メモリO(1)。累積和の構築は呼び出し側で行う。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//Rangesum2D(２次元累積和をとった２次元配列, 範囲のはじめ行, 範囲のはじめ列, 終わり行, 終わり列)
//0 <= a, b, c, d <= len - 1

int Rangesum2D(vector<vector<int>> &sum, int a, int b, int c, int d) {
    if (a == 0 && b == 0) {
        return sum[c][d];
    }
    else if (a == 0) {
        return sum[c][d] - sum[c][b - 1];
    }
    else if (b == 0) {
        return sum[c][d] - sum[a - 1][d];
    }
    else {
        return sum[c][d] - sum[a - 1][d] - (sum[c][b - 1] - sum[a - 1][b - 1]);
    }
}
