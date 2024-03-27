/*
Rangesum(累積和をとった配列, 範囲のはじめ, 終わり)
0 <= l, r <= len - 1
制約を守ってない場合は0を返す
*/
ll Rangesum(vector<ll> &vec, int l, int r) {
    ll ansL, ansR;
    if (l > r) swap(l, r);
    if (l < 0) return 0;
    if (r >= (int)vec.size()) return 0;
    if (l == 0) ansL = 0;
    else ansL = vec[l - 1];
    ansR = vec[r];
    return ansR - ansL;
}
