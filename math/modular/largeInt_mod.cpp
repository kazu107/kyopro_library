// https://github.com/kazu107/kyopro_library/blob/main/math/modular/largeInt_mod.cpp
// SPDX-License-Identifier: CC0-1.0
//文字列で受け取った大きい整数を任意のmodであれできる
ll mod_of_large_int(string large_int, ll mod) {
    ll remainder = 0;
    for (int i = 0; i < large_int.length(); i++) {
        remainder = (remainder * 10 + (large_int[i] - '0')) % mod;
    }
    return remainder;
}