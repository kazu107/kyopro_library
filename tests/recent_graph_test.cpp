// https://github.com/kazu107/kyopro_library/blob/main/tests/recent_graph_test.cpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 辞書順BFS・車輪距離・最小頂点優先探索・木上Mo・パス頻度を独立した参照解と比較する。
使い方: python tests/run_tests.py。正常終了時に比較したケース数を表示し、失敗時はassertで停止する。
前提: 0始まり。依存と標準ヘッダはランナーが補う。assertを有効にし、GNU C++17以降で実行する。
  パスの両端を含み、頻度の範囲も閉区間。固定seedの乱数・全ラベル木・公式サンプル・長い鎖を使う。
計算量: 小規模比較は全頂点対とN<=6のPrufer列全列挙、長い鎖はO(N log N+N sqrt(Q))程度。
*/
// 依存: graph/lexicographic_bfs.hpp, graph/WheelGraphDistance.hpp, graph/min_first_search.hpp, tree/MoOnTree.hpp, tree/TreePathFrequencyQueries.hpp

#ifdef NDEBUG
#error "Assertions must be enabled."
#endif

namespace recent_graph_test {
std::mt19937 random_engine(20261029);
long long shortlex_sources = 0, wheel_pairs = 0, search_sources = 0;
long long prufer_roots = 0, permutation_counts = 0, mo_queries = 0, frequency_queries = 0;
int official_samples = 0;
int random_int(int upper) { return static_cast<int>(random_engine() % upper); }

template <class Exception, class Function>
void expect_throw(Function function) {
    bool caught = false;
    try { function(); } catch (const Exception&) { caught = true; }
    assert(caught);
}

using Word = std::vector<long long>;
using WordKey = std::pair<int, Word>;
std::vector<std::optional<Word>> reference_words(const cp::Graph<long long>& graph, int source) {
    using State = std::tuple<int, Word, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    std::vector<std::optional<Word>> answer(graph.size());
    queue.emplace(0, Word{}, source);
    while (!queue.empty()) {
        auto [length, word, vertex] = queue.top(); queue.pop();
        assert(length == static_cast<int>(word.size()));
        if (answer[vertex]) continue;
        answer[vertex] = word;
        for (const auto& edge : graph[vertex]) if (!answer[edge.to]) {
            auto next = word; next.push_back(edge.cost);
            queue.emplace(length + 1, std::move(next), edge.to);
        }
    }
    return answer;
}

void check_shortlex(const cp::Graph<long long>& graph, int source, bool digits) {
    ++shortlex_sources;
    const auto expected = reference_words(graph, source);
    const auto actual = cp::lexicographic_bfs(graph, source);
    std::vector<bool> discovered(graph.size(), false);
    std::vector<int> position(graph.size(), -1);
    for (int i = 0; i < static_cast<int>(actual.discovery_order.size()); ++i) {
        const int vertex = actual.discovery_order[i];
        assert(!discovered[vertex]); discovered[vertex] = true; position[vertex] = i;
    }
    for (int vertex = 0; vertex < graph.size(); ++vertex) {
        assert(actual.reachable(vertex) == expected[vertex].has_value());
        assert(discovered[vertex] == expected[vertex].has_value());
        if (!expected[vertex]) {
            assert(actual.distance[vertex] == -1 && actual.word_rank[vertex] == -1);
            assert(actual.parent[vertex] == -1 && actual.parent_edge[vertex] == -1);
            assert(actual.path_to(vertex).empty() && actual.edge_path_to(vertex).empty());
            assert(actual.labels_to(vertex).empty());
            continue;
        }
        assert(actual.labels_to(vertex) == *expected[vertex]);
        assert(actual.distance[vertex] == static_cast<int>(expected[vertex]->size()));
        const auto path = actual.path_to(vertex), edges = actual.edge_path_to(vertex);
        assert(path.front() == source && path.back() == vertex && path.size() == edges.size() + 1);
        for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
            const auto& edge = graph.edges().at(edges[i]);
            assert((edge.from == path[i] && edge.to == path[i + 1])
                   || (!graph.is_directed() && edge.to == path[i] && edge.from == path[i + 1]));
            assert(edge.cost == expected[vertex]->at(i));
            assert(position[path[i]] < position[path[i + 1]]);
        }
        for (int other = 0; other < graph.size(); ++other) if (expected[other]) {
            const WordKey left{static_cast<int>(expected[vertex]->size()), *expected[vertex]};
            const WordKey right{static_cast<int>(expected[other]->size()), *expected[other]};
            assert((actual.word_rank[vertex] < actual.word_rank[other]) == (left < right));
            assert((actual.word_rank[vertex] == actual.word_rank[other]) == (left == right));
        }
    }
    if (digits) for (long long mod : {1LL, 8LL, 998244353LL, std::numeric_limits<long long>::max()}) {
        const auto values = cp::lexicographic_path_values(actual, 10, mod);
        for (int vertex = 0; vertex < graph.size(); ++vertex) {
            long long expected_value = -1;
            if (expected[vertex]) {
                expected_value = 0;
                for (long long label : *expected[vertex]) expected_value = static_cast<long long>(
                    (static_cast<__int128>(expected_value) * 10 + label) % mod);
            }
            assert(values[vertex] == expected_value);
        }
    }
}

void test_shortlex() {
    // 同じprefix {1}の頂点をまとめず、頂点ごとに次を確定すると{1,9}を選んでしまう。
    cp::Graph<long long> equal_prefix(6, true);
    equal_prefix.add_edge(0, 1, 1); equal_prefix.add_edge(0, 2, 1);
    equal_prefix.add_edge(1, 3, 9); equal_prefix.add_edge(2, 3, 2);
    equal_prefix.add_edge(0, 4, 8); equal_prefix.add_edge(3, 4, -100);
    check_shortlex(equal_prefix, 0, false);
    const auto same = cp::lexicographic_bfs(equal_prefix, 0);
    assert((same.labels_to(3) == Word{1, 2}) && (same.labels_to(4) == Word{8}));
    for (int iteration = 0; iteration < 400; ++iteration) {
        const int n = 1 + random_int(9);
        const bool digits = iteration % 2 == 0;
        cp::Graph<long long> graph(n, iteration % 3 == 0);
        for (int edge = 0, count = random_int(3 * n + 1); edge < count; ++edge)
            graph.add_edge(random_int(n), random_int(n), digits ? random_int(10) : random_int(11) - 5);
        for (int source = 0; source < n; ++source) check_shortlex(graph, source, digits);
    }
    cp::Graph<long long> first(4, true);
    first.add_edge(0, 1, 1); first.add_edge(0, 2, 1);
    first.add_edge(1, 3, 9); first.add_edge(2, 3, 2);
    assert((cp::lexicographic_path_values(cp::lexicographic_bfs(first, 0)) == std::vector<long long>{0, 1, 1, 12}));
    cp::Graph<long long> second(3, true);
    second.add_edge(0, 1, 9); second.add_edge(0, 1, 2); second.add_edge(1, 1, 1);
    second.add_edge(0, 2, 9); second.add_edge(1, 2, 1);
    assert((cp::lexicographic_path_values(cp::lexicographic_bfs(second, 0)) == std::vector<long long>{0, 2, 9}));
    const auto valid = cp::lexicographic_bfs(first, 0);
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(valid, 1, 7); });
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(valid, 10, 0); });
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(valid, 10, -3); });
    cp::Graph<long long> invalid(2, true); invalid.add_edge(0, 1, -1);
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(cp::lexicographic_bfs(invalid, 0)); });
    cp::Graph<unsigned long long> large(2, true); large.add_edge(0, 1, std::numeric_limits<unsigned long long>::max());
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(cp::lexicographic_bfs(large, 0)); });
    cp::Graph<long long> base(2, true); base.add_edge(0, 1, 10);
    expect_throw<std::invalid_argument>([&] { cp::lexicographic_path_values(cp::lexicographic_bfs(base, 0)); });
}

void check_wheel(const std::vector<long long>& ring, const std::vector<long long>& spokes) {
    const int n = static_cast<int>(ring.size());
    cp::WheelGraphDistance<long long> wheel(ring, spokes);
    const long long infinity = cp::distance_inf<long long>();
    std::vector<std::vector<long long>> distance(n + 1, std::vector<long long>(n + 1, infinity));
    for (int i = 0; i <= n; ++i) distance[i][i] = 0;
    auto add = [&](int u, int v, long long cost) { distance[u][v] = distance[v][u] = std::min(distance[u][v], cost); };
    for (int i = 0; i < n; ++i) { add(i, (i + 1) % n, ring[i]); add(i, n, spokes[i]); }
    for (int k = 0; k <= n; ++k) for (int i = 0; i <= n; ++i) for (int j = 0; j <= n; ++j)
        distance[i][j] = std::min(distance[i][j], static_cast<long long>(std::min<__int128>(infinity,
            static_cast<__int128>(distance[i][k]) + distance[k][j])));
    assert(wheel.ring_size() == n && wheel.hub() == n);
    assert(wheel.circumference() == std::accumulate(ring.begin(), ring.end(), 0LL));
    for (int i = 0; i <= n; ++i) {
        assert(wheel.distance_to_hub(i) == distance[i][n]);
        for (int j = 0; j <= n; ++j) { ++wheel_pairs; assert(wheel.distance(i, j) == distance[i][j]); }
    }
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) {
        long long forward = 0;
        for (int v = i; v != j; v = (v + 1) % n) forward += ring[v];
        assert(wheel.ring_distance(i, j) == std::min(forward, wheel.circumference() - forward));
    }
}

void test_wheel() {
    check_wheel({0}, {0}); check_wheel({7}, {9}); check_wheel({8, 3}, {11, 2});
    check_wheel({0, 0, 0}, {0, 0, 0}); check_wheel({1, 1, 100}, {100, 1, 100});
    assert(cp::WheelGraphDistance<long long>({1, 1, 100}, {100, 1, 100}).distance_to_hub(0) == 2);
    for (int iteration = 0; iteration < 500; ++iteration) {
        const int n = 1 + random_int(11);
        std::vector<long long> ring(n), spokes(n);
        for (int i = 0; i < n; ++i) { ring[i] = random_int(31); spokes[i] = random_int(51); }
        check_wheel(ring, spokes);
    }
    const cp::WheelGraphDistance<long long> sample({1, 3, 4, 2, 5}, {5, 2, 4, 3, 7});
    assert(sample.distance(1, 4) == 6 && sample.distance(4, 5) == 5 && sample.distance(0, 3) == 6);
    const long long inf = cp::distance_inf<long long>();
    check_wheel({inf - 1}, {inf - 1}); check_wheel({inf - 2, 1}, {inf - 1, inf - 1});
    expect_throw<std::invalid_argument>([] { cp::WheelGraphDistance<long long>({}, {}); });
    expect_throw<std::invalid_argument>([] { cp::WheelGraphDistance<long long>({1}, {1, 2}); });
    expect_throw<std::invalid_argument>([] { cp::WheelGraphDistance<long long>({-1}, {0}); });
    expect_throw<std::invalid_argument>([] { cp::WheelGraphDistance<long long>({0}, {-1}); });
    expect_throw<std::overflow_error>([&] { cp::WheelGraphDistance<long long>({inf}, {0}); });
    expect_throw<std::overflow_error>([&] { cp::WheelGraphDistance<long long>({0}, {inf}); });
    expect_throw<std::overflow_error>([&] { cp::WheelGraphDistance<long long>({inf - 1, 1}, {0, 0}); });
    expect_throw<std::overflow_error>([] { cp::WheelGraphDistance<long long>({std::numeric_limits<long long>::max()}, {0}); });
}

cp::MinFirstSearchResult reference_search(const cp::Graph<long long>& graph, int source, const std::vector<int>& priorities) {
    cp::MinFirstSearchResult result{{}, std::vector<int>(graph.size(), -1),
        std::vector<int>(graph.size(), -1), std::vector<int>(graph.size(), -1)};
    std::vector<bool> discovered(graph.size(), false); discovered[source] = true;
    std::set<std::pair<int, int>> frontier{{priorities[source], source}};
    while (!frontier.empty()) {
        const int vertex = frontier.begin()->second; frontier.erase(frontier.begin());
        result.position[vertex] = static_cast<int>(result.order.size()); result.order.push_back(vertex);
        for (const auto& edge : graph[vertex]) if (!discovered[edge.to]) {
            discovered[edge.to] = true; result.parent[edge.to] = vertex; result.parent_edge[edge.to] = edge.id;
            frontier.insert({priorities[edge.to], edge.to});
        }
    }
    return result;
}

void test_search() {
    for (int iteration = 0; iteration < 300; ++iteration) {
        const int n = 1 + random_int(12);
        cp::Graph<long long> graph(n, iteration % 2);
        for (int i = 0, m = random_int(n * 3 + 1); i < m; ++i) graph.add_edge(random_int(n), random_int(n), random_int(31) - 15);
        std::vector<int> priorities(n), standard(n);
        for (int& priority : priorities) priority = random_int(5) - 2;
        std::iota(standard.begin(), standard.end(), 0);
        for (int source = 0; source < n; ++source) {
            ++search_sources;
            const auto expected = reference_search(graph, source, priorities), actual = cp::min_first_search(graph, source, priorities);
            assert(actual.order == expected.order && actual.position == expected.position);
            assert(actual.parent == expected.parent && actual.parent_edge == expected.parent_edge);
            assert(cp::min_first_search(graph, source).order == reference_search(graph, source, standard).order);
        }
    }
    cp::Graph<long long> singleton(1);
    expect_throw<std::invalid_argument>([&] { cp::min_first_search(singleton, 0, std::vector<int>{}); });
}

cp::Graph<long long> prufer_tree(int n, int code) {
    cp::Graph<long long> graph(n, false);
    std::vector<int> sequence(std::max(0, n - 2)), degree(n, 1);
    for (int& vertex : sequence) { vertex = code % n; code /= n; ++degree[vertex]; }
    for (int vertex : sequence) {
        int leaf = 0; while (degree[leaf] != 1) ++leaf;
        graph.add_edge(leaf, vertex); --degree[leaf]; --degree[vertex];
    }
    if (n > 1) {
        std::vector<int> remaining;
        for (int vertex = 0; vertex < n; ++vertex) if (degree[vertex] == 1) remaining.push_back(vertex);
        graph.add_edge(remaining[0], remaining[1]);
    }
    return graph;
}

void test_tree_count() {
    assert(cp::count_min_first_trees({}) == 1 && cp::count_min_first_trees({}, 1) == 0);
    for (int n = 1; n <= 6; ++n) {
        int tree_count = 1;
        for (int i = 0; i < n - 2; ++i) tree_count *= n;
        std::vector<int> priority(n); std::iota(priority.begin(), priority.end(), 0);
        std::map<std::vector<int>, long long> distribution;
        for (int code = 0; code < tree_count; ++code) {
            const auto graph = prufer_tree(n, code);
            for (int root = 0; root < n; ++root) {
                ++prufer_roots;
                ++distribution[reference_search(graph, root, priority).order];
            }
        }
        auto permutation = priority;
        do {
            ++permutation_counts;
            const long long expected = distribution[permutation];
            for (long long mod : {1LL, 6LL, 97LL, 998244353LL, std::numeric_limits<long long>::max()})
                assert(cp::count_min_first_trees(permutation, mod) == expected % mod);
        } while (std::next_permutation(permutation.begin(), permutation.end()));
    }
    assert(cp::count_min_first_trees({0, 2, 3, 1, 4}) == 8);
    std::vector<int> ascending(20); std::iota(ascending.begin(), ascending.end(), 0);
    assert(cp::count_min_first_trees(ascending) == 868586527);
    std::vector<int> descending(200000); std::iota(descending.rbegin(), descending.rend(), 0);
    assert(cp::count_min_first_trees(descending) == 1);
    for (const auto& invalid : std::vector<std::vector<int>>{{0, 0}, {-1}, {1}, {0, 2}})
        expect_throw<std::invalid_argument>([&] { cp::count_min_first_trees(invalid); });
    expect_throw<std::invalid_argument>([] { cp::count_min_first_trees({}, 0); });
    expect_throw<std::invalid_argument>([] { cp::count_min_first_trees({}, -7); });
}

std::vector<int> reference_path(const cp::Graph<long long>& graph, int source, int target) {
    std::vector<int> parent(graph.size(), -1), queue{source}; parent[source] = source;
    for (std::size_t i = 0; i < queue.size(); ++i) for (const auto& edge : graph[queue[i]]) if (parent[edge.to] == -1) {
        parent[edge.to] = queue[i]; queue.push_back(edge.to);
    }
    if (parent[target] == -1) return {};
    std::vector<int> path;
    for (int vertex = target; vertex != source; vertex = parent[vertex]) path.push_back(vertex);
    path.push_back(source); std::sort(path.begin(), path.end()); return path;
}

void check_mo(const cp::Graph<long long>& graph, const std::vector<long long>& labels, int root) {
    cp::MoOnTree mo(graph, root);
    cp::TreePathFrequencyQueries<long long> frequency(graph, labels, root);
    std::vector<std::vector<int>> paths;
    std::vector<int> expected_frequency;
    std::vector<int> occurrences(graph.size(), 0);
    for (int vertex : mo.tour()) ++occurrences[vertex];
    for (int count : occurrences) assert(count == 2);
    for (int u = 0; u < graph.size(); ++u) for (int v = 0; v < graph.size(); ++v) {
        const auto path = reference_path(graph, u, v);
        if (path.empty()) {
            const int before = mo.query_count(), before_frequency = frequency.query_count();
            expect_throw<std::invalid_argument>([&] { mo.add_query(u, v); });
            expect_throw<std::invalid_argument>([&] { frequency.add_query(u, v, 1, graph.size()); });
            assert(mo.query_count() == before && frequency.query_count() == before_frequency);
            continue;
        }
        assert(mo.add_query(u, v) == static_cast<int>(paths.size())); paths.push_back(path); ++mo_queries;
        std::map<long long, int> counts;
        for (int vertex : path) ++counts[labels[vertex]];
        const int low = 1 + random_int(graph.size() + 3);
        const int high = random_int(7) == 0 ? std::numeric_limits<int>::max() : low + random_int(graph.size() + 3);
        int expected = 0;
        for (const auto& entry : counts) expected += low <= entry.second && entry.second <= high;
        assert(frequency.add_query(u, v, low, high) == static_cast<int>(expected_frequency.size()));
        expected_frequency.push_back(expected); ++frequency_queries;
    }
    for (int block_size : {0, 1, 7, std::numeric_limits<int>::max()}) {
        std::set<int> active;
        std::vector<bool> answered(paths.size(), false);
        mo.run([&](int vertex) { assert(active.insert(vertex).second); },
               [&](int vertex) { assert(active.erase(vertex) == 1); },
               [&](int id) { assert(!answered[id]); answered[id] = true;
                   assert(std::vector<int>(active.begin(), active.end()) == paths[id]); }, block_size);
        assert(active.empty() && std::all_of(answered.begin(), answered.end(), [](bool value) { return value; }));
    }
    assert(frequency.solve() == expected_frequency && frequency.solve() == expected_frequency);
    expect_throw<std::invalid_argument>([&] { mo.run([](int) {}, [](int) {}, [](int) {}, -1); });
}

void test_mo() {
    cp::Graph<long long> empty;
    check_mo(empty, {}, 0);
    cp::Graph<long long> no_queries(3); no_queries.add_edge(0, 1); no_queries.add_edge(1, 2);
    cp::MoOnTree zero(no_queries);
    zero.run([](int) { assert(false); }, [](int) { assert(false); }, [](int) { assert(false); });
    assert(cp::TreePathFrequencyQueries<long long>(no_queries, {1, 1, 1}).solve().empty());
    for (int iteration = 0; iteration < 150; ++iteration) {
        const int n = 1 + random_int(23);
        cp::Graph<long long> graph(n, false);
        for (int v = 1; v < n; ++v) if (iteration % 3 != 0 || random_int(4) != 0)
            graph.add_edge(v, random_int(v), std::numeric_limits<long long>::max() - random_int(100));
        std::vector<long long> labels(n);
        const std::vector<long long> alphabet{0, -17, 1LL << 50, std::numeric_limits<long long>::min(), 91};
        for (auto& label : labels) label = alphabet[random_int(static_cast<int>(alphabet.size()))];
        check_mo(graph, labels, random_int(n));
    }
    cp::Graph<long long> sample(5);
    for (int v = 1; v < 5; ++v) sample.add_edge(v - 1, v);
    cp::TreePathFrequencyQueries<int> queries(sample, {1, 2, 3, 1, 2});
    queries.add_query(0, 4, 1, 2); queries.add_query(0, 3, 2, 3);
    queries.add_query(2, 4, 1, 1); queries.add_query(1, 3, 2, 5);
    assert((queries.solve() == std::vector<int>{3, 1, 3, 0}));
    expect_throw<std::invalid_argument>([&] { cp::TreePathFrequencyQueries<int> mismatch(sample, {1}); });
    expect_throw<std::invalid_argument>([&] { queries.add_query(0, 4, 0, 1); });
    expect_throw<std::invalid_argument>([&] { queries.add_query(0, 4, 3, 2); });
    cp::Graph<long long> directed(2, true); directed.add_edge(0, 1);
    expect_throw<std::invalid_argument>([&] { cp::MoOnTree invalid(directed); });
    cp::Graph<long long> loop(1); loop.add_edge(0, 0);
    expect_throw<std::invalid_argument>([&] { cp::MoOnTree invalid(loop); });
    cp::Graph<long long> parallel(2); parallel.add_edge(0, 1); parallel.add_edge(0, 1);
    expect_throw<std::invalid_argument>([&] { cp::MoOnTree invalid(parallel); });
    cp::Graph<long long> cycle(3); cycle.add_edge(0, 1); cycle.add_edge(1, 2); cycle.add_edge(2, 0);
    expect_throw<std::invalid_argument>([&] { cp::MoOnTree invalid(cycle); });
}

template <class Solve>
void check_official_sample(const std::string& input, const std::string& expected, Solve solve) {
    std::istringstream in(input);
    std::ostringstream out;
    solve(in, out);
    assert(out.str() == expected);
    ++official_samples;
}

void test_official_samples() {
    auto labels = [](std::istream& in, std::ostream& out) {
        int n, m; in >> n >> m;
        cp::Graph<long long> graph(n, true);
        for (int i = 0; i < m; ++i) {
            int u, v; long long digit; in >> u >> v >> digit;
            graph.add_edge(u - 1, v - 1, digit);
        }
        auto values = cp::lexicographic_path_values(cp::lexicographic_bfs(graph, 0));
        for (int v = 1; v < n; ++v) out << values[v] << '\n';
    };
    check_official_sample("4 4\n1 2 1\n1 3 1\n2 4 9\n3 4 2\n", "1\n1\n12\n", labels);
    check_official_sample("3 5\n1 2 9\n1 2 2\n2 2 1\n1 3 9\n2 3 1\n", "2\n9\n", labels);
    check_official_sample("11 11\n1 2 9\n2 3 9\n3 4 8\n4 5 2\n5 6 4\n6 7 4\n7 8 3\n8 9 5\n9 10 3\n9 10 2\n10 11 9\n",
        "9\n99\n998\n9982\n99824\n998244\n9982443\n99824435\n998244352\n998244352\n", labels);

    auto tree_count = [](std::istream& in, std::ostream& out) {
        int n; in >> n;
        std::vector<int> order(n);
        for (int& v : order) { in >> v; --v; }
        out << cp::count_min_first_trees(order) << '\n';
    };
    check_official_sample("5\n1 3 4 2 5\n", "8\n", tree_count);
    check_official_sample("20\n1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20\n", "868586527\n", tree_count);
    check_official_sample("30\n1 16 23 14 18 10 12 21 27 30 17 28 4 9 20 13 22 11 19 3 25 26 6 7 2 8 5 24 15 29\n",
        "910753763\n", tree_count);

    auto wheel = [](std::istream& in, std::ostream& out) {
        int n, q; in >> n >> q;
        std::vector<long long> ring(n), spokes(n);
        for (auto& x : ring) in >> x;
        for (auto& x : spokes) in >> x;
        cp::WheelGraphDistance<> distances(ring, spokes);
        for (int i = 0; i < q; ++i) { int u, v; in >> u >> v; out << distances.distance(u - 1, v - 1) << '\n'; }
    };
    check_official_sample("5 3\n1 3 4 2 5\n5 2 4 3 7\n2 5\n5 6\n1 4\n", "6\n5\n6\n", wheel);
    check_official_sample(
        "10 12\n95786828 55052989 7398452 90695030 73248844 120697567 77915140 68865651 15301338 68674005\n"
        "690874817 21524935 244587369 529970100 736247510 757265588 993115119 576136368 21553212 219853538\n"
        "5 6\n5 8\n2 6\n3 7\n1 3\n3 4\n8 11\n1 10\n2 6\n6 8\n8 9\n5 8\n",
        "73248844\n265090269\n226395315\n244911927\n150839817\n7398452\n90418863\n68674005\n226395315\n198612707\n68865651\n265090269\n", wheel);

    auto frequency = [](std::istream& in, std::ostream& out) {
        int n, q; in >> n >> q;
        std::vector<int> values(n);
        for (int& x : values) in >> x;
        cp::Graph<> graph(n);
        for (int i = 1; i < n; ++i) { int u, v; in >> u >> v; graph.add_edge(u - 1, v - 1); }
        cp::TreePathFrequencyQueries<int> queries(graph, values);
        for (int i = 0; i < q; ++i) { int u, v, low, high; in >> u >> v >> low >> high; queries.add_query(u - 1, v - 1, low, high); }
        for (int answer : queries.solve()) out << answer << '\n';
    };
    check_official_sample("5 4\n1 2 3 1 2\n1 2\n2 3\n3 4\n4 5\n1 5 1 2\n1 4 2 3\n3 5 1 1\n2 4 2 5\n",
        "3\n1\n3\n0\n", frequency);
    check_official_sample(
        "11 8\n1 2 3 1 4 2 2 5 2 1 2\n1 2\n1 3\n2 4\n1 5\n4 6\n6 7\n3 8\n7 9\n4 10\n9 11\n"
        "8 11 3 5\n6 10 1 5\n8 9 5 6\n1 11 1 4\n4 6 2 5\n5 6 2 6\n3 7 1 1\n5 7 5 8\n",
        "1\n2\n0\n1\n0\n2\n1\n0\n", frequency);
    assert(official_samples == 10);
}

void test_large_chain() {
    const int n = 200000;
    cp::Graph<long long> graph(n, false);
    for (int vertex = 1; vertex < n; ++vertex) graph.add_edge(vertex - 1, vertex, std::numeric_limits<long long>::max());
    // 重い辺を使うが、Mo内部は単位重みコピーなのでLCA前処理の距離がoverflowしない。
    cp::MoOnTree mo(graph, n / 2);
    std::vector<std::pair<int, int>> endpoints{{0, n - 1}, {n - 1, n - 1}, {0, 0}, {n / 2, n / 2}};
    for (int i = 0; i < 250; ++i) endpoints.push_back({random_int(20), n - 1 - random_int(20)});
    for (int i = 0; i < 250; ++i) endpoints.push_back({n / 2 - random_int(20), n / 2 + random_int(20)});
    for (const auto& [u, v] : endpoints) mo.add_query(u, v);
    int active_count = 0;
    long long active_sum = 0;
    std::vector<bool> active(n, false), answered(endpoints.size(), false);
    auto add = [&](int vertex) { assert(!active[vertex]); active[vertex] = true; ++active_count; active_sum += vertex; };
    auto remove = [&](int vertex) { assert(active[vertex]); active[vertex] = false; --active_count; active_sum -= vertex; };
    mo.run(add, remove, [&](int id) {
        assert(!answered[id]); answered[id] = true;
        const auto [u, v] = endpoints[id]; const int low = std::min(u, v), high = std::max(u, v);
        assert(active_count == high - low + 1);
        assert(active_sum == static_cast<long long>(low + high) * active_count / 2);
    });
    assert(active_count == 0 && active_sum == 0 && std::none_of(active.begin(), active.end(), [](bool value) { return value; }));
    assert(std::all_of(answered.begin(), answered.end(), [](bool value) { return value; }));
    mo_queries += static_cast<long long>(endpoints.size());
    cp::TreePathFrequencyQueries<long long> frequency(graph, std::vector<long long>(n, -1));
    frequency.add_query(0, n - 1, n, n); frequency.add_query(0, n - 1, 1, n - 1);
    frequency.add_query(n / 2, n / 2, 1, 1);
    assert((frequency.solve() == std::vector<int>{1, 0, 1})); frequency_queries += 3;
    cp::Graph<long long> labeled(n, true);
    for (int vertex = 1; vertex < n; ++vertex) labeled.add_edge(vertex - 1, vertex, vertex % 10);
    const auto bfs = cp::lexicographic_bfs(labeled, 0);
    assert(bfs.distance.back() == n - 1 && bfs.path_to(n - 1).size() == static_cast<std::size_t>(n));
    const auto values = cp::lexicographic_path_values(bfs, 10, 998244353);
    long long expected = 0;
    for (int vertex = 1; vertex < n; ++vertex) expected = (expected * 10 + vertex % 10) % 998244353;
    assert(values.back() == expected);
    const auto search = cp::min_first_search(labeled, 0);
    assert(search.order.size() == static_cast<std::size_t>(n) && search.order.back() == n - 1);
}
} // namespace recent_graph_test

int main() {
    using namespace recent_graph_test;
    test_shortlex(); test_wheel(); test_search(); test_tree_count(); test_mo(); test_official_samples(); test_large_chain();
    std::cout << "PASS recent graph/tree: " << shortlex_sources << " shortlex sources, " << wheel_pairs
              << " wheel pairs, " << search_sources << " min-first sources, " << prufer_roots
              << " Prufer roots, " << permutation_counts << " permutations, " << mo_queries
              << " Mo paths, " << frequency_queries << " frequency queries, " << official_samples
              << " official sample cases and 200000-vertex chains\n";
}
