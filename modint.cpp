long long modint(long long x, long long y, long long z) {
    if (y == 0) return 1;
    else if (y % 2 == 0) {
        long long t = modint(x, y / 2, z);
        return t * t % z;
    }
    else return modint(x, y - 1, z) * x % z;
}