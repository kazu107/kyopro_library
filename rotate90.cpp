//rotate_poly(回転させたい2次元配列)
//時計回りに９０度回転!
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