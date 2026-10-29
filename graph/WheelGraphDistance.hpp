// https://github.com/kazu107/kyopro_library/blob/main/graph/WheelGraphDistance.hpp
// SPDX-License-Identifier: CC0-1.0
/*
できること: 重み付きの輪と、その全頂点に接続する中心を持つ車輪グラフで、任意の2頂点間距離をO(1)で取得する。
使い方:
  cp::WheelGraphDistance<long long> wheel(ring, spokes);
  auto answer = wheel.distance(u, v); // 輪の頂点0..N-1、中心はwheel.hub()==N。
  auto without_hub = wheel.ring_distance(u, v); auto center_distance = wheel.distance_to_hub(u);
前提: ring[i]はiと(i+1)%Nの重み、spokes[i]はiと中心の重み。N>=1、同じ長さ、非負の符号付き整数。
  輪の重みの総和と各spokeはdistance_inf<Cost>()未満。負重み/長さ違いはinvalid_argument、範囲超過はoverflow_error。
  グラフは作成後固定。N=1/2、0重み、同じ頂点への取得も可。経路そのものは返さない。
計算量: 構築O(N log(N+1))、距離取得O(1)、メモリO(N)。
参考問題: https://atcoder.jp/contests/abc477/tasks/abc477_e
標準ヘッダは提出用テンプレート側で用意する。
*/
// 依存: graph/dijkstra.hpp

namespace cp {
template <class Cost = long long>
class WheelGraphDistance {
    int n_;
    std::vector<Cost> prefix_, to_hub_;
    void check(int v) const { assert(0 <= v && v <= n_); }
public:
    WheelGraphDistance(const std::vector<Cost>& ring, const std::vector<Cost>& spokes) {
        if (ring.empty() || ring.size() != spokes.size()
            || ring.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::invalid_argument("Wheel requires equally sized nonempty arrays");
        n_ = static_cast<int>(ring.size());
        const Cost inf = distance_inf<Cost>();
        prefix_.assign(n_ + 1, 0);
        Graph<Cost> graph(n_ + 1, false);
        for (int i = 0; i < n_; ++i) {
            if (ring[i] < 0 || spokes[i] < 0) throw std::invalid_argument("Wheel requires nonnegative weights");
            const __int128 next = static_cast<__int128>(prefix_[i]) + ring[i];
            if (next >= inf || spokes[i] >= inf) throw std::overflow_error("Wheel weights exceed distance range");
            prefix_[i + 1] = static_cast<Cost>(next);
            graph.add_edge(i, (i + 1) % n_, ring[i]);
            graph.add_edge(i, n_, spokes[i]);
        }
        to_hub_ = dijkstra(graph, n_).dist;
    }
    int ring_size() const { return n_; }
    int hub() const { return n_; }
    Cost circumference() const { return prefix_.back(); }
    Cost ring_distance(int u, int v) const {
        assert(0 <= u && u < n_ && 0 <= v && v < n_);
        const Cost delta = prefix_[u] < prefix_[v] ? prefix_[v] - prefix_[u] : prefix_[u] - prefix_[v];
        return std::min(delta, prefix_.back() - delta);
    }
    Cost distance_to_hub(int v) const { check(v); return to_hub_[v]; }
    Cost distance(int u, int v) const {
        check(u); check(v);
        if (u == n_) return to_hub_[v];
        if (v == n_) return to_hub_[u];
        return std::min(ring_distance(u, v), saturated_distance_add(to_hub_[u], to_hub_[v]));
    }
};
} // namespace cp
