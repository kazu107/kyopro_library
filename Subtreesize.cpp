//ある根の部分木の大きさを数える・
//tr(根, 部分木, 頂点の数のbool配列(falseで初期化))
int tr(int x, vector<vector<int>> &vec, vector<bool> &check) {
    if ((int)vec[x].size() <= 1) return 1;
    int sum = 0;
    for (int i = 0; i < (int)vec[x].size(); i ++) {
        if (check[vec[x][i]]) continue;
        check[vec[x][i]] = true;
        sum += tr(vec[x][i], vec, check);
    }
    return sum + 1;
}