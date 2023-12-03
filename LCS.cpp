//最長共通部分列
string lcs (string s, string t) {
    int s_len = (int)s.size(), t_len = (int)t.size();
    vector<vector<int>> vec(s_len + 1, vector<int>(t_len + 1));
    for (int i = 0; i < s_len; i ++) {
        for (int j = 0; j < t_len; j ++) {
            if (s[i] == t[j]) vec[i + 1][j + 1] = vec[i][j] + 1;
            else vec[i + 1][j + 1] = max(vec[i][j + 1], vec[i + 1][j]);
        }
    }
    int x = s_len, y = t_len;
    string lcsans = "";
    while (vec[x][y]) {
        if (s[x - 1] == t[y - 1]) {
            lcsans += s[x - 1];
            x--; y--;
        }
        else {
            if (x == 0) y--;
            else if (y == 0) x--;
            else if (vec[x][y - 1] > vec[x - 1][y]) y--;
            else x--;
        }
    }
    reverse(lcsans.begin(), lcsans.end());
    return lcsans;
}