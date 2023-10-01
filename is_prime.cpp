bool is_prime(long long x) {
    if (x <= 1) return false;
    for (long long i = 2; i * i <= x; i ++) if(x % i == 0) return false;
    return true;
}
