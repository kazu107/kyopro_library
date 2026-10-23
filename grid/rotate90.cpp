// https://github.com/kazu107/kyopro_library/blob/main/grid/rotate90.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 文字列の二次元配列、またはintの二次元配列を反時計回りに90度回転して書き換える。
使い方:
  vector<string> grid = {"ab", "cd"}; Rotate90(grid); // {"bd", "ac"}になる。
  // vector<vector<int>>でも同じRotate90を呼び出せる。
前提: 空でない長方形の配列。各行の長さを一致させる。stdの名前解決を利用側で用意する。
計算量: H行W列に対して時間O(HW)、追加メモリO(HW)。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: なし

//Rotate90(回転させたい2次元配列)
//反時計回りに90度回転。
//２次元配列なら改造可!

void Rotate90(vector<string> &vec) {//string ver.
    int x = (int)vec.size(), y = (int)vec[0].size();
    string tmp = "";
    for (int i = 0; i < x; i ++) tmp += ".";
    vector<string> s(y, tmp);
    for (int i = 0; i < x; i ++) {
        for (int j = 0; j < y; j ++) {
            s[y - j - 1][i] = vec[i][j];
        }
    }
    vec = s;
}

void Rotate90(vector<vector<int>> &vec) { //int ver.
    int x = (int)vec.size(), y = (int)vec[0].size();
    vector<vector<int>> s(y, vector<int>(x));
    for (int i = 0; i < x; i ++) {
        for (int j = 0; j < y; j ++) {
            s[y - j - 1][i] = vec[i][j];
        }
    }
    vec = s;
}
