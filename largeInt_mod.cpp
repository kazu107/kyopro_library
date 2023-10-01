ll mod_of_large_int(string large_int, ll mod) {
    ll remainder = 0;
    for (int i = 0; i < large_int.length(); i++) {
        remainder = (remainder * 10 + (large_int[i] - '0')) % mod;
    }
    return remainder;
}