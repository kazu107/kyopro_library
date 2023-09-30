/*
Nの約数をvectorに格納
戻り値はvector<ll>
*/
vector<long long> divEnum(long long N) {
    vector<ll> num1, num2;
    for (ll i = 1; i * i <= N; i ++) {
        if (N % i == 0) {
            num1.push_back(i);
            num2.push_back(N / i);
        }
    }
    if (num1[(int)num1.size()] == num2[(int)num2.size()]) num2.pop_back();
    for (int i = (int)num2.size() - 1; i >= 0; i --) {
        num1.push_back(num2[i]);
    }
    return num1;
}