// https://github.com/kazu107/kyopro_library/blob/main/tests/graph_test.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 最短路・連結性・閉路・SCC・Lowlink・最小全域森・2-SATを独立した正解と照合する。
使い方:
  python tests/run_tests.py // このテストを含む全テストを実行。--debugで境界チェックを有効化。
前提: このファイル単体にはヘッダを含めない。依存コードと標準ヘッダはテストランナーが補い、assertを有効にする。
計算量: ケース数と全列挙の規模に依存。固定seedの比較テストと境界条件を実行。
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/bfs.hpp, graph/dfs.hpp, graph/zero_one_bfs.hpp, graph/dijkstra.hpp, graph/bellman_ford.hpp, graph/floyd_warshall.hpp, graph/connected_components.hpp, graph/bipartite.hpp, graph/topological_sort.hpp, graph/cycle_detection.hpp, graph/scc.hpp, graph/lowlink.hpp, graph/kruskal.hpp, graph/TwoSat.hpp

// 全探索・独立した辺数制限DP・頂点/辺の削除との比較。seed固定。

#ifdef NDEBUG
#error "Assertions must be enabled."
#endif
using ll = long long;
constexpr ll REF_INF = 1000000000000LL;

std::vector<std::vector<bool>> reachability(const cp::Graph<>& g) {
    const int n = g.size();
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n, false));
    for (int s = 0; s < n; ++s) {
        std::vector<int> stack{s}; reach[s][s] = true;
        while (!stack.empty()) {
            const int v = stack.back(); stack.pop_back();
            for (const auto& e : g[v]) if (!reach[s][e.to]) { reach[s][e.to] = true; stack.push_back(e.to); }
        }
    }
    return reach;
}
int component_count(const cp::Graph<>& g, int removed_vertex = -1, int removed_edge = -1, int mask = -1) {
    std::vector<bool> visited(g.size(), false);
    int count = 0;
    for (int start = 0; start < g.size(); ++start) if (start != removed_vertex && !visited[start]) {
        ++count; std::vector<int> stack{start}; visited[start] = true;
        while (!stack.empty()) {
            const int v = stack.back(); stack.pop_back();
            for (const auto& e : g[v]) {
                if (e.to == removed_vertex || e.id == removed_edge || (mask != -1 && !(mask >> e.id & 1))) continue;
                if (!visited[e.to]) { visited[e.to] = true; stack.push_back(e.to); }
            }
        }
    }
    return count;
}
void verify_path(const cp::Graph<>& g, int s, int t, const std::vector<int>& path,
                 const std::vector<int>& edges, ll expected, bool unit = false) {
    assert(!path.empty() && path.front() == s && path.back() == t);
    assert(path.size() == edges.size() + 1);
    std::set<int> unique_vertices(path.begin(), path.end());
    assert(unique_vertices.size() == path.size());
    ll cost = 0;
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        const auto& e = g.edges().at(edges[i]);
        const bool forward = e.from == path[i] && e.to == path[i + 1];
        const bool backward = !g.is_directed() && e.to == path[i] && e.from == path[i + 1];
        assert(forward || backward);
        cost += unit ? 1 : e.cost;
    }
    assert(cost == expected);
}
void check_shortest(const cp::Graph<>& g) {
    const int n = g.size();
    const auto reach = reachability(g);
    const auto all = cp::floyd_warshall(g);
    for (int s = 0; s < n; ++s) {
        // 同じ配列内では緩和せず、使える辺数を1本ずつ増やす独立したDP。
        std::vector<ll> best(n, REF_INF); best[s] = 0;
        std::vector<bool> negative(n, false);
        for (int step = 0; step < n; ++step) {
            auto next = best;
            for (int v = 0; v < n; ++v) if (best[v] != REF_INF)
                for (const auto& e : g[v]) next[e.to] = std::min(next[e.to], best[v] + e.cost);
            if (step == n - 1) for (int v = 0; v < n; ++v) if (next[v] < best[v])
                for (int t = 0; t < n; ++t) if (reach[v][t]) negative[t] = true;
            best = std::move(next);
        }
        const auto single = cp::bellman_ford(g, s);
        for (int t = 0; t < n; ++t) {
            assert(single.negative[t] == negative[t] && all.negative[s][t] == negative[t]);
            assert(single.reachable(t) == reach[s][t]);
            if (negative[t]) {
                assert(single.dist[t] == -cp::distance_inf<ll>() && all.dist[s][t] == -cp::distance_inf<ll>());
                assert(single.path_to(t).empty() && all.path(s, t).empty());
            } else if (!reach[s][t]) {
                assert(single.dist[t] == cp::distance_inf<ll>() && all.dist[s][t] == cp::distance_inf<ll>());
                assert(single.path_to(t).empty() && all.edge_path(s, t).empty());
            } else {
                assert(single.dist[t] == best[t] && all.dist[s][t] == best[t]);
                verify_path(g, s, t, single.path_to(t), single.edge_path_to(t), best[t]);
                verify_path(g, s, t, all.path(s, t), all.edge_path(s, t), best[t]);
            }
        }
        const auto unit = cp::bfs(g, s);
        std::vector<ll> hops(n, REF_INF); hops[s] = 0;
        for (int k = 0; k < n; ++k) {
            auto next = hops;
            for (int v = 0; v < n; ++v) if (hops[v] != REF_INF)
                for (const auto& e : g[v]) next[e.to] = std::min(next[e.to], hops[v] + 1);
            hops = std::move(next);
        }
        for (int t = 0; t < n; ++t) if (reach[s][t]) {
            assert(unit.dist[t] == hops[t]);
            verify_path(g, s, t, unit.path_to(t), unit.edge_path_to(t), hops[t], true);
        } else assert(!unit.reachable(t));
    }
}
void check_structure(const cp::Graph<>& g) {
    const int n = g.size();
    const auto reach = reachability(g);
    const auto dfs = cp::dfs_forest(g);
    assert(static_cast<int>(dfs.order.size()) == n && static_cast<int>(dfs.postorder.size()) == n);
    std::set<int> times;
    for (int v = 0; v < n; ++v) {
        assert(dfs.tin[v] < dfs.tout[v]); times.insert(dfs.tin[v]); times.insert(dfs.tout[v]);
        if (dfs.parent[v] != -1) {
            assert(dfs.depth[v] == dfs.depth[dfs.parent[v]] + 1);
            assert(dfs.tin[dfs.parent[v]] < dfs.tin[v] && dfs.tout[v] < dfs.tout[dfs.parent[v]]);
        }
    }
    assert(static_cast<int>(times.size()) == 2 * n);
    const auto cycle = cp::find_cycle(g);
    bool has_cycle = false;
    if (g.is_directed()) {
        for (const auto& e : g.edges()) if (reach[e.to][e.from]) has_cycle = true;
        const auto topo = cp::topological_sort(g);
        assert(topo.is_dag == !has_cycle);
        if (topo.is_dag) {
            std::vector<int> position(n);
            for (int i = 0; i < n; ++i) position[topo.order[i]] = i;
            for (const auto& e : g.edges()) assert(position[e.from] < position[e.to]);
        }
        const auto scc = cp::strongly_connected_components(g);
        int members = 0;
        for (int id = 0; id < static_cast<int>(scc.groups.size()); ++id)
            for (int v : scc.groups[id]) { assert(scc.component[v] == id); ++members; }
        assert(members == n);
        for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v)
            assert((scc.component[u] == scc.component[v]) == (reach[u][v] && reach[v][u]));
        for (const auto& e : g.edges()) if (scc.component[e.from] != scc.component[e.to])
            assert(scc.component[e.from] < scc.component[e.to]);
    } else {
        const int components = component_count(g);
        has_cycle = g.edge_count() > n - components;
        const auto cc = cp::connected_components(g);
        assert(static_cast<int>(cc.groups.size()) == components);
        for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v)
            assert((cc.component[u] == cc.component[v]) == reach[u][v]);
        std::vector<int> bridges, cut;
        for (int id = 0; id < g.edge_count(); ++id) if (component_count(g, -1, id) > components) bridges.push_back(id);
        for (int v = 0; v < n; ++v) if (component_count(g, v) > components) cut.push_back(v);
        const auto lowlink = cp::lowlink(g);
        assert(lowlink.bridges == bridges && lowlink.articulation_points == cut);
        bool possible = false;
        for (int mask = 0; mask < (1 << n); ++mask) {
            bool valid = true;
            for (const auto& e : g.edges()) if (((mask >> e.from) & 1) == ((mask >> e.to) & 1)) valid = false;
            if (valid) { possible = true; break; }
        }
        const auto bipartite = cp::bipartite_check(g);
        assert(bipartite.is_bipartite == possible);
        if (possible) for (const auto& e : g.edges()) assert(bipartite.color[e.from] != bipartite.color[e.to]);

        ll minimum = REF_INF;
        for (int mask = 0; mask < (1 << g.edge_count()); ++mask) {
            if (__builtin_popcount(static_cast<unsigned>(mask)) != n - components) continue;
            if (component_count(g, -1, -1, mask) != components) continue;
            ll cost = 0;
            for (const auto& e : g.edges()) if (mask >> e.id & 1) cost += e.cost;
            minimum = std::min(minimum, cost);
        }
        const auto mst = cp::kruskal(g);
        assert(mst.cost == minimum && mst.components == components);
        assert(mst.connected == (n == 0 || components == 1));
        int chosen = 0; ll cost = 0;
        for (int id : mst.edge_ids) { assert(!(chosen >> id & 1)); chosen |= 1 << id; cost += g.edges()[id].cost; }
        assert(cost == minimum && component_count(g, -1, -1, chosen) == components);
    }
    assert(cycle.vertices.empty() == !has_cycle);
    if (has_cycle) {
        assert(cycle.vertices.front() == cycle.vertices.back());
        assert(cycle.vertices.size() == cycle.edge_ids.size() + 1);
        std::set<int> ids;
        for (int i = 0; i < static_cast<int>(cycle.edge_ids.size()); ++i) {
            const auto& e = g.edges()[cycle.edge_ids[i]]; ids.insert(e.id);
            assert((e.from == cycle.vertices[i] && e.to == cycle.vertices[i + 1]) ||
                   (!g.is_directed() && e.to == cycle.vertices[i] && e.from == cycle.vertices[i + 1]));
        }
        assert(ids.size() == cycle.edge_ids.size());
    }
}
void two_sat_tests(std::mt19937& random) {
    for (int trial = 0; trial < 1200; ++trial) {
        const int n = static_cast<int>(random() % 7);
        cp::TwoSat sat(n);
        std::vector<std::tuple<int, bool, int, bool>> clauses;
        const int m = n == 0 ? 0 : static_cast<int>(random() % 20);
        for (int i = 0; i < m; ++i) {
            const int u = static_cast<int>(random() % n), v = static_cast<int>(random() % n);
            const bool f = random() % 2, g = random() % 2;
            clauses.push_back({u, f, v, g}); sat.add_clause(u, f, v, g);
        }
        bool possible = false;
        for (int mask = 0; mask < (1 << n); ++mask) {
            bool valid = true;
            for (auto [u, f, v, g] : clauses) if (((mask >> u & 1) != f) && ((mask >> v & 1) != g)) valid = false;
            possible |= valid;
        }
        assert(sat.satisfiable() == possible);
        if (possible) for (auto [u, f, v, g] : clauses) assert(sat.answer()[u] == f || sat.answer()[v] == g);
    }
    cp::TwoSat sat(2); sat.force(0, true); sat.add_implication(0, true, 1, false);
    assert(sat.satisfiable() && sat.answer()[0] && !sat.answer()[1]);
    sat.force(1, true); assert(!sat.satisfiable());
}
int main() {
    std::mt19937 random(20261024);
    for (int trial = 0; trial < 650; ++trial) {
        const int n = static_cast<int>(random() % 8);
        cp::Graph<> g(n, trial % 2);
        const int m = n == 0 ? 0 : static_cast<int>(random() % 11);
        for (int i = 0; i < m; ++i) g.add_edge(static_cast<int>(random() % n), static_cast<int>(random() % n), static_cast<ll>(random() % 13) - 4);
        check_shortest(g); check_structure(g);
    }
    for (int trial = 0; trial < 300; ++trial) {
        const int n = 1 + static_cast<int>(random() % 9);
        cp::Graph<> g(n, trial % 2);
        for (int i = 0, m = static_cast<int>(random() % 30); i < m; ++i)
            g.add_edge(static_cast<int>(random() % n), static_cast<int>(random() % n), random() % 2);
        const auto all = cp::floyd_warshall(g);
        for (int s = 0; s < n; ++s) {
            const auto dijkstra = cp::dijkstra(g, s), zero_one = cp::zero_one_bfs(g, s);
            assert(dijkstra.dist == all.dist[s] && zero_one.dist == all.dist[s]);
            for (int t = 0; t < n; ++t) if (dijkstra.finite(t)) {
                verify_path(g, s, t, dijkstra.path_to(t), dijkstra.edge_path_to(t), dijkstra.dist[t]);
                verify_path(g, s, t, zero_one.path_to(t), zero_one.edge_path_to(t), zero_one.dist[t]);
            }
        }
        const auto multi = cp::bfs(g, std::vector<int>{0, n - 1, 0});
        const auto a = cp::bfs(g, 0), b = cp::bfs(g, n - 1);
        for (int v = 0; v < n; ++v) assert(multi.dist[v] == std::min(a.dist[v], b.dist[v]));
    }
    two_sat_tests(random);
    cp::Graph<int> ints(2, true); ints.add_edge(0, 1, 7);
    assert(cp::dijkstra(ints, 0).dist[1] == 7 && cp::bellman_ford(ints, 0).dist[1] == 7);
    assert(cp::floyd_warshall(ints).dist[0][1] == 7);
    bool rejected = false; ints.add_edge(1, 1, -1);
    try { (void)cp::dijkstra(ints, 0); } catch (const std::invalid_argument&) { rejected = true; } assert(rejected);
    rejected = false;
    try { (void)cp::zero_one_bfs(ints, 0); } catch (const std::invalid_argument&) { rejected = true; } assert(rejected);
    cp::Graph<> huge(4, true); huge.add_edge(0, 1, -1000000000000000000LL); huge.add_edge(1, 1, -1000000000000000000LL); huge.add_edge(1, 2, 1);
    auto neg = cp::bellman_ford(huge, 0); assert(!neg.negative[0] && neg.negative[1] && neg.negative[2] && !neg.reachable(3));
    const int deep_n = 100000;
    cp::Graph<> line(deep_n), dag(deep_n, true);
    for (int v = 1; v < deep_n; ++v) { line.add_edge(v - 1, v); dag.add_edge(v - 1, v); }
    assert(cp::dfs_forest(line).depth.back() == deep_n - 1);
    assert(static_cast<int>(cp::lowlink(line).bridges.size()) == deep_n - 1);
    assert(cp::find_cycle(line).vertices.empty());
    assert(static_cast<int>(cp::strongly_connected_components(dag).groups.size()) == deep_n);
    assert(cp::topological_sort(dag).is_dag);
    std::cout << "PASS graph: 950 random graphs, 1200 brute-force 2-SAT cases, paths/cuts/forests, 100000-vertex chains\n";
}
