// https://github.com/kazu107/kyopro_library/blob/main/tests/flow_test.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 最大流・最小費用流・二部マッチングを、全カット・流量全列挙・部分集合DPと照合する。
使い方:
  python tests/run_tests.py // このテストを含む全テストを実行。--debugで境界チェックを有効化。
前提: このファイル単体にはヘッダを含めない。依存コードと標準ヘッダはテストランナーが補い、assertを有効にする。
計算量: ケース数と全列挙の規模に依存。固定seedの比較テストと境界条件を実行。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: flow/Dinic.hpp, flow/MinCostFlow.hpp, flow/BipartiteMatching.hpp

// 最大流は全カット、最小費用流は各辺の流量を全列挙、マッチングは部分集合DPと照合。

#ifdef NDEBUG
#error "Assertions must be enabled."
#endif
using ll=long long;
struct InputEdge{int u,v,capacity,cost;};
ll min_cut_reference(int n,const std::vector<InputEdge>& edges){
    ll best=std::numeric_limits<ll>::max();
    for(int mask=0;mask<(1<<n);++mask)if((mask&1)&&!(mask>>(n-1)&1)){
        ll cut=0;for(auto e:edges)if((mask>>e.u&1)&&!(mask>>e.v&1))cut+=e.capacity;
        best=std::min(best,cut);
    }
    return best;
}
std::pair<ll,ll> min_cost_reference(int n,const std::vector<InputEdge>& edges,int limit){
    std::vector<int> balance(n);std::pair<ll,ll> best{0,0};
    std::function<void(int,ll)> enumerate=[&](int index,ll cost){
        if(index==static_cast<int>(edges.size())){
            int flow=balance[0];if(flow<0||flow>limit||balance[n-1]!=-flow)return;
            for(int v=1;v<n-1;++v)if(balance[v]!=0)return;
            if(flow>best.first||(flow==best.first&&cost<best.second))best={flow,cost};
            return;
        }
        auto e=edges[index];
        for(int sent=0;sent<=e.capacity;++sent){
            balance[e.u]+=sent;balance[e.v]-=sent;
            enumerate(index+1,cost+static_cast<ll>(sent)*e.cost);
            balance[e.u]-=sent;balance[e.v]+=sent;
        }
    };
    enumerate(0,0);return best;
}
template<class Edge>
void check_flow(int n,const std::vector<Edge>& edges,ll total){
    std::vector<ll> balance(n);
    for(const auto& e:edges){
        assert(0<=e.flow&&e.flow<=e.capacity);
        balance[e.from]+=e.flow;balance[e.to]-=e.flow;
    }
    assert(balance[0]==total&&balance[n-1]==-total);
    for(int v=1;v<n-1;++v)assert(balance[v]==0);
}
int matching_reference(int right_n,const std::vector<std::vector<int>>& graph){
    std::vector<int> dp(1<<right_n,-1);dp[0]=0;
    for(const auto& neighbors:graph){
        auto next=dp;
        for(int mask=0;mask<(1<<right_n);++mask)if(dp[mask]>=0)
            for(int v:neighbors)if(!(mask>>v&1))next[mask|(1<<v)]=std::max(next[mask|(1<<v)],dp[mask]+1);
        dp=std::move(next);
    }
    return *std::max_element(dp.begin(),dp.end());
}
int main(){
    std::mt19937 random(20261027);
    for(int trial=0;trial<550;++trial){
        int n=2+static_cast<int>(random()%6);std::vector<InputEdge> input;cp::Dinic<> flow(n);
        int m=static_cast<int>(random()%17);
        for(int i=0;i<m;++i){
            InputEdge e{static_cast<int>(random()%n),static_cast<int>(random()%n),static_cast<int>(random()%5),0};
            input.push_back(e);assert(flow.add_edge(e.u,e.v,e.capacity)==i);
        }
        ll expected=min_cut_reference(n,input);ll limit=random()%7;
        ll first=flow.flow(0,n-1,limit);assert(first==std::min(expected,limit));
        ll rest=flow.flow(0,n-1);assert(first+rest==expected);assert(flow.flow(0,n-1)==0);
        check_flow(n,flow.edges(),expected);
        auto cut=flow.min_cut(0);assert(cut[0]&&!cut[n-1]);
        ll capacity=0;for(auto e:input)if(cut[e.u]&&!cut[e.v])capacity+=e.capacity;assert(capacity==expected);
        for(int id=0;id<m;++id){auto e=flow.get_edge(id);assert(e.from==input[id].u&&e.to==input[id].v&&e.capacity==input[id].capacity);}
    }
    for(int trial=0;trial<420;++trial){
        int n=2+static_cast<int>(random()%4),m=static_cast<int>(random()%8);std::vector<InputEdge> input;cp::MinCostFlow<> flow(n);
        for(int i=0;i<m;++i){
            int u=static_cast<int>(random()%n),v=static_cast<int>(random()%n),cost;
            if(trial%2==0){
                // 負費用も使うが、初期グラフはDAGにして負閉路を作らない。
                if(u==v)v=(v+1)%n;
                if(u>v)std::swap(u,v);
                cost=static_cast<int>(random()%11)-5;
            }else cost=static_cast<int>(random()%8);
            InputEdge e{u,v,static_cast<int>(random()%3),cost};input.push_back(e);
            assert(flow.add_edge(e.u,e.v,e.capacity,e.cost)==i);
        }
        int limit=static_cast<int>(random()%7);
        auto expected_first=min_cost_reference(n,input,limit);auto first=flow.flow(0,n-1,limit);assert(first==expected_first);
        auto expected_all=min_cost_reference(n,input,1000);auto rest=flow.flow(0,n-1);
        assert(first.first+rest.first==expected_all.first&&first.second+rest.second==expected_all.second);
        check_flow(n,flow.edges(),expected_all.first);
        ll cost=0;for(const auto& e:flow.edges())cost+=e.flow*e.cost;assert(cost==expected_all.second);
        for(int id=0;id<m;++id){auto e=flow.get_edge(id);assert(e.capacity==input[id].capacity&&e.cost==input[id].cost);}
    }
    for(int trial=0;trial<1000;++trial){
        int left=static_cast<int>(random()%8),right=static_cast<int>(random()%8);
        cp::BipartiteMatching matching(left,right);std::vector<std::vector<int>> graph(left);
        if(left&&right)for(int i=0,m=static_cast<int>(random()%35);i<m;++i){
            int u=static_cast<int>(random()%left),v=static_cast<int>(random()%right);matching.add_edge(u,v);graph[u].push_back(v);
        }
        int expected=matching_reference(right,graph);assert(matching.solve()==expected);assert(matching.solve()==expected);
        assert(static_cast<int>(matching.pairs().size())==expected);
        std::vector<bool> seen_left(left,false),seen_right(right,false);
        for(auto [u,v]:matching.pairs()){
            assert(!seen_left[u]&&!seen_right[v]);seen_left[u]=true;seen_right[v]=true;
            assert(std::find(graph[u].begin(),graph[u].end(),v)!=graph[u].end());
            assert(matching.left_match[u]==v&&matching.right_match[v]==u);
        }
        auto cover=matching.minimum_vertex_cover();
        assert(static_cast<int>(cover.first.size()+cover.second.size())==expected);
        std::vector<bool> covered_left(left,false),covered_right(right,false);
        for(int u:cover.first)covered_left[u]=true;
        for(int v:cover.second)covered_right[v]=true;
        for(int u=0;u<left;++u)for(int v:graph[u])assert(covered_left[u]||covered_right[v]);
        if(left&&right){matching.add_edge(0,0);graph[0].push_back(0);assert(matching.solve()==matching_reference(right,graph));}
    }
    cp::MinCostFlow<> reroute(6);
    reroute.add_edge(0,1,1,0);reroute.add_edge(0,2,1,0);
    reroute.add_edge(1,3,1,0);reroute.add_edge(1,4,1,1);
    reroute.add_edge(2,3,1,1);reroute.add_edge(2,4,1,100);
    reroute.add_edge(3,5,1,0);reroute.add_edge(4,5,1,0);
    assert((reroute.flow(0,5)==std::pair<ll,ll>{2,2}));
    cp::MinCostFlow<> invalid(3);invalid.add_edge(0,1,1,0);invalid.add_edge(1,1,1,-1);invalid.add_edge(1,2,1,1);
    bool rejected=false;try{(void)invalid.flow(0,2);}catch(const std::domain_error&){rejected=true;}assert(rejected);
    for(auto e:invalid.edges())assert(e.flow==0);
    cp::MinCostFlow<> unreachable(4);unreachable.add_edge(0,1,2,1);unreachable.add_edge(3,3,1,-1);
    assert((unreachable.flow(0,1)==std::pair<ll,ll>{2,2}));
    cp::MinCostFlow<> large(2);large.add_edge(0,1,1000000000000LL,1000);
    assert((large.flow(0,1)==std::pair<ll,ll>{1000000000000LL,1000000000000000LL}));
    cp::Dinic<> wide(2);wide.add_edge(0,1,3000000000000000000LL);assert(wide.flow(0,1)==3000000000000000000LL);
    const int n=100000;cp::Dinic<> chain(n);
    for(int v=1;v<n;++v)chain.add_edge(v-1,v,1);
    assert(chain.flow(0,n-1)==1);
    const int bip_n=30000;cp::BipartiteMatching long_path(bip_n,bip_n);
    for(int u=0;u<bip_n-1;++u){long_path.add_edge(u,u);long_path.add_edge(u,u+1);}
    long_path.add_edge(bip_n-1,0);assert(long_path.solve()==bip_n);
    std::cout<<"PASS flow: 550 all-cut checks, 420 exhaustive min-cost cases, 1000 matching DP cases, deep augmenting paths\n";
}
