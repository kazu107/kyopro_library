/*
Rangesum(累積和をとった配列, 範囲のはじめ, 終わり)
0 <= l, r <= len - 1
*/

ll Rangesum(vector<ll> &vec, int l, int r) {
    ll ansL, ansR;
    if (l == 0) ansL = 0;
    else ansL = vec[l - 1];
    ansR = vec[r];
    return ansR - ansL;
}
