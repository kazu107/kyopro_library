// https://github.com/kazu107/kyopro_library/blob/main/math/number_theory/is_prime.cpp
// SPDX-License-Identifier: CC0-1.0
//素数判定
bool is_prime(long long x) {
    if (x <= 1) return false;
    for (long long i = 2; i * i <= x; i ++) if(x % i == 0) return false;
    return true;
}
