// https://github.com/kazu107/kyopro_library/blob/main/tests/tree_test.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: LCA・部分木・HLD・直径・全方位木DPを素朴な経路・DPと照合し、遅延木との組み合わせも確認する。
使い方:
  python tests/run_tests.py // このテストを含む全テストを実行。--debugで境界チェックを有効化。
前提: このファイル単体にはヘッダを含めない。依存コードと標準ヘッダはテストランナーが補い、assertを有効にする。
計算量: ケース数と全列挙の規模に依存。固定seedの比較テストと境界条件を実行。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: tree/Lca.hpp, tree/EulerTour.hpp, tree/HeavyLightDecomposition.hpp, tree/diameter.hpp, tree/Rerooting.hpp, tree/rerooting.cpp, tree/Subtreesize.cpp, data_structure/LagySegTree.cpp

// BFSで求めた親・実経路と、全頂点を根にした素朴DPを正解にする。

#ifdef NDEBUG
#error "Assertions must be enabled."
#endif
using ll=long long;
struct Naive {
    std::vector<int> parent,depth,component;
    explicit Naive(const cp::Graph<>& g,int root):parent(g.size(),-2),depth(g.size()),component(g.size(),-1){
        if(g.size()==0)return;
        int id=0;
        for(int i=-1;i<g.size();++i){
            int start=i==-1?root:i;
            if(parent[start]!=-2)continue;
            std::queue<int> q;q.push(start);parent[start]=-1;component[start]=id++;
            while(!q.empty()){
                int v=q.front();q.pop();
                for(const auto& e:g[v])if(parent[e.to]==-2){
                    parent[e.to]=v;depth[e.to]=depth[v]+1;component[e.to]=component[v];q.push(e.to);
                }
            }
        }
    }
    int lca(int u,int v)const {
        if(component[u]!=component[v])return -1;
        while(depth[u]>depth[v])u=parent[u];
        while(depth[v]>depth[u])v=parent[v];
        while(u!=v){u=parent[u];v=parent[v];}
        return u;
    }
    bool ancestor(int u,int v)const {for(int x=v;x!=-1;x=parent[x])if(x==u)return true;return false;}
    std::vector<int> path(int u,int v)const {
        int w=lca(u,v);if(w==-1)return {};
        std::vector<int> result,tail;
        for(int x=u;x!=w;x=parent[x])result.push_back(x);
        result.push_back(w);
        for(int x=v;x!=w;x=parent[x])tail.push_back(x);
        for(auto it=tail.rbegin();it!=tail.rend();++it)result.push_back(*it);
        return result;
    }
};
ll path_cost(const cp::Graph<>& g,const std::vector<int>& path){
    ll result=0;
    for(int i=1;i<static_cast<int>(path.size());++i){
        bool found=false;
        for(const auto& e:g[path[i-1]])if(e.to==path[i]){result+=e.cost;found=true;break;}
        assert(found);
    }
    return result;
}
struct SumDistances {
    struct DP{ll sum;int count;};
    static DP identity(){return {0,0};}
    static DP merge(const DP&a,const DP&b){return {a.sum+b.sum,a.count+b.count};}
    template<class Cost> static DP lift(const DP& x,const cp::Edge<Cost>& e){return {x.sum+x.count*e.cost,x.count};}
    static DP add_vertex(const DP& x,int){return {x.sum,x.count+1};}
};
struct Trace {
    using DP=std::string;
    static DP identity(){return "";}
    static DP merge(const DP&a,const DP&b){return a+b;}
    template<class Cost>static DP lift(const DP& x,const cp::Edge<Cost>& e){return "<"+std::to_string(e.from)+":"+std::to_string(e.to)+":"+x+">";}
    static DP add_vertex(const DP& x,int v){return "("+std::to_string(v)+x+")";}
};
std::vector<int> expand(const cp::HeavyLightDecomposition<>& h,const std::vector<cp::PathSegment>& segments){
    std::vector<int> result;
    for(const auto& segment:segments){
        assert(0<=segment.l&&segment.l<segment.r&&segment.r<=static_cast<int>(h.pos.size()));
        if(segment.reversed)for(int p=segment.r-1;p>=segment.l;--p)result.push_back(h.vertex_at[p]);
        else for(int p=segment.l;p<segment.r;++p)result.push_back(h.vertex_at[p]);
    }
    return result;
}
int main(){
    std::mt19937 random(20261026);
    int integration_operations=0;
    for(int trial=0;trial<180;++trial){
        int n=static_cast<int>(random()%29),root=n==0?0:static_cast<int>(random()%n);
        cp::Graph<> g(n);
        for(int v=1;v<n;++v)if(random()%5!=0)g.add_edge(v,static_cast<int>(random()%v),
            trial%3==0?static_cast<ll>(random()%15)-5:static_cast<ll>(random()%11));
        Naive naive(g,root);cp::Lca<> lca(g,root);cp::EulerTour<> euler(g,root);cp::HeavyLightDecomposition<> hld(g,root);
        const auto maximum=cp::Rerooting<>(g).build(root);
        const auto sum=cp::Rerooting<SumDistances>(g).build(root);
        ll diameter=0;
        for(int u=0;u<n;++u){
            int size=0;ll sum_distance=0,max_distance=0;int component_size=0;
            for(int v=0;v<n;++v){
                size+=naive.ancestor(u,v);
                assert(euler.is_ancestor(u,v)==naive.ancestor(u,v));
                int w=naive.lca(u,v);assert(lca.lca(u,v)==w&&hld.lca(u,v)==w);
                const auto path=naive.path(u,v);
                assert(expand(hld,hld.path_segments(u,v))==path);
                std::vector<int> edge_vertices;
                for(int i=1;i<static_cast<int>(path.size());++i)
                    edge_vertices.push_back(naive.depth[path[i-1]]>naive.depth[path[i]]?path[i-1]:path[i]);
                assert(expand(hld,hld.path_segments(u,v,true))==edge_vertices);
                if(w==-1){assert(!lca.distance(u,v));assert(lca.kth_on_path(u,v,0)==-1);}
                else{
                    ll distance=path_cost(g,path);assert(lca.distance(u,v)==distance);
                    sum_distance+=distance;max_distance=std::max(max_distance,distance);++component_size;diameter=std::max(diameter,distance);
                    for(int k=0;k<static_cast<int>(path.size());++k)assert(lca.kth_on_path(u,v,k)==path[k]);
                    assert(lca.kth_on_path(u,v,static_cast<int>(path.size()))==-1);
                }
            }
            assert(sum[u].sum==sum_distance&&sum[u].count==component_size&&maximum[u]==max_distance);
            auto [el,er]=euler.subtree_range(u);auto [hl,hr]=hld.subtree_range(u);
            assert(er-el==size&&hr-hl==size&&euler.subtree_size[u]==size);
            for(int p=el;p<er;++p)assert(naive.ancestor(u,euler.vertex_at[p]));
            for(int p=hl;p<hr;++p)assert(naive.ancestor(u,hld.vertex_at[p]));
            auto [bl,br]=hld.subtree_range(u,true);assert(br-bl==size-1&&bl==hl+1);
            int ancestor=u;
            for(int k=0;k<=naive.depth[u];++k){assert(lca.kth_ancestor(u,k)==ancestor);ancestor=naive.parent[ancestor];}
            assert(lca.kth_ancestor(u,-1)==-1&&lca.kth_ancestor(u,1000000000000LL)==-1);
        }
        int components=0;for(int v=0;v<n;++v)components+=naive.parent[v]==-1;
        bool nonnegative=true;for(const auto& edge:g.edges())if(edge.cost<0)nonnegative=false;
        if(components<=1&&nonnegative){
            auto result=cp::tree_diameter(g);assert(result.length==diameter);
            if(n==0)assert(result.from==-1&&result.vertices.empty());
            else {
                assert(result.vertices==naive.path(result.from,result.to));assert(path_cost(g,result.vertices)==diameter);
                assert(result.edge_ids.size()+1==result.vertices.size());
                for(int i=0;i<static_cast<int>(result.edge_ids.size());++i){
                    const auto& edge=g.edges()[result.edge_ids[i]];
                    assert((edge.from==result.vertices[i]&&edge.to==result.vertices[i+1])||
                           (edge.to==result.vertices[i]&&edge.from==result.vertices[i+1]));
                }
            }
        }else{
            bool rejected=false;try{(void)cp::tree_diameter(g);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
        }
        if(n==0)continue;
        std::vector<ll> values(n),flattened(n);
        for(int v=0;v<n;++v){values[v]=static_cast<ll>(random()%21)-10;flattened[hld.pos[v]]=values[v];}
        LazySegTree<RangeAddSum> sums(flattened);LazySegTree<RangeAddMinIndex> minima(flattened);
        for(int step=0;step<150;++step){
            int u=static_cast<int>(random()%n),v=static_cast<int>(random()%n);
            if(naive.component[u]!=naive.component[v])continue;
            auto path=naive.path(u,v);auto segments=hld.path_segments(u,v);
            if(random()%2){
                ll delta=static_cast<ll>(random()%13)-6;
                for(auto segment:segments){sums.range_apply(segment.l,segment.r,delta);minima.range_apply(segment.l,segment.r,delta);}
                for(int x:path)values[x]+=delta;
            }else{
                ll total=0,expected_sum=0,minimum=std::numeric_limits<ll>::max();int best=-1;
                for(int x:path){
                    expected_sum+=values[x];
                    if(values[x]<minimum||(values[x]==minimum&&(best==-1||hld.pos[x]<best))){minimum=values[x];best=hld.pos[x];}
                }
                ll found_min=std::numeric_limits<ll>::max();int found_index=-1;
                for(auto segment:segments){
                    total+=sums.prod(segment.l,segment.r);auto result=minima.prod(segment.l,segment.r);
                    if(result.value<found_min||(result.value==found_min&&(found_index==-1||result.index<found_index))){found_min=result.value;found_index=result.index;}
                }
                assert(total==expected_sum&&found_min==minimum&&found_index==best);
                assert(std::find(path.begin(),path.end(),hld.vertex_at[found_index])!=path.end());
            }
            ++integration_operations;
        }
    }
    cp::Graph<> trace_graph(7);trace_graph.add_edge(3,1);trace_graph.add_edge(1,0);trace_graph.add_edge(3,5);trace_graph.add_edge(3,2);trace_graph.add_edge(5,4);
    auto trace=cp::Rerooting<Trace>(trace_graph).build(5);
    std::function<std::string(int,int)> solve=[&](int v,int parent){
        std::string acc;
        for(const auto& e:trace_graph[v])if(e.to!=parent)acc=Trace::merge(acc,Trace::lift(solve(e.to,v),cp::Edge<ll>{e.to,v,e.cost,e.id}));
        return Trace::add_vertex(acc,v);
    };
    for(int v=0;v<trace_graph.size();++v)assert(trace[v]==solve(v,-1));
    for(int kind=0;kind<3;++kind){
        cp::Graph<> invalid(3,kind==2);
        invalid.add_edge(0,1);if(kind==0)invalid.add_edge(0,1);else if(kind==1)invalid.add_edge(1,1);
        bool rejected=false;try{cp::HeavyLightDecomposition<> h(invalid);(void)h;}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
    }
    cp::Graph<> negative(2);negative.add_edge(0,1,-1);bool rejected=false;
    try{(void)cp::tree_diameter(negative);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
    const int n=100000;cp::Graph<> line(n);std::vector<std::vector<int>> adjacency(n);
    Rerooting legacy(n);
    for(int v=1;v<n;++v){line.add_edge(v-1,v);adjacency[v-1].push_back(v);adjacency[v].push_back(v-1);legacy.add_edge(v-1,v);}
    cp::Lca<> large(line);assert(large.lca(70000,99999)==70000&&large.distance(0,n-1)==n-1);
    cp::HeavyLightDecomposition<> h(line);assert(h.path_segments(n-1,0).size()==1);
    assert(cp::EulerTour<>(line).subtree_size[0]==n&&cp::tree_diameter(line).length==n-1);
    auto answers=legacy.build();assert(answers.front()==n-1&&answers[n/2]==n/2);
    std::vector<bool> visited(n,false);assert(tr(0,adjacency,visited)==n);
    std::fill(visited.begin(),visited.end(),false);visited[3]=true;assert(tr(0,adjacency,visited)==3);
    std::cout<<"PASS tree: 180 forests, all-pairs paths/LCA/subtrees/rerooting, "<<integration_operations<<" HLD/lazy operations, 100000-vertex chain\n";
}
