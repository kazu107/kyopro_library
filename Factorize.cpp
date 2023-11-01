//nを素因数分解をする。a^bの形でvector<pair<int, int>>に格納

vector<pair<int, int>> Factorize(ll n){
    vector<bool> prime1(n+1, true);
    for(int i = 2; i*i <= n; i++){
        if(prime1[i]){
            for(int j = 2; i*j <= n; j++){
                prime1[i*j] = false;
            }
        }
    }

    vector<int> prime2;
    for(int i = 2; i <= n; i++){
        if(prime1[i]) prime2.push_back(i);
    }
    vector<pair<int, int>> p;
    for (int x : prime2) {
        int cnt = 0;
        while (n % x == 0) {
            cnt++;
            n /= x;
        }
        p.push_back(make_pair(x, cnt));
    }
    return p;
}
