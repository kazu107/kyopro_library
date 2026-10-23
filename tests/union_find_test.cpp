// https://github.com/kazu107/kyopro_library/blob/main/tests/union_find_test.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 通常・重み付き・RollbackのUnion-FindをBFSの連結性・既知の差・履歴の削除と照合する。
使い方:
  python tests/run_tests.py // このテストを含む全テストを実行。--debugで境界チェックを有効化。
前提: このファイル単体にはヘッダを含めない。依存コードと標準ヘッダはテストランナーが補い、assertを有効にする。
計算量: ケース数と全列挙の規模に依存。固定seedの比較テストと境界条件を実行。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: data_structure/UnionFind.hpp, data_structure/WeightedUnionFind.hpp, data_structure/RollbackUnionFind.hpp

// BFSによる連結性と、既知のポテンシャル差を正解として比較する。

#ifdef NDEBUG
#error "Assertions must be enabled."
#endif

std::vector<std::vector<bool>> reference(int n, const std::vector<std::pair<int,int>>& edges) {
    std::vector<std::vector<int>> graph(n);
    for (auto [u,v] : edges) { graph[u].push_back(v); graph[v].push_back(u); }
    std::vector<std::vector<bool>> result(n,std::vector<bool>(n,false));
    for(int s=0;s<n;++s) {
        std::vector<int> stack{s};result[s][s]=true;
        while(!stack.empty()) {
            int v=stack.back();stack.pop_back();
            for(int to:graph[v]) if(!result[s][to]) {result[s][to]=true;stack.push_back(to);}
        }
    }
    return result;
}
template<class UF>
void check(UF& uf,int n,const std::vector<std::pair<int,int>>& edges) {
    const auto reach=reference(n,edges);
    std::set<int> leaders;
    for(int u=0;u<n;++u) {
        int size=0;leaders.insert(uf.leader(u));
        for(int v=0;v<n;++v) {assert(uf.same(u,v)==reach[u][v]);size+=reach[u][v];}
        assert(uf.size(u)==size);
    }
    assert(uf.component_count()==static_cast<int>(leaders.size()));
}
int main(){
    std::mt19937 random(20261025);
    int operations=0;
    for(int n: {1,2,3,7,19}) {
        cp::UnionFind uf(n);
        std::vector<std::pair<int,int>> edges;
        for(int step=0;step<1000;++step){
            int u=static_cast<int>(random()%n),v=static_cast<int>(random()%n);
            bool already=reference(n,edges)[u][v];assert(uf.merge(u,v)==!already);
            edges.push_back({u,v});
            if(step%23==0) check(uf,n,edges);
            ++operations;
        }
        check(uf,n,edges);
        auto groups=uf.groups();int seen=0;
        for(const auto& group:groups) {assert(!group.empty());seen+=static_cast<int>(group.size());for(int v:group)assert(uf.same(group.front(),v));}
        assert(seen==n);

        cp::RollbackUnionFind rollback(n);
        edges.clear();
        for(int step=0;step<2000;++step){
            int action=static_cast<int>(random()%5);
            if(action<3||edges.empty()){
                int u=static_cast<int>(random()%n),v=static_cast<int>(random()%n);
                bool already=reference(n,edges)[u][v];assert(rollback.merge(u,v)==!already);edges.push_back({u,v});
            } else if(action==3) {rollback.undo();edges.pop_back();}
            else {
                std::size_t token=random()%(edges.size()+1);rollback.rollback(token);edges.resize(token);
            }
            assert(rollback.snapshot()==edges.size());
            check(rollback,n,edges);++operations;
        }
        rollback.rollback(0);edges.clear();check(rollback,n,edges);

        std::vector<long long> potential(n);
        for(auto& value:potential)value=static_cast<long long>(random()%2001)-1000;
        cp::WeightedUnionFind<> weighted(n);edges.clear();
        for(int step=0;step<1000;++step){
            int u=static_cast<int>(random()%n),v=static_cast<int>(random()%n);
            const auto reach=reference(n,edges);
            const long long difference=potential[v]-potential[u];
            if(reach[u][v]&&random()%3==0) assert(!weighted.merge(u,v,difference+1));
            else {assert(weighted.merge(u,v,difference));edges.push_back({u,v});}
            if(step%19==0){
                check(weighted,n,edges);const auto connected=reference(n,edges);
                for(int a=0;a<n;++a)for(int b=0;b<n;++b){
                    auto diff=weighted.diff(a,b);assert(diff.has_value()==connected[a][b]);
                    if(diff)assert(*diff==potential[b]-potential[a]);
                }
            }
            ++operations;
        }
    }
    cp::UnionFind empty;assert(empty.component_count()==0&&empty.groups().empty());
    cp::RollbackUnionFind zero;zero.rollback(0);assert(zero.component_count()==0);
    cp::WeightedUnionFind<> none;assert(none.component_count()==0);
    std::cout<<"PASS union-find: "<<operations<<" operations, BFS connectivity/differences/rollback/no-op undo\n";
}
