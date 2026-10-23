// https://github.com/kazu107/kyopro_library/blob/main/math/modular/modint.cpp
// SPDX-License-Identifier: CC0-1.0
//x^y%z 
long long modint(long long x, long long y, long long z) {
    if (y == 0) return 1;
    else if (y % 2 == 0) {
        long long t = modint(x, y / 2, z);
        return t * t % z;
    }
    else return modint(x, y - 1, z) * x % z;
}