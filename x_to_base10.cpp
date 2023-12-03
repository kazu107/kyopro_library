//x進数の整数Nを10進数に変換する
int base_x(string N, int x) {
    int ans = 0;
    int d = 0;
    for (int i = N.size() - 1; i >= 0; i--) {
        if ('0' <= N[i] && N[i] <= '9') {
            d = N[i] - '0';
        }
        else {
            d = N[i] - 'A' + 10;
        }
        ans += d * pow(x, N.size() - i - 1);
    }
    return ans;
}