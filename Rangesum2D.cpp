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