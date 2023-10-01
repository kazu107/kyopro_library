vector<int> vecprime(int n){
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
    return prime2;
}