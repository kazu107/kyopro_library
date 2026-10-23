// https://github.com/kazu107/kyopro_library/blob/main/string/LCS.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 2つの文字列の最長共通部分列を1つ復元する。連続する部分文字列とは異なる。
使い方:
  string answer = lcs("abcde", "ace"); // "ace"。
前提: stdの名前解決を利用側で用意する。最長の答えが複数ならそのうち1つ。空文字列にも対応。
計算量: 長さN,Mに対して時間・メモリO((N+1)(M+1))。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

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
